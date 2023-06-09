#include "LCD_Driver.h"
#include "stm32f1xx_hal.h"

SPI_HandleTypeDef* hspi;
struct LCDDEV lcddev;

uint16_t POINT_COLOR = LCD_COLOR_BLACK, BACK_COLOR = LCD_COLOR_WHITE;

const unsigned char asc2_1206[95][12]={
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*" ",0*/
        {0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x04,0x00,0x04,0x00,0x00},/*"!",1*/
        {0x00,0x14,0x0A,0x0A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*""",2*/
        {0x00,0x00,0x14,0x14,0x3F,0x14,0x0A,0x3F,0x0A,0x0A,0x00,0x00},/*"#",3*/
        {0x00,0x04,0x1E,0x15,0x05,0x06,0x0C,0x14,0x15,0x0F,0x04,0x00},/*"$",4*/
        {0x00,0x00,0x12,0x15,0x0D,0x0A,0x14,0x2C,0x2A,0x12,0x00,0x00},/*"%",5*/
        {0x00,0x00,0x04,0x0A,0x0A,0x1E,0x15,0x15,0x09,0x36,0x00,0x00},/*"&",6*/
        {0x00,0x02,0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"'",7*/
        {0x00,0x20,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x10,0x20,0x00},/*"(",8*/
        {0x00,0x02,0x04,0x08,0x08,0x08,0x08,0x08,0x08,0x04,0x02,0x00},/*")",9*/
        {0x00,0x00,0x00,0x04,0x15,0x0E,0x0E,0x15,0x04,0x00,0x00,0x00},/*"*",10*/
        {0x00,0x00,0x04,0x04,0x04,0x1F,0x04,0x04,0x04,0x00,0x00,0x00},/*"+",11*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x02,0x01},/*",",12*/
        {0x00,0x00,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x00,0x00,0x00},/*"-",13*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00},/*".",14*/
        {0x00,0x10,0x08,0x08,0x08,0x04,0x04,0x02,0x02,0x02,0x01,0x00},/*"/",15*/
        {0x00,0x00,0x0E,0x11,0x11,0x11,0x11,0x11,0x11,0x0E,0x00,0x00},/*"0",16*/
        {0x00,0x00,0x04,0x06,0x04,0x04,0x04,0x04,0x04,0x0E,0x00,0x00},/*"1",17*/
        {0x00,0x00,0x0E,0x11,0x11,0x08,0x04,0x02,0x01,0x1F,0x00,0x00},/*"2",18*/
        {0x00,0x00,0x0E,0x11,0x10,0x0C,0x10,0x10,0x11,0x0E,0x00,0x00},/*"3",19*/
        {0x00,0x00,0x08,0x0C,0x0A,0x0A,0x09,0x1E,0x08,0x18,0x00,0x00},/*"4",20*/
        {0x00,0x00,0x1F,0x01,0x01,0x0F,0x10,0x10,0x11,0x0E,0x00,0x00},/*"5",21*/
        {0x00,0x00,0x0E,0x09,0x01,0x0F,0x11,0x11,0x11,0x0E,0x00,0x00},/*"6",22*/
        {0x00,0x00,0x1F,0x09,0x08,0x04,0x04,0x04,0x04,0x04,0x00,0x00},/*"7",23*/
        {0x00,0x00,0x0E,0x11,0x11,0x0E,0x11,0x11,0x11,0x0E,0x00,0x00},/*"8",24*/
        {0x00,0x00,0x0E,0x11,0x11,0x11,0x1E,0x10,0x12,0x0E,0x00,0x00},/*"9",25*/
        {0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x04,0x00,0x00},/*":",26*/
        {0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x04,0x04,0x00},/*";",27*/
        {0x00,0x20,0x10,0x08,0x04,0x02,0x04,0x08,0x10,0x20,0x00,0x00},/*"<",28*/
        {0x00,0x00,0x00,0x00,0x1F,0x00,0x00,0x1F,0x00,0x00,0x00,0x00},/*"=",29*/
        {0x00,0x02,0x04,0x08,0x10,0x20,0x10,0x08,0x04,0x02,0x00,0x00},/*">",30*/
        {0x00,0x00,0x0E,0x11,0x11,0x08,0x04,0x04,0x00,0x04,0x00,0x00},/*"?",31*/
        {0x00,0x00,0x0E,0x11,0x19,0x15,0x15,0x1D,0x01,0x1E,0x00,0x00},/*"@",32*/
        {0x00,0x00,0x04,0x04,0x0C,0x0A,0x0A,0x1E,0x12,0x33,0x00,0x00},/*"A",33*/
        {0x00,0x00,0x0F,0x12,0x12,0x0E,0x12,0x12,0x12,0x0F,0x00,0x00},/*"B",34*/
        {0x00,0x00,0x1E,0x11,0x01,0x01,0x01,0x01,0x11,0x0E,0x00,0x00},/*"C",35*/
        {0x00,0x00,0x0F,0x12,0x12,0x12,0x12,0x12,0x12,0x0F,0x00,0x00},/*"D",36*/
        {0x00,0x00,0x1F,0x12,0x0A,0x0E,0x0A,0x02,0x12,0x1F,0x00,0x00},/*"E",37*/
        {0x00,0x00,0x1F,0x12,0x0A,0x0E,0x0A,0x02,0x02,0x07,0x00,0x00},/*"F",38*/
        {0x00,0x00,0x1C,0x12,0x01,0x01,0x39,0x11,0x12,0x0C,0x00,0x00},/*"G",39*/
        {0x00,0x00,0x33,0x12,0x12,0x1E,0x12,0x12,0x12,0x33,0x00,0x00},/*"H",40*/
        {0x00,0x00,0x1F,0x04,0x04,0x04,0x04,0x04,0x04,0x1F,0x00,0x00},/*"I",41*/
        {0x00,0x00,0x3E,0x08,0x08,0x08,0x08,0x08,0x08,0x09,0x07,0x00},/*"J",42*/
        {0x00,0x00,0x37,0x12,0x0A,0x06,0x0A,0x0A,0x12,0x37,0x00,0x00},/*"K",43*/
        {0x00,0x00,0x07,0x02,0x02,0x02,0x02,0x02,0x22,0x3F,0x00,0x00},/*"L",44*/
        {0x00,0x00,0x1B,0x1B,0x1B,0x1B,0x15,0x15,0x15,0x15,0x00,0x00},/*"M",45*/
        {0x00,0x00,0x3B,0x12,0x16,0x16,0x1A,0x1A,0x12,0x17,0x00,0x00},/*"N",46*/
        {0x00,0x00,0x0E,0x11,0x11,0x11,0x11,0x11,0x11,0x0E,0x00,0x00},/*"O",47*/
        {0x00,0x00,0x0F,0x12,0x12,0x0E,0x02,0x02,0x02,0x07,0x00,0x00},/*"P",48*/
        {0x00,0x00,0x0E,0x11,0x11,0x11,0x11,0x17,0x19,0x0E,0x18,0x00},/*"Q",49*/
        {0x00,0x00,0x0F,0x12,0x12,0x0E,0x0A,0x12,0x12,0x37,0x00,0x00},/*"R",50*/
        {0x00,0x00,0x1E,0x11,0x01,0x06,0x08,0x10,0x11,0x0F,0x00,0x00},/*"S",51*/
        {0x00,0x00,0x1F,0x15,0x04,0x04,0x04,0x04,0x04,0x0E,0x00,0x00},/*"T",52*/
        {0x00,0x00,0x33,0x12,0x12,0x12,0x12,0x12,0x12,0x0C,0x00,0x00},/*"U",53*/
        {0x00,0x00,0x33,0x12,0x12,0x0A,0x0A,0x0C,0x04,0x04,0x00,0x00},/*"V",54*/
        {0x00,0x00,0x15,0x15,0x15,0x0E,0x0A,0x0A,0x0A,0x0A,0x00,0x00},/*"W",55*/
        {0x00,0x00,0x1B,0x0A,0x0A,0x04,0x04,0x0A,0x0A,0x1B,0x00,0x00},/*"X",56*/
        {0x00,0x00,0x1B,0x0A,0x0A,0x04,0x04,0x04,0x04,0x0E,0x00,0x00},/*"Y",57*/
        {0x00,0x00,0x1F,0x09,0x08,0x04,0x04,0x02,0x12,0x1F,0x00,0x00},/*"Z",58*/
        {0x00,0x1C,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x1C,0x00},/*"[",59*/
        {0x00,0x02,0x02,0x02,0x04,0x04,0x08,0x08,0x08,0x10,0x00,0x00},/*"\",60*/
        {0x00,0x0E,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x0E,0x00},/*"]",61*/
        {0x00,0x04,0x0A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"^",62*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3F},/*"_",63*/
        {0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"`",64*/
        {0x00,0x00,0x00,0x00,0x00,0x0C,0x12,0x1C,0x12,0x3C,0x00,0x00},/*"a",65*/
        {0x00,0x00,0x03,0x02,0x02,0x0E,0x12,0x12,0x12,0x0E,0x00,0x00},/*"b",66*/
        {0x00,0x00,0x00,0x00,0x00,0x1C,0x12,0x02,0x02,0x1C,0x00,0x00},/*"c",67*/
        {0x00,0x00,0x18,0x10,0x10,0x1C,0x12,0x12,0x12,0x3C,0x00,0x00},/*"d",68*/
        {0x00,0x00,0x00,0x00,0x00,0x0C,0x12,0x1E,0x02,0x1C,0x00,0x00},/*"e",69*/
        {0x00,0x00,0x38,0x04,0x04,0x1E,0x04,0x04,0x04,0x1E,0x00,0x00},/*"f",70*/
        {0x00,0x00,0x00,0x00,0x00,0x3C,0x12,0x0C,0x02,0x1E,0x22,0x1C},/*"g",71*/
        {0x00,0x00,0x03,0x02,0x02,0x0E,0x12,0x12,0x12,0x37,0x00,0x00},/*"h",72*/
        {0x00,0x00,0x04,0x00,0x00,0x06,0x04,0x04,0x04,0x0E,0x00,0x00},/*"i",73*/
        {0x00,0x00,0x08,0x00,0x00,0x0C,0x08,0x08,0x08,0x08,0x08,0x07},/*"j",74*/
        {0x00,0x00,0x03,0x02,0x02,0x3A,0x0A,0x0E,0x12,0x37,0x00,0x00},/*"k",75*/
        {0x00,0x00,0x07,0x04,0x04,0x04,0x04,0x04,0x04,0x1F,0x00,0x00},/*"l",76*/
        {0x00,0x00,0x00,0x00,0x00,0x0F,0x15,0x15,0x15,0x15,0x00,0x00},/*"m",77*/
        {0x00,0x00,0x00,0x00,0x00,0x0F,0x12,0x12,0x12,0x37,0x00,0x00},/*"n",78*/
        {0x00,0x00,0x00,0x00,0x00,0x0C,0x12,0x12,0x12,0x0C,0x00,0x00},/*"o",79*/
        {0x00,0x00,0x00,0x00,0x00,0x0F,0x12,0x12,0x12,0x0E,0x02,0x07},/*"p",80*/
        {0x00,0x00,0x00,0x00,0x00,0x1C,0x12,0x12,0x12,0x1C,0x10,0x38},/*"q",81*/
        {0x00,0x00,0x00,0x00,0x00,0x1B,0x06,0x02,0x02,0x07,0x00,0x00},/*"r",82*/
        {0x00,0x00,0x00,0x00,0x00,0x1E,0x02,0x0C,0x10,0x1E,0x00,0x00},/*"s",83*/
        {0x00,0x00,0x00,0x04,0x04,0x0E,0x04,0x04,0x04,0x18,0x00,0x00},/*"t",84*/
        {0x00,0x00,0x00,0x00,0x00,0x1B,0x12,0x12,0x12,0x3C,0x00,0x00},/*"u",85*/
        {0x00,0x00,0x00,0x00,0x00,0x37,0x12,0x0A,0x0C,0x04,0x00,0x00},/*"v",86*/
        {0x00,0x00,0x00,0x00,0x00,0x15,0x15,0x0E,0x0A,0x0A,0x00,0x00},/*"w",87*/
        {0x00,0x00,0x00,0x00,0x00,0x1B,0x0A,0x04,0x0A,0x1B,0x00,0x00},/*"x",88*/
        {0x00,0x00,0x00,0x00,0x00,0x37,0x12,0x0A,0x0C,0x04,0x04,0x03},/*"y",89*/
        {0x00,0x00,0x00,0x00,0x00,0x1E,0x08,0x04,0x04,0x1E,0x00,0x00},/*"z",90*/
        {0x00,0x18,0x08,0x08,0x08,0x04,0x08,0x08,0x08,0x08,0x18,0x00},/*"{",91*/
        {0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08},/*"|",92*/
        {0x00,0x06,0x04,0x04,0x04,0x08,0x04,0x04,0x04,0x04,0x06,0x00},/*"}",93*/
        {0x02,0x25,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00} /*"~",94*/
};
const unsigned char asc2_1608[95][16]={
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*" ",0*/
        {0x00,0x00,0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x00,0x00,0x18,0x18,0x00,0x00},/*"!",1*/
        {0x00,0x48,0x6C,0x24,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*""",2*/
        {0x00,0x00,0x00,0x24,0x24,0x24,0x7F,0x12,0x12,0x12,0x7F,0x12,0x12,0x12,0x00,0x00},/*"#",3*/
        {0x00,0x00,0x08,0x1C,0x2A,0x2A,0x0A,0x0C,0x18,0x28,0x28,0x2A,0x2A,0x1C,0x08,0x08},/*"$",4*/
        {0x00,0x00,0x00,0x22,0x25,0x15,0x15,0x15,0x2A,0x58,0x54,0x54,0x54,0x22,0x00,0x00},/*"%",5*/
        {0x00,0x00,0x00,0x0C,0x12,0x12,0x12,0x0A,0x76,0x25,0x29,0x11,0x91,0x6E,0x00,0x00},/*"&",6*/
        {0x00,0x06,0x06,0x04,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"'",7*/
        {0x00,0x40,0x20,0x10,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x10,0x10,0x20,0x40,0x00},/*"(",8*/
        {0x00,0x02,0x04,0x08,0x08,0x10,0x10,0x10,0x10,0x10,0x10,0x08,0x08,0x04,0x02,0x00},/*")",9*/
        {0x00,0x00,0x00,0x00,0x08,0x08,0x6B,0x1C,0x1C,0x6B,0x08,0x08,0x00,0x00,0x00,0x00},/*"*",10*/
        {0x00,0x00,0x00,0x00,0x08,0x08,0x08,0x08,0x7F,0x08,0x08,0x08,0x08,0x00,0x00,0x00},/*"+",11*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x06,0x04,0x03},/*",",12*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"-",13*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x06,0x00,0x00},/*".",14*/
        {0x00,0x00,0x80,0x40,0x40,0x20,0x20,0x10,0x10,0x08,0x08,0x04,0x04,0x02,0x02,0x00},/*"/",15*/
        {0x00,0x00,0x00,0x18,0x24,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x24,0x18,0x00,0x00},/*"0",16*/
        {0x00,0x00,0x00,0x08,0x0E,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x3E,0x00,0x00},/*"1",17*/
        {0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x20,0x20,0x10,0x08,0x04,0x42,0x7E,0x00,0x00},/*"2",18*/
        {0x00,0x00,0x00,0x3C,0x42,0x42,0x20,0x18,0x20,0x40,0x40,0x42,0x22,0x1C,0x00,0x00},/*"3",19*/
        {0x00,0x00,0x00,0x20,0x30,0x28,0x24,0x24,0x22,0x22,0x7E,0x20,0x20,0x78,0x00,0x00},/*"4",20*/
        {0x00,0x00,0x00,0x7E,0x02,0x02,0x02,0x1A,0x26,0x40,0x40,0x42,0x22,0x1C,0x00,0x00},/*"5",21*/
        {0x00,0x00,0x00,0x38,0x24,0x02,0x02,0x1A,0x26,0x42,0x42,0x42,0x24,0x18,0x00,0x00},/*"6",22*/
        {0x00,0x00,0x00,0x7E,0x22,0x22,0x10,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x00,0x00},/*"7",23*/
        {0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x24,0x18,0x24,0x42,0x42,0x42,0x3C,0x00,0x00},/*"8",24*/
        {0x00,0x00,0x00,0x18,0x24,0x42,0x42,0x42,0x64,0x58,0x40,0x40,0x24,0x1C,0x00,0x00},/*"9",25*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00},/*":",26*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x00,0x00,0x08,0x08,0x04},/*";",27*/
        {0x00,0x00,0x00,0x40,0x20,0x10,0x08,0x04,0x02,0x04,0x08,0x10,0x20,0x40,0x00,0x00},/*"<",28*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x7F,0x00,0x00,0x00,0x00,0x00},/*"=",29*/
        {0x00,0x00,0x00,0x02,0x04,0x08,0x10,0x20,0x40,0x20,0x10,0x08,0x04,0x02,0x00,0x00},/*">",30*/
        {0x00,0x00,0x00,0x3C,0x42,0x42,0x46,0x40,0x20,0x10,0x10,0x00,0x18,0x18,0x00,0x00},/*"?",31*/
        {0x00,0x00,0x00,0x1C,0x22,0x5A,0x55,0x55,0x55,0x55,0x2D,0x42,0x22,0x1C,0x00,0x00},/*"@",32*/
        {0x00,0x00,0x00,0x08,0x08,0x18,0x14,0x14,0x24,0x3C,0x22,0x42,0x42,0xE7,0x00,0x00},/*"A",33*/
        {0x00,0x00,0x00,0x1F,0x22,0x22,0x22,0x1E,0x22,0x42,0x42,0x42,0x22,0x1F,0x00,0x00},/*"B",34*/
        {0x00,0x00,0x00,0x7C,0x42,0x42,0x01,0x01,0x01,0x01,0x01,0x42,0x22,0x1C,0x00,0x00},/*"C",35*/
        {0x00,0x00,0x00,0x1F,0x22,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x22,0x1F,0x00,0x00},/*"D",36*/
        {0x00,0x00,0x00,0x3F,0x42,0x12,0x12,0x1E,0x12,0x12,0x02,0x42,0x42,0x3F,0x00,0x00},/*"E",37*/
        {0x00,0x00,0x00,0x3F,0x42,0x12,0x12,0x1E,0x12,0x12,0x02,0x02,0x02,0x07,0x00,0x00},/*"F",38*/
        {0x00,0x00,0x00,0x3C,0x22,0x22,0x01,0x01,0x01,0x71,0x21,0x22,0x22,0x1C,0x00,0x00},/*"G",39*/
        {0x00,0x00,0x00,0xE7,0x42,0x42,0x42,0x42,0x7E,0x42,0x42,0x42,0x42,0xE7,0x00,0x00},/*"H",40*/
        {0x00,0x00,0x00,0x3E,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x3E,0x00,0x00},/*"I",41*/
        {0x00,0x00,0x00,0x7C,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x11,0x0F},/*"J",42*/
        {0x00,0x00,0x00,0x77,0x22,0x12,0x0A,0x0E,0x0A,0x12,0x12,0x22,0x22,0x77,0x00,0x00},/*"K",43*/
        {0x00,0x00,0x00,0x07,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x42,0x7F,0x00,0x00},/*"L",44*/
        {0x00,0x00,0x00,0x77,0x36,0x36,0x36,0x36,0x2A,0x2A,0x2A,0x2A,0x2A,0x6B,0x00,0x00},/*"M",45*/
        {0x00,0x00,0x00,0xE3,0x46,0x46,0x4A,0x4A,0x52,0x52,0x52,0x62,0x62,0x47,0x00,0x00},/*"N",46*/
        {0x00,0x00,0x00,0x1C,0x22,0x41,0x41,0x41,0x41,0x41,0x41,0x41,0x22,0x1C,0x00,0x00},/*"O",47*/
        {0x00,0x00,0x00,0x3F,0x42,0x42,0x42,0x42,0x3E,0x02,0x02,0x02,0x02,0x07,0x00,0x00},/*"P",48*/
        {0x00,0x00,0x00,0x1C,0x22,0x41,0x41,0x41,0x41,0x41,0x4D,0x53,0x32,0x1C,0x60,0x00},/*"Q",49*/
        {0x00,0x00,0x00,0x3F,0x42,0x42,0x42,0x3E,0x12,0x12,0x22,0x22,0x42,0xC7,0x00,0x00},/*"R",50*/
        {0x00,0x00,0x00,0x7C,0x42,0x42,0x02,0x04,0x18,0x20,0x40,0x42,0x42,0x3E,0x00,0x00},/*"S",51*/
        {0x00,0x00,0x00,0x7F,0x49,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x1C,0x00,0x00},/*"T",52*/
        {0x00,0x00,0x00,0xE7,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x3C,0x00,0x00},/*"U",53*/
        {0x00,0x00,0x00,0xE7,0x42,0x42,0x22,0x24,0x24,0x14,0x14,0x18,0x08,0x08,0x00,0x00},/*"V",54*/
        {0x00,0x00,0x00,0x6B,0x49,0x49,0x49,0x49,0x55,0x55,0x36,0x22,0x22,0x22,0x00,0x00},/*"W",55*/
        {0x00,0x00,0x00,0xE7,0x42,0x24,0x24,0x18,0x18,0x18,0x24,0x24,0x42,0xE7,0x00,0x00},/*"X",56*/
        {0x00,0x00,0x00,0x77,0x22,0x22,0x14,0x14,0x08,0x08,0x08,0x08,0x08,0x1C,0x00,0x00},/*"Y",57*/
        {0x00,0x00,0x00,0x7E,0x21,0x20,0x10,0x10,0x08,0x04,0x04,0x42,0x42,0x3F,0x00,0x00},/*"Z",58*/
        {0x00,0x78,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x78,0x00},/*"[",59*/
        {0x00,0x00,0x02,0x02,0x04,0x04,0x08,0x08,0x08,0x10,0x10,0x20,0x20,0x20,0x40,0x40},/*"\",60*/
        {0x00,0x1E,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x1E,0x00},/*"]",61*/
        {0x00,0x38,0x44,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"^",62*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF},/*"_",63*/
        {0x00,0x06,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"`",64*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x42,0x78,0x44,0x42,0x42,0xFC,0x00,0x00},/*"a",65*/
        {0x00,0x00,0x00,0x03,0x02,0x02,0x02,0x1A,0x26,0x42,0x42,0x42,0x26,0x1A,0x00,0x00},/*"b",66*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x44,0x02,0x02,0x02,0x44,0x38,0x00,0x00},/*"c",67*/
        {0x00,0x00,0x00,0x60,0x40,0x40,0x40,0x78,0x44,0x42,0x42,0x42,0x64,0xD8,0x00,0x00},/*"d",68*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x42,0x7E,0x02,0x02,0x42,0x3C,0x00,0x00},/*"e",69*/
        {0x00,0x00,0x00,0xF0,0x88,0x08,0x08,0x7E,0x08,0x08,0x08,0x08,0x08,0x3E,0x00,0x00},/*"f",70*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7C,0x22,0x22,0x1C,0x02,0x3C,0x42,0x42,0x3C},/*"g",71*/
        {0x00,0x00,0x00,0x03,0x02,0x02,0x02,0x3A,0x46,0x42,0x42,0x42,0x42,0xE7,0x00,0x00},/*"h",72*/
        {0x00,0x00,0x00,0x0C,0x0C,0x00,0x00,0x0E,0x08,0x08,0x08,0x08,0x08,0x3E,0x00,0x00},/*"i",73*/
        {0x00,0x00,0x00,0x30,0x30,0x00,0x00,0x38,0x20,0x20,0x20,0x20,0x20,0x20,0x22,0x1E},/*"j",74*/
        {0x00,0x00,0x00,0x03,0x02,0x02,0x02,0x72,0x12,0x0A,0x16,0x12,0x22,0x77,0x00,0x00},/*"k",75*/
        {0x00,0x00,0x00,0x0E,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x3E,0x00,0x00},/*"l",76*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7F,0x92,0x92,0x92,0x92,0x92,0xB7,0x00,0x00},/*"m",77*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3B,0x46,0x42,0x42,0x42,0x42,0xE7,0x00,0x00},/*"n",78*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3C,0x42,0x42,0x42,0x42,0x42,0x3C,0x00,0x00},/*"o",79*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1B,0x26,0x42,0x42,0x42,0x22,0x1E,0x02,0x07},/*"p",80*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x44,0x42,0x42,0x42,0x44,0x78,0x40,0xE0},/*"q",81*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x77,0x4C,0x04,0x04,0x04,0x04,0x1F,0x00,0x00},/*"r",82*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7C,0x42,0x02,0x3C,0x40,0x42,0x3E,0x00,0x00},/*"s",83*/
        {0x00,0x00,0x00,0x00,0x00,0x08,0x08,0x3E,0x08,0x08,0x08,0x08,0x08,0x30,0x00,0x00},/*"t",84*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x63,0x42,0x42,0x42,0x42,0x62,0xDC,0x00,0x00},/*"u",85*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE7,0x42,0x24,0x24,0x14,0x08,0x08,0x00,0x00},/*"v",86*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xEB,0x49,0x49,0x55,0x55,0x22,0x22,0x00,0x00},/*"w",87*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x76,0x24,0x18,0x18,0x18,0x24,0x6E,0x00,0x00},/*"x",88*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xE7,0x42,0x24,0x24,0x14,0x18,0x08,0x08,0x07},/*"y",89*/
        {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x22,0x10,0x08,0x08,0x44,0x7E,0x00,0x00},/*"z",90*/
        {0x00,0xC0,0x20,0x20,0x20,0x20,0x20,0x10,0x20,0x20,0x20,0x20,0x20,0x20,0xC0,0x00},/*"{",91*/
        {0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10},/*"|",92*/
        {0x00,0x06,0x08,0x08,0x08,0x08,0x08,0x10,0x08,0x08,0x08,0x08,0x08,0x08,0x06,0x00},/*"}",93*/
        {0x0C,0x32,0xC2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"~",94*/
};

void LCD_Set_SPI_Handle(SPI_HandleTypeDef* h)
{
    hspi = h;
}

void LCD_Write_Reg(char data)
{

    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(hspi, &data, 1, 100);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}
void LCD_Write_Data(char data)
{
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
    HAL_SPI_Transmit(hspi, &data, 1, 100);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}
void SPI_Write_Byte(char data)
{
    HAL_SPI_Transmit(hspi, &data, 1, 100);
}
void LCD_Draw_Point_16Bit(uint16_t color)
{
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);


    SPI_Write_Byte(color>>8);
    SPI_Write_Byte(color);

    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}
void LCD_RESET(void)
{
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(50);
}

void LCD_WriteReg(char LCD_Reg, uint16_t LCD_RegValue)
{
    LCD_Write_Reg(LCD_Reg);
    LCD_Write_Data(LCD_RegValue);
}


void LCD_SetParam()
{
    lcddev.wramcmd=0x2C;
#if USE_HORIZONTAL==1	//Ê¹ÓÃºáÆÁ
    lcddev.dir=1;//ºáÆÁ
    lcddev.width=320;
    lcddev.height=240;
    lcddev.setxcmd=0x2A;
    lcddev.setycmd=0x2B;
    LCD_WriteReg(0x36,0x6C);

#else//ÊúÆÁ
    lcddev.dir=0;//ÊúÆÁ
    lcddev.width=240;
    lcddev.height=320;
    lcddev.setxcmd=0x2A;
    lcddev.setycmd=0x2B;
    LCD_WriteReg(0x36,0xC9);
#endif
}

void LCD_Init()
{

    LCD_RESET(); //Reset before LCD Init.


    //************* Start Initial Sequence **********//
    LCD_Write_Reg(0xCF);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0xC1);
    LCD_Write_Data(0X30);
    LCD_Write_Reg(0xED);
    LCD_Write_Data(0x64);
    LCD_Write_Data(0x03);
    LCD_Write_Data(0X12);
    LCD_Write_Data(0X81);
    LCD_Write_Reg(0xE8);
    LCD_Write_Data(0x85);
    LCD_Write_Data(0x10);
    LCD_Write_Data(0x7A);
    LCD_Write_Reg(0xCB);
    LCD_Write_Data(0x39);
    LCD_Write_Data(0x2C);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x34);
    LCD_Write_Data(0x02);
    LCD_Write_Reg(0xF7);
    LCD_Write_Data(0x20);
    LCD_Write_Reg(0xEA);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x00);
    LCD_Write_Reg(0xC0);    //Power control
    LCD_Write_Data(0x1B);   //VRH[5:0]
    LCD_Write_Reg(0xC1);    //Power control
    LCD_Write_Data(0x01);   //SAP[2:0];BT[3:0]
    LCD_Write_Reg(0xC5);    //VCM control
    LCD_Write_Data(0x30); 	 //3F
    LCD_Write_Data(0x30); 	 //3C
    LCD_Write_Reg(0xC7);    //VCM control2
    LCD_Write_Data(0XB7);
    LCD_Write_Reg(0x36);    // Memory Access Control
    LCD_Write_Data(0x48);
    LCD_Write_Reg(0x3A);
    LCD_Write_Data(0x55);
    LCD_Write_Reg(0xB1);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x1A);
    LCD_Write_Reg(0xB6);    // Display Function Control
    LCD_Write_Data(0x0A);
    LCD_Write_Data(0xA2);
    LCD_Write_Reg(0xF2);    // 3Gamma Function Disable
    LCD_Write_Data(0x00);
    LCD_Write_Reg(0x26);    //Gamma curve selected
    LCD_Write_Data(0x01);
    LCD_Write_Reg(0xE0);    //Set Gamma
    LCD_Write_Data(0x0F);
    LCD_Write_Data(0x2A);
    LCD_Write_Data(0x28);
    LCD_Write_Data(0x08);
    LCD_Write_Data(0x0E);
    LCD_Write_Data(0x08);
    LCD_Write_Data(0x54);
    LCD_Write_Data(0XA9);
    LCD_Write_Data(0x43);
    LCD_Write_Data(0x0A);
    LCD_Write_Data(0x0F);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x00);
    LCD_Write_Reg(0XE1);    //Set Gamma
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x15);
    LCD_Write_Data(0x17);
    LCD_Write_Data(0x07);
    LCD_Write_Data(0x11);
    LCD_Write_Data(0x06);
    LCD_Write_Data(0x2B);
    LCD_Write_Data(0x56);
    LCD_Write_Data(0x3C);
    LCD_Write_Data(0x05);
    LCD_Write_Data(0x10);
    LCD_Write_Data(0x0F);
    LCD_Write_Data(0x3F);
    LCD_Write_Data(0x3F);
    LCD_Write_Data(0x0F);
    LCD_Write_Reg(0x2B);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x01);
    LCD_Write_Data(0x3f);
    LCD_Write_Reg(0x2A);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0xef);
    LCD_Write_Reg(0x11); //Exit Sleep
    HAL_Delay(120);
    LCD_Write_Reg(0x29); //display on


    LCD_SetParam();


}

