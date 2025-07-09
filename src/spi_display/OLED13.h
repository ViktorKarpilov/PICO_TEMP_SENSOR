//
// Created by PC on 7/5/2025.
//

#ifndef OLED13_H
#define OLED13_H

#define USE_SPI 1
#define USE_IIC 0

#define IIC_CMD        0X00
#define IIC_RAM        0X40


#define OLED_1in3_C_WIDTH  128//OLED width
#define OLED_1in3_C_HEIGHT 64 //OLED height

#define OLED_CS_0      DEV_Digital_Write(LCD_CS_PIN,0)
#define OLED_CS_1      DEV_Digital_Write(LCD_CS_PIN,1)

#define OLED_RST_0      DEV_Digital_Write(LCD_RST_PIN,0)
#define OLED_RST_1      DEV_Digital_Write(LCD_RST_PIN,1)

#define OLED_DC_0       DEV_Digital_Write(LCD_DC_PIN,0)
#define OLED_DC_1       DEV_Digital_Write(LCD_DC_PIN,1)
#include "Config/DEV_Config.h"

int OLED_1in3_C_test(void);
void Paint_Clear(UWORD Color);
int OLED_Init();


#endif //OLED13_H
