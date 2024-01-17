/***********************************************************
* 模块名称：OLED驱动模块
* 文件名  ：BSP_OLED.c
* 说明    ：提供接口函数的源码
* 版 本 号：V1.0
* 修改记录：
*           版本号       日期         作者        
*	           V1.0     2020-12-14    qiqiang        
*************************************************************/


/***************************************************************
                     本模块包含的头文件
***************************************************************/

#include "stm32f4xx_hal.h"
#include "BSP_OLED.h"
#include "Font.h"
#include "spi.h"            // 包含SPI1的句柄定义

/**********************************************************************
                     本模块的宏定义
**********************************************************************/

// 驱动芯片SSD1306，分频率128*64，3.3V供电电压

#define     X_WIDTH       128				// 宽度
#define     Y_WIDTH       64				// 长度
#define		  PAGE			    8	        // 页数量

/*****************   使用IIC接口的定义   *********************/

// 使用硬件I2C，I2C1，PB8--SCL PB9--SDA
//#define SSD1306_ADD	0x78 // SSD1306驱动芯片的I2C地址
//#define COM				  0x00 // 地址字节之后的控制字节，表示发送命令
//#define DAT 			  0x40 // 地址字节之后的控制字节，表示发送数据

/*****************   使用SPI接口的定义   *********************/

// 使用SPI1
// D0 CLK  接PA5  (SPI1_SCK)  SPI时钟线   
// D1 DIN  接PA7  (SPI1_MOSI) SPI数据输出线  (PA6--SPI1_MISO不可用)
// CS      接PA4  (SPI1_NSS)  SPI片选信号
// RES     接PB6   复位OLED
// DC      接PC7   命令/数据标志（低电平—命令/ 高电平—数据）；

// D/C PIN DEFINE
#define  DC_Pin          GPIO_PIN_7
#define  DC_GPIO_Port    GPIOC
#define  OLED_DC_Set()   HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_SET);
#define  OLED_DC_Clr()   HAL_GPIO_WritePin(DC_GPIO_Port, DC_Pin, GPIO_PIN_RESET);


// RESET PIN   DEFINE
#define  RES_Pin         GPIO_PIN_6
#define  RES_GPIO_Port   GPIOB
#define  OLED_RST_Set()  HAL_GPIO_WritePin(RES_GPIO_Port, RES_Pin, GPIO_PIN_SET);
#define  OLED_RST_Clr()  HAL_GPIO_WritePin(RES_GPIO_Port, RES_Pin, GPIO_PIN_RESET);


/***************************************************************
                     本模块内部变量定义
***************************************************************/
// 显示缓冲区，与显示驱动芯片SSD1396的显存对应
// 存放格式如下.
// Page0: [0,0] [1,0] ... [127,0]	
// Page1: [0,1] [1,1] ... [127,1]	
// Page2: [0,2] [1,2] ... [127,2]	
// Page3: [0,3] [1,3] ... [127,3]	
// Page4: [0,4] [1,4] ... [127,4]	
// Page5: [0,5] [1,5] ... [127,5]	
// Page6: [0,6] [1,6] ... [127,6]	
// Page7: [0,7] [1,7] ... [127,7]	
// 数组元素的第一个下标相当于column，列坐标 即X方向的坐标
// 数组元素的第二个下标相当于Page，行坐标，即Y方向的坐标
// 单个数组元素相当于显存某一页的一列数据，8bit
uint8_t OLED_DispBuf[X_WIDTH][Y_WIDTH/8];  // 显示缓冲区[128][8]


/***************************************************************
                       本模块的接口函数
***************************************************************/

/***************************************************************
 * @name 	     BSP_OLED_Init
 * @brief 	   Initialize OLED module
 * @param[in]  None
 * @return 	   None
 * @note
***************************************************************/

