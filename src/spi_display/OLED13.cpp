//
// Created by PC on 7/5/2025.
//

#include "OLED13.h"

#include <cstdio>
#include <cstdlib>

#include "ImageData.h"
#include "Config/DEV_Config.h"
#include "GUI/GUI_Paint.h"
#include "hardware/stdio/stdio.h"
#include "OLED/OLED_1in3_c.h"

UBYTE* BlackImage = nullptr;
// void OLED_Reset()
// {
//     OLED_RST_1;
//     DEV_Delay_ms(100);
//     OLED_RST_0;
//     DEV_Delay_ms(100);
//     OLED_RST_1;
//     DEV_Delay_ms(100);
// }

int OLED_Init()
{
    if(DEV_Module_Init()!=0){
        while(true){
            log("END\r\n");
        }
    }
    
    OLED_1in3_C_Init();
    
    OLED_1in3_C_Clear();

    if (UWORD Imagesize = ((OLED_1in3_C_WIDTH % 8 == 0) ? (OLED_1in3_C_WIDTH / 8) : (OLED_1in3_C_WIDTH / 8 + 1)) *
        OLED_1in3_C_HEIGHT; (BlackImage = static_cast<uint8_t*>(malloc(Imagesize))) == nullptr)
    {
        while(true){
            log("Failed to apply for black memory...\r\n");
        }
    }
    Paint_NewImage(BlackImage, OLED_1in3_C_WIDTH, OLED_1in3_C_HEIGHT, 0, WHITE);	
    
    // Drawing on the image
    log("Drawing:page 2\r\n");
    Paint_DrawString_EN(10, 0, "Pico-OLED", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(10, 17, "hello world", &Font8, WHITE, BLACK);
    Paint_DrawNum(10, 30, 123.1, &Font8,2, WHITE, BLACK);
    Paint_DrawNum(10, 43, 987654.2, &Font12,2, WHITE, BLACK);

    // Show image on page2
    OLED_1in3_C_Display(BlackImage);
    Paint_Clear(BLACK);

    return 0;

}

