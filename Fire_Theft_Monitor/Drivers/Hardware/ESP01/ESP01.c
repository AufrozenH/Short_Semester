#include "ESP01.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UI.h"
#include "MPU6050.h"

ESP01 g_esp01;

UART_HandleTypeDef* pESPHandle;
osMessageQueueId_t EspQueueHandle = NULL;
const osMessageQueueAttr_t EspQueue_attributes = {
  .name = "EspQueue"
};

void SendATCmd(char *cmd, int waitms)
{
	if (NULL != cmd)
	{
		HAL_UART_Transmit(pESPHandle, (uint8_t *)cmd, strlen(cmd), 0xFFFF);
		osDelay(waitms);
	}
}

void SendEspStr(char *str)
{
	if (NULL != str)
		HAL_UART_Transmit(pESPHandle, (uint8_t *)str, strlen(str), 0xFFFF);
}

void InitEsp01(UART_HandleTypeDef* pUartHandle)
{
	osDelay(100);
	
	if (!pUartHandle)
		return;
	
	pESPHandle = pUartHandle;
	if (!EspQueueHandle)
		EspQueueHandle = osMessageQueueNew (8, RXBUF_SIZE, &EspQueue_attributes);
	HAL_UART_Receive_IT(pESPHandle, g_esp01.rxdata.rx_buf, sizeof(g_esp01.rxdata.rx_buf) - 1);
	HAL_UART_AbortReceive_IT(pESPHandle);
	
	HAL_UARTEx_ReceiveToIdle_IT(pESPHandle, g_esp01.rxdata.rx_buf, sizeof(g_esp01.rxdata.rx_buf) - 1);
	
	char buf[80];
	g_esp01.bAtOK = 0;
	g_esp01.bConnect = 0;
	
	printf("�˳�ESP01ģ��͸��ģʽ...\n");
	SendATCmd("+++", 500);
	
	printf("����ESP01ģ���Ƿ����...\n");
	SendATCmd("AT\r\n", 1000);
	
	if (g_esp01.bAtOK)
	{
		printf("�ر�ģ�����\n");
		SendATCmd("ATE0\r\n", 500);
		printf("�鿴ģ��汾��Ϣ...\n");
		SendATCmd("AT+GMR\r\n", 1000);

		printf("����AP+STAģʽ\n");
		SendATCmd("AT+CWMODE=3\r\n", 500);
		
		printf("��ѯSSID\n");
		SendATCmd("AT+CWSAP?\r\n", 500);

		printf("��ѯ��ǰ���ӵ��ȵ�\n");
		SendATCmd("AT+CWJAP?\r\n", 500);
	
		if (strcmp(g_esp01.strAPName, AP_NAME))
		{
//		printf("�Ͽ���ǰ���ӵ��ȵ�\n");
//		SendATCmd("AT+CWQAP\r\n", 500);
			printf("����WiFi�ȵ�\n");
			g_esp01.bConnect = 1;
			sprintf(buf, "AT+CWJAP=\"%s\",\"%s\"\r\n", AP_NAME, AP_PSW);
			SendATCmd(buf, 10000);
		}
		else
			g_esp01.bConnect = 2;
		
		if (g_esp01.bConnect > 1)
		{
			printf("��ѯ�豸IP\n");
			SendATCmd("AT+CIFSR\r\n", 500);
			
			printf("��ѯ��ǰ���ӵ�TCP������\n");
			SendATCmd("AT+CIPSTATUS\r\n", 500);
			
			printf("����Զ��TCP������...\n");
			sprintf(buf, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", TCP_SERVER, TCP_PORT);
			SendATCmd(buf, 2000);
			printf("����͸��ģʽ\n");
			SendATCmd("AT+CIPMODE=1\r\n", 500);
			printf("��ʼ͸��\n");
			SendATCmd("AT+CIPSEND\r\n", 500);
		}
	}
}

void RxEvent(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart == pESPHandle)
	{
		if (Size > 0)
		{
			g_esp01.rxdata.rx_buf[Size] = '\0';
			g_esp01.rxdata.rx_len = Size;
			osMessageQueuePut(EspQueueHandle, &g_esp01.rxdata, NULL, 0);
		}
		
		__HAL_UNLOCK(huart);
		HAL_UARTEx_ReceiveToIdle_IT(pESPHandle, g_esp01.rxdata.rx_buf, sizeof(g_esp01.rxdata.rx_buf) - 1);
	}
}

// ���ڽ����¼��ص����������Ҫ֧�ֶ�����ڽ��գ��Ѹú����Ƶ�FreeRTOS.c�ļ�ĩβ������ESP01.h�н�RxEvent��������һ��
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	RxEvent(huart, Size);
}

void ProcEsp01Data(USART_RX_DATA *pdata)
{
	if (pdata)
	{
		if (pdata->rx_len > 0)
		{
			char *pstr = (char *)pdata->rx_buf;
			printf("ESP01 rx:%s", pstr);
			
			if (strstr(pstr, "OK") >= pstr)
				g_esp01.bAtOK = 1;

			if (g_esp01.bAtOK)
			{
				if (strstr(pstr, "\r\nOK") == pstr)
				{
					if (1 == g_esp01.bConnect)
					{
						g_esp01.bConnect = 2;	// ������Wifi�ȵ�
						printf("ESP01 conn:2\n");
					}
				}
				
				if (strstr(pstr, "WIFI CONNECTED") == pstr)
				{
					g_esp01.bConnect = 1;
					printf("ESP01 conn:1\n");
				}

				if (strstr(pstr, "CONNECT\r\n\r\nOK") == pstr && g_esp01.bConnect == 2)
				{
					g_esp01.bConnect = 3;		// ������TCP������
					printf("ESP01 conn:3\n");
				}
				else if (strstr(pstr, "ALREADY CONNECTED") == pstr && g_esp01.bConnect == 2)
				{
					g_esp01.bConnect = 3;		// ������TCP������
					printf("ESP01 conn:3\n");
					
				}
				else if (strstr(pstr, "+CWSAP:") == pstr)
				{
					char *pc = strstr(pstr + 8, "\"");
					if (pc > pstr + 8)
					{
						// SSID����
						strncpy(g_esp01.strESPName, pstr + 8, pc - pstr - 8);
					}
				}
				else if (strstr(pstr, "+CWJAP:") == pstr)
				{
					char *pc = strstr(pstr + 8, "\"");
					if (pc > pstr + 8)
					{
						// �����ӵ��ȵ�����
						strncpy(g_esp01.strAPName, pstr + 8, pc - pstr - 8);
					}
				}
			}
		}
	}
}

uint8_t EspRxDataOk(void)
{
	USART_RX_DATA tdat;
	if (osMessageQueueGet(EspQueueHandle, &tdat, 0, 0) == osOK)
	{
		ProcEsp01Data(&tdat);
		return 1;
	}
	else
		return 0;
}
