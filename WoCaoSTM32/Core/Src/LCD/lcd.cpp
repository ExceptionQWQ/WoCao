//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//����Ӳ������Ƭ��STM32F407ZGT6,����ԭ��Explorer STM32F4������,��Ƶ168MHZ������12MHZ
//QDtech-TFTҺ������ for STM32 IOģ��
//xiao��@ShenZhen QDtech co.,LTD
//��˾��վ:www.qdtft.com
//�Ա���վ��http://qdtech.taobao.com
//wiki������վ��http://www.lcdwiki.com
//��˾�ṩ����֧�֣��κμ������⻶ӭ��ʱ����ѧϰ
//�̻�(����) :+86 0755-23594567 
//�ֻ�:15989313508���빤�� 
//����:lcdwiki01@gmail.com    support@lcdwiki.com    goodtft@163.com 
//����֧��QQ:3002773612  3002778157
//��������QQȺ:324828016
//��������:2018/08/22
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������ȫ�����Ӽ������޹�˾ 2018-2028
//All rights reserved
/****************************************************************************************************
//=========================================��Դ����================================================//
//     LCDģ��                STM32��Ƭ��
//      VCC          ��        DC5V/3.3V      //��Դ
//      GND          ��          GND          //��Դ��
//=======================================Һ���������߽���==========================================//
//��ģ��Ĭ��������������ΪSPI����
//     LCDģ��                STM32��Ƭ��    
//       SDA         ��          PB5          //Һ����SPI��������д�ź�
//=======================================Һ���������߽���==========================================//
//     LCDģ�� 					      STM32��Ƭ�� 
//       CLK         ��          PB3          //Һ����SPI����ʱ���ź�
//       RS          ��          PB14         //Һ��������/��������ź�
//       RST         ��          PB12         //Һ������λ�����ź�
//       CS          ��          PB15         //Һ����Ƭѡ�����ź�
//=========================================������������=========================================//
//���ģ�鲻���������ܻ��ߴ��д������ܣ����ǲ���Ҫ�������ܣ�����Ҫ���д���������
//	   LCDģ��                STM32��Ƭ�� 
//      T_IRQ        ��          PB1          //�����������ж��ź�
//      T_DO         ��          PB2          //������SPI���߶��ź�
//      T_DIN        ��          PF11         //������SPI����д�ź�
//      T_CS         ��          PC5          //������Ƭѡ�����ź�
//      T_CLK        ��          PB0          //������SPI����ʱ���ź�
**************************************************************************************************/		
 /* @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**************************************************************************************************/	
#include "LCD/lcd.h"
#include "stdlib.h"



	   
//����LCD��Ҫ����
//Ĭ��Ϊ����
_lcd_dev lcddev;

//������ɫ,������ɫ
u16 POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;  
u16 DeviceCode;	 

/*****************************************************************************
 * @name       :void LCD_WR_REG(u8 data)
 * @date       :2018-08-09 
 * @function   :Write an 8-bit command to the LCD screen
 * @parameters :data:Command value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_REG(u8 data)
{ 
   LCD_CS_CLR;     
	 LCD_RS_CLR;
    HAL_SPI_Transmit(&hspi1, &data, 1, 10);
   LCD_CS_SET;	
}

/*****************************************************************************
 * @name       :void LCD_WR_DATA(u8 data)
 * @date       :2018-08-09 
 * @function   :Write an 8-bit data to the LCD screen
 * @parameters :data:data value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_DATA(u8 data)
{
   LCD_CS_CLR;
	 LCD_RS_SET;
    HAL_SPI_Transmit(&hspi1, &data, 1, 10);
   LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
 * @date       :2018-08-09 
 * @function   :Write data into registers
 * @parameters :LCD_Reg:Register address
                LCD_RegValue:Data to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	Lcd_WriteData_16Bit(LCD_RegValue);	    		 
}	   

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @date       :2018-08-09 
 * @function   :Write GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 
void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}	 

/*****************************************************************************
 * @name       :void Lcd_WriteData_16Bit(u16 Data)
 * @date       :2018-08-09 
 * @function   :Write an 16-bit command to the LCD screen
 * @parameters :Data:Data to be written
 * @retvalue   :None
******************************************************************************/	 
void Lcd_WriteData_16Bit(u16 Data)
{	
   LCD_CS_CLR;
   LCD_RS_SET;
   uint8_t data[2] = {(u8)((Data >> 8) & 0xff), (u8)(Data & 0xff)};
    HAL_SPI_Transmit(&hspi1, data, 2, 10);
   LCD_CS_SET;
}

