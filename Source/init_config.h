#pragma once  //ȷ��ͷ�ļ����ݽ����״ΰ���ʱ��Ч�����������Զ�����.
// Ӳ�����Ӷ���
#define TFT_WIDTH 128  // 1.44����ͷֱ���
#define TFT_HEIGHT 128


// ��Ļ�ߴ�궨�壨�����ʵ����Ļ�޸ģ�
#define LCD_WIDTH 128
#define LCD_HEIGHT 128
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 16

// �ı���ʾ��ز���
#define LINE_START_OFFSET 5                                      // ÿ�п�ʼ��5������
#define LINE_SPACING 2                                           // �м��(����)
#define MAX_LINES ((TFT_HEIGHT) / (CHAR_HEIGHT + LINE_SPACING))  // �����ʾ����

// ȫ�ֱ�����¼��ǰ����λ�ú�״̬
static uint16_t cursor_x = LINE_START_OFFSET;
static uint16_t cursor_y = LINE_START_OFFSET;
static uint16_t current_line = 0;


// ���Ŷ��壨��ʵ������ϸ��Ӧ��
#define LCD_RES_PIN GPIO_Pin_2   // PA2 -> RES
#define LCD_DC_PIN GPIO_Pin_3    // PA3 -> DC
#define LCD_CS_PIN GPIO_Pin_4    // PA4 -> CS
#define LCD_BL_PIN GPIO_Pin_1    // PA1 -> BL��������ƣ�
#define LCD_SCK_PIN GPIO_Pin_5   // PA5 -> SCL��SPIʱ�ӣ�
#define LCD_MOSI_PIN GPIO_Pin_7  // PA7 -> SDA��SPI���ݣ�

// SPI����ѡ��
#define LCD_SPI SPI1

// ������ɫ��RGB565��ʽ��
#define BLACK 0x0000
#define WHITE 0xFFFF
#define BLUE 0xF800
#define GREEN 0x07E0
#define RED 0x001F

// ��������
void LCD_GPIO_Init (void);
void SPI1_Init (void);
void LCD_WriteCommand (uint8_t cmd);
void LCD_WriteData (uint8_t data);
void LCD_Init (void);
void LCD_SetWindow (uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void LCD_Fill (uint16_t color);

// GPIO��ʼ������
void LCD_GPIO_Init (void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // ʹ��GPIOAʱ��
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);

    // ����SPI�������ţ�SCK/MOSI��
    GPIO_InitStructure.GPIO_Pin = LCD_SCK_PIN | LCD_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // �����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    // ���ÿ������ţ�RES/DC/CS/BL��
    GPIO_InitStructure.GPIO_Pin = LCD_RES_PIN | LCD_DC_PIN | LCD_CS_PIN | LCD_BL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // ͨ���������
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    // ��ʼ״̬����
    GPIO_WriteBit (GPIOA, LCD_CS_PIN, Bit_SET);   // Ĭ�ϲ�ѡ��
    GPIO_WriteBit (GPIOA, LCD_RES_PIN, Bit_SET);  // ��λ���Ÿߵ�ƽ
}

// SPI1��ʼ����ģʽ0��MSB���ȣ�
void SPI1_Init (void) {
    SPI_InitTypeDef SPI_InitStructure = {0};

    RCC_APB2PeriphClockCmd (RCC_APB2Periph_SPI1, ENABLE);

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;           // ���߷���
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                       // ����ģʽ
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                   // 8λ����
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                          // ʱ�Ӽ���
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                        // ʱ����λ
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                           // ���NSS
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;  // 18MHz
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                  // MSB����

    SPI_Init (LCD_SPI, &SPI_InitStructure);
    SPI_Cmd (LCD_SPI, ENABLE);  // ʹ��SPI����
}

