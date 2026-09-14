#ifndef __OLED_H
#define __OLED_H

#include "main.h"
#include "i2c.h"

#define OLED_ADDR 0x78
#define OLED_CMD 0x00
#define OLED_DATA 0X40

HAL_StatusTypeDef OLED_WriteCommand(uint8_t cmd);
void OLED_WriteData(uint8_t data);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_SetCursor(uint8_t page, uint8_t col);
void OLED_Lamp(void);
void OLED_DrawPixel(uint8_t x,uint8_t y);
void OLED_ClearPixel(uint8_t x, uint8_t y);
void OLED_Refresh(void);
void OLED_badapple(void);
#endif
