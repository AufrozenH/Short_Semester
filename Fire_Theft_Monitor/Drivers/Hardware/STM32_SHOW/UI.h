/*
 * @Author: wangzihao 2281036574@qq.com	21041428
 * @Date: 2024-03-17 
 */
#ifndef __UI_H
#define __UI_H

#include "main.h"
#include "Device.h"

#define KEY_GPIO_Port GPIOE											//按键总脚

typedef enum
{
	GUI_LOGO = 0,//启动logo界面
	GUI_MONI,		 //实时监测界面
	GUI_CURV,    //数据曲线界面
	GUI_COMM,    //无线通信界面
	GUI_PARA     //参数设置界面
}GUI_STATE;    //用枚举类型表示UI界面状态

typedef enum
{
	LOGO_TITLE = 0,//logo名称界面
	NUM_NAME,		 //成员学号界面
	PEO_PIC,     //大头照界面
}LOGO_PAGE;    //用枚举类型表示LOGO界面状态

typedef enum
{
	TEMP_SHOCK = 0,//温度和震动页面
	THREE_ACC,	   //三轴加速度页面
	THREE_ANG 		 //三轴角速度页面
}MONI_PAGE;			 //用枚举类型表示实时监测页面

typedef enum
{
	TEMP_CURV = 0,//温度曲线页面
	PITCH_CURV,		//俯仰角曲线页面
	ROLL_CURV,		//横滚角曲线页面
	YAW_CURV,		  //航向角曲线页面
	THREE_DIM 		//3D立体图页面
}CURV_PAGE;     //用枚举类型表示数据曲线页面

typedef enum
{
	SERV_CONNE = 0,//服务连接页面
	DATA_INF       //数据信息页面
}COMM_PAGE;      //用枚举类型表示无线通信页面

typedef enum
{
	PARA_TEMP = 0,//温度上限值
	PARA_SHOC,	  //震动灵敏度
	PARA_ALAR,	  //报警时长
	PARA_UPLO 	  //上传间隔
}PARA_SELECT;	  //用枚举类型表示参数选择

typedef enum
{
  KEY_UP = 1,
  KEY_DOWN,
  KEY_ADD,
  KEY_SUB,
  KEY_ENTER,
  KEY_LEAVE
} KEY_TYPE;//用枚举类型表示按键状态

typedef struct pid_error
{
    int kp;
    int ki;
    int kd;
    float error;
    float sum_srror;
    float last_error;
}Pid_Error;

typedef struct
{
	char*str;
	uint8_t len;
	uint8_t page;
}SETTING_LIST;

extern char UI_Select;//初始选择启动界面
extern float Temp;//实际温度值
extern char sTemp[5];//记录实际温度数组
extern char Ax[10];//记录实际陀螺仪ax数组
extern char Ay[10];//记录实际陀螺仪ay数组
extern char Az[10];//记录实际陀螺仪az数组
extern char Gx[10];//记录实际陀螺仪gx数组
extern char Gy[10];//记录实际陀螺仪gy数组
extern char Gz[10];//记录实际陀螺仪gz数组
extern uint8_t Temp_stand;//温度上限值0~90℃
extern uint8_t Shock_sens;//震动灵敏度0~9级
extern uint8_t Alarm_time;//报警时长0~60S
extern float Upload_inter;//上传间隔100ms~10S
extern uint8_t Temp_state;//温度报警标志位 1：温度超过阈值
extern uint8_t Shock_state;//震动报警标志位 1：加速度超过阈值
extern uint8_t mpuok;//mpu6050初始化状态
extern uint8_t num[4];
extern float vTemp[MAX_DATALEN];//温度数据缓存
extern int cTemp;//温度数据缓存计数
extern float vPitch[MAX_DATALEN];//俯仰角数据缓存
extern int cPitch;//俯仰角数据缓存计数
extern float vRoll[MAX_DATALEN];//横滚角数据缓存
extern int cRoll;//横滚角数据缓存计数
extern float vYaw[MAX_DATALEN];//航向角数据缓存
extern int cYaw;//航向角数据缓存计数
extern char FAX[10];//记录实际陀螺仪俯仰角数组
extern char FAY[10];//记录实际陀螺仪横滚角数组
extern char FAZ[10];//记录实际陀螺仪航向角数组
extern int mpu_gap;//读取陀螺仪数值间隔时间（默认 50 可切换为1000）

void SYS_Init(void);
void UI_Init(void);
void SYS_state(void);
void UI_Show(void);
void LOGO_state(void);
void UI_logo(void);
void UI_menu(void);
void UI_page(void);
void UI_MONI(void);
void UI_CURV(void);
void UI_PARA(void);
void UI_key(void);
void UI_run(short *a,short *a_trg,uint8_t step,uint8_t slow_cnt);
int PID(int Targrt, int Now, Pid_Error *Obj);
void GUI_Point(int16_t x,int16_t y);
void GUI_Line(int16_t x1,int16_t y1,int16_t x2,int16_t y2);

#endif