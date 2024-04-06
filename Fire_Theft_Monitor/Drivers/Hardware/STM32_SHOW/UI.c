/*
 * @Author: wangzihao 2281036574@qq.com	21041428
 * @Date: 2024-03-17 
 */
#include "UI.h"
#include "u8g2.h"
#include "stm32_u8g2.h"
#include "gpio.h"
#include "cmsis_os.h"
#include "stdio.h"
#include "DS_18B20.h"
#include "MPU6050.h"  
#include <Display_3D.h>

u8g2_t u8g2;

SETTING_LIST list[]=
{
	{"实时监测",4,3},
	{"数据曲线",4,5},
	{"无线通信",4,2},
	{"参数设置",4,1},
};
SETTING_LIST Para[]=
{
	{"温度上限:",25,1},
	{"震动灵敏度:",14,1},
	{"报警时长:",24,1},
	{"上传间隔:",26,1},
};

Pid_Error Dynamic_menu = {450, 300, 200, 0, 0, 0};//动态菜单选择效果pid
Pid_Error PARA_Line = {450, 300, 200, 0, 0, 0}, PARA_Wide = {400, 200, 150, 0, 0, 0};//动态参数选择效果pid

char UI_Select = GUI_LOGO;//初始选择启动界面
char LOGO_Select = LOGO_TITLE;//初始选择启动界面
char PAGE_Select = 0;//初始子页面
char PARA_Select = PARA_TEMP;//初始参数:温度上限值
short item_wide=50;//item_wide  = list[UI_Select-1].len*14+1;
short frame_y,frame_y_trg;//选择框目前y值，选择框目标y值
short frame_x,frame_x_trg;//滑动框目前x值，滑动框目标x值
short para_y,para_y_trg;//参数框目前y值，参数框目标y值
short para_x,para_x_trg;//参数框目前x值，参数框目标x值
uint8_t Temp_stand=30;//温度上限值0~90℃
uint8_t Shock_sens=1;//震动灵敏度0~9级
uint8_t Alarm_time=30;//报警时长0~60S
float Upload_inter=0.1;//上传间隔100ms~10S
float Temp=0;//实际温度值
char sTemp[5];//记录实际温度数组
char Ax[10];//记录实际陀螺仪ax数组
char Ay[10];//记录实际陀螺仪ay数组
char Az[10];//记录实际陀螺仪az数组
char Gx[10];//记录实际陀螺仪gx数组
char Gy[10];//记录实际陀螺仪gy数组
char Gz[10];//记录实际陀螺仪gz数组
char FAX[10];//记录实际陀螺仪俯仰角数组
char FAY[10];//记录实际陀螺仪横滚角数组
char FAZ[10];//记录实际陀螺仪航向角数组

uint8_t Temp_state=0;//温度报警标志位 1：温度超过阈值
uint8_t Shock_state=0;//震动报警标志位 1：加速度超过阈值
uint32_t logo_tick=0;//定义静态变量，存储进入启动界面时的时间戳 
uint32_t led_tick=0;//led灯秒闪的时间戳
uint8_t leds_sta=0x00;//led灯初始状态（全灭）
uint8_t start_beep=0;//启动蜂鸣器标志位
uint8_t num[4]={0};
uint8_t mpuok = 0;//mpu6050初始化状态
int mpu_gap = 0;//记录数值间隔时间（默认 0 可切换为1000）0意味着50记录一次

float vTemp[MAX_DATALEN];//温度数据缓存
int cTemp=0;//温度数据缓存计数
float vPitch[MAX_DATALEN];//俯仰角数据缓存
int cPitch=0;//俯仰角数据缓存计数
float vRoll[MAX_DATALEN];//横滚角数据缓存
int cRoll=0;//横滚角数据缓存计数
float vYaw[MAX_DATALEN];//航向角数据缓存
int cYaw=0;//航向角数据缓存计数

