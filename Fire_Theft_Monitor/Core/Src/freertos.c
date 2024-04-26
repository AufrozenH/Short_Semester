/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "UI.h"
#include <stdio.h>
//#include "MPU6050.h"
#include "ESP01.h"
#include "Device.h"
#include "stdlib.h"
#include "string.h"
#include "w25qxx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for MainTask */
osThreadId_t MainTaskHandle;
const osThreadAttr_t MainTask_attributes = {
  .name = "MainTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for KeyTask */
osThreadId_t KeyTaskHandle;
const osThreadAttr_t KeyTask_attributes = {
  .name = "KeyTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for UartTask */
osThreadId_t UartTaskHandle;
const osThreadAttr_t UartTask_attributes = {
  .name = "UartTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for GUITask */
osThreadId_t GUITaskHandle;
const osThreadAttr_t GUITask_attributes = {
  .name = "GUITask",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for DataTask */
osThreadId_t DataTaskHandle;
const osThreadAttr_t DataTask_attributes = {
  .name = "DataTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal1,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartMainTask(void *argument);
void StartKeyTask(void *argument);
void StartUartTask(void *argument);
void StartGUITask(void *argument);
void StartDataTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of MainTask */
  MainTaskHandle = osThreadNew(StartMainTask, NULL, &MainTask_attributes);

  /* creation of KeyTask */
  KeyTaskHandle = osThreadNew(StartKeyTask, NULL, &KeyTask_attributes);

  /* creation of UartTask */
  UartTaskHandle = osThreadNew(StartUartTask, NULL, &UartTask_attributes);

  /* creation of GUITask */
  GUITaskHandle = osThreadNew(StartGUITask, NULL, &GUITask_attributes);

  /* creation of DataTask */
  DataTaskHandle = osThreadNew(StartDataTask, NULL, &DataTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartMainTask */
/**
  * @brief  Function implementing the MainTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartMainTask */
void StartMainTask(void *argument)
{
  /* USER CODE BEGIN StartMainTask */
  //等待mpu6050初始�?
  printf("in mainTask!");
  while(!mpuok){osDelay(1);}
  /* Infinite loop */
  for(;;)
  {
    //系统状态机控制
		SYS_state();
    osDelay(1);
  }
  /* USER CODE END StartMainTask */
}

/* USER CODE BEGIN Header_StartKeyTask */
/**
* @brief Function implementing the KeyTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartKeyTask */
void StartKeyTask(void *argument)
{
  /* USER CODE BEGIN StartKeyTask */
  //等待mpu6050初始化
  printf("in keyTask");
	while(!mpuok){osDelay(1);}
  /* Infinite loop */
  for(;;)
  {
    //通过if else间接锁定了按键，只要报警了，按键被锁定
		if(Temp_state || Shock_state)Ulock();
		else UI_key();
    osDelay(1);
  }
  /* USER CODE END StartKeyTask */
}

/* USER CODE BEGIN Header_StartUartTask */
/**
* @brief Function implementing the UartTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUartTask */
void StartUartTask(void *argument)
{
  /* USER CODE BEGIN StartUartTask */
  /* Infinite loop */
  for(;;)
  {
    if (EspRxDataOk())
    {
      if(g_esp01.rxdata.rx_len>0){
        char* pstr=(char *)(g_esp01.rxdata.rx_buf);
        if(strstr(pstr,"UPON")==pstr){
          g_bupting=1;
        }else if(strstr(pstr,"UPOFF")==pstr){
          g_bupting=0;
        }else if(strstr(pstr,"GETPARA")==pstr){
          char tmp_upstr[50];
          sprintf(tmp_upstr,"param:%6d,%6d,%6d,%6.1f\n",sys_set.Temp_stand,sys_set.Shock_sens,sys_set.Alarm_time,sys_set.Upload_inter);
          esp01_send_cnt+=strlen(tmp_upstr);
			    SendEspStr(tmp_upstr);
        }else if(strstr(pstr,"ParamSet:")==pstr){
          PARA_SYS tmp_sys_set;
          char* p=pstr+9;
          tmp_sys_set.Temp_stand=atoi(p);
          p=strstr(p,",");
          if(p>pstr){
            tmp_sys_set.Shock_sens=atoi(p+1);
            p=strstr(p+1,",");
            if(p>pstr){
              tmp_sys_set.Alarm_time=atoi(p+1);
              p=strstr(p+1,",");
              if(p>pstr){
                tmp_sys_set.Upload_inter=(float)atoi(p+1)/1000.0;
                if((tmp_sys_set.Temp_stand>=0 && tmp_sys_set.Temp_stand<=90) &&
                  (tmp_sys_set.Shock_sens>=0 && tmp_sys_set.Shock_sens<=9) &&
                  (tmp_sys_set.Alarm_time>=0 && tmp_sys_set.Alarm_time<=60) &&
                  (tmp_sys_set.Upload_inter>=0.1 && tmp_sys_set.Upload_inter<=10)){
                    memcpy(&sys_set,&tmp_sys_set,sizeof(sys_set));
                    printf("Save: %d℃, %d, %d秒, %.1f毫秒",sys_set.Temp_stand,sys_set.Shock_sens,sys_set.Alarm_time,sys_set.Upload_inter);
					          W25QXX_Write((uint8_t *)&sys_set,0,sizeof(sys_set));//写入参数
                  }
              }
            }
          }
        }
      }
    }
    osDelay(1);
  }
  /* USER CODE END StartUartTask */
}

/* USER CODE BEGIN Header_StartGUITask */
/**
* @brief Function implementing the GUITask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartGUITask */
void StartGUITask(void *argument)
{
  /* USER CODE BEGIN StartGUITask */
	UI_Init();
  //等待mpu6050初始�?
	while(!mpuok){osDelay(1);}
  /* Infinite loop */
  for(;;)
  {
		UI_Show();
    osDelay(1);
  }
  /* USER CODE END StartGUITask */
}

/* USER CODE BEGIN Header_StartDataTask */
/**
* @brief Function implementing the DataTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDataTask */
void StartDataTask(void *argument)
{
  /* USER CODE BEGIN StartDataTask */
	SYS_Init();
  /* Infinite loop */
  for(;;)
  {
		Information_Update();
		osDelay(5);
  }
  /* USER CODE END StartDataTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