/*****************************************************************************
 * @name       :void LCD_DrawPoint(u16 x,u16 y)
 * @date       :2018-08-09 
 * @function   :Write a pixel data at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/	
void LCD_DrawPoint(u16 x,u16 y)
{
	LCD_SetCursor(x,y);//���ù��λ�� 
	Lcd_WriteData_16Bit(POINT_COLOR); 
}

/*****************************************************************************
 * @name       :void LCD_Clear(u16 Color)
 * @date       :2018-08-09 
 * @function   :Full screen filled LCD screen
 * @parameters :color:Filled color
 * @retvalue   :None
******************************************************************************/	
void LCD_Clear(u16 Color)
{
  unsigned int i,m;  
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);   
	LCD_CS_CLR;
	LCD_RS_SET;
	for(i=0;i<lcddev.height;i++)
	{
    for(m=0;m<lcddev.width;m++)
    {	
			Lcd_WriteData_16Bit(Color);
		}
	}
	 LCD_CS_SET;
} 

/*****************************************************************************
 * @name       :void LCD_GPIOInit(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen GPIO
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Reset LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCD_RESET(void)
{
	LCD_RST_CLR;
    HAL_Delay(100);
	LCD_RST_SET;
    HAL_Delay(50);
}

/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Initialization LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 	 
void LCD_Init(void)
{
 	LCD_RESET(); //LCD ��λ
//************* ILI9225��ʼ��**********//	
	LCD_WriteReg(0x01,0x011c);
	LCD_WriteReg(0x02,0x0100);
	LCD_WriteReg(0x03,0x1038);
	LCD_WriteReg(0x08,0x0808); // set BP and FP
	LCD_WriteReg(0x0B,0x1100); //frame cycle
	LCD_WriteReg(0x0C,0x0000); // RGB interface setting R0Ch=0x0110 for RGB 18Bit and R0Ch=0111for RGB16Bit
	LCD_WriteReg(0x0F,0x1401); // Set frame rate----0801
	LCD_WriteReg(0x15,0x0000); //set system interface
	LCD_WriteReg(0x20,0x0000); // Set GRAM Address
	LCD_WriteReg(0x21,0x0000); // Set GRAM Address
	//*************Power On sequence ****************//
    HAL_Delay(50);
	LCD_WriteReg(0x10,0x0800); // Set SAP,DSTB,STB----0A00
	LCD_WriteReg(0x11,0x1F3F); // Set APON,PON,AON,VCI1EN,VC----1038
    HAL_Delay(50);
	LCD_WriteReg(0x12,0x0121); // Internal reference voltage= Vci;----1121
	LCD_WriteReg(0x13,0x006F); // Set GVDD----0066
	LCD_WriteReg(0x14,0x4349); // Set VCOMH/VCOML voltage----5F60
	//-------------- Set GRAM area -----------------//
	LCD_WriteReg(0x30,0x0000);
	LCD_WriteReg(0x31,0x00DB);
	LCD_WriteReg(0x32,0x0000);
	LCD_WriteReg(0x33,0x0000);
	LCD_WriteReg(0x34,0x00DB);
	LCD_WriteReg(0x35,0x0000);
	LCD_WriteReg(0x36,0x00AF);
	LCD_WriteReg(0x37,0x0000);
	LCD_WriteReg(0x38,0x00DB);
	LCD_WriteReg(0x39,0x0000);
	// ----------- Adjust the Gamma Curve ----------//
	LCD_WriteReg(0x50,0x0001);  //0400
	LCD_WriteReg(0x51,0x200B);  //060B
	LCD_WriteReg(0x52,0x0000);  //0C0A
	LCD_WriteReg(0x53,0x0404);  //0105
	LCD_WriteReg(0x54,0x0C0C);  //0A0C
	LCD_WriteReg(0x55,0x000C);  //0B06
	LCD_WriteReg(0x56,0x0101);  //0004
	LCD_WriteReg(0x57,0x0400);  //0501
	LCD_WriteReg(0x58,0x1108);  //0E00
	LCD_WriteReg(0x59,0x050C);  //000E
    HAL_Delay(50);
	LCD_WriteReg(0x07,0x1017);
	LCD_WR_REG(0x22);

  LCD_direction(3);//����LCD��ʾ����
//	LCD_LED=1;//��������	 
	LCD_Clear(WHITE);//��ȫ����ɫ
}
 
/*****************************************************************************
 * @name       :void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
 * @date       :2018-08-09 
 * @function   :Setting LCD display window
 * @parameters :xStar:the bebinning x coordinate of the LCD display window
								yStar:the bebinning y coordinate of the LCD display window
								xEnd:the endning x coordinate of the LCD display window
								yEnd:the endning y coordinate of the LCD display window
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{	
	u16 x,y,tmp;
	switch(lcddev.dir)
	{
		case 0:
			x = xStar;
			y = yStar;
			break;
		case 1:
			tmp = yStar;
			yStar = xStar;
			xStar = LCD_W-1-yEnd;
			yEnd = xEnd;
			xEnd = LCD_W-1-tmp;
			x = xEnd;
			y = yStar;
			break;
		case 2:
			tmp = xStar;
			xStar = LCD_W-1-xEnd;
			xEnd = LCD_W-1-tmp;
			tmp = yStar;
			yStar = LCD_H-1-yEnd;
			yEnd = LCD_H-1-tmp;
			x = xEnd;
			y = yEnd;
			break;
		case 3:
			tmp = xStar;
			xStar = yStar;
			yStar = LCD_H-1-xEnd;
			xEnd = yEnd;
			yEnd = LCD_H-1-tmp;
			x = xStar;
			y = yEnd;
			break;
		default:
			break;
	}
	LCD_WriteReg(0x36, xEnd);
	LCD_WriteReg(0x37, xStar);
	LCD_WriteReg(0x38, yEnd);
	LCD_WriteReg(0x39, yStar);
	LCD_WriteReg(lcddev.setxcmd, x);
	LCD_WriteReg(lcddev.setycmd, y);
	LCD_WriteRAM_Prepare();	//��ʼд��GRAM			
}   

/*****************************************************************************
 * @name       :void LCD_SetCursor(u16 Xpos, u16 Ypos)
 * @date       :2018-08-09 
 * @function   :Set coordinate value
 * @parameters :Xpos:the  x coordinate of the pixel
								Ypos:the  y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{	  	    			
	LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);	
} 

/*****************************************************************************
 * @name       :void LCD_direction(u8 direction)
 * @date       :2018-08-09 
 * @function   :Setting the display direction of LCD screen
 * @parameters :direction:0-0 degree
                          1-90 degree
													2-180 degree
													3-270 degree
 * @retvalue   :None
******************************************************************************/ 
void LCD_direction(u8 direction)
{ 
			lcddev.dir = direction&3;
			lcddev.setxcmd=0x20;
			lcddev.setycmd=0x21;
			lcddev.wramcmd=0x22;
	switch(lcddev.dir){		  
		case 0:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;		
			LCD_WriteReg(0x03,0x1030);
		break;
		case 1:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x03,0x1028);
		break;
		case 2:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;	
			LCD_WriteReg(0x03,0x1000);
		break;
		case 3:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x03,0x1018);
		break;	
		default:break;
	}		
}

void LCD_Draw_Number_Top(uint16_t x, uint16_t y, uint8_t size)
{
    uint16_t width = 5 * size, length = 30 * size;
    for (uint16_t i = x; i < x + length; ++i) {
        for (uint16_t j = y; j < y + width; ++j) {
            LCD_DrawPoint(i, j);
        }
    }
}

