/************************************************************************
* ģ�����ƣ�OLEDģ��
* �ļ���  ��BSP_OLED.h
* ˵��    ��ͷ�ļ����ṩ�Զ����������ͼ��ⲿ���õĽӿں���
* �� �� �ţ�V1.0
* �޸ļ�¼��
*		    �汾��       ����        ����                 
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