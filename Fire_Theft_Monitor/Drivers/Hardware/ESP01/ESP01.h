#ifndef __ESP01_H__
#define __ESP01_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include <cmsis_os.h>

#define AP_NAME		"martinx"
#define AP_PSW		"2257546277"
// #define AP_NAME		"martinx"
// #define AP_PSW		"2257546277"

#define TCP_SERVER 		"192.168.43.33"
#define TCP_PORT 			8686
#define TCP_SERVER_STR 	"43.33: 8686"
// #define TCP_SERVER 		"192.168.87.78"
// #define TCP_PORT 			4321
// #define TCP_SERVER_STR 	"16.1: 4321"

typedef struct {
	USART_RX_DATA rxdata;
	uint8_t bAtOK;
	uint8_t bConnect;
	char strAPName[20];
	char strESPName[20];
} ESP01;

extern uint16_t send_state;
extern ESP01 g_esp01;
uint8_t EspRxDataOk(void);		
void SendEspStr(char *str);		
void InitEsp01(UART_HandleTypeDef* pUartHandle);	

#ifdef __cplusplus
}
#endif

#endif /* __ESP01_H__ */
