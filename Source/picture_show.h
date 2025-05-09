#include "init_config.h"
#include "baidu_logo.h"



// logo居中显示（假设baidu_logo.h中定义了LOGO_WIDTH和LOGO_HEIGHT）
uint16_t x_pos = (TFT_WIDTH - LOGO_WIDTH) / 2;
uint16_t y_pos = (TFT_HEIGHT - LOGO_HEIGHT) / 2;

// 显示RGB565格式的位图
void LCD_DrawImage (uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *image) {
    LCD_SetWindow (x, y, x + width - 1, y + height - 1);

    for (uint32_t i = 0; i < width * height; i++) {
        LCD_WriteData (image[i] >> 8);    // 高字节
        LCD_WriteData (image[i] & 0xFF);  // 低字节
    }
}
