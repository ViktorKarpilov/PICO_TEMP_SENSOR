//
// Created by PC on 7/5/2025.
//

#include "OLED13.h"

#include <cstdio>
#include <cstdlib>

#include "ImageData.h"
#include "Config/DEV_Config.h"
#include "GUI/GUI_Paint.h"
#include "OLED/OLED_1in3_c.h"

void OLED_Reset()
{
    OLED_RST_1;
    DEV_Delay_ms(100);
    OLED_RST_0;
    DEV_Delay_ms(100);
    OLED_RST_1;
    DEV_Delay_ms(100);
}

int OLED_1in3_C_test()
{
    DEV_Delay_ms(100);
    
    
    if(DEV_Module_Init()!=0){
        while(true){
            printf("END\r\n");
        }
    }
    
    OLED_1in3_C_Init();
    
    OLED_1in3_C_Clear();

    UBYTE *BlackImage;
    if (UWORD Imagesize = ((OLED_1in3_C_WIDTH % 8 == 0) ? (OLED_1in3_C_WIDTH / 8) : (OLED_1in3_C_WIDTH / 8 + 1)) *
        OLED_1in3_C_HEIGHT; (BlackImage = static_cast<uint8_t*>(malloc(Imagesize))) == nullptr)
    {
        while(true){
            printf("Failed to apply for black memory...\r\n");
        }
    }
    // printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, OLED_1in3_C_WIDTH, OLED_1in3_C_HEIGHT, 0, WHITE);	
    
    
    
    // printf("Drawing\r\n");
    // //1.Select Image
    // Paint_SelectImage(BlackImage);
    // DEV_Delay_ms(500);
    // Paint_Clear(BLACK);
    //
    // // 2.Drawing on the image
    // printf("Drawing:page 1\r\n");
    // Paint_DrawPoint(20, 10, WHITE, DOT_PIXEL_1X1, DOT_STYLE_DFT);
    // Paint_DrawPoint(30, 10, WHITE, DOT_PIXEL_2X2, DOT_STYLE_DFT);
    // Paint_DrawPoint(40, 10, WHITE, DOT_PIXEL_3X3, DOT_STYLE_DFT);
    // Paint_DrawLine(10, 10, 10, 20, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    // Paint_DrawLine(20, 20, 20, 30, WHITE, DOT_PIXEL_1X1, LINE_STYLE_SOLID);
    // Paint_DrawLine(30, 30, 30, 40, WHITE, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    // Paint_DrawLine(40, 40, 40, 50, WHITE, DOT_PIXEL_1X1, LINE_STYLE_DOTTED);
    // Paint_DrawCircle(60, 30, 15, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    // Paint_DrawCircle(100, 40, 20, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    // Paint_DrawRectangle(50, 30, 60, 40, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
    // Paint_DrawRectangle(90, 30, 110, 50, BLACK, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    // BlackImage[0] = 0xf0;
    // // 3.Show image on page1
    // OLED_1in3_C_Display(BlackImage);
    // DEV_Delay_ms(2000);
    // Paint_Clear(BLACK);
    
    
    
    // Drawing on the image
    printf("Drawing:page 2\r\n");
    Paint_DrawString_EN(10, 0, "Pico-OLED", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(10, 17, "hello world", &Font8, WHITE, BLACK);
    Paint_DrawNum(10, 30, 123.1, &Font8,2, WHITE, BLACK);
    Paint_DrawNum(10, 43, 987654.2, &Font12,2, WHITE, BLACK);
    // Show image on page2
    OLED_1in3_C_Display(BlackImage);
    DEV_Delay_ms(2000);
    Paint_Clear(BLACK);

    return 0;
    
    // Drawing on the image
    // printf("Drawing:page 3\r\n");
    // // Show image on page3
    // OLED_1in3_C_Display(BlackImage);
    // DEV_Delay_ms(2000);
    //
    // // Show image on page4
    // OLED_1in3_C_Display(gImage_1inch3_C_1);
    // DEV_Delay_ms(5000);
    //
    //
    // Paint_NewImage(BlackImage, OLED_1in3_C_WIDTH, OLED_1in3_C_HEIGHT, 180, WHITE);	
    // Paint_Clear(BLACK);
    // int key0 = 15; 
    // int key1 = 17;
    // int key = 0;
    // DEV_GPIO_Mode(key0, 0);
    // DEV_GPIO_Mode(key1, 0);
    //
    // Paint_Clear(BLACK);
    // OLED_1in3_C_Display(BlackImage);
    //
    //
    // // while(true){
    // //     if(DEV_Digital_Read(key1 ) == 0){
    // //         Paint_DrawRectangle(115, 5, 125, 15, WHITE, DOT_PIXEL_2X2,DRAW_FILL_FULL);
    // //         key = 1;
    // //     }else {
    // //         Paint_DrawRectangle(115, 5, 125, 15, BLACK, DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
    // //     }
    // //         
    // //     if(DEV_Digital_Read(key0 ) == 0){
    // //         Paint_DrawRectangle(115, 50, 125, 60, WHITE, DOT_PIXEL_2X2,DRAW_FILL_FULL);
    // //     }else {
    // //         Paint_DrawRectangle(115, 50, 125, 60, BLACK, DOT_PIXEL_2X2,DRAW_FILL_EMPTY);
    // //         key = 1;
    // //         
    // //     }
    // //     if(key == 1){
    // //         OLED_1in3_C_Display(BlackImage);
    // //         Paint_Clear(BLACK);
    // //     }
    // //     
    // // }
}

int OLED_Init()
{
    if(DEV_Module_Init()!=0){
        while(true){
            printf("END\r\n");
        }
    }
    
    OLED_1in3_C_Init();
    
    OLED_1in3_C_Clear();

    UBYTE *BlackImage;
    if (UWORD Imagesize = ((OLED_1in3_C_WIDTH % 8 == 0) ? (OLED_1in3_C_WIDTH / 8) : (OLED_1in3_C_WIDTH / 8 + 1)) *
        OLED_1in3_C_HEIGHT; (BlackImage = static_cast<uint8_t*>(malloc(Imagesize))) == nullptr)
    {
        while(true){
            printf("Failed to apply for black memory...\r\n");
        }
    }
    Paint_NewImage(BlackImage, OLED_1in3_C_WIDTH, OLED_1in3_C_HEIGHT, 0, WHITE);	
    
    // Drawing on the image
    printf("Drawing:page 2\r\n");
    Paint_DrawString_EN(10, 0, "Pico-OLED", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(10, 17, "hello world", &Font8, WHITE, BLACK);
    Paint_DrawNum(10, 30, 123.1, &Font8,2, WHITE, BLACK);
    Paint_DrawNum(10, 43, 987654.2, &Font12,2, WHITE, BLACK);
    // Show image on page2
    OLED_1in3_C_Display(BlackImage);
    // DEV_Delay_ms(2000);
    // Paint_Clear(RED);

    return 0;

}