// �������DC=0��
void LCD_WriteCommand (uint8_t cmd) {
    GPIO_WriteBit (GPIOA, LCD_DC_PIN, Bit_RESET);  // ����ģʽ
    GPIO_WriteBit (GPIOA, LCD_CS_PIN, Bit_RESET);  // ѡ���豸
    SPI_I2S_SendData (LCD_SPI, cmd);
    while (SPI_I2S_GetFlagStatus (LCD_SPI, SPI_I2S_FLAG_TXE) == RESET)
        ;                                        // �ȴ��������
    GPIO_WriteBit (GPIOA, LCD_CS_PIN, Bit_SET);  // ȡ��ѡ��
}

// �������ݣ�DC=1��
void LCD_WriteData (uint8_t data) {
    GPIO_WriteBit (GPIOA, LCD_DC_PIN, Bit_SET);  // ����ģʽ
    GPIO_WriteBit (GPIOA, LCD_CS_PIN, Bit_RESET);
    SPI_I2S_SendData (LCD_SPI, data);
    while (SPI_I2S_GetFlagStatus (LCD_SPI, SPI_I2S_FLAG_TXE) == RESET)
        ;
    GPIO_WriteBit (GPIOA, LCD_CS_PIN, Bit_SET);
}

// ��ʾ����ʼ����ST7735S������
void LCD_Init (void) {
    // Ӳ����λʱ��
    GPIO_WriteBit (GPIOA, LCD_RES_PIN, Bit_RESET);
    Delay_Ms (100);
    GPIO_WriteBit (GPIOA, LCD_RES_PIN, Bit_SET);
    Delay_Ms (120);

    // ��ʼ����������
    LCD_WriteCommand (0x11);  // Sleep out
    Delay_Ms (120);

    LCD_WriteCommand (0xB1);  // ֡�ʿ���
    LCD_WriteData (0x05);
    LCD_WriteData (0x3C);
    LCD_WriteData (0x3C);

    LCD_WriteCommand (0xB2);  // ��ʾ����
    LCD_WriteData (0x05);
    LCD_WriteData (0x3C);
    LCD_WriteData (0x3C);

    LCD_WriteCommand (0xB3);  // ��Դ����
    LCD_WriteData (0x05);
    LCD_WriteData (0x3C);
    LCD_WriteData (0x3C);
    LCD_WriteData (0x05);
    LCD_WriteData (0x3C);
    LCD_WriteData (0x3C);

    LCD_WriteCommand (0xB4);  // ��ʾ��ת����
    LCD_WriteData (0x03);

    LCD_WriteCommand (0x36);  // ��Ļ����0xA0Ϊ������0x00Ϊ������
    LCD_WriteData (0xA0);

    LCD_WriteCommand (0x3A);  // ��ɫ��ʽ
    LCD_WriteData (0x05);     // RGB565

    LCD_WriteCommand (0x29);  // ������ʾ
}

// ������ʾ����
void LCD_SetWindow (uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    LCD_WriteCommand (0x2A);  // �е�ַ����
    LCD_WriteData (x0 >> 8);
    LCD_WriteData (x0 & 0xFF);
    LCD_WriteData (x1 >> 8);
    LCD_WriteData (x1 & 0xFF);

    LCD_WriteCommand (0x2B);  // �е�ַ����
    LCD_WriteData (y0 >> 8);
    LCD_WriteData (y0 & 0xFF);
    LCD_WriteData (y1 >> 8);
    LCD_WriteData (y1 & 0xFF);

    LCD_WriteCommand (0x2C);  // ��ʼд��GRAM
}

// ȫ�������ɫ
void LCD_Fill (uint16_t color) {
    LCD_SetWindow (0, 0, TFT_WIDTH - 1, TFT_HEIGHT - 1);
    for (uint32_t i = 0; i < TFT_WIDTH * TFT_HEIGHT; i++) {
        LCD_WriteData (color >> 8);    // ���ֽ�
        LCD_WriteData (color & 0xFF);  // ���ֽ�
    }
}
