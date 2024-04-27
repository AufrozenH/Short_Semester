#include "ESP01.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UI.h"
#include "MPU6050.h"

uint16_t send_state=0;
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
	
	printf("退出ESP01模块透传模式...\n");
	SendATCmd("+++", 500);
	
	printf("测试ESP01模块是否存在...\n");
	SendATCmd("AT\r\n", 1000);
	
	if (g_esp01.bAtOK)
	{
		printf("关闭模块回显\n");
		SendATCmd("ATE0\r\n", 500);
		printf("查看模块版本信息...\n");
		SendATCmd("AT+GMR\r\n", 1000);

		printf("开启AP+STA模式\n");
		SendATCmd("AT+CWMODE=3\r\n", 500);
		
		printf("查询SSID\n");
		SendATCmd("AT+CWSAP?\r\n", 500);

		printf("查询当前连接的热点\n");
		SendATCmd("AT+CWJAP?\r\n", 500);
	
		if (strcmp(g_esp01.strAPName, AP_NAME))
		{
//		printf("�Ͽ���ǰ���ӵ��ȵ�\n");
//		SendATCmd("AT+CWQAP\r\n", 500);
			printf("连接WiFi热点\n");
			g_esp01.bConnect = 1;
			sprintf(buf, "AT+CWJAP=\"%s\",\"%s\"\r\n", AP_NAME, AP_PSW);
			SendATCmd(buf, 10000);
		}
		else
			g_esp01.bConnect = 2;
		
		if (g_esp01.bConnect > 1)
		{
			printf("查询设备IP\n");
			SendATCmd("AT+CIFSR\r\n", 500);
			
			printf("查询当前连接的TCP服务器\n");
			SendATCmd("AT+CIPSTATUS\r\n", 500);
			
			printf("连接远程TCP服务器...\n");
			sprintf(buf, "AT+CIPSTART=\"TCP\",\"%s\",%d\r\n", TCP_SERVER, TCP_PORT);
			SendATCmd(buf, 2000);
			printf("开启透传模式\n");
			SendATCmd("AT+CIPMODE=1\r\n", 500);
			printf("开始透传\n");
			SendATCmd("AT+CIPSEND\r\n", 500);			
			send_state=1;
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
					g_esp01.bConnect = 3;		// 已连接TCP服务器
					printf("ESP01 conn:3\n");
				}
				else if (strstr(pstr, "ALREADY CONNECTED") == pstr && g_esp01.bConnect == 2)
				{
					g_esp01.bConnect = 3;		// 已连接TCP服务器
					printf("ESP01 conn:3\n");
					
				}
				else if (strstr(pstr, "+CWSAP:") == pstr)
				{
					char *pc = strstr(pstr + 8, "\"");
					if (pc > pstr + 8)
					{
						// SSID名称
						strncpy(g_esp01.strESPName, pstr + 8, pc - pstr - 8);
					}
				}
				else if (strstr(pstr, "+CWJAP:") == pstr)
				{
					char *pc = strstr(pstr + 8, "\"");
					if (pc > pstr + 8)
					{
						// 已连接的热点名称
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
