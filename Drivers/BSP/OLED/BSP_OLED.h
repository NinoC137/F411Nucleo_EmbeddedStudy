/************************************************************************
* 模块名称：OLED模块
* 文件名  ：BSP_OLED.h
* 说明    ：头文件，提供自定义数据类型及外部调用的接口函数
* 版 本 号：V1.0
* 修改记录：
*		    版本号       日期        作者                 
*		    V1.0     2020-12-14     qiqiang        
***********************************************************************/

#ifndef __BSP_OLED_H
#define __BSP_OLED_H

/***************************************************************
                        本模块包含的头文件
***************************************************************/

#include "stm32f4xx_hal.h"


/*********************************************************************
                     本模块提供给外部调用的函数
**********************************************************************/

void BSP_OLED_Init(void);                    // 初始化
void BSP_OLED_CLS(void);                     // 清屏  
void BSP_OLED_Refresh(void);                 // 更新显示

// 显示单个字符
void BSP_OLED_ShowChar(uint8_t x, uint8_t y, uint8_t ch);
// 显示字符串
void BSP_OLED_ShowString(uint8_t x, uint8_t y, char *pStr); 
// 显示单个汉字
void BSP_OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t index); 
// 显示图片
void BSP_OLED_ShowBMP(uint8_t x0,uint8_t y0,uint8_t x1,uint8_t y1,const uint8_t BMP[]);


/*********************************************************************
                     本模块提供给内部调用的函数
**********************************************************************/

static void SSD1306_WriteCmd(uint8_t command);      // SSD1306命令写入函数
static void SSD1306_WriteDat(uint8_t* dat, size_t size);          // SSD1306数据写入函数


#endif
