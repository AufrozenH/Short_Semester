#ifndef __ESP01_H__
#define __ESP01_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include <cmsis_os.h>

// 修改以下内容为你的共享热点名称、密码
// #define AP_NAME		"AufrozenH"
// #define AP_PSW		"wzh200388#"
#define AP_NAME		"martinx"
#define AP_PSW		"2257546277"

// 修改以下内容为你的热点所在电脑或手机IP地址和TCP服务器端口
// #define TCP_SERVER 		"192.168.43.33"
// #define TCP_PORT 			8686
// #define TCP_SERVER_STR 	"43.33: 8686"
#define TCP_SERVER 		"192.168.191.78"
#define TCP_PORT 			6789
#define TCP_SERVER_STR 	"191.78: 6789"

/* 将以下USART_RX_DATA数据类型定义放到usart.h头文件
#define RXBUF_SIZE 	512
typedef struct {
	uint16_t rx_len;
	uint8_t rx_buf[RXBUF_SIZE - 2];
} USART_RX_DATA;
*/

/* 将以下代码复制到串口任务中进行接收数据处理
if (EspRxDataOk())
{
	// 接收数据处理
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
uint8_t EspRxDataOk(void);		// 接收数据处理
void SendEspStr(char *str);		// 发送字符串
void InitEsp01(UART_HandleTypeDef* pUartHandle);	// 初始化ESP01串口

#ifdef __cplusplus
}
#endif

#endif /* __ESP01_H__ */