void BSP_OLED_Init(void)
{
	HAL_Delay(100);         // 延时
	// SPI接口的OLED增加了硬件复位操作
	OLED_RST_Clr();         
	HAL_Delay(200);         
	OLED_RST_Set();  
	
  //SSD1306初始化序列
  SSD1306_WriteCmd(0xAE); // 关闭显示
	SSD1306_WriteCmd(0x00); // 设置列起始地址的低四位
	SSD1306_WriteCmd(0x10); // 设置列起始地址的高四位
	SSD1306_WriteCmd(0x40); // Set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	SSD1306_WriteCmd(0x81); // 设置对比度寄存器
	SSD1306_WriteCmd(0xFF); // 亮度调节 0x00~0xFF (数字越大越亮)
	SSD1306_WriteCmd(0xA1); // Set SEG/Column Mapping       0xa0左右反置 0xa1正常
	SSD1306_WriteCmd(0xC8); // Set COM/Row Scan Direction  屏幕左上角为原点0xc0上下反置 0xc8正常
	SSD1306_WriteCmd(0xA6); // Set normal display
	SSD1306_WriteCmd(0xA8); // Set multiplex ratio(1 to 64)
	SSD1306_WriteCmd(0x3f); // 1/64 duty
	SSD1306_WriteCmd(0xD3); // Set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	SSD1306_WriteCmd(0x00); // Not offset
	SSD1306_WriteCmd(0xD5); // 设置时钟分频因子/震荡频率
	SSD1306_WriteCmd(0x80); // Set divide ratio, Set Clock as 100 Frames/Sec
	SSD1306_WriteCmd(0xD9); // Set pre-charge period
	SSD1306_WriteCmd(0xF1); // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	SSD1306_WriteCmd(0xDA); // Set com pins hardware configuration
	SSD1306_WriteCmd(0x12);
	SSD1306_WriteCmd(0xDB); // Set vcomh
	SSD1306_WriteCmd(0x40); // Set VCOM Deselect Level
	SSD1306_WriteCmd(0x20); // 设置存储器模式 (0x00/0x01/0x02)
	SSD1306_WriteCmd(0x02); // 页寻址模式 
	SSD1306_WriteCmd(0x8D); // Set Charge Pump enable/disable
	SSD1306_WriteCmd(0x14); // Set(0x10) disable
	SSD1306_WriteCmd(0xA4); // Disable Entire Display On (0xa4/0xa5)
	SSD1306_WriteCmd(0xA6); // Disable Inverse Display On (0xa6/a7) 
	BSP_OLED_CLS();
	SSD1306_WriteCmd(0xAF); // 开启显示
}

/***************************************************************
 * @name 	     BSP_OLED_Refresh
 * @brief 	   将显示数据写入到OLED的GDDRAM
 * @param[in]  None
 * @return 	   None
 * @note
***************************************************************/

void BSP_OLED_Refresh(void)
{
	uint8_t page,column;
	uint8_t SSD1306_DRAMBuf[128];

	for( page = 0; page < 8; page++)
	{
		// 存储器寻址模式为页寻址模式，从PAGE0开始
		SSD1306_WriteCmd(0xB0 + page);	     // 设置页地址（0~7）
		// 设置列起始地址从SEG0开始
		SSD1306_WriteCmd(0x00);		           // 设置列起始地址的低四位
		SSD1306_WriteCmd(0x10);		           // 设置列起始地址的高四位
		// 按页填充数据
		for(int i = 0; i< X_WIDTH; i++){
			SSD1306_DRAMBuf[i] = OLED_DispBuf[i][page];
		}
		
		SSD1306_WriteDat(SSD1306_DRAMBuf, X_WIDTH);
	}
}

/***************************************************************
 * @name 	     BSP_OLED_CLS
 * @brief 	   Clear screen
 * @param[in]  None
 * @return 	   None
 * @note       黑色背景
***************************************************************/

void BSP_OLED_CLS(void)
{
	uint8_t x,y;
	for( y = 0; y < Y_WIDTH/8; y++)   // 历遍所有行
	{
	  for( x = 0; x < X_WIDTH; x++)   // 历遍所有列
		{
			OLED_DispBuf[x][y] = 0;       // 设置显示缓冲区所有数据为0
		}
  }
	BSP_OLED_Refresh();               // 更新显示
}

/***************************************************************
 * @name 	     BSP_OLED_DrawPoint
 * @brief 	   在显示屏的任意位置上画点
 * @param[in]  x    : x positon，0～127
               y    : y positon，0～63
               point: 为1填充，为0清除
 * @return 	   None
 * @note       该点内容只是写入了显存，并没有送入OLED进行显示
***************************************************************/
void BSP_OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t point)
{
	uint8_t page,line,dat;
	if( x >=X_WIDTH || y >= Y_WIDTH)  // 参数判断：坐标超出范围，直接返回
	{
		return;     
	}
	page = y / 8;       // 根据行坐标换算为页地址（0-7）
	line = y % 8;       // 根据行坐标换算为该页的第几行（0-7）
	dat  = 1 << line;   // 根据行坐标换算为数组元素的第几位
	if(point)           // 该点为显示状态，该位置1
  {
	  OLED_DispBuf[x][page] = OLED_DispBuf[x][page] | dat;
	}
	else                // 该点为关闭状态，该位清零  
	{
		OLED_DispBuf[x][page] = OLED_DispBuf[x][page] & (~dat);
	}
}


