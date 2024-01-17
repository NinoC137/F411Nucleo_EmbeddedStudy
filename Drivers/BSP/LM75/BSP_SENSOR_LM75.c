/***********************************************************
* 模块名称：LM75温度传感器驱动模块
* 文件名  ：BSP_SENSOR_LM75.c
* 说明    ：提供接口函数的源码
* 版 本 号：V2.0
* 修改记录：
*           版本号       日期         作者        
*	           V1.0     2021-01-24    qiqiang        
*************************************************************/


/***************************************************************
                     本模块包含的头文件
***************************************************************/

#include "stm32f4xx_hal.h"
#include "BSP_SENSOR_LM75.h"
#include "i2c.h"                // 包含IIC1的句柄定义

/***************************************************************
                        本模块的宏定义
***************************************************************/

/*******************  1. 数据类型宏定义  **********************/

// 传感器结构体模板定义
typedef struct 
{
   float value;          // 传感器测量值   
   float upper_range;    // 传感器阈值上限
   float lower_range;    // 传感器阈值下限
}T_SENSOR;

/*******************  2. 硬件宏定义  **********************/

// 使用硬件I2C，I2C1，PB8--SCL  PB9--SDA

#define LM75_ADDR         0x90  // 器件地址
#define LM75_REG_TEMP     0x00  // 温度寄存器: 地址值0x00
#define LM75_REG_CONF     0x01  // 配置寄存器: 地址值0x01
#define LM75_REG_THYST    0x02  // 滞后寄存器: 地址值0x02
#define LM75_REG_TOS      0x03  // 阈值寄存器: 地址值0x03 

/*******************  3. 滤波算法宏定义  **********************/

#define N   11

/***************************************************************
                         本模块的变量定义
***************************************************************/

static T_SENSOR Temperature;  // 定义用于温度测量的传感器结构体变量

/***************************************************************
                         本模块的接口函数
***************************************************************/

/***************************************************************
 * @name 	     BSP_GetSensorValue
 * @brief 	   Get sensor data
 * @param[in]  None
 * @return 	   sensor data after filter processing
 * @note       
***************************************************************/
float BSP_GetSensorValue(void)
{
	return(Temperature.value);
}

/***************************************************************
 * @name 	     BSP_UpdateSensorValue
 * @brief 	   Update sensor data.
 * @param[in]  None
 * @return 	   sensor data after filter processing
 * @note       
***************************************************************/

void BSP_UpdateSensorValue(void)
{
	float filterValue;
	filterValue = 0;
	filterValue = Sensor_Filter();
	Temperature.value = filterValue;
}

/***************************************************************
 * @name 	     BSP_SetSensorRange
 * @brief 	   Set sensor range.
 * @param[in]  
 * @return 	   
 * @note       
***************************************************************/

void BSP_SetSensorRange(float upRange,float lowRange)
{
	Temperature.upper_range = upRange;
	Temperature.lower_range = lowRange;
}

/***************************************************************
 * @name 	     BSP_GetSensor_UpRange
 * @brief 	   Get sensor upper range.
 * @param[in]  
 * @return 	   Sensor upper range
 * @note       
***************************************************************/

float BSP_GetSensor_UpRange(void)
{
	return(Temperature.upper_range);
}

/***************************************************************
 * @name 	     BSP_GetSensor_LowRange
 * @brief 	   Get sensor lower range.
 * @param[in]  
 * @return 	   Sensor lower range
 * @note       
***************************************************************/
float BSP_GetSensor_LowRange(void)
{
	return(Temperature.lower_range);
}

/***************************************************************
                         本模块的内部函数
***************************************************************/

/***************************************************************
 * @name 	     Sensor_Filter
 * @brief 	   Use soft filter algorithm to process sensor data .
 * @param[in]  none
 * @return 	   sensor data after filter processing
 * @note       采用中位值滤波法，冒泡排序
***************************************************************/
static float Sensor_Filter(void)
{
	float rawValueBuf[N];     // 传感器原始数据缓冲区
	uint8_t i,j;              // 循环变量
	float temp;               // 暂存变量
	
	for (i = 0; i < N; i++)   // 读取传感器数据，存入缓冲区
	{
    rawValueBuf[i] = Get_Sensor_Raw_Value();
    HAL_Delay(10);
	}
	// 冒泡法排序，寻找中位值
  for(j = 0; j < (N-1); j++)
	{
    for(i = 0; i< (N-1-j); i++)
    {
      if (rawValueBuf[i] > rawValueBuf[i+1])
      {
        temp             = rawValueBuf[i];
        rawValueBuf[i]   = rawValueBuf[i+1];
        rawValueBuf[i+1] = temp;
      }
    }
  }
	return (rawValueBuf[(N-1)/2]);
}
    
/***************************************************************
 * @name 	     Get_Sensor_Raw_Value
 * @brief 	   Get senso's raw value
 * @param[in]  none
 * @return 	   rawValue: sensor's raw value 
 * @note 
   Temp register
   -------------------------------------------------------------
   MSByte                         |LSByte
   7   6   5   4   3   2   1   0  |7   6  5   4   3   2    1   0
   D10 D9 D8  D7  D6  D5  D4  D3  |D2  D1 D0  X   X   X    X   X
   -------------------------------------------------------------
   D10=0 温度为正, D10=1 温度为负
***************************************************************/
static float Get_Sensor_Raw_Value(void)
{
	uint8_t  tempHL[2];    // 16位温度值
	uint16_t temp;         
	float rawValue;	       // 转换后的温度值

  HAL_I2C_Mem_Read(&hi2c1, LM75_ADDR, LM75_REG_TEMP, I2C_MEMADD_SIZE_8BIT, tempHL, 2, 100);
	temp = ((tempHL[0] << 8) | tempHL[1]);        // 拼接温度值
	temp = temp>>5;                               
  if ((tempHL[0] & 0x80) != 0)                  // 温度为负数
  {
		rawValue = (-(~temp + 1)) * 0.125 ; 
  }
	else                                          // 温度为正数
	{
		rawValue  = temp * 0.125  ;
	}
	return(rawValue);
}


