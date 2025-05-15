#include "init_config.h"

void LCD_DrawPixel (uint16_t x, uint16_t y, uint16_t color);
void LCD_DrawString (const char *str, uint16_t color, uint16_t bg_color);

const char *long_text[] = {
    "This is a long text that will demonstrate the auto-scrolling feature of the display.",
    "Each line starts with a small offset of a few pixels.",
    "When the text reaches the bottom of the screen,",
    "it will automatically scroll up to make room for new lines.",
    "This is line number five.",
    "Line six continues the demonstration.",
    "Line seven shows that scrolling works properly.",
    "Line eight is where we should see the first scroll happen.",
    "Line nine appears after scrolling.",
    0  // NULL terminated array of strings to print one after another.
};

const char *test_strings[] = {
    "[System Boot]",
    "[Initialize Hardware]",
    "[Loading Configuration]",
    "[Checking Sensors]",
    "[Starting Services]",
    0};

int i = 0, j = 0, k = 0, l = 0;

// 绘制像素点
void LCD_DrawPixel (uint16_t x, uint16_t y, uint16_t color) {
    if (x >= TFT_WIDTH || y >= TFT_HEIGHT)
        return;
    LCD_SetWindow (x, y, x, y);
    LCD_WriteData (color >> 8);
    LCD_WriteData (color & 0xFF);
}

// 获取 ASCII 字符的字模数据指针（优化版）
const uint8_t *GetFontData (char ch) {
    // 检查字符是否在字库范围内（空格 0x20 到 'z' 0x122 ~为126）
    if (ch < 0x20 || ch > 0x126) {
        return NULL;                    // 不支持字符
    }
    uint16_t index = (ch - 0x20) * 16;  // 统一计算公式
    return &font8x16[index];
}

// 绘制单个字符（8x16像素）
void LCD_DrawChar (uint8_t x, uint8_t y, char ch, uint16_t color, uint16_t bg_color) {
    const uint8_t *char_data = GetFontData (ch);  // 直接调用索引计算
    if (!char_data)
        return;                                   // 过滤不支持字符

    for (uint8_t row = 0; row < 16; row++) {
        uint8_t pixels = char_data[row];
        for (uint8_t col = 0; col < 8; col++) {
            LCD_DrawPixel (x + col, y + row, (pixels & (0x80 >> col)) ? color : bg_color);
        }
    }
}

// 清屏并重置光标位置
void LCD_ClearScreen (uint16_t color) {
    LCD_Fill (color);
    cursor_x = LINE_START_OFFSET;
    cursor_y = LINE_START_OFFSET;
    current_line = 0;
}

// 屏幕向上滚动一行(高度为CHAR_HEIGHT + LINE_SPACING)
void LCD_ScrollUp (uint16_t bg_color) {
    // ST7735S支持硬件滚动，但我们这里用软件实现更通用

    // Step1:将整个屏幕向上移动一行(除了最后一行)
    for (uint16_t y = CHAR_HEIGHT + LINE_SPACING; y < TFT_HEIGHT; y++) {
        for (uint16_t x = 0; x < TFT_WIDTH; x++) {
            // TODO:这里可以优化为直接操作GRAM而不是逐个像素读取
            uint16_t pixel_color = BLUE;  // TODO:需要实现LCD_ReadPixel才能正确获取颜色

            if (y >= (TFT_HEIGHT - (CHAR_HEIGHT + LINE_SPACING))) {
                pixel_color = bg_color;  // 最后几行直接用背景色填充
            }

            LCD_DrawPixel (x, y - (CHAR_HEIGHT + LINE_SPACING), pixel_color);
        }
    }

    // Step2:清除最下面一行(用背景色填充)
    for (uint16_t y = TFT_HEIGHT - (CHAR_HEIGHT + LINE_SPACING); y < TFT_HEIGHT; y++) {
        for (uint16_t x = 0; x < TFT_WIDTH; x++) {
            LCD_DrawPixel (x, y, bg_color);
        }
    }

    current_line--;
}

// 显示字符串(带自动换行和滚动功能)
void LCD_DrawString (const char *str, uint16_t color, uint16_t bg_color) {
    while (*str) {
        // 检查是否需要换行
        if (cursor_x + CHAR_WIDTH > TFT_WIDTH || *str == '\n') {
            cursor_x = LINE_START_OFFSET;
            cursor_y += CHAR_HEIGHT + LINE_SPACING;

            if (cursor_y + CHAR_HEIGHT > TFT_HEIGHT) {
                LCD_ScrollUp (bg_color);
                cursor_y -= CHAR_HEIGHT + LINE_SPACING;
                current_line++;
            }

            if (*str == '\n') {
                str++;
                continue;
            }
        }

        LCD_DrawChar (cursor_x, cursor_y, *str++, color, bg_color);
        cursor_x += CHAR_WIDTH;

        // 处理回车符\r，回到行首但不换行
        if (*str == '\r') {
            str++;
            cursor_x = LINE_START_OFFSET;
        }
    }
}
