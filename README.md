# CH32V307VCT6_TFTLCD_ST7735
*****************
适用芯片CH32V307VCT6 TFT_LCD ST7735驱动

AUTHOR:MrThinker.Zhang
DATE:2025-05-08 17:01

*****************
一、接线方式
#define LCD_RES_PIN GPIO_Pin_2   // PA2 -> RES
#define LCD_DC_PIN GPIO_Pin_3    // PA3 -> DC
#define LCD_CS_PIN GPIO_Pin_4    // PA4 -> CS
#define LCD_BL_PIN GPIO_Pin_1    // PA1 -> BL（背光控制）
#define LCD_SCK_PIN GPIO_Pin_5   // PA5 -> SCL（SPI时钟）
#define LCD_MOSI_PIN GPIO_Pin_7  // PA7 -> SDA（SPI数据）
二.实现功能：
1）TFT_LCD显示ASCII码字符。
2)滚动多页显示多行字符。
3）滚动页面时交替显示不同LCD背光。
AAAAAAAAAAAAAAAAAA
