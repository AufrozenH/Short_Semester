#ifndef __ESP01_H__
#define __ESP01_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include <cmsis_os.h>

// �޸���������Ϊ��Ĺ����ȵ����ơ�����?
// #define AP_NAME		"AufrozenH"
// #define AP_PSW		"wzh200388#"
// #define AP_NAME		"martinx"
// #define AP_PSW		"2257546277"

// �޸���������Ϊ����ȵ����ڵ��Ի��ֻ�IP��ַ��TCP�������˿�
// #define TCP_SERVER 		"192.168.43.33"
// #define TCP_PORT 			8686
// #define TCP_SERVER_STR 	"43.33: 8686"
#define TCP_SERVER 		"192.168.87.78"
#define TCP_PORT 			4321
#define TCP_SERVER_STR 	"16.1: 4321"

/* ������USART_RX_DATA�������Ͷ���ŵ�usart.hͷ�ļ�
#define RXBUF_SIZE 	512
typedef struct {
	uint16_t rx_len;
	uint8_t rx_buf[RXBUF_SIZE - 2];
} USART_RX_DATA;
*/

/* �����´��븴�Ƶ����������н��н������ݴ���
if (EspRxDataOk())
{
	// �������ݴ���
}
*/

typedef struct {
	USART_RX_DATA rxdata;
	uint8_t bAtOK;
	uint8_t bConnect;
	char strAPName[20];
	char strESPName[20];
} ESP01;

extern ESP01 g_esp01;
uint8_t EspRxDataOk(void);		// �������ݴ���
void SendEspStr(char *str);		// �����ַ���
void InitEsp01(UART_HandleTypeDef* pUartHandle);	// ��ʼ��ESP01����

#ifdef __cplusplus
}
#endif

#endif /* __ESP01_H__ */
