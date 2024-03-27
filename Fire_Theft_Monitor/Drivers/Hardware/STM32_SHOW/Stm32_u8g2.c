/*
 * @Author: wangzihao 2281036574@qq.com 21041428
 * @Date: 2024-03-16  
 */
#include "stm32_u8g2.h"

const uint8_t wzhbmp[]={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x03,0x00,0x00,0x00,0x00,0x00,0x80,0xFF,0x3F,0x00,0x00,0x00,0x00,0x00,0xF0,0xFF,0x7F,0x00,0x00,0x00,
0x00,0x00,0xFC,0xFF,0xFF,0x01,0x00,0x00,0x00,0x00,0xFE,0xFF,0xFF,0x03,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x07,0x00,0x00,0x00,0x80,0xFF,0xFF,0xFF,0x0F,0x00,0x00,
0x00,0xC0,0xFF,0xFF,0xFF,0x1F,0x00,0x00,0x00,0xE0,0xFF,0xFF,0xFF,0x1F,0x00,0x00,0x00,0xF0,0xFF,0xFF,0xFF,0x3F,0x00,0x00,0x00,0xF8,0xFF,0xFF,0xFF,0x7F,0x00,0x00,
0x00,0xF8,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xF8,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFC,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xFC,0x7F,0xCF,0xFB,0xFF,0x01,0x00,
0x00,0xFC,0x0F,0x80,0xF1,0xFF,0x01,0x00,0x00,0xFC,0x03,0x00,0xC0,0xFF,0x01,0x00,0x00,0xFC,0x01,0x00,0x00,0xFE,0x01,0x00,0x00,0xFC,0x01,0x00,0x00,0xFE,0x01,0x00,
0x00,0xFC,0x01,0x00,0x00,0xFE,0x01,0x00,0x00,0xFC,0x01,0x00,0x00,0xFC,0x01,0x00,0x00,0xFC,0x00,0x00,0x00,0xFE,0x01,0x00,0x00,0xFC,0x00,0x00,0x00,0xFE,0x01,0x00,
0x00,0xFC,0x3C,0x00,0xF0,0xF9,0x01,0x00,0x00,0x7C,0xFE,0x00,0xFC,0xF3,0x01,0x00,0x00,0x3C,0xFF,0x01,0xFE,0xF7,0x00,0x00,0x00,0x3C,0x87,0x03,0x0E,0xE6,0x00,0x00,
0x00,0xB8,0x03,0x02,0x00,0xEC,0x00,0x00,0x00,0xB8,0x01,0x00,0x78,0xFC,0x00,0x00,0x00,0x38,0xF8,0x00,0xF8,0xFD,0x03,0x00,0x00,0x3A,0xFC,0x02,0xB8,0xF7,0x03,0x00,
0x00,0x3A,0x6E,0x03,0xB4,0xF7,0x03,0x00,0x00,0x32,0x00,0x00,0xC0,0x73,0x03,0x00,0x00,0x30,0x00,0x00,0x00,0x70,0x01,0x00,0x00,0x30,0x00,0x00,0x00,0x78,0x00,0x00,
0x00,0x70,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x60,0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x70,0x00,0x00,0x02,0x78,0x00,0x00,
0x00,0x60,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0xE0,0x00,0x8C,0x01,0x38,0x00,0x00,0x00,0xC0,0x00,0xDC,0x03,0x18,0x00,0x00,0x00,0x80,0x00,0x00,0x02,0x0C,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x0C,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x01,0xFC,0x03,0x04,0x00,0x00,
0x00,0x00,0x02,0xFF,0x07,0x02,0x00,0x00,0x00,0x00,0x02,0xFF,0x07,0x02,0x00,0x00,0x00,0x00,0x04,0x06,0x03,0x01,0x00,0x00,0x00,0x00,0x04,0xFC,0x83,0x01,0x00,0x00,
0x00,0x00,0x0C,0xF8,0x80,0x01,0x00,0x00,0x00,0x00,0x1C,0x00,0xC0,0x01,0x00,0x00,0x00,0x00,0x3C,0x00,0xE0,0x03,0x00,0x00,0x00,0x00,0x7C,0x00,0xF0,0x03,0x00,0x00,
0x00,0x00,0xEC,0x00,0xF8,0x03,0x00,0x00,0x00,0x00,0x8C,0x03,0xBE,0x0F,0x00,0x00,0x00,0x00,0x04,0xFF,0x1F,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x00,0x00,
};//大头照存储数组

