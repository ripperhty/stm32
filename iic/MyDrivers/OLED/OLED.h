#ifndef __OLED_H
#define __OLED_H

#include "stm32f1xx_hal.h"

/* 引脚定义：PB8=SCL, PB9=SDA，可自行修改 */
#define OLED_SCL_PIN    GPIO_PIN_8
#define OLED_SDA_PIN    GPIO_PIN_9
#define OLED_GPIO_PORT  GPIOB

/* 函数声明 */
void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

#endif