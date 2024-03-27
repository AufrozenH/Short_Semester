/*
 * @Author: wangzihao 2281036574@qq.com 21041428
 * @Date: 2024-03-18 
 */
#include "main.h"
#include "tim.h"
#include "UI.h"
#include "Device.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "MPU6050.h"
#include <stdlib.h>

uint16_t beep_tick=0;
uint8_t Show_Sec=30;//秒
uint32_t TEMP_tick=0;
uint32_t MPU_tick=0;
uint32_t warntick = 0;
uint8_t warncnt=0;
uint32_t alarm_tick = 0;
uint32_t alarm_i = 0;
uint32_t alarm_fre = 800;
uint8_t alarm_step = 0;

uint16_t fire_music[]=
{
		M6,M3,M6,M3,M6,M3,M6,M3
};
uint16_t shock_music[]=
{
	H6,H6,M3,M3,H6,H6,M3,M3,H6,H6,M3,M3
};
//-------------------------------------------------------------------------------------------------------------------
//  @brief      各种数据更新以及判断
//  @param      dat              
//  @return     float
//  Sample usage:Information_Update();
//-------------------------------------------------------------------------------------------------------------------
void Information_Update(void)
{ 
	if(UI_Select != GUI_LOGO)
	{
		//温度数据更新
		DS18B20_TEMP();		
		//陀螺仪数据更新
		MPU_6050data();
		//温度报警判断
		if(Temp>Temp_stand && Temp < 80 && Temp_state==0 )//小于80防止温度第一次初始化异常报警
		{
			Temp_state = 1;
			warntick = osKernelGetTickCount();
		}	
		else if(((abs(gx) + abs(gy)  + abs(gz)) > ((10 -Shock_sens) * 1000 )) && Shock_state == 0 )//震动报警判断
		{
			if(++warncnt >= 3)
			{
				Shock_state = 1;
				warntick = osKernelGetTickCount();
			}
		}
		else
				warncnt = 0;
	}		
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      每隔50毫秒读取陀螺仪值         
//  @return     void
//  Sample usage:MPU_6050data();
//-------------------------------------------------------------------------------------------------------------------
void MPU_6050data(void)
{
		uint32_t MPU_cnt=osKernelGetTickCount();
		if(MPU_cnt>=MPU_tick){
			MPU_tick=MPU_cnt+50;//50毫秒时间间隔
			if(mpuok)
				MPU_getdata();
			sprintf(Ax, "%d", ax);	
			sprintf(Ay, "%d", ay);
			sprintf(Az, "%d", az);	
			sprintf(Gx, "%d", gx);	
			sprintf(Gy, "%d", gy);	
			sprintf(Gz, "%d", gz);
		} 
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      每隔一秒读取温度值
//  @param      dat              
//  @return     float
//  Sample usage:DS18B20_TEMP();
//-------------------------------------------------------------------------------------------------------------------
void DS18B20_TEMP(void)
{
		uint32_t TEMP_cnt=osKernelGetTickCount();
		if(TEMP_cnt>=TEMP_tick){
			TEMP_tick=TEMP_cnt+1000;//1秒时间间隔
		  Temp=ds18b20_read();//读取传感器温度数据
			sprintf(sTemp, "%.1f", Temp);			
		} 
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      数码管30秒倒计时
//  @param      dat              
//  @return     float
//  Sample usage:Warn_Count();
//-------------------------------------------------------------------------------------------------------------------
void Warn_Count(void)
{
	if(Temp_state || Shock_state)
	{
		if(osKernelGetTickCount() >= warntick + Alarm_time*1000)//报警结束清空各种标志位
		{
			Temp_state = Shock_state = 0;
			alarm_tick = 0;
			alarm_i = 0;
			alarm_fre=800;
		  	alarm_step=0;
			HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);//关闭蜂鸣器
		}				
		else
		{
			uint32_t tic = warntick + Alarm_time*1000 - osKernelGetTickCount();
			num[0] = (tic/10000)%10;
			num[1] = (tic/1000)%10;
			num[2] = (tic/100)%10;
			num[3] = (tic/10)%10;
		}
	}
	else
	{
		num[0] = num[1] = num[2] = num[3] =' ';
	}		
	DispSeg(num,2);
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      LED亮灭控制，低电平亮，高电平灭
//  @param      dat              
//  @return     void
//  Sample usage:SetLeds(0xFF);
//-------------------------------------------------------------------------------------------------------------------
void SetLeds(uint8_t dat){
	//8个灯对应dat的低电平
	HAL_GPIO_WritePin(L1_GPIO_Port,L1_Pin,(dat & 0x01) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L2_GPIO_Port,L2_Pin,(dat & 0x02) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L3_GPIO_Port,L3_Pin,(dat & 0x04) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L4_GPIO_Port,L4_Pin,(dat & 0x08) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L5_GPIO_Port,L5_Pin,(dat & 0x10) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L6_GPIO_Port,L6_Pin,(dat & 0x20) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L7_GPIO_Port,L7_Pin,(dat & 0x40) ? GPIO_PIN_RESET : GPIO_PIN_SET);
	HAL_GPIO_WritePin(L8_GPIO_Port,L8_Pin,(dat & 0x80) ? GPIO_PIN_RESET : GPIO_PIN_SET);
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      蜂鸣器报警函数
//  @param      state   状态  1：温度报警 2：震动报警          
//  @return     void
//  Sample usage:Alarm_beep(1);
//-------------------------------------------------------------------------------------------------------------------
void Alarm_beep(uint8_t state)
{
	HAL_TIM_Base_Start_IT(&htim3);
	uint32_t alarm_cnt = osKernelGetTickCount();
	
	if(state == 1)//温度报警
	{
		if(alarm_cnt >= alarm_tick)
		{
			alarm_tick = alarm_cnt + 5;//5ms更换频率
			if(alarm_step == 0)
			{
				alarm_fre+=2;
				if(alarm_fre==1400)//温度报警频率上限
					alarm_step=1;
			}
			else if(alarm_step)
			{
				alarm_fre-=2;
				if(alarm_fre==800)//温度报警频率下限
					alarm_step=0;
			}
			int pre=1000000/alarm_fre;
			__HAL_TIM_SET_AUTORELOAD(&htim3,pre);
			__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,pre/2);
			HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
		}
	}
	else if(state == 2)//震动报警
	{
		if(alarm_cnt >= alarm_tick)
		{
			alarm_tick = alarm_cnt + 200;//200ms更换频率
			if(shock_music[alarm_i] == 0){
				__HAL_TIM_SET_AUTORELOAD(&htim3,1);
				__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,0);
			}
			else
			{
				int pre=1000000/shock_music[alarm_i];
				__HAL_TIM_SET_AUTORELOAD(&htim3,pre);
				__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,pre/2);
				HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
			}
			alarm_i++;
			alarm_i %= (sizeof(shock_music)/sizeof(shock_music[0]));
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      蜂鸣器鸣叫函数
//  @param      tune   音调 对应频率表（C4-B4）
//  @param      time   时间（微秒）          
//  @return     void
//  Sample usage:Beep(50,5);
//-------------------------------------------------------------------------------------------------------------------
void Beep(uint16_t time, uint8_t tune){
	const uint16_t tab[8] = {494,523,588,660,698,784,880,988};
	HAL_TIM_Base_Start_IT(&htim3);
	
	if(tune>=1 && tune<=7){	//如果是有效音调
		
		int pre=1000000/tab[tune];
		__HAL_TIM_SET_AUTORELOAD(&htim3,pre);//重新调整定时频率
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,pre/2);
		
		beep_tick = osKernelGetTickCount() + time;
		HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	}
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      蜂鸣器停止函数         
//  @return     void
//  Sample usage:BeepDone();
//-------------------------------------------------------------------------------------------------------------------
void BeepDone(void)
{
	if(Temp_state || Shock_state)//存在报警状态
	{
		if(Temp_state)Alarm_beep(1);
		else Alarm_beep(2);	
	}
	if(beep_tick>0 && osKernelGetTickCount() >= beep_tick)
	{
		beep_tick = 0;
		HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
	}
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      单个数码管显示
//  @param      sel   
//  @param      num   
//  @param      bdot   
//  @return     void
//  Sample usage:Write595(sel, num, bdot);
//-------------------------------------------------------------------------------------------------------------------
void Write595(uint8_t sel, uint8_t num, uint8_t bdot){

    // 共阴数码管，'0'～'9'，'A'～'F' 编码

    static const uint8_t TAB[16] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,

        0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

    // 74HC138关数码管显示

    HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_RESET);


    uint8_t dat = TAB[num & 0x0F] | (bdot ? 0x80 : 0x00);
				

		if(num > 0x0F)
			dat = 0;

    for (uint8_t i = 0; i < 8; ++i) {

        HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(SER_GPIO_Port, SER_Pin, (dat & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);

        dat <<= 1;

        HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, GPIO_PIN_SET);

    }

    // DISLK脉冲锁存8位输出

    HAL_GPIO_WritePin(DISLK_GPIO_Port, DISLK_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(DISLK_GPIO_Port, DISLK_Pin, GPIO_PIN_SET);

    // 4位数码管片选

    HAL_GPIO_WritePin(A0_GPIO_Port, A0_Pin,  (sel & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(A1_GPIO_Port, A1_Pin,  (sel & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(A2_GPIO_Port, A2_Pin, GPIO_PIN_RESET);

    // 74HC138开数码管显示

    HAL_GPIO_WritePin(A3_GPIO_Port, A3_Pin, GPIO_PIN_SET);

}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      4位数码管动态扫描显示
//  @param      dat   
//  @param      num   
//  @param      bdot   
//  @return     void
//  Sample usage:DispSeg(num, dot)
//-------------------------------------------------------------------------------------------------------------------
void DispSeg(uint8_t num[4], uint8_t dot) {

	for(int i = 0;i<4;i++)
	{
		Write595(i,num[i],(dot == (i + 1)) ? 1 : 0);
		osDelay(1);
	}
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      解锁模式判断
//  @param      null              
//  @return     void
//  Sample usage:Ulock();
//-------------------------------------------------------------------------------------------------------------------
void Ulock(void)
{
	static uint8_t ulock_state = 0;
	static uint8_t cnt = 0;
	static uint32_t tick = 0;
	uint32_t ct=osKernelGetTickCount();	
	if(ct>=tick){//0.5秒时间间隔
		tick=ct+500;	
		if(ulock_state==0 && Ulock_scan())//刚按下
		{
			ulock_state=1;
		}
		else if(ulock_state)
		{
			if(Ulock_scan())
			{
				cnt++;
				if(cnt>=6)//持续3s成功解锁
				{
					cnt=0;
					ulock_state=0;
					Temp_state=0;
          			Shock_state=0;	
					alarm_tick = 0;
					alarm_i = 0;
					alarm_fre=800;
					alarm_step=0;
					HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
				}
			}
			else
			{
				cnt=0;
				ulock_state=0;
			}
		}
	}	
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      解锁模式按键扫描
//  @param      null              
//  @return     uint8_t
//  Sample usage:Ulock_scan();
//-------------------------------------------------------------------------------------------------------------------
uint8_t Ulock_scan(void)
{
	if((HAL_GPIO_ReadPin(KEY_GPIO_Port, K1_Pin) == GPIO_PIN_RESET)&&(HAL_GPIO_ReadPin(KEY_GPIO_Port, K2_Pin) == GPIO_PIN_RESET)&&(HAL_GPIO_ReadPin(KEY_GPIO_Port, K3_Pin) == GPIO_PIN_RESET)&&(HAL_GPIO_ReadPin(KEY_GPIO_Port, K4_Pin) == GPIO_PIN_RESET))
  {
    HAL_Delay(10);
    if((HAL_GPIO_ReadPin(KEY_GPIO_Port, K1_Pin) == GPIO_PIN_RESET)&&(HAL_GPIO_ReadPin(KEY_GPIO_Port, K2_Pin) == GPIO_PIN_RESET)&&(HAL_GPIO_ReadPin(KEY_GPIO_Port, K3_Pin) == GPIO_PIN_RESET)&&(HAL_GPIO_ReadPin(KEY_GPIO_Port, K4_Pin) == GPIO_PIN_RESET))
    {
      return 1;
    }
  }
	return 0;	
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      正常模式按键扫描
//  @param      null              
//  @return     uint8_t
//  Sample usage:ScanKey();
//-------------------------------------------------------------------------------------------------------------------
uint8_t ScanKey(void){
	//按键按下时，K1-K4低电平有效,K5、K6高电平有效
	static uint8_t oldkey = 0;
	static uint8_t keycnt = 0;
	uint8_t key=0;	
	if(HAL_GPIO_ReadPin(KEY_GPIO_Port,K1_Pin) == GPIO_PIN_RESET) key |= KEY1;
	if(HAL_GPIO_ReadPin(KEY_GPIO_Port,K2_Pin) == GPIO_PIN_RESET) key |= KEY2;
	if(HAL_GPIO_ReadPin(KEY_GPIO_Port,K3_Pin) == GPIO_PIN_RESET) key |= KEY3;
	if(HAL_GPIO_ReadPin(KEY_GPIO_Port,K4_Pin) == GPIO_PIN_RESET) key |= KEY4;
	if(HAL_GPIO_ReadPin(KEY_GPIO_Port,K5_Pin) == GPIO_PIN_SET)	  key |= KEY5;
	if(HAL_GPIO_ReadPin(KEY_GPIO_Port,K6_Pin) == GPIO_PIN_SET)	  key |= KEY6;
	
	if(key > 0 && key == oldkey){
		++keycnt;//按键消抖
		if(keycnt == KEYCNT)//返回有效按键	（无连按效果）
		{
			switch(key){
				case KEY1: return KEY_UP;
				case KEY2: return KEY_ADD;
				case KEY3: return KEY_SUB;
				case KEY4: return KEY_DOWN;
				case KEY5: return KEY_ENTER;
				case KEY6: return KEY_LEAVE;				
			}
		}	
	}
	else keycnt=0;
	
	if(key != oldkey)
		oldkey = key;
	
	return 0;
}