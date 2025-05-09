#include "init_config.h"
#include "baidu_logo.h"



// logo������ʾ������baidu_logo.h�ж�����LOGO_WIDTH��LOGO_HEIGHT��
uint16_t x_pos = (TFT_WIDTH - LOGO_WIDTH) / 2;
uint16_t y_pos = (TFT_HEIGHT - LOGO_HEIGHT) / 2;

// ��ʾRGB565��ʽ��λͼ
void LCD_DrawImage (uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *image) {
    LCD_SetWindow (x, y, x + width - 1, y + height - 1);

    for (uint32_t i = 0; i < width * height; i++) {
        LCD_WriteData (image[i] >> 8);    // ���ֽ�
        LCD_WriteData (image[i] & 0xFF);  // ���ֽ�
    }
}