const uint8_t yjhbmp[]={
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x03,0x00,0x00,0x00,0x00,0x00,0xC0,0xFF,0x1F,0x00,0x00,0x00,0x00,0x00,0xF8,0xFF,0x3F,0x00,0x00,0x00,
0x00,0x00,0xFE,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x01,0x00,0x00,0x00,0x80,0xFF,0xFF,0xFF,0x07,0x00,0x00,0x00,0x80,0xFF,0xFF,0xFF,0x0F,0x00,0x00,
0x00,0xC0,0xFF,0xFF,0xFF,0x1F,0x00,0x00,0x00,0xE0,0xFF,0xFF,0xFF,0x1F,0x00,0x00,0x00,0xE0,0xFF,0xFF,0xFF,0x7F,0x00,0x00,0x00,0xF0,0xFF,0xFF,0xFF,0x7F,0x00,0x00,
0x00,0xF8,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xF8,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xF8,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0xF8,0xFF,0xFF,0xFF,0xFF,0x01,0x00,
0x00,0xFC,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x00,0xFC,0xFF,0x07,0xF6,0xFF,0x01,0x00,0x00,0xFC,0x3F,0x00,0xF0,0xFF,0x01,0x00,0x00,0xFC,0x1F,0x00,0x80,0xFF,0x01,0x00,
0x00,0xFC,0x0F,0x00,0x00,0xFE,0x01,0x00,0x00,0xFC,0x0F,0x00,0x00,0xFC,0x01,0x00,0x00,0xFC,0x01,0x00,0x00,0xFC,0x03,0x00,0x00,0xFC,0x01,0x00,0x00,0xF8,0x01,0x00,
0x00,0xFC,0x00,0x00,0x00,0xF8,0x01,0x00,0x00,0x7C,0x18,0x00,0x00,0xF8,0x01,0x00,0x00,0x7C,0x7C,0x00,0xF0,0xFB,0x01,0x00,0x00,0x3C,0xFE,0x00,0xFC,0xFF,0x01,0x00,
0x00,0x38,0x8F,0x01,0x06,0xFF,0x00,0x00,0x00,0x38,0x01,0x01,0x00,0xE4,0x01,0x00,0x00,0x3C,0x7C,0x02,0x30,0xE0,0x01,0x00,0x00,0x38,0x7E,0x00,0xBC,0xE1,0x01,0x00,
0x00,0x18,0x7E,0x07,0xBC,0x60,0x01,0x00,0x00,0x18,0x40,0x00,0x10,0xE0,0x01,0x00,0x00,0x10,0x00,0x00,0x00,0xE0,0x00,0x00,0x00,0x30,0x00,0x00,0x00,0xE0,0x01,0x00,
0x00,0x30,0x00,0x00,0x00,0xE0,0x01,0x00,0x00,0x74,0x00,0x04,0x00,0x60,0x01,0x00,0x00,0x7C,0x00,0x04,0x00,0xE0,0x00,0x00,0x00,0xF8,0x00,0x06,0x00,0xA0,0x00,0x00,
0x00,0xB0,0x00,0x04,0x00,0xA0,0x00,0x00,0x00,0x50,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0xC0,0x01,0x8E,0x01,0x10,0x00,0x00,0x00,0x80,0x00,0x04,0x00,0x10,0x00,0x00,
0x00,0x80,0x01,0x00,0x00,0x10,0x00,0x00,0x00,0x80,0x01,0x00,0x00,0x10,0x00,0x00,0x00,0x80,0x01,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x03,0xFE,0x01,0x00,0x00,0x00,
0x00,0x00,0x87,0xFF,0x0F,0x08,0x00,0x00,0x00,0x00,0x06,0x06,0x02,0x00,0x00,0x00,0x00,0x00,0x0E,0x8F,0x01,0x04,0x00,0x00,0x00,0x00,0x1C,0xFC,0x00,0x02,0x00,0x00,
0x00,0x00,0x1C,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x24,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x64,0x00,0xC0,0x02,0x00,0x00,0x00,0x00,0x04,0x00,0x20,0x02,0x00,0x00,
0x00,0x00,0x84,0x01,0x00,0x04,0x00,0x00,0x00,0x00,0x06,0x7F,0x02,0x0C,0x00,0x00,0x00,0x00,0x07,0x30,0x00,0x1C,0x00,0x00,0x00,0x00,0x07,0x00,0x00,0x38,0x00,0x00,
};//大头照存储数组