void LCD_Draw_Number_Top_Left(uint16_t x, uint16_t y, uint8_t size)
{
    uint16_t width = 5 * size, length = 30 * size;
    for (uint16_t i = x; i < x + width; ++i) {
        for (uint16_t j = y; j < y + length; ++j) {
            LCD_DrawPoint(i, j);
        }
    }
}
void LCD_Draw_Number_Top_Right(uint16_t x, uint16_t y, uint8_t size)
{
    uint16_t width = 5 * size, length = 30 * size;
    for (uint16_t i = x + length - width; i < x + length; ++i) {
        for (uint16_t j = y; j < y + length; ++j) {
            LCD_DrawPoint(i, j);
        }
    }
}
void LCD_Draw_Number_Middle(uint16_t x, uint16_t y, uint8_t size)
{
    uint16_t width = 5 * size, length = 30 * size;
    for (uint16_t i = x; i < x + length; ++i) {
        for (uint16_t j = y + length; j < y + length + width; ++j) {
            LCD_DrawPoint(i, j);
        }
    }
}
void LCD_Draw_Number_Bottom_Left(uint16_t x, uint16_t y, uint8_t size)
{
    uint16_t width = 5 * size, length = 30 * size;
    for (uint16_t i = x; i < x + width; ++i) {
        for (uint16_t j = y + length ; j < y + 2 * length + width; ++j) {
            LCD_DrawPoint(i, j);
        }
    }
}
void LCD_Draw_Number_Bottom_Right(uint16_t x, uint16_t y, uint8_t size)
{
    uint16_t width = 5 * size, length = 30 * size;
    for (uint16_t i = x + length - width; i < x + length; ++i) {
        for (uint16_t j = y + length ; j < y + 2 * length + width; ++j) {
            LCD_DrawPoint(i, j);
        }
    }
}
void LCD_Draw_Number_Bottom(uint16_t x, uint16_t y, uint8_t size)
{
    uint16_t width = 5 * size, length = 30 * size;
    for (uint16_t i = x; i < x + length; ++i) {
        for (uint16_t j = y + 2 * length; j < y + 2 * length + width; ++j) {
            LCD_DrawPoint(i, j);
        }
    }
}
void LCD_Draw_Number_0(uint16_t x, uint16_t y, uint8_t size)
{
    LCD_Draw_Number_Top(x, y, size);
    LCD_Draw_Number_Top_Left(x, y, size);
    LCD_Draw_Number_Top_Right(x, y, size);
//    LCD_Draw_Number_Middle(x, y, size);
    LCD_Draw_Number_Bottom_Left(x, y, size);
    LCD_Draw_Number_Bottom_Right(x, y, size);
    LCD_Draw_Number_Bottom(x, y, size);
}
void LCD_Draw_Number_1(uint16_t x, uint16_t y, uint8_t size)
{
//    LCD_Draw_Number_Top(x, y, size);
//    LCD_Draw_Number_Top_Left(x, y, size);
    LCD_Draw_Number_Top_Right(x, y, size);
//    LCD_Draw_Number_Middle(x, y, size);
//    LCD_Draw_Number_Bottom_Left(x, y, size);
    LCD_Draw_Number_Bottom_Right(x, y, size);
//    LCD_Draw_Number_Bottom(x, y, size);
}
void LCD_Draw_Number_2(uint16_t x, uint16_t y, uint8_t size)
{
    LCD_Draw_Number_Top(x, y, size);
//    LCD_Draw_Number_Top_Left(x, y, size);
    LCD_Draw_Number_Top_Right(x, y, size);
    LCD_Draw_Number_Middle(x, y, size);
    LCD_Draw_Number_Bottom_Left(x, y, size);
//    LCD_Draw_Number_Bottom_Right(x, y, size);
    LCD_Draw_Number_Bottom(x, y, size);
}
void LCD_Draw_Number_3(uint16_t x, uint16_t y, uint8_t size)
{
    LCD_Draw_Number_Top(x, y, size);
//    LCD_Draw_Number_Top_Left(x, y, size);
    LCD_Draw_Number_Top_Right(x, y, size);
    LCD_Draw_Number_Middle(x, y, size);
//    LCD_Draw_Number_Bottom_Left(x, y, size);
    LCD_Draw_Number_Bottom_Right(x, y, size);
    LCD_Draw_Number_Bottom(x, y, size);
}
void LCD_Draw_Number_4(uint16_t x, uint16_t y, uint8_t size)
{
//    LCD_Draw_Number_Top(x, y, size);
    LCD_Draw_Number_Top_Left(x, y, size);
    LCD_Draw_Number_Top_Right(x, y, size);
    LCD_Draw_Number_Middle(x, y, size);
//    LCD_Draw_Number_Bottom_Left(x, y, size);
    LCD_Draw_Number_Bottom_Right(x, y, size);
//    LCD_Draw_Number_Bottom(x, y, size);
}
void LCD_Draw_Number_5(uint16_t x, uint16_t y, uint8_t size)
{
    LCD_Draw_Number_Top(x, y, size);
    LCD_Draw_Number_Top_Left(x, y, size);
//    LCD_Draw_Number_Top_Right(x, y, size);
    LCD_Draw_Number_Middle(x, y, size);
//    LCD_Draw_Number_Bottom_Left(x, y, size);
    LCD_Draw_Number_Bottom_Right(x, y, size);
    LCD_Draw_Number_Bottom(x, y, size);
}
void LCD_Draw_Number_6(uint16_t x, uint16_t y, uint8_t size)
{
    LCD_Draw_Number_Top(x, y, size);
    LCD_Draw_Number_Top_Left(x, y, size);
//    LCD_Draw_Number_Top_Right(x, y, size);
    LCD_Draw_Number_Middle(x, y, size);
    LCD_Draw_Number_Bottom_Left(x, y, size);
    LCD_Draw_Number_Bottom_Right(x, y, size);
    LCD_Draw_Number_Bottom(x, y, size);
}
void LCD_Draw_Number_7(uint16_t x, uint16_t y, uint8_t size)
{
    LCD_Draw_Number_Top(x, y, size);
//    LCD_Draw_Number_Top_Left(x, y, size);
    LCD_Draw_Number_Top_Right(x, y, size);
//    LCD_Draw_Number_Middle(x, y, size);
//    LCD_Draw_Number_Bottom_Left(x, y, size);
    LCD_Draw_Number_Bottom_Right(x, y, size);
//    LCD_Draw_Number_Bottom(x, y, size);
}
void LCD_Draw_Number_8(uint16_t x, uint16_t y, uint8_t size)
{
    LCD_Draw_Number_Top(x, y, size);
    LCD_Draw_Number_Top_Left(x, y, size);
    LCD_Draw_Number_Top_Right(x, y, size);
    LCD_Draw_Number_Middle(x, y, size);
    LCD_Draw_Number_Bottom_Left(x, y, size);
    LCD_Draw_Number_Bottom_Right(x, y, size);
    LCD_Draw_Number_Bottom(x, y, size);
}
void LCD_Draw_Number_9(uint16_t x, uint16_t y, uint8_t size)
{
    LCD_Draw_Number_Top(x, y, size);
    LCD_Draw_Number_Top_Left(x, y, size);
    LCD_Draw_Number_Top_Right(x, y, size);
    LCD_Draw_Number_Middle(x, y, size);
//    LCD_Draw_Number_Bottom_Left(x, y, size);
    LCD_Draw_Number_Bottom_Right(x, y, size);
    LCD_Draw_Number_Bottom(x, y, size);
}

