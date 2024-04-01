/*
 * @Author: wangzihao 2281036574@qq.com 21041428
 * @Date: 2024-03-16  
 */
#ifndef __STM32_U8G2_H
#define __STM32_U8G2_H

/* Includes ------------------------------------------------------------------*/
#include "u8g2.h"
#include "main.h"
/* USER CODE BEGIN Includes */
extern const uint8_t wzhbmp[];
extern const uint8_t yjhbmp[];
extern const uint8_t TAB_Hao[];
/* USER CODE END Includes */
 
 
 
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */
 
/* USER CODE BEGIN Prototypes */
uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr);
void u8g2Init(u8g2_t *u8g2);
void draw_logo(u8g2_t *u8g2);
void testDrawPixelToFillScreen(u8g2_t *u8g2);

#endif