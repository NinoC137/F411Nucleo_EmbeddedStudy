#include "main.h"

/***************************************************************
 * @name 	     userApp_1
 * @brief 	   Get temperature and show it on OLED
 * @param[in]  None
 * @return 	   None
 * @note
***************************************************************/
void userApp_1(){
	float Temperature;
	char TemperatureString[16];
	
	if(LEDUpdateFlag != 0){
		LEDUpdateFlag = 0;
		static int ledupdate;
		ledupdate++;
		switch(ledupdate){
			case 1:
				HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
				break;
			case 2:
				HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
				break;
			case 3:
				HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
				break;
			case 4:
				HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
				break;
			case 5:
				ledupdate = 0;
				break;
			default:
				break;
		}
	}

	if(OLEDUpdateFlag != 0){
		OLEDUpdateFlag = 0; 
		
		BSP_UpdateSensorValue();
		Temperature = BSP_GetSensorValue();
		sprintf(TemperatureString, "Temp: %4.2f'c", Temperature);	 	//将变量拷贝到数组str1
		BSP_OLED_ShowString(12, 16, TemperatureString);//显示字符串
		BSP_OLED_Refresh();
	}
}

/***************************************************************
 * @name 	     userApp_2
 * @brief 	   Get ADC value and show it on OLED
							if value too high or too low will trigge BEEP
 * @param[in]  None
 * @return 	   None
 * @note
***************************************************************/
void userApp_2(){
	uint32_t adcValue;
	char strADC[16];
	HAL_ADC_Start(&hadc1);
	
	if(OLEDUpdateFlag != 0){
		OLEDUpdateFlag = 0; 
		
		BSP_UpdateSensorValue();
		adcValue = HAL_ADC_GetValue(&hadc1);
		sprintf(strADC, "Voltage: %2.3f", (float)adcValue/4095*3.3);
		BSP_OLED_ShowString(12, 16, strADC);
		BSP_OLED_Refresh();
	}
	
	if(adcValue < 300 || adcValue > 3700){
		HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);
	}else{
		HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);
	}
}

/***************************************************************
 * @name 	     userApp_3
 * @brief 	   None..for now
 * @param[in]  None
 * @return 	   None
 * @note
***************************************************************/
void userApp_3(){

}

/***************************************************************
 * @name 	     userApp_4
 * @brief 	   None..for now
 * @param[in]  None
 * @return 	   None
 * @note
***************************************************************/
void userApp_4(){

}