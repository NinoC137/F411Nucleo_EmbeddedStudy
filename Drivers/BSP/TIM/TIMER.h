/************************************************************************
* 模块名称：OLED模块
* 文件名  ：BSP_OLED.h
* 说明    ：头文件，提供自定义数据类型及外部调用的接口函数
* 版 本 号：V1.0
* 修改记录：
*		    版本号       日期        作者                 
*		    V1.0     2024-1-17				Nino      
***********************************************************************/

#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f4xx_hal.h"
#include "main.h"

#define LEDUPDATE_MS 100
#define OLEDREFRESH_MS 5

typedef struct Counters{
	uint16_t counters;
	
	uint8_t LEDUpdate;
	uint8_t TemperatureUpdate;
	
	uint8_t keyPressTimer;
	
	uint32_t timestamp_ms;
	uint32_t timestamp_min;
} Counters;

extern Counters Tim11Counter;
extern uint8_t LEDUpdateFlag;
extern uint8_t OLEDUpdateFlag;

#endif