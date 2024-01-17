/***********************************************************
* ģ�����ƣ�OLED����ģ��
* �ļ���  ��BSP_OLED.c
* ˵��    ���ṩ�ӿں�����Դ��
* �� �� �ţ�V1.0
* �޸ļ�¼��
*           �汾��       ����         ����        
*	           V1.0     2020-12-14    qiqiang        
*************************************************************/


/***************************************************************
                     ��ģ�������ͷ�ļ�
***************************************************************/

#include "stm32f4xx_hal.h"
#include "BSP_OLED.h"
#include "Font.h"
#include "spi.h"            // ����SPI1�ľ������

/**********************************************************************
                     ��ģ��ĺ궨��
**********************************************************************/

// ����оƬSSD1306����Ƶ��128*64��3.3V�����ѹ

#define     X_WIDTH       128				// ���
#define     Y_WIDTH       64				// ����
#define		  PAGE			    8	        // ҳ����

/*****************   ʹ��IIC�ӿڵĶ���   *********************/

// ʹ��Ӳ��I2C��I2C1��PB8--SCL PB9--SDA
//#define SSD1306_ADD	0x78 // SSD1306����оƬ��I2C��ַ
//#define COM				  0x00 // ��ַ�ֽ�֮��Ŀ����ֽڣ���ʾ��������
//#define DAT 			  0x40 // ��ַ�ֽ�֮��Ŀ����ֽڣ���ʾ��������

/*****************   ʹ��SPI�ӿڵĶ���   *********************/

// ʹ��SPI1
// D0 CLK  ��PA5  (SPI1_SCK)  SPIʱ����   
// D1 DIN  ��PA7  (SPI1_MOSI) SPI���������  (PA6--SPI1_MISO������)
// CS      ��PA4  (SPI1_NSS)  SPIƬѡ�ź�
// RES     ��PB6   ��λOLED
// DC      ��PC7   ����/���ݱ�־���͵�ƽ������/ �ߵ�ƽ�����ݣ���

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
                     ��ģ���ڲ���������
***************************************************************/
// ��ʾ������������ʾ����оƬSSD1396���Դ��Ӧ
// ��Ÿ�ʽ����.
// Page0: [0,0] [1,0] ... [127,0]	
// Page1: [0,1] [1,1] ... [127,1]	
// Page2: [0,2] [1,2] ... [127,2]	
// Page3: [0,3] [1,3] ... [127,3]	
// Page4: [0,4] [1,4] ... [127,4]	
// Page5: [0,5] [1,5] ... [127,5]	
// Page6: [0,6] [1,6] ... [127,6]	
// Page7: [0,7] [1,7] ... [127,7]	
// ����Ԫ�صĵ�һ���±��൱��column�������� ��X���������
// ����Ԫ�صĵڶ����±��൱��Page�������꣬��Y���������
// ��������Ԫ���൱���Դ�ĳһҳ��һ�����ݣ�8bit
uint8_t OLED_DispBuf[X_WIDTH][Y_WIDTH/8];  // ��ʾ������[128][8]


