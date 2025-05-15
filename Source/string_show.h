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

// �������ص�
void LCD_DrawPixel (uint16_t x, uint16_t y, uint16_t color) {
    if (x >= TFT_WIDTH || y >= TFT_HEIGHT)
        return;
    LCD_SetWindow (x, y, x, y);
    LCD_WriteData (color >> 8);
    LCD_WriteData (color & 0xFF);
}

// ��ȡ ASCII �ַ�����ģ����ָ�루�Ż��棩
const uint8_t *GetFontData (char ch) {
    // ����ַ��Ƿ����ֿⷶΧ�ڣ��ո� 0x20 �� 'z' 0x122 ~Ϊ126��
    if (ch < 0x20 || ch > 0x126) {
        return NULL;                    // ��֧���ַ�
    }
    uint16_t index = (ch - 0x20) * 16;  // ͳһ���㹫ʽ
    return &font8x16[index];
}

// ���Ƶ����ַ���8x16���أ�
void LCD_DrawChar (uint8_t x, uint8_t y, char ch, uint16_t color, uint16_t bg_color) {
    const uint8_t *char_data = GetFontData (ch);  // ֱ�ӵ�����������
    if (!char_data)
        return;                                   // ���˲�֧���ַ�

    for (uint8_t row = 0; row < 16; row++) {
        uint8_t pixels = char_data[row];
        for (uint8_t col = 0; col < 8; col++) {
            LCD_DrawPixel (x + col, y + row, (pixels & (0x80 >> col)) ? color : bg_color);
        }
    }
}

// ���������ù��λ��
void LCD_ClearScreen (uint16_t color) {
    LCD_Fill (color);
    cursor_x = LINE_START_OFFSET;
    cursor_y = LINE_START_OFFSET;
    current_line = 0;
}

// ��Ļ���Ϲ���һ��(�߶�ΪCHAR_HEIGHT + LINE_SPACING)
void LCD_ScrollUp (uint16_t bg_color) {
    // ST7735S֧��Ӳ�����������������������ʵ�ָ�ͨ��

    // Step1:��������Ļ�����ƶ�һ��(�������һ��)
    for (uint16_t y = CHAR_HEIGHT + LINE_SPACING; y < TFT_HEIGHT; y++) {
        for (uint16_t x = 0; x < TFT_WIDTH; x++) {
            // TODO:��������Ż�Ϊֱ�Ӳ���GRAM������������ض�ȡ
            uint16_t pixel_color = BLUE;  // TODO:��Ҫʵ��LCD_ReadPixel������ȷ��ȡ��ɫ

            if (y >= (TFT_HEIGHT - (CHAR_HEIGHT + LINE_SPACING))) {
                pixel_color = bg_color;  // �����ֱ���ñ���ɫ���
            }

            LCD_DrawPixel (x, y - (CHAR_HEIGHT + LINE_SPACING), pixel_color);
        }
    }

    // Step2:���������һ��(�ñ���ɫ���)
    for (uint16_t y = TFT_HEIGHT - (CHAR_HEIGHT + LINE_SPACING); y < TFT_HEIGHT; y++) {
        for (uint16_t x = 0; x < TFT_WIDTH; x++) {
            LCD_DrawPixel (x, y, bg_color);
        }
    }

    current_line--;
}

// ��ʾ�ַ���(���Զ����к͹�������)
void LCD_DrawString (const char *str, uint16_t color, uint16_t bg_color) {
    while (*str) {
        // ����Ƿ���Ҫ����
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

        // ����س���\r���ص����׵�������
        if (*str == '\r') {
            str++;
            cursor_x = LINE_START_OFFSET;
        }
    }
}
