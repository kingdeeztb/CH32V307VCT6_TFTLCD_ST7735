#pragma once  //确保头文件内容仅在首次包含时生效，后续包含自动跳过.
// 硬件连接定义
#define TFT_WIDTH 128  // 1.44寸典型分辨率
#define TFT_HEIGHT 128


// 屏幕尺寸宏定义（需根据实际屏幕修改）
#define LCD_WIDTH 128
#define LCD_HEIGHT 128
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 16

// 文本显示相关参数
#define LINE_START_OFFSET 5                                      // 每行开始空5个像素
#define LINE_SPACING 2                                           // 行间距(像素)
#define MAX_LINES ((TFT_HEIGHT) / (CHAR_HEIGHT + LINE_SPACING))  // 最大显示行数

// 全局变量记录当前绘制位置和状态
static uint16_t cursor_x = LINE_START_OFFSET;
static uint16_t cursor_y = LINE_START_OFFSET;
static uint16_t current_line = 0;


// 引脚定义（与实物接线严格对应）
#define LCD_RES_PIN GPIO_Pin_2   // PA2 -> RES
#define LCD_DC_PIN GPIO_Pin_3    // PA3 -> DC
#define LCD_CS_PIN GPIO_Pin_4    // PA4 -> CS
#define LCD_BL_PIN GPIO_Pin_1    // PA1 -> BL（背光控制）
#define LCD_SCK_PIN GPIO_Pin_5   // PA5 -> SCL（SPI时钟）
#define LCD_MOSI_PIN GPIO_Pin_7  // PA7 -> SDA（SPI数据）

// SPI外设选择
#define LCD_SPI SPI1

// 常用颜色（RGB565格式）
#define BLACK 0x0000
#define WHITE 0xFFFF
#define BLUE 0xF800
#define GREEN 0x07E0
#define RED 0x001F

// 函数声明
void LCD_GPIO_Init (void);
void SPI1_Init (void);
void LCD_WriteCommand (uint8_t cmd);
void LCD_WriteData (uint8_t data);
void LCD_Init (void);
void LCD_SetWindow (uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void LCD_Fill (uint16_t color);

// GPIO初始化函数
void LCD_GPIO_Init (void) {
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);

    // 配置SPI复用引脚（SCK/MOSI）
    GPIO_InitStructure.GPIO_Pin = LCD_SCK_PIN | LCD_MOSI_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    // 配置控制引脚（RES/DC/CS/BL）
    GPIO_InitStructure.GPIO_Pin = LCD_RES_PIN | LCD_DC_PIN | LCD_CS_PIN | LCD_BL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 通用推挽输出
    GPIO_Init (GPIOA, &GPIO_InitStructure);

    // 初始状态设置
    GPIO_WriteBit (GPIOA, LCD_CS_PIN, Bit_SET);   // 默认不选中
    GPIO_WriteBit (GPIOA, LCD_RES_PIN, Bit_SET);  // 复位引脚高电平
}

// SPI1初始化（模式0，MSB优先）
void SPI1_Init (void) {
    SPI_InitTypeDef SPI_InitStructure = {0};

    RCC_APB2PeriphClockCmd (RCC_APB2Periph_SPI1, ENABLE);

    SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;           // 单线发送
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                       // 主机模式
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                   // 8位数据
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                          // 时钟极性
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                        // 时钟相位
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                           // 软件NSS
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;  // 18MHz
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                  // MSB优先

    SPI_Init (LCD_SPI, &SPI_InitStructure);
    SPI_Cmd (LCD_SPI, ENABLE);  // 使能SPI外设
}

// 发送命令（DC=0）
void LCD_WriteCommand (uint8_t cmd) {
    GPIO_WriteBit (GPIOA, LCD_DC_PIN, Bit_RESET);  // 命令模式
    GPIO_WriteBit (GPIOA, LCD_CS_PIN, Bit_RESET);  // 选中设备
    SPI_I2S_SendData (LCD_SPI, cmd);
    while (SPI_I2S_GetFlagStatus (LCD_SPI, SPI_I2S_FLAG_TXE) == RESET)
        ;                                        // 等待发送完成
    GPIO_WriteBit (GPIOA, LCD_CS_PIN, Bit_SET);  // 取消选中
}

// 发送数据（DC=1）
void LCD_WriteData (uint8_t data) {
    GPIO_WriteBit (GPIOA, LCD_DC_PIN, Bit_SET);  // 数据模式
    GPIO_WriteBit (GPIOA, LCD_CS_PIN, Bit_RESET);
    SPI_I2S_SendData (LCD_SPI, data);
    while (SPI_I2S_GetFlagStatus (LCD_SPI, SPI_I2S_FLAG_TXE) == RESET)
        ;
    GPIO_WriteBit (GPIOA, LCD_CS_PIN, Bit_SET);
}

// 显示屏初始化（ST7735S驱动）
void LCD_Init (void) {
    // 硬件复位时序
    GPIO_WriteBit (GPIOA, LCD_RES_PIN, Bit_RESET);
    Delay_Ms (100);
    GPIO_WriteBit (GPIOA, LCD_RES_PIN, Bit_SET);
    Delay_Ms (120);

    // 初始化命令序列
    LCD_WriteCommand (0x11);  // Sleep out
    Delay_Ms (120);

    LCD_WriteCommand (0xB1);  // 帧率控制
    LCD_WriteData (0x05);
    LCD_WriteData (0x3C);
    LCD_WriteData (0x3C);

    LCD_WriteCommand (0xB2);  // 显示配置
    LCD_WriteData (0x05);
    LCD_WriteData (0x3C);
    LCD_WriteData (0x3C);

    LCD_WriteCommand (0xB3);  // 电源配置
    LCD_WriteData (0x05);
    LCD_WriteData (0x3C);
    LCD_WriteData (0x3C);
    LCD_WriteData (0x05);
    LCD_WriteData (0x3C);
    LCD_WriteData (0x3C);

    LCD_WriteCommand (0xB4);  // 显示反转控制
    LCD_WriteData (0x03);

    LCD_WriteCommand (0x36);  // 屏幕方向（0xA0为竖屏，0x00为横屏）
    LCD_WriteData (0xA0);

    LCD_WriteCommand (0x3A);  // 颜色格式
    LCD_WriteData (0x05);     // RGB565

    LCD_WriteCommand (0x29);  // 开启显示
}

// 设置显示窗口
void LCD_SetWindow (uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    LCD_WriteCommand (0x2A);  // 列地址设置
    LCD_WriteData (x0 >> 8);
    LCD_WriteData (x0 & 0xFF);
    LCD_WriteData (x1 >> 8);
    LCD_WriteData (x1 & 0xFF);

    LCD_WriteCommand (0x2B);  // 行地址设置
    LCD_WriteData (y0 >> 8);
    LCD_WriteData (y0 & 0xFF);
    LCD_WriteData (y1 >> 8);
    LCD_WriteData (y1 & 0xFF);

    LCD_WriteCommand (0x2C);  // 开始写入GRAM
}

// 全屏填充颜色
void LCD_Fill (uint16_t color) {
    LCD_SetWindow (0, 0, TFT_WIDTH - 1, TFT_HEIGHT - 1);
    for (uint32_t i = 0; i < TFT_WIDTH * TFT_HEIGHT; i++) {
        LCD_WriteData (color >> 8);    // 高字节
        LCD_WriteData (color & 0xFF);  // 低字节
    }
}