void LCD_WriteRAM_Prepare(void)
{
    LCD_Write_Reg(lcddev.wramcmd);
}

void LCD_SetWindows(uint16_t xStar, uint16_t yStar, uint16_t xEnd, uint16_t yEnd)
{
    LCD_Write_Reg(lcddev.setxcmd);
    LCD_Write_Data(xStar>>8);
    LCD_Write_Data(0x00FF&xStar);
    LCD_Write_Data(xEnd>>8);
    LCD_Write_Data(0x00FF&xEnd);

    LCD_Write_Reg(lcddev.setycmd);
    LCD_Write_Data(yStar>>8);
    LCD_Write_Data(0x00FF&yStar);
    LCD_Write_Data(yEnd>>8);
    LCD_Write_Data(0x00FF&yEnd);

    LCD_WriteRAM_Prepare();	//¿ªÊ¼Ð´ÈëGRAM
}
void LCD_Set_Cursor(uint16_t Xpos, uint16_t Ypos)
{
    LCD_Write_Reg(lcddev.setxcmd);
    LCD_Write_Data(Xpos>>8);
    LCD_Write_Data(0x00FF&Xpos);


    LCD_Write_Reg(lcddev.setycmd);
    LCD_Write_Data(Ypos>>8);
    LCD_Write_Data(0x00FF&Ypos);

    LCD_WriteRAM_Prepare();	//¿ªÊ¼Ð´ÈëGRAM
}
void LCD_Clear(uint16_t Color)
{
    uint32_t index=0;
    LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
    for(index=0;index<lcddev.width*lcddev.height;index++)
    {
        LCD_Write_Data(Color>>8);
        LCD_Write_Data(Color);
    }
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}
void LCD_Draw_Point(uint16_t x,uint16_t y)
{
    LCD_Set_Cursor(x,y);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
    SPI_Write_Byte(POINT_COLOR>>8);
    SPI_Write_Byte(POINT_COLOR);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}
