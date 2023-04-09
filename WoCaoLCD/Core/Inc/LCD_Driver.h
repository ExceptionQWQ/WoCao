#include "stm32f1xx_hal.h"
#include "main.h"

#define LCD_COLOR_WHITE       0xFFFF
#define LCD_COLOR_BLACK      	0x0000
#define LCD_COLOR_BLUE       	0x001F
#define LCD_COLOR_BRED        0XF81F
#define LCD_COLOR_GRED 			 	0XFFE0
#define LCD_COLOR_GBLUE			 	0X07FF
#define LCD_COLOR_RED         0xF800
#define LCD_COLOR_MAGENTA     0xF81F
#define LCD_COLOR_GREEN       0x07E0
#define LCD_COLOR_CYAN        0x7FFF
#define LCD_COLOR_YELLOW      0xFFE0
#define LCD_COLOR_BROWN 			0XBC40
#define LCD_COLOR_BRRED 			0XFC07
#define LCD_COLOR_GRAY  			0X8430

struct LCDDEV
{
    uint16_t width;			//LCD ¿í¶È
    uint16_t height;			//LCD ¸ß¶È
    uint16_t id;				//LCD ID
    uint8_t dir;			//ºáÆÁ»¹ÊÇÊúÆÁ¿ØÖÆ£º0£¬ÊúÆÁ£»1£¬ºáÆÁ¡£
    uint16_t	 wramcmd;		//¿ªÊ¼Ð´gramÖ¸Áî
    uint16_t  setxcmd;		//ÉèÖÃx×ø±êÖ¸Áî
    uint16_t  setycmd;		//ÉèÖÃy×ø±êÖ¸Áî
};

#define  USE_HORIZONTAL 1



void LCD_Set_SPI_Handle(SPI_HandleTypeDef* h);
void LCD_Init();
void LCD_Clear(uint16_t Color);
void LCD_Show_Char(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t num, uint8_t size, uint8_t mode);
void LCD_Show_String(uint16_t x, uint16_t y, uint8_t size, uint8_t *p, uint8_t mode);
void LCD_Draw_Number(uint16_t x, uint16_t y, uint8_t size, uint8_t number);
void LCD_Draw_Add_Char(uint16_t x, uint16_t y, uint8_t size);
