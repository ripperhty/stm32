#include "OLED.h"
#include "OLED_Font.h"

/*==================== 引脚操作宏 ====================*/
#define OLED_W_SCL(x) HAL_GPIO_WritePin(OLED_GPIO_PORT, OLED_SCL_PIN, (GPIO_PinState)(x))
#define OLED_W_SDA(x) HAL_GPIO_WritePin(OLED_GPIO_PORT, OLED_SDA_PIN, (GPIO_PinState)(x))

/*==================== 引脚初始化 ====================*/
void OLED_I2C_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = OLED_SCL_PIN | OLED_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; // 开漏输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(OLED_GPIO_PORT, &GPIO_InitStruct);

    OLED_W_SCL(1);
    OLED_W_SDA(1);
}

/*==================== I2C 时序 ====================*/
void OLED_I2C_Start(void)
{
    OLED_W_SDA(1);
    OLED_W_SCL(1);
    OLED_W_SDA(0);
    OLED_W_SCL(0);
}

void OLED_I2C_Stop(void)
{
    OLED_W_SDA(0);
    OLED_W_SCL(1);
    OLED_W_SDA(1);
}

void OLED_I2C_SendByte(uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        OLED_W_SDA(!!(Byte & (0x80 >> i)));
        OLED_W_SCL(1);
        OLED_W_SCL(0);
    }
    OLED_W_SCL(1); // 额外时钟，不处理应答信号
    OLED_W_SCL(0);
}

/*==================== 写命令 / 写数据 ====================*/
void OLED_WriteCommand(uint8_t Command)
{
    OLED_I2C_Start();
    OLED_I2C_SendByte(0x78); // 从机地址
    OLED_I2C_SendByte(0x00); // 写命令
    OLED_I2C_SendByte(Command);
    OLED_I2C_Stop();
}

void OLED_WriteData(uint8_t Data)
{
    OLED_I2C_Start();
    OLED_I2C_SendByte(0x78); // 从机地址
    OLED_I2C_SendByte(0x40); // 写数据
    OLED_I2C_SendByte(Data);
    OLED_I2C_Stop();
}

/*==================== 设置光标 ====================*/
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
    OLED_WriteCommand(0xB0 | Y);                 // 设置页
    OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4)); // 列高 4 位
    OLED_WriteCommand(0x00 | (X & 0x0F));        // 列低 4 位
}

/*==================== 清屏 ====================*/
void OLED_Clear(void)
{
    uint8_t i, j;
    for (j = 0; j < 8; j++)
    {
        OLED_SetCursor(j, 0);
        for (i = 0; i < 128; i++)
        {
            OLED_WriteData(0x00);
        }
    }
}

/*==================== 显示一个字符 ====================*/
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
    uint8_t i;
    OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8); // 上半部分
    for (i = 0; i < 8; i++)
    {
        OLED_WriteData(OLED_F8x16[Char - ' '][i]);
    }
    OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8); // 下半部分
    for (i = 0; i < 8; i++)
    {
        OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);
    }
}

/*==================== 显示字符串 ====================*/
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++)
    {
        OLED_ShowChar(Line, Column + i, String[i]);
    }
}

/*==================== 次方函数 ====================*/
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
    uint32_t Result = 1;
    while (Y--)
    {
        Result *= X;
    }
    return Result;
}

/*==================== 显示十进制正数 ====================*/
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(Line, Column + i,
                      Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/*==================== 显示带符号十进制数 ====================*/
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
    uint8_t i;
    uint32_t Number1;
    if (Number >= 0)
    {
        OLED_ShowChar(Line, Column, '+');
        Number1 = Number;
    }
    else
    {
        OLED_ShowChar(Line, Column, '-');
        Number1 = -Number;
    }
    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(Line, Column + i + 1,
                      Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
    }
}

/*==================== 显示十六进制数 ====================*/
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i, SingleNumber;
    for (i = 0; i < Length; i++)
    {
        SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
        if (SingleNumber < 10)
        {
            OLED_ShowChar(Line, Column + i, SingleNumber + '0');
        }
        else
        {
            OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
        }
    }
}

/*==================== 显示二进制数 ====================*/
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
    uint8_t i;
    for (i = 0; i < Length; i++)
    {
        OLED_ShowChar(Line, Column + i,
                      Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
    }
}

/*==================== OLED 初始化 ====================*/
void OLED_Init(void)
{
    HAL_Delay(100); // 上电延时

    OLED_I2C_Init(); // 端口初始化

    OLED_WriteCommand(0xAE); // 关闭显示

    OLED_WriteCommand(0xD5); // 设置显示时钟分频比/振荡器频率
    OLED_WriteCommand(0x80);

    OLED_WriteCommand(0xA8); // 设置多路复用率
    OLED_WriteCommand(0x3F);

    OLED_WriteCommand(0xD3); // 设置显示偏移
    OLED_WriteCommand(0x00);

    OLED_WriteCommand(0x40); // 设置显示开始行

    OLED_WriteCommand(0xA1); // 左右方向，0xA1 正常

    OLED_WriteCommand(0xC8); // 上下方向，0xC8 正常

    OLED_WriteCommand(0xDA); // COM 引脚硬件配置
    OLED_WriteCommand(0x12);

    OLED_WriteCommand(0x81); // 对比度控制
    OLED_WriteCommand(0xCF);

    OLED_WriteCommand(0xD9); // 预充电周期
    OLED_WriteCommand(0xF1);

    OLED_WriteCommand(0xDB); // VCOMH 取消选择级别
    OLED_WriteCommand(0x30);

    OLED_WriteCommand(0xA4); // 整个显示打开/关闭

    OLED_WriteCommand(0xA6); // 正常/倒转显示

    OLED_WriteCommand(0x8D); // 电荷泵
    OLED_WriteCommand(0x14);

    OLED_WriteCommand(0xAF); // 开启显示

    OLED_Clear(); // 清屏
}