void LCD_Show_Char(uint16_t x, uint16_t y, uint16_t fc, uint16_t bc, uint8_t num, uint8_t size, uint8_t mode)
{
    uint8_t temp;
    uint8_t pos,t;
    uint16_t colortemp=POINT_COLOR;

    num=num-' ';
    LCD_SetWindows(x,y,x+size/2-1,y+size-1);
    if(!mode)
    {
        for(pos=0;pos<size;pos++)
        {
            if(size==12)temp=asc2_1206[num][pos];
            else temp=asc2_1608[num][pos];
            for(t=0;t<size/2;t++)
            {
                if(temp&0x01)LCD_Draw_Point_16Bit(fc);
                else LCD_Draw_Point_16Bit(bc);
                temp>>=1;
            }
        }
    }else
    {
        for(pos=0;pos<size;pos++)
        {
            if(size==12)temp=asc2_1206[num][pos];
            else temp=asc2_1608[num][pos];
            for(t=0;t<size/2;t++)
            {
                POINT_COLOR=fc;
                if(temp&0x01)LCD_Draw_Point(x+t,y+pos);
                temp>>=1;
            }
        }
    }
    POINT_COLOR=colortemp;
    LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);
}
void LCD_Draw_Number_Top(uint16_t x, uint16_t y, uint8_t size)
{
    uint16_t width = 3 * size, length = 20 * size;
    for (uint16_t i = x; i < x + length; ++i) {
        for (uint16_t j = y; j < y + width; ++j) {
            LCD_Draw_Point(i, j);
        }
    }
}
void LCD_Draw_Number_Top_Left(uint16_t x, uint16_t y, uint8_t size)
{
    uint16_t width = 3 * size, length = 20 * size;
    for (uint16_t i = x; i < x + width; ++i) {
        for (uint16_t j = y; j < y + length; ++j) {
            LCD_Draw_Point(i, j);
        }
    }
}
void LCD_Draw_Number_Top_Right(uint16_t x, uint16_t y, uint8_t size)
{
    uint16_t width = 3 * size, length = 20 * size;
    for (uint16_t i = x + length - width; i < x + length; ++i) {
        for (uint16_t j = y; j < y + length; ++j) {
            LCD_Draw_Point(i, j);
        }
    }
}
void LCD_Draw_Number_Middle(uint16_t x, uint16_t y, uint8_t size)
{
    uint16_t width = 3 * size, length = 20 * size;
    for (uint16_t i = x; i < x + length; ++i) {
        for (uint16_t j = y + length; j < y + length + width; ++j) {
            LCD_Draw_Point(i, j);
        }
    }
}
void LCD_Draw_Number_Bottom_Left(uint16_t x, uint16_t y, uint8_t size)
{
    uint16_t width = 3 * size, length = 20 * size;
    for (uint16_t i = x; i < x + width; ++i) {
        for (uint16_t j = y + length ; j < y + 2 * length + width; ++j) {
            LCD_Draw_Point(i, j);
        }
    }
}
void LCD_Draw_Number_Bottom_Right(uint16_t x, uint16_t y, uint8_t size)
{
    uint16_t width = 3 * size, length = 20 * size;
    for (uint16_t i = x + length - width; i < x + length; ++i) {
        for (uint16_t j = y + length ; j < y + 2 * length + width; ++j) {
            LCD_Draw_Point(i, j);
        }
    }
}
void LCD_Draw_Number_Bottom(uint16_t x, uint16_t y, uint8_t size)
{
    uint16_t width = 3 * size, length = 20 * size;
    for (uint16_t i = x; i < x + length; ++i) {
        for (uint16_t j = y + 2 * length; j < y + 2 * length + width; ++j) {
            LCD_Draw_Point(i, j);
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
    uint16_t width = 3 * size, length = 20 * size;
    for (uint16_t i = x + length; i < x + length + width; ++i) {
        for (uint16_t j = y; j < y + 2 * length + width; ++j) {
            LCD_Draw_Point(i, j);
        }
    }
    for (uint16_t i = x; i < x + 2 * length + width; ++i) {
        for (uint16_t j = y + length; j < y + length + width; ++j) {
            LCD_Draw_Point(i, j);
        }
    }
}

void LCD_Show_String(uint16_t x, uint16_t y, uint8_t size, uint8_t *p, uint8_t mode)
{
    while((*p<='~')&&(*p>=' '))//ÅÐ¶ÏÊÇ²»ÊÇ·Ç·¨×Ö·û!
    {
        if(x>(lcddev.width-1)||y>(lcddev.height-1))
            return;
        LCD_Show_Char(x,y,POINT_COLOR,BACK_COLOR,*p,size,mode);
        x+=size/2;
        p++;
    }
}