//-------------------------------------------------------------------------------------------------------------------
//  @brief      系统初始化
//  @param      null              
//  @return     void
//  Sample usage:SYS_Init();
//-------------------------------------------------------------------------------------------------------------------
void SYS_Init(void)
{
	osDelay(1000);
	ds18b20_init();//温度传感器初始化
	mpuok = MPU_init();//MPU6050初始化
	uint8_t cnt = 0;
	while(cnt++ < 3 && !mpuok)
	{
		osDelay(250);
		mpuok = MPU_init();
	}
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      UI初始化
//  @param      null              
//  @return     void
//  Sample usage:UI_Init();
//-------------------------------------------------------------------------------------------------------------------
void UI_Init(void)
{
	u8g2Init(&u8g2);//屏幕初始化
	frame_x=frame_x_trg=60;//子页面滑动框x初始值
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      系统状态
//  @param      null              
//  @return     void
//  Sample usage:SYS_state();
//-------------------------------------------------------------------------------------------------------------------
void SYS_state(void)
{
	switch(UI_Select){
		case GUI_LOGO:{LOGO_state();break;}
		default :break;
	}
	if(start_beep)	
	{
		Beep(250,5);//蜂鸣器启动鸣叫		
		start_beep=0;
	}
	Warn_Count();
	BeepDone();
	SetLeds(leds_sta);
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      UI界面显示
//  @param      null              
//  @return     void
//  Sample usage:UI_Show();
//-------------------------------------------------------------------------------------------------------------------
void UI_Show(void)
{
	switch(UI_Select){
		case GUI_LOGO:{UI_logo();break;}
		default :{UI_menu();break;}
	}
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      logo状态
//  @param      null              
//  @return     void
//  Sample usage:LOGO_state();
//-------------------------------------------------------------------------------------------------------------------
void LOGO_state(void)
{
	if(osKernelGetTickCount()>=led_tick + 500)
	{						
		//LED灯闪烁
		leds_sta = ~leds_sta;
		led_tick=osKernelGetTickCount();
	}
	
	if(0==logo_tick)
		logo_tick=osKernelGetTickCount();//开始进入界面时，记录时间戳
	else
	{
		if(osKernelGetTickCount() <= logo_tick + 2000)
		{
			//显示选题名称
			LOGO_Select = LOGO_TITLE;
		}
		else if(osKernelGetTickCount() <= logo_tick + 5000)
		{
			//显示学号姓名
			LOGO_Select = NUM_NAME;
		}
		else if(osKernelGetTickCount() <= logo_tick+8000)
		{
			//显示组内成员大头照
			LOGO_Select = PEO_PIC;
		}
		else
		{
			UI_Select = GUI_MONI;               //logo界面状态跳转到主菜单界面
			//logo界面常规初始化预备
			LOGO_Select = LOGO_TITLE;
			leds_sta = 0x00;					
			logo_tick=0;
			led_tick=0;
			start_beep=1;
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      UI初始界面显示
//  @param      null              
//  @return     void
//  Sample usage:UI_logo();
//-------------------------------------------------------------------------------------------------------------------
void UI_logo(void)
{
	u8g2_SetFont(&u8g2,u8g2_font_wqy16_t_gb2312b); //设置启动界面字体
	u8g2_ClearBuffer(&u8g2);  
	switch(LOGO_Select){
		case LOGO_TITLE:
			//显示选题名称
			u8g2_DrawUTF8(&u8g2,31,15,"专业实践");	
			u8g2_DrawUTF8(&u8g2,23,35,"综合设计Ⅱ");
			u8g2_DrawUTF8(&u8g2,7,55,"防火防盗监测器");		
			break;
		case NUM_NAME:
			//显示学号姓名
			u8g2_DrawUTF8(&u8g2,0,15,"成员1：王子豪");	
			u8g2_DrawUTF8(&u8g2,55,30,"21041428");
			u8g2_DrawUTF8(&u8g2,0,45,"成员2：杨佳翰");	
			u8g2_DrawUTF8(&u8g2,55,60,"21041432");
			break;
		case PEO_PIC:
			//显示组内成员大头照                                                                                                                                                                                                            
			u8g2_DrawXBMP(&u8g2,2,0,60,60,wzhbmp); //成员wzh
			u8g2_DrawLine(&u8g2,63, 0, 63, 63);    //画分割线
			u8g2_DrawXBMP(&u8g2,65,0,60,60,yjhbmp);//成员yjh
			break;	
	}
	u8g2_SendBuffer(&u8g2);
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      UI主菜单界面显示
//  @param      null              
//  @return     void
//  Sample usage:UI_menu();
//-------------------------------------------------------------------------------------------------------------------
void UI_menu(void)
{
		u8g2_SetFont(&u8g2,u8g2_font_wqy12_t_gb2312b); //设置字体
		u8g2_ClearBuffer(&u8g2);
		int list_len = sizeof(list)/sizeof(SETTING_LIST);
		
		for(int i = 0;i < list_len ; i++)
		{
			u8g2_DrawUTF8(&u8g2,2,(i+1)*16-4,list[i].str);//显示子菜单名称		
		}
		
		
	  frame_y_trg=(UI_Select-1)*16;
		frame_y = PID(frame_y_trg, frame_y, &Dynamic_menu);
		
		u8g2_SetDrawColor(&u8g2,2);
		u8g2_DrawBox(&u8g2,0,frame_y,item_wide,16);//显示选择框	
		u8g2_SetDrawColor(&u8g2,1);
		
		u8g2_DrawLine(&u8g2,0, 0, 0, 63);  //画边框左竖线
		u8g2_DrawLine(&u8g2,50, 0, 50, 63);//画边框右竖线	
			
		UI_page();
		u8g2_SendBuffer(&u8g2);
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      UI子页面显示
//  @param      null              
//  @return     void
//  Sample usage:UI_page();
//-------------------------------------------------------------------------------------------------------------------
void UI_page(void)
{
	u8g2_SetFont(&u8g2,u8g2_font_wqy12_t_gb2312b); //设置字体
	
	//绘制具体子页面
	switch(UI_Select){
		case GUI_MONI:{UI_MONI();break;}
		case GUI_PARA:{UI_PARA();break;}
		case GUI_CURV:{UI_CURV();break;}						
	}
	//绘制滑动页面框
	uint8_t frame_len =(128-53)/list[UI_Select-1].page;
	if(PAGE_Select==list[UI_Select-1].page-1)frame_x_trg=127-frame_len;
	else frame_x_trg=52+frame_len*PAGE_Select;
	//切换显示
	if(mpu_gap == 1000)u8g2_DrawRFrame(&u8g2,frame_x,60,frame_len,4,1);
	else u8g2_DrawFrame(&u8g2,frame_x,60,frame_len,4);
	UI_run(&frame_x,&frame_x_trg,10,5);		
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      UI实时监测界面显示
//  @param      null              
//  @return     void
//  Sample usage:UI_MONI();
//-------------------------------------------------------------------------------------------------------------------
void UI_MONI(void)
{
	switch(PAGE_Select){
		case TEMP_SHOCK://绘制温度和震动页面
			u8g2_DrawUTF8(&u8g2,52,13,"当前温度:");	
			u8g2_DrawStr(&u8g2, 90, 28, sTemp);
			u8g2_DrawUTF8(&u8g2,115,28,"℃");
			u8g2_DrawUTF8(&u8g2,52,43,"震动报警:");
			if(Shock_state)u8g2_DrawUTF8(&u8g2, 90, 58, "有");
			else u8g2_DrawUTF8(&u8g2, 90, 58, "无");
			break;
		case THREE_ACC://绘制三轴加速度页面
			u8g2_DrawUTF8(&u8g2,60,10,"AX:");
			u8g2_DrawStr(&u8g2, 80, 10, Ax);
			if(ax*65/32768 > 0)u8g2_DrawBox(&u8g2,60,13,ax*65/32768,6);
			else if(ax*65/32768 < 0)u8g2_DrawFrame(&u8g2,60,13,-ax*65/32768,6);			

			u8g2_DrawUTF8(&u8g2,60,30,"AY:");
			u8g2_DrawStr(&u8g2, 80, 30, Ay);	
			if(ay*65/32768 > 0)u8g2_DrawBox(&u8g2,60,33,ay*65/32768,6);
			else if(ay*65/32768 < 0)u8g2_DrawFrame(&u8g2,60,33,-ay*65/32768,6);		

			u8g2_DrawUTF8(&u8g2,60,50,"AZ:");
			u8g2_DrawStr(&u8g2, 80, 50, Az);
			if(az*65/32768 > 0)u8g2_DrawBox(&u8g2,60,53,az*65/32768,6);
			else if(az*65/32768 < 0)u8g2_DrawFrame(&u8g2,60,53,-az*65/32768,6);

			break;
		case THREE_ANG://绘制三轴角速度页面
			u8g2_DrawUTF8(&u8g2,60,10,"GX:");
			u8g2_DrawStr(&u8g2, 80, 10, Gx);
			if(gx*65/32768 > 0)u8g2_DrawBox(&u8g2,60,13,gx*65/32768,6);
			else if(gx*65/32768 < 0)u8g2_DrawFrame(&u8g2,60,13,-gx*65/32768,6);	

			u8g2_DrawUTF8(&u8g2,60,30,"GY:");
			u8g2_DrawStr(&u8g2, 80, 30, Gy);
			if(gy*65/32768 > 0)u8g2_DrawBox(&u8g2,60,33,gy*65/32768,6);
			else if(gy*65/32768 < 0)u8g2_DrawFrame(&u8g2,60,33,-gy*65/32768,6);		

			u8g2_DrawUTF8(&u8g2,60,50,"GZ:");
			u8g2_DrawStr(&u8g2, 80, 50, Gz);
			if(gz*65/32768 > 0)u8g2_DrawBox(&u8g2,60,53,gz*65/32768,6);
			else if(gz*65/32768 < 0)u8g2_DrawFrame(&u8g2,60,53,-gz*65/32768,6);

			break;
	}
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      UI数据曲线界面显示
//  @param      null              
//  @return     void
//  Sample usage:UI_CURV();
//-------------------------------------------------------------------------------------------------------------------
void UI_CURV(void)
{
	uint8_t sw = 128 - 53;//曲线x范围
	uint8_t sh = 64 - 14 - 14;//曲线y范围
	uint8_t ox = 53;//坐标x原点
	uint8_t oy = 12 + sh;//坐标y原点
	
	switch(PAGE_Select){
		case TEMP_CURV://绘制温度曲线页面
		{
			u8g2_DrawUTF8(&u8g2,52,12,"温度:");
			u8g2_DrawStr(&u8g2, 85, 12, sTemp);
			u8g2_DrawUTF8(&u8g2,110,12,"℃");
			float Temp_Max = 35;//绘制上限
			float Temp_Min = 25;//绘制下限
			float dh = sh / (Temp_Max - Temp_Min);
			//温度阈值Temp_stand
			for (uint8_t i = 0; i < MAX_DATALEN ; i += 6)
				u8g2_DrawHLine(&u8g2, ox + i, oy - (Temp_stand - Temp_Min) * dh , 3);//绘制温度阈值线
			for (uint8_t i = 0; i < cTemp && i < MAX_DATALEN ; i++)
				u8g2_DrawPixel(&u8g2 , ox + i, oy - (vTemp[i] - Temp_Min) * dh);//绘制温度点
			break;
		}		
		case PITCH_CURV://绘制俯仰角曲线页面
		{
			u8g2_DrawUTF8(&u8g2,52,12,"俯仰角:");	
			u8g2_DrawStr(&u8g2, 93, 12, FAX);
			float Pitch_Max = 90;//绘制上限
			float Pitch_Min = -90;//绘制下限
			float dh = sh / (Pitch_Max - Pitch_Min);	
			u8g2_DrawHLine(&u8g2, ox, oy + Pitch_Min * dh , sw);//绘制0°线
			for (uint8_t i = 0; i < cPitch && i < MAX_DATALEN ; i++)
				u8g2_DrawPixel(&u8g2 , ox + i, oy - (vPitch[i] - Pitch_Min) * dh);//绘制俯仰角点
			break;
		}
		case ROLL_CURV://绘制横滚角曲线页面
		{
			u8g2_DrawUTF8(&u8g2,52,12,"横滚角:");
			u8g2_DrawStr(&u8g2, 93, 12, FAY);
			float Roll_Max = 90;//绘制上限
			float Roll_Min = -90;//绘制下限
			float dh = sh / (Roll_Max - Roll_Min);
			u8g2_DrawHLine(&u8g2, ox, oy + Roll_Min * dh , sw);//绘制0°线
			for (uint8_t i = 0; i < cRoll && i < MAX_DATALEN ; i++)
				u8g2_DrawPixel(&u8g2 , ox + i, oy - (vRoll[i] - Roll_Min) * dh);//绘制横滚角点
			break;
		}		
		case YAW_CURV://绘制航向角曲线页面
		{
			u8g2_DrawUTF8(&u8g2,52,12,"航向角:");
			u8g2_DrawStr(&u8g2, 93, 12, FAZ);
			float Yaw_Max = 180;//绘制上限
			float Yaw_Min = -180;//绘制下限
			float dh = sh / (Yaw_Max - Yaw_Min);
			u8g2_DrawHLine(&u8g2, ox, oy + Yaw_Min * dh , sw);//绘制0°线
			for (uint8_t i = 0; i < cYaw && i < MAX_DATALEN ; i++)
				u8g2_DrawPixel(&u8g2 , ox + i, oy - (vYaw[i] - Yaw_Min) * dh);//绘制航向角点
			break;
		}
		case THREE_DIM://绘制3D立体图页面
		{
			u8g2_DrawUTF8(&u8g2,52,12,"3D立体图");
			ox = (52 + 128) /2;
			oy = (14 + 58) /2;
			RateCube(fAY, -fAX, fAZ,ox,oy);
			break;
		}

	}
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      UI参数设置界面显示
//  @param      null              
//  @return     void
//  Sample usage:UI_PARA();
//-------------------------------------------------------------------------------------------------------------------
void UI_PARA(void)
{
	for(int i = 0;i < 4 ; i++)
	{
		u8g2_DrawUTF8(&u8g2,52,(i+1)*15-2,Para[i].str);//显示可设置参数名称		
	}
	//显示温度上限值
	char strTemp[5];	//定义温度上限值数组
	sprintf(strTemp, "%d", Temp_stand);
	u8g2_DrawStr(&u8g2, 104, 13, strTemp);
	u8g2_DrawUTF8(&u8g2,117,13,"℃");
	//显示震动灵敏度
	char strSHOCK[5];	//定义震动灵敏度数组
	sprintf(strSHOCK, "%d", Shock_sens);
	u8g2_DrawStr(&u8g2, 118, 28, strSHOCK);
	//显示报警时长
	char strALARM[5];	//定义报警时长数组
	sprintf(strALARM, "%d", Alarm_time);
	u8g2_DrawStr(&u8g2, 108, 43, strALARM);
	u8g2_DrawUTF8(&u8g2,122,43,"S");
	//显示上传间隔
	char strUPLOAD[6];	//定义上传间隔数组
	if(Upload_inter<10)sprintf(strUPLOAD, "%.1f", Upload_inter);
	else sprintf(strUPLOAD, "%.0f", Upload_inter);	
	u8g2_DrawStr(&u8g2, 104, 58, strUPLOAD);
	u8g2_DrawUTF8(&u8g2,122,58,"S");
	//丝滑选择框显示
	
	para_y_trg=PARA_Select*15+2;
	para_y = PID(para_y_trg, para_y, &PARA_Line);
	para_x_trg=128-Para[PARA_Select].len;
	para_x = PID(para_x_trg, para_x, &PARA_Wide);
			
	u8g2_SetDrawColor(&u8g2,2);
	u8g2_DrawRBox(&u8g2,para_x,para_y,Para[PARA_Select].len,14,0);//显示选择框	
	u8g2_SetDrawColor(&u8g2,1);

}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      UI按键事件
//  @param      null              
//  @return     void
//  Sample usage:UI_key();
//-------------------------------------------------------------------------------------------------------------------
void UI_key(void)
{
	uint8_t key = ScanKey();//扫描按键键码
	switch(key){
		case KEY_UP://子菜单向上选择
			PAGE_Select=0;//刷新子页码
			if(UI_Select == GUI_MONI){UI_Select=GUI_PARA;break;}
			else if(UI_Select == GUI_LOGO){break;}
			else {UI_Select--;break;}
		case KEY_DOWN://子菜单向下选择
			PAGE_Select=0;//刷新子页码
			if(UI_Select == GUI_PARA){UI_Select=GUI_MONI;break;}
			else if(UI_Select == GUI_LOGO){break;}
			else {UI_Select++;break;}
		case KEY_ADD:
			if(UI_Select != GUI_LOGO&& UI_Select != GUI_PARA)//非启动界面和参数设置界面左滑
			{
				if(PAGE_Select<=0){PAGE_Select=list[UI_Select-1].page-1;}
				else{PAGE_Select--;}				
			}
			else if(UI_Select == GUI_PARA)//参数设置界面:参数自加
			{
				switch(PARA_Select){
					case PARA_TEMP:
						if(Temp_stand==90)Temp_stand=0;
						else Temp_stand++;
						break;
					case PARA_SHOC:
						if(Shock_sens==9)Shock_sens=0;
						else Shock_sens++;
						break;
					case PARA_ALAR:
						if(Alarm_time==60)Alarm_time=0;
						else Alarm_time++;
						break;
					case PARA_UPLO:
						if(Upload_inter>=10)Upload_inter=0;
						else Upload_inter += 0.1;
						break;
				}
			}
		  	break;
		case KEY_SUB:
			if(UI_Select != GUI_LOGO && UI_Select != GUI_PARA)//非启动界面和参数设置界面右滑
			{
				if(PAGE_Select>=list[UI_Select-1].page-1){PAGE_Select=0;}
				else{PAGE_Select++;}
			}
			else if(UI_Select == GUI_PARA)//参数设置界面:参数自加
			{
				switch(PARA_Select){
					case PARA_TEMP:
						if(Temp_stand==0)Temp_stand=90;
						else Temp_stand--;
						break;
					case PARA_SHOC:
						if(Shock_sens==0)Shock_sens=9;
						else Shock_sens--;
						break;
					case PARA_ALAR:
						if(Alarm_time==0)Alarm_time=60;
						else Alarm_time--;
						break;
					case PARA_UPLO:
						if(Upload_inter<0.1)Upload_inter=10;
						else Upload_inter -= 0.1;
						break;
				}
			}
			break;
		case KEY_ENTER:
			if(UI_Select == GUI_LOGO)//启动界面进入主菜单界面
			{
				//logo界面常规初始化预备
				UI_Select=GUI_MONI;
				LOGO_Select = LOGO_TITLE;
				leds_sta=0;
				logo_tick=0;
				led_tick=0;
				start_beep=1;		
			}
			else if(UI_Select == GUI_CURV)//切换陀螺仪读取速度50 1000
			{
				if(mpu_gap == 0)mpu_gap = 1000;
				else mpu_gap = 0;
			}
			else if(UI_Select == GUI_PARA)
			{
				if(PARA_Select == PARA_UPLO)PARA_Select = PARA_TEMP;
				else PARA_Select++;
			}
			break;
		case KEY_LEAVE:
			if(UI_Select != GUI_LOGO)
			{
				UI_Select=GUI_LOGO;
				PAGE_Select=0;//刷新子页码
				break;
			}//主菜单界面返回启动界面
	}
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      子页面滑动框线性移动
//  @param      *a 			 当前值
//  @param      *a_trg   目标值
//  @param      step     步长
//  @param      slow_cnt 最小距离   
//  @return     void
//  Sample usage:UI_run(&frame_x,&frame_x_trg,10,5);
//-------------------------------------------------------------------------------------------------------------------
void UI_run(short *a,short *a_trg,uint8_t step,uint8_t slow_cnt)
{
	uint8_t temp;
	
	if(*a < *a_trg)
	{
		temp = (*a_trg - *a) > slow_cnt ? step:1;
		*a += temp;
	}
	else if(*a > *a_trg)
	{
		temp = (*a - *a_trg) > slow_cnt ? step:1;
		*a -= temp;
	}
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      选择框动态丝滑PID
//  @param      Targrt 目标值
//  @param      Now    当前值
//  @param      *Obj   指针对象
//  @return     int
//  Sample usage:PID(frame_y_trg, frame_y, &Dynamic_menu);
//-------------------------------------------------------------------------------------------------------------------
int PID(int Targrt, int Now, Pid_Error *Obj)
{
    int x = Now;
    float Kp = (float)(Obj->kp)/1000.00, Ki = (float)(Obj->ki)/1000.00, Kd = (float)(Obj->kd)/1000.00;
    Obj->error = Targrt - x;
    Obj->sum_srror += Obj->error;
    float delta_error = Obj->error - Obj->last_error;
    float velocity = Kp * Obj->error + Ki * Obj->sum_srror + Kd * delta_error;
    x += velocity;
    Obj->last_error = Obj->error;
    return x;
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      画点函数
//  @param      x
//  @param      y
//  @return     void
//  Sample usage:GUI_Point(1,2);
//-------------------------------------------------------------------------------------------------------------------
void GUI_Point(int16_t x,int16_t y)
{
	u8g2_DrawPixel(&u8g2,x,y);
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      画线函数
//  @param      x1 x2
//  @param      y1 y2
//  @return     void
//  Sample usage:GUI_Line(1,2,3,4);
//-------------------------------------------------------------------------------------------------------------------
void GUI_Line(int16_t x1,int16_t y1,int16_t x2,int16_t y2)
{
	u8g2_DrawLine(&u8g2,x1,y1,x2,y2);
}