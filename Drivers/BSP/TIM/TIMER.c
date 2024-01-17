#include "TIMER.h"

Counters Tim11Counter = {.counters = 0,
												 .LEDUpdate = 0,
												 .OLEDRefresh = 0,
												 .timestamp_ms = 0};

uint8_t LEDUpdateFlag;
uint8_t OLEDUpdateFlag;

//Timer Counter Interrupt
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  UNUSED(htim);
	//timer-handler should be defined by user. e.x.: &htim11
	if(htim == &htim11){
		Tim11Counter.LEDUpdate++;
		Tim11Counter.OLEDRefresh++;
		
		if(Tim11Counter.LEDUpdate == LEDUPDATE_MS){
			Tim11Counter.LEDUpdate = 0;
			LEDUpdateFlag = 1;
		}
		
		if(Tim11Counter.OLEDRefresh == OLEDREFRESH_MS){
			Tim11Counter.OLEDRefresh = 0;
			OLEDUpdateFlag = 1;
		}
		
		Tim11Counter.timestamp_ms++;
		if(Tim11Counter.timestamp_ms == 1000 * 60){
			Tim11Counter.timestamp_ms = 0;
			Tim11Counter.timestamp_min ++;
		}
	}
}