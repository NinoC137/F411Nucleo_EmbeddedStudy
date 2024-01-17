#include "TIMER.h"

Counters Tim11Counter = {.counters = 0,
												 .LEDUpdate = 0,
												 .TemperatureUpdate = 0,
												 .keyPressTimer = 0,
												 .timestamp_ms = 0};

uint8_t LEDUpdateFlag;
uint8_t OLEDUpdateFlag;

//Timer Counter Interrupt
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  UNUSED(htim);
	//timer-handler should be defined by user. e.x.: &htim11
	if(htim == &htim11){
		/******************vvvvv**LED**vvvvv******************/
		Tim11Counter.LEDUpdate++;
		if(Tim11Counter.LEDUpdate == LEDUPDATE_MS){
			Tim11Counter.LEDUpdate = 0;
			LEDUpdateFlag = 1;
		}
		/*******************^^^^^^^^^^^^^^^^***************/
		
		/******************vvvvv**OLED**vvvvv**************/
		Tim11Counter.TemperatureUpdate++;
		if(Tim11Counter.TemperatureUpdate == OLEDREFRESH_MS){
			Tim11Counter.TemperatureUpdate = 0;
			OLEDUpdateFlag = 1;
		}
		/*******************^^^^^^^^^^^^^^^^***************/
		
		/*****************vvvvv**TIMER**vvvvv***************/
		Tim11Counter.timestamp_ms++;
		if(Tim11Counter.timestamp_ms == 1000 * 60){
			Tim11Counter.timestamp_ms = 0;
			Tim11Counter.timestamp_min ++;
		}
		/*******************^^^^^^^^^^^^^^^^***************/
	}
}