/***************************************************************
                       ��ģ��Ľӿں���
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
	HAL_Delay(100);         // ��ʱ
	// SPI�ӿڵ�OLED������Ӳ����λ����
	OLED_RST_Clr();         
	HAL_Delay(200);         
	OLED_RST_Set();  
	
  //SSD1306��ʼ������
  SSD1306_WriteCmd(0xAE); // �ر���ʾ
	SSD1306_WriteCmd(0x00); // ��������ʼ��ַ�ĵ���λ
	SSD1306_WriteCmd(0x10); // ��������ʼ��ַ�ĸ���λ
	SSD1306_WriteCmd(0x40); // Set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	SSD1306_WriteCmd(0x81); // ���öԱȶȼĴ���
	SSD1306_WriteCmd(0xFF); // ���ȵ��� 0x00~0xFF (����Խ��Խ��)
	SSD1306_WriteCmd(0xA1); // Set SEG/Column Mapping       0xa0���ҷ��� 0xa1����
	SSD1306_WriteCmd(0xC8); // Set COM/Row Scan Direction  ��Ļ���Ͻ�Ϊԭ��0xc0���·��� 0xc8����
	SSD1306_WriteCmd(0xA6); // Set normal display
	SSD1306_WriteCmd(0xA8); // Set multiplex ratio(1 to 64)
	SSD1306_WriteCmd(0x3f); // 1/64 duty
	SSD1306_WriteCmd(0xD3); // Set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	SSD1306_WriteCmd(0x00); // Not offset
	SSD1306_WriteCmd(0xD5); // ����ʱ�ӷ�Ƶ����/��Ƶ��
	SSD1306_WriteCmd(0x80); // Set divide ratio, Set Clock as 100 Frames/Sec
	SSD1306_WriteCmd(0xD9); // Set pre-charge period
	SSD1306_WriteCmd(0xF1); // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	SSD1306_WriteCmd(0xDA); // Set com pins hardware configuration
	SSD1306_WriteCmd(0x12);
	SSD1306_WriteCmd(0xDB); // Set vcomh
	SSD1306_WriteCmd(0x40); // Set VCOM Deselect Level
	SSD1306_WriteCmd(0x20); // ���ô洢��ģʽ (0x00/0x01/0x02)
	SSD1306_WriteCmd(0x02); // ҳѰַģʽ 
	SSD1306_WriteCmd(0x8D); // Set Charge Pump enable/disable
	SSD1306_WriteCmd(0x14); // Set(0x10) disable
	SSD1306_WriteCmd(0xA4); // Disable Entire Display On (0xa4/0xa5)
	SSD1306_WriteCmd(0xA6); // Disable Inverse Display On (0xa6/a7) 
	BSP_OLED_CLS();
	SSD1306_WriteCmd(0xAF); // ������ʾ
}

/***************************************************************
 * @name 	     BSP_OLED_Refresh
 * @brief 	   ����ʾ����д�뵽OLED��GDDRAM
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
		// �洢��ѰַģʽΪҳѰַģʽ����PAGE0��ʼ
		SSD1306_WriteCmd(0xB0 + page);	     // ����ҳ��ַ��0~7��
		// ��������ʼ��ַ��SEG0��ʼ
		SSD1306_WriteCmd(0x00);		           // ��������ʼ��ַ�ĵ���λ
		SSD1306_WriteCmd(0x10);		           // ��������ʼ��ַ�ĸ���λ
		// ��ҳ�������
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
 * @note       ��ɫ����
***************************************************************/

void BSP_OLED_CLS(void)
{
	uint8_t x,y;
	for( y = 0; y < Y_WIDTH/8; y++)   // ����������
	{
	  for( x = 0; x < X_WIDTH; x++)   // ����������
		{
			OLED_DispBuf[x][y] = 0;       // ������ʾ��������������Ϊ0
		}
  }
	BSP_OLED_Refresh();               // ������ʾ
}

/***************************************************************
 * @name 	     BSP_OLED_DrawPoint
 * @brief 	   ����ʾ��������λ���ϻ���
 * @param[in]  x    : x positon��0��127
               y    : y positon��0��63
               point: Ϊ1��䣬Ϊ0���
 * @return 	   None
 * @note       �õ�����ֻ��д�����Դ棬��û������OLED������ʾ
***************************************************************/
void BSP_OLED_DrawPoint(uint8_t x,uint8_t y,uint8_t point)
{
	uint8_t page,line,dat;
	if( x >=X_WIDTH || y >= Y_WIDTH)  // �����жϣ����곬����Χ��ֱ�ӷ���
	{
		return;     
	}
	page = y / 8;       // ���������껻��Ϊҳ��ַ��0-7��
	line = y % 8;       // ���������껻��Ϊ��ҳ�ĵڼ��У�0-7��
	dat  = 1 << line;   // ���������껻��Ϊ����Ԫ�صĵڼ�λ
	if(point)           // �õ�Ϊ��ʾ״̬����λ��1
  {
	  OLED_DispBuf[x][page] = OLED_DispBuf[x][page] | dat;
	}
	else                // �õ�Ϊ�ر�״̬����λ����  
	{
		OLED_DispBuf[x][page] = OLED_DispBuf[x][page] & (~dat);
	}
}


/***************************************************************
 * @name 	     BSP_OLED_ShowChar
 * @brief 	   Display a character at specified postion
 * @param[in]  x       : x positon��0��127
               y       : y positon��0��63 
							 ch      : the character,use ''
 * @return 	   None
 * @note       ��ģ����ȡģ����λ��ǰ
               ���뵥���ַ�����''
							 Ĭ�ϵ�ASCII���СΪ8*16
****************************************************************/

