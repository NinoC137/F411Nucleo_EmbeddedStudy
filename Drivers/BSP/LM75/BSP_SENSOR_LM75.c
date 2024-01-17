/***********************************************************
* ģ�����ƣ�LM75�¶ȴ���������ģ��
* �ļ���  ��BSP_SENSOR_LM75.c
* ˵��    ���ṩ�ӿں�����Դ��
* �� �� �ţ�V2.0
* �޸ļ�¼��
*           �汾��       ����         ����        
*	           V1.0     2021-01-24    qiqiang        
*************************************************************/


/***************************************************************
                     ��ģ�������ͷ�ļ�
***************************************************************/

#include "stm32f4xx_hal.h"
#include "BSP_SENSOR_LM75.h"
#include "i2c.h"                // ����IIC1�ľ������

/***************************************************************
                        ��ģ��ĺ궨��
***************************************************************/

/*******************  1. �������ͺ궨��  **********************/

// �������ṹ��ģ�嶨��
typedef struct 
{
   float value;          // ����������ֵ   
   float upper_range;    // ��������ֵ����
   float lower_range;    // ��������ֵ����
}T_SENSOR;

/*******************  2. Ӳ���궨��  **********************/

// ʹ��Ӳ��I2C��I2C1��PB8--SCL  PB9--SDA

#define LM75_ADDR         0x90  // ������ַ
#define LM75_REG_TEMP     0x00  // �¶ȼĴ���: ��ֵַ0x00
#define LM75_REG_CONF     0x01  // ���üĴ���: ��ֵַ0x01
#define LM75_REG_THYST    0x02  // �ͺ�Ĵ���: ��ֵַ0x02
#define LM75_REG_TOS      0x03  // ��ֵ�Ĵ���: ��ֵַ0x03 

/*******************  3. �˲��㷨�궨��  **********************/

#define N   11

/***************************************************************
                         ��ģ��ı�������
***************************************************************/

static T_SENSOR Temperature;  // ���������¶Ȳ����Ĵ������ṹ�����

/***************************************************************
                         ��ģ��Ľӿں���
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
                         ��ģ����ڲ�����
***************************************************************/

/***************************************************************
 * @name 	     Sensor_Filter
 * @brief 	   Use soft filter algorithm to process sensor data .
 * @param[in]  none
 * @return 	   sensor data after filter processing
 * @note       ������λֵ�˲�����ð������
***************************************************************/
static float Sensor_Filter(void)
{
	float rawValueBuf[N];     // ������ԭʼ���ݻ�����
	uint8_t i,j;              // ѭ������
	float temp;               // �ݴ����
	
	for (i = 0; i < N; i++)   // ��ȡ���������ݣ����뻺����
	{
    rawValueBuf[i] = Get_Sensor_Raw_Value();
    HAL_Delay(10);
	}
	// ð�ݷ�����Ѱ����λֵ
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
   D10=0 �¶�Ϊ��, D10=1 �¶�Ϊ��
***************************************************************/
static float Get_Sensor_Raw_Value(void)
{
	uint8_t  tempHL[2];    // 16λ�¶�ֵ
	uint16_t temp;         
	float rawValue;	       // ת������¶�ֵ

  HAL_I2C_Mem_Read(&hi2c1, LM75_ADDR, LM75_REG_TEMP, I2C_MEMADD_SIZE_8BIT, tempHL, 2, 100);
	temp = ((tempHL[0] << 8) | tempHL[1]);        // ƴ���¶�ֵ
	temp = temp>>5;                               
  if ((tempHL[0] & 0x80) != 0)                  // �¶�Ϊ����
  {
		rawValue = (-(~temp + 1)) * 0.125 ; 
  }
	else                                          // �¶�Ϊ����
	{
		rawValue  = temp * 0.125  ;
	}
	return(rawValue);
}