/***************************************************************
 * @name 	     BSP_OLED_ShowChar
 * @brief 	   Display a character at specified postion
 * @param[in]  x       : x positon，0～127
               y       : y positon，0～63 
							 ch      : the character,use ''
 * @return 	   None
 * @note       字模按列取模，低位在前
               输入单个字符，用''
							 默认的ASCII码大小为8*16
****************************************************************/

void BSP_OLED_ShowChar(uint8_t x, uint8_t y, uint8_t ch)
{
	uint8_t x0,y0,i,j,temp;
	x0 = x;                        // 保存初始位置
	y0 = y; 
	ch = ch - 32;                  // 减去ASCII码表中的不显示字符
	for(i = 0; i < 16; i++)        // 外循环输出字符字母的字节数
	{
		temp = F8X16[ch*16 + i];     // 找到该字符在字库中的第一个字节
		for(j = 0; j < 8; j++)       // 内循环输出一个字节，即1列8行数据
		{
			 // 显示一个点
			 if(temp&0x01)
			 {
				 BSP_OLED_DrawPoint(x,y+j,1);
			 }
       else
			 {
				 BSP_OLED_DrawPoint(x,y+j,0);
       }
       temp = temp>>1;		 
		}
		// 显示下一个字节，调整显示位置：x+1；y不变
	  x++;
		if(x == (x0 + 8))      // 完成字符上半部分显示，切换到下半部分，调整位置
		{
			x = x0;
			y = y0 + 8;         
		}
	}
}	
	
/***************************************************************
 * @name 	     BSP_OLED_ShowString
 * @brief 	   Display a string at specified postion
 * @param[in]  x       : x positon，0～127
               y       : y positon，0～63 
							 pStr    : a pointer to a string
 * @return 	   None
 * @note       字模按列取模，低位在前
               输入字符串，用""
							 默认的ASCII码大小为8*16
****************************************************************/
		
void BSP_OLED_ShowString(uint8_t x, uint8_t y, char *pStr)
{
	while((*pStr) != '\0')
	{
    // 参数修正
	  if( x > 120)                 // 当前行的x位置大于120，则切换的下两页（字体8*16）   
    {  
		  x = 0;
	    y = y + 16;
	  }	
    BSP_OLED_ShowChar(x,y,*pStr); 
		x = x + 8;                    // x方向右移8位（字体8*16）
		pStr++;                       // 指向下一个字符
	}
}


/***************************************************************
 * @name 	     BSP_OLED_ShowChinese
 * @brief 	   Display a Chinese at specified postion
 * @param[in]  x       : x positon，0～127
               y       : y positon，0～63 
							 index   : 汉字在字库中的索引
 * @return 	   None
 * @note       字模按列取模，低位在前
							 默认的汉字大小为16*16
****************************************************************/

void BSP_OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t index)
{
  uint8_t i,j,temp,x0,y0;
  x0 = x;                   // 保存初始位置 
	y0 = y;
	for(i = 0; i< 32; i++)    // 外循环输出汉字字模的字节数	
	{
		temp = HZ[index*32+i];  // 查找该汉字在字库中的起始位置
		for(j = 0; j< 8; j++)   // 内循环输出一个字节，即1列8行数据
		{
			if(temp&0x01)         // 显示该点
			{
				BSP_OLED_DrawPoint(x,y+j,1);
			}
			else
			{
				BSP_OLED_DrawPoint(x,y+j,0);
			}
			temp = temp>>1;
		}
		// 显示下一个字节，调整显示位置：x+1；y不变
		x++;                     
		if(x == ( x0 + 16))       // 完成汉字上半部分显示，切换到下半部分，调整位置
		{
			x  = x0;
			y  = y0 + 8;
		}	              
	}		
}

/***************************************************************
 * @name 	     BSP_OLED_ShowBMP
 * @brief 	   显示BMP位图
 * @param[in]  x,y -- 起始点坐标(x:0~127, y:0~63)
               sizex,sizey -- 图片大小，以像素点为单位，如128x64
							 BMP   : 图像数据
 * @return 	   None
 * @note       注意：取模时，y方向不是8的倍数，将按照8位补足
****************************************************************/