void LCD_Draw_Number(uint16_t x, uint16_t y, uint8_t size, uint8_t number)
{
    switch (number) {
        case 0:
            LCD_Draw_Number_0(x, y, size);
            break;
        case 1:
            LCD_Draw_Number_1(x, y, size);
            break;
        case 2:
            LCD_Draw_Number_2(x, y, size);
            break;
        case 3:
            LCD_Draw_Number_3(x, y, size);
            break;
        case 4:
            LCD_Draw_Number_4(x, y, size);
            break;
        case 5:
            LCD_Draw_Number_5(x, y, size);
            break;
        case 6:
            LCD_Draw_Number_6(x, y, size);
            break;
        case 7:
            LCD_Draw_Number_7(x, y, size);
            break;
        case 8:
            LCD_Draw_Number_8(x, y, size);
            break;
        case 9:
            LCD_Draw_Number_9(x, y, size);
            break;
    }
}
void LCD_Draw_Add_Char(uint16_t x, uint16_t y, uint8_t size)
{
    uint16_t width = 5 * size, length = 30 * size;
    for (uint16_t i = x + length; i < x + length + width; ++i) {
        for (uint16_t j = y; j < y + 2 * length + width; ++j) {
            LCD_DrawPoint(i, j);
        }
    }
    for (uint16_t i = x; i < x + 2 * length + width; ++i) {
        for (uint16_t j = y + length; j < y + length + width; ++j) {
            LCD_DrawPoint(i, j);
        }
    }
}