uint8_t u8g2_gpio_and_delay_stm32(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
	switch(msg)
	{
		
		case U8X8_MSG_DELAY_MILLI://Function which implements a delay, arg_int contains the amount of ms
			HAL_Delay(arg_int);
		break;
		
		case U8X8_MSG_DELAY_10MICRO://Function which delays 10us
			for (uint16_t n = 0; n < 320; n++)
			{
				__NOP();
			}
		break;
		
		case U8X8_MSG_DELAY_100NANO://Function which delays 100ns
			__NOP();
		break;	
		
		case U8X8_MSG_GPIO_I2C_CLOCK://Function to define the logic level of the RESET line
			if (arg_int) HAL_GPIO_WritePin(I2C_SCL_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_SET);
			else HAL_GPIO_WritePin(I2C_SCL_GPIO_Port, I2C_SCL_Pin, GPIO_PIN_RESET);
		break;
		
		case U8X8_MSG_GPIO_I2C_DATA://Function to define the logic level of the RESET line
			if (arg_int) HAL_GPIO_WritePin(I2C_SDA_GPIO_Port, I2C_SDA_Pin, GPIO_PIN_SET);
			else HAL_GPIO_WritePin(I2C_SDA_GPIO_Port, I2C_SDA_Pin, GPIO_PIN_RESET);
		break;
		
		default:
			return 0; //A message was received which is not implemented, return 0 to indicate an error
	}

	return 1; // command processed successfully.
}

void u8g2Init(u8g2_t *u8g2)
{
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(u8g2, U8G2_R2, u8x8_byte_sw_i2c, u8g2_gpio_and_delay_stm32);//U8G2_R0:正向显示 U8G2_R2：逆向显示
	u8g2_InitDisplay(u8g2); 			
  u8g2_SetPowerSave(u8g2, 0);  
	u8g2_ClearBuffer(u8g2);
}


void draw_logo(u8g2_t *u8g2)
{
	u8g2_ClearBuffer(u8g2); 
	
    u8g2_SetFontMode(u8g2, 1); 
    u8g2_SetFontDirection(u8g2, 0); 
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf); 
    u8g2_DrawStr(u8g2, 0, 20, "U");
    
    u8g2_SetFontDirection(u8g2, 1);
    u8g2_SetFont(u8g2, u8g2_font_inb30_mn);
    u8g2_DrawStr(u8g2, 21,8,"8");
        
    u8g2_SetFontDirection(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
    u8g2_DrawStr(u8g2, 51,30,"g");
    u8g2_DrawStr(u8g2, 67,30,"\xb2");
    
    u8g2_DrawHLine(u8g2, 2, 35, 47);
    u8g2_DrawHLine(u8g2, 3, 36, 47);
    u8g2_DrawVLine(u8g2, 45, 32, 12);
    u8g2_DrawVLine(u8g2, 46, 33, 12);
  
    u8g2_SetFont(u8g2, u8g2_font_4x6_tr);
    u8g2_DrawStr(u8g2, 1,54,"github.com/olikraus/u8g2");
		
	u8g2_SendBuffer(u8g2);
	HAL_Delay(1000);
}

void testDrawPixelToFillScreen(u8g2_t *u8g2)
{
	u8g2_ClearBuffer(u8g2);

  for (int j = 0; j < 64; j++)
  {
    for (int i = 0; i < 128; i++)
    {
      u8g2_DrawPixel(u8g2,i, j);
    }
  }
	u8g2_SendBuffer(u8g2);
  HAL_Delay(1000);
}