void BSP_OLED_ShowChar(uint8_t x, uint8_t y, uint8_t ch)
{
	uint8_t x0,y0,i,j,temp;
	x0 = x;                        // �����ʼλ��
	y0 = y; 
	ch = ch - 32;                  // ��ȥASCII����еĲ���ʾ�ַ�
	for(i = 0; i < 16; i++)        // ��ѭ������ַ���ĸ���ֽ���
	{
		temp = F8X16[ch*16 + i];     // �ҵ����ַ����ֿ��еĵ�һ���ֽ�
		for(j = 0; j < 8; j++)       // ��ѭ�����һ���ֽڣ���1��8������
		{
			 // ��ʾһ����
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
		// ��ʾ��һ���ֽڣ�������ʾλ�ã�x+1��y����
	  x++;
		if(x == (x0 + 8))      // ����ַ��ϰ벿����ʾ���л����°벿�֣�����λ��
		{
			x = x0;
			y = y0 + 8;         
		}
	}
}	
	
/***************************************************************
 * @name 	     BSP_OLED_ShowString
 * @brief 	   Display a string at specified postion
 * @param[in]  x       : x positon��0��127
               y       : y positon��0��63 
							 pStr    : a pointer to a string
 * @return 	   None
 * @note       ��ģ����ȡģ����λ��ǰ
               �����ַ�������""
							 Ĭ�ϵ�ASCII���СΪ8*16
****************************************************************/
		
void BSP_OLED_ShowString(uint8_t x, uint8_t y, char *pStr)
{
	while((*pStr) != '\0')
	{
    // ��������
	  if( x > 120)                 // ��ǰ�е�xλ�ô���120�����л�������ҳ������8*16��   
    {  
		  x = 0;
	    y = y + 16;
	  }	
    BSP_OLED_ShowChar(x,y,*pStr); 
		x = x + 8;                    // x��������8λ������8*16��
		pStr++;                       // ָ����һ���ַ�
	}
}


/***************************************************************
 * @name 	     BSP_OLED_ShowChinese
 * @brief 	   Display a Chinese at specified postion
 * @param[in]  x       : x positon��0��127
               y       : y positon��0��63 
							 index   : �������ֿ��е�����
 * @return 	   None
 * @note       ��ģ����ȡģ����λ��ǰ
							 Ĭ�ϵĺ��ִ�СΪ16*16
****************************************************************/

void BSP_OLED_ShowChinese(uint8_t x, uint8_t y, uint8_t index)
{
  uint8_t i,j,temp,x0,y0;
  x0 = x;                   // �����ʼλ�� 
	y0 = y;
	for(i = 0; i< 32; i++)    // ��ѭ�����������ģ���ֽ���	
	{
		temp = HZ[index*32+i];  // ���Ҹú������ֿ��е���ʼλ��
		for(j = 0; j< 8; j++)   // ��ѭ�����һ���ֽڣ���1��8������
		{
			if(temp&0x01)         // ��ʾ�õ�
			{
				BSP_OLED_DrawPoint(x,y+j,1);
			}
			else
			{
				BSP_OLED_DrawPoint(x,y+j,0);
			}
			temp = temp>>1;
		}
		// ��ʾ��һ���ֽڣ�������ʾλ�ã�x+1��y����
		x++;                     
		if(x == ( x0 + 16))       // ��ɺ����ϰ벿����ʾ���л����°벿�֣�����λ��
		{
			x  = x0;
			y  = y0 + 8;
		}	              
	}		
}

/***************************************************************
 * @name 	     BSP_OLED_ShowBMP
 * @brief 	   ��ʾBMPλͼ
 * @param[in]  x,y -- ��ʼ������(x:0~127, y:0~63)
               sizex,sizey -- ͼƬ��С�������ص�Ϊ��λ����128x64
							 BMP   : ͼ������
 * @return 	   None
 * @note       ע�⣺ȡģʱ��y������8�ı�����������8λ����
****************************************************************/


// ����һ���������ֽ�����x����ɨ�裬y�Զ��仯������ѭ����ע���޸�y�ĳ�ʼֵ��

void BSP_OLED_ShowBMP(uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,const uint8_t BMP[])
{
	uint8_t temp,x0,y0;
	uint16_t i,j;             // ͼƬ�ֽ������ܴ���256�������16λ����
  uint16_t n;               // ͼƬ�����ֽ���
	x0 = x;                   // �����ʼλ��
	y0 = y;
	// ����ͼƬ���ֽ���
	n = sizex * (sizey/8) + sizex * ((sizey%8)?1:0);
	for(i = 0; i< n; i++)     // ��ѭ�����ͼƬ���ֽ���	
	{
		temp = BMP[i];           
		for(j = 0; j< 8; j++)   // ��ѭ�����һ���ֽڣ���1��8������
		{
			if(temp&0x01)         // ��ʾ�õ�
			{
				BSP_OLED_DrawPoint(x,y+j,1);
			}
			else
			{
				BSP_OLED_DrawPoint(x,y+j,0);
			}
			temp = temp>>1;
		}
		// ��ʾ��һ���ֽڣ�������ʾλ�ã�x+1��y����
		x++;                     
		if(x == ( x0 + sizex))       // ���ͼƬ��һ��8�е���ʾ���л�����һ��
		{
			x  = x0;
			y  = y0 + 8;             
			y0 = y0 + 8;               // ����ͼƬ�Ƕ���У����޸ĳ�ʼ�е�λ��
		}	              
	}
}	
	

// ����������x������Y��������ѭ��

//void BSP_OLED_ShowBMP(uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,uint8_t BMP[])
//{
//  uint16_t j = 0;
//	uint8_t i,n,temp,m,x0,y0;
//	x0 = x;           // �����������
//	y0 = y;
//	// ����Y����ı仯
//	sizey = sizey/8 + ((sizey%8)?1:0);  
//	for(n = 0;n < sizey; n++)    
//	{
//		for(i = 0;i < sizex; i++)     // һ�����ͼƬ��һҳ������8�У�
//		{
//			temp = BMP[j];
//			j++;
//			for(m = 0;m < 8; m++)       // ��ʾ������Ԫ�أ���Ӧ8��
//			{
//				if(temp&0x01)             // ��ʾ�õ�
//			  {
//				  BSP_OLED_DrawPoint(x,y,1);
//			  }
//			  else
//			  {
//				  BSP_OLED_DrawPoint(x,y,0);
//			  }
//				temp = temp>>1;
//			  y++;                      // �л�����һ��
//			}	
//			x++;                        // ���һ������Ԫ����ʾ���л�x����,yΪ��ʼֵ
//			y=y0;
//			if((x-x0)==sizex)           // ���һҳ��ʾ��8�У�������λ��
//			{
//				x  = x0;
//				y0 = y0+8;                // ��ʼ��һҳ
//			}
//    }
//	}
//}

/*********************************************************************
                     ��ģ���ṩ���ڲ����õĺ���
**********************************************************************/


/**********************************************************************
 * @name 	     SSD1306_WriteCmd
 * @brief 	   Wire command to SSD1306
 * @param[in]  I2C_Command: SSD1306 control command
 * @return 	   None
 * @note       Use I2C1��PB8--SCL PB9--SDA
**********************************************************************/

static void SSD1306_WriteCmd(uint8_t command)
{
	
	  /******   IIC�ӿڴ���  ******/
//	static uint8_t datBuf[2];
//	datBuf[0] = COM;          // �����ֽ�Ϊ��������
//	datBuf[1] = command;      // ��������
//	HAL_I2C_Master_Transmit(&hi2c1,SSD1306_ADD,datBuf,2,10);
	  
	  /******   SPI�ӿڴ���  ******/
	  OLED_DC_Clr();            // ��������
	  HAL_SPI_Transmit(&hspi1, &command, 1, 1000);
	
	
}

/**********************************************************************
 * @name 	     SSD1306_WriteDat
 * @brief 	   Wire data to SSD1306
 * @param[in]  Command: SSD1306 display data
 * @return 	   None
 * @note       Use I2C1��PB8--SCL PB9--SDA 
**********************************************************************/

static void SSD1306_WriteDat(uint8_t* dat, size_t size)
{
  
	  /******   IIC�ӿڴ���  ******/
//	static uint8_t datBuf[2];
//	datBuf[0] = DAT;          // �����ֽ�Ϊ��������
//	datBuf[1] = dat;          // ��������
//	HAL_I2C_Master_Transmit(&hi2c1,SSD1306_ADD,datBuf,2,10);
	  
	  /******   SPI�ӿڴ���  ******/
		OLED_DC_Set();            // ��������
	  HAL_SPI_Transmit(&hspi1, dat, size, 1000);

}