// 方法一：计算总字节数，x方向扫描，y自动变化，两重循环，注意修改y的初始值。

void BSP_OLED_ShowBMP(uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,const uint8_t BMP[])
{
	uint8_t temp,x0,y0;
	uint16_t i,j;             // 图片字节数可能大于256，因此用16位定义
  uint16_t n;               // 图片的总字节数
	x0 = x;                   // 保存初始位置
	y0 = y;
	// 计算图片总字节数
	n = sizex * (sizey/8) + sizex * ((sizey%8)?1:0);
	for(i = 0; i< n; i++)     // 外循环输出图片的字节数	
	{
		temp = BMP[i];           
		for(j = 0; j< 8; j++)   // 内循环输出一个字节，即1列8行数据
		{
			if(temp&0x01)         // 显示该点
			{
				BSP_OLED_DrawPoint(x,y+j,1);
			}
			else
			{
				BSP_OLED_DrawPoint(x,y+j,0);
			}
			temp = temp>>1;
		}
		// 显示下一个字节，调整显示位置：x+1；y不变
		x++;                     
		if(x == ( x0 + sizex))       // 完成图片中一行8列的显示，切换到下一行
		{
			x  = x0;
			y  = y0 + 8;             
			y0 = y0 + 8;               // 由于图片是多个行，需修改初始行的位置
		}	              
	}
}	
	

// 方法二：先x方向，在Y方向，三重循环

//void BSP_OLED_ShowBMP(uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,uint8_t BMP[])
//{
//  uint16_t j = 0;
//	uint8_t i,n,temp,m,x0,y0;
//	x0 = x;           // 保留起点坐标
//	y0 = y;
//	// 计算Y方向的变化
//	sizey = sizey/8 + ((sizey%8)?1:0);  
//	for(n = 0;n < sizey; n++)    
//	{
//		for(i = 0;i < sizex; i++)     // 一次填充图片的一页（包含8列）
//		{
//			temp = BMP[j];
//			j++;
//			for(m = 0;m < 8; m++)       // 显示数组中元素，对应8列
//			{
//				if(temp&0x01)             // 显示该点
//			  {
//				  BSP_OLED_DrawPoint(x,y,1);
//			  }
//			  else
//			  {
//				  BSP_OLED_DrawPoint(x,y,0);
//			  }
//				temp = temp>>1;
//			  y++;                      // 切换到下一行
//			}	
//			x++;                        // 完成一个数组元素显示后，切换x方向,y为初始值
//			y=y0;
//			if((x-x0)==sizex)           // 完成一页显示（8行），调整位置
//			{
//				x  = x0;
//				y0 = y0+8;                // 开始下一页
//			}
//    }
//	}
//}

/*********************************************************************
                     本模块提供给内部调用的函数
**********************************************************************/


/**********************************************************************
 * @name 	     SSD1306_WriteCmd
 * @brief 	   Wire command to SSD1306
 * @param[in]  I2C_Command: SSD1306 control command
 * @return 	   None
 * @note       Use I2C1，PB8--SCL PB9--SDA
**********************************************************************/

static void SSD1306_WriteCmd(uint8_t command)
{
	
	  /******   IIC接口代码  ******/
//	static uint8_t datBuf[2];
//	datBuf[0] = COM;          // 控制字节为传送命令
//	datBuf[1] = command;      // 发送命令
//	HAL_I2C_Master_Transmit(&hi2c1,SSD1306_ADD,datBuf,2,10);
	  
	  /******   SPI接口代码  ******/
	  OLED_DC_Clr();            // 发送命令
	  HAL_SPI_Transmit(&hspi1, &command, 1, 1000);
	
	
}

/**********************************************************************
 * @name 	     SSD1306_WriteDat
 * @brief 	   Wire data to SSD1306
 * @param[in]  Command: SSD1306 display data
 * @return 	   None
 * @note       Use I2C1，PB8--SCL PB9--SDA 
**********************************************************************/

static void SSD1306_WriteDat(uint8_t* dat, size_t size)
{
  
	  /******   IIC接口代码  ******/
//	static uint8_t datBuf[2];
//	datBuf[0] = DAT;          // 控制字节为传送数据
//	datBuf[1] = dat;          // 发送数据
//	HAL_I2C_Master_Transmit(&hi2c1,SSD1306_ADD,datBuf,2,10);
	  
	  /******   SPI接口代码  ******/
		OLED_DC_Set();            // 发送数据
	  HAL_SPI_Transmit(&hspi1, dat, size, 1000);

}

