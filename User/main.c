/*

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
*/

#include "debug.h"
#include "ch32v30x_gpio.h"
#include "ch32v30x_spi.h"
#include "ch32v30x_rcc.h"
#include "../Source/font.h"
#include "../Source/picture_show.h"
#include "../Source/string_show.h"

int main (void) {
    NVIC_PriorityGroupConfig (NVIC_PriorityGroup_2);
    Delay_Init();                                // 初始化延时函数
    USART_Printf_Init (115200);                  // 初始化调试串口

    LCD_GPIO_Init();                             // 初始化GPIO
    SPI1_Init();                                 // 初始化SPI1
    LCD_Init();                                  // 初始化显示屏

    GPIO_WriteBit (GPIOA, LCD_BL_PIN, Bit_SET);  // 开启背光

    LCD_ClearScreen (BLUE);                      // 蓝色背景填充


    LCD_DrawImage (x_pos, y_pos, LOGO_WIDTH, LOGO_HEIGHT, baidu_logo);  // 展示开机baidu_logo画面2秒钟
    Delay_Ms (2000);
    LCD_ClearScreen (BLUE);                                             // 清除屏幕，填充蓝色，显示滚动字幕。

    while (1) {
        printf ("Running test %d\n", k++);

        if (long_text[i]) {
            Delay_Ms (500);
            printf ("%s\n", long_text[i]);
            LCD_ClearScreen (BLUE);
            LCD_DrawString (long_text[i++], WHITE, BLUE);
            continue;
        }

        i = 0;

        if (test_strings[j]) {
            Delay_Ms (1000);
            printf ("%s\n", test_strings[j]);
            LCD_ClearScreen (RED);
            LCD_DrawString (test_strings[j++], WHITE, RED);
            continue;
        }
        j = 0;

        switch (l % 4) {
        case 0:
            LCD_ClearScreen (BLUE);
            break;
        case 1:
            LCD_ClearScreen (BLUE);
            break;
        case 2:
            LCD_ClearScreen (BLUE);
            break;
        case 3:
            LCD_ClearScreen (BLUE);
            break;
        }

        l++;
        Delay_Ms (1000);  // Wait 20 seconds before clearing and restarting the sequence.
    }
}