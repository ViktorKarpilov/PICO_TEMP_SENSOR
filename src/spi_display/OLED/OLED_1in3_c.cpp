/*****************************************************************************
* | File        :   OLED_1in3_c.c
* | Author      :   
* | Function    :   1.3inch OLED  Drive function
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2021-03-16
* | Info        :
* -----------------------------------------------------------------------------
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
******************************************************************************/
#include "OLED_1in3_c.h"
#include "stdio.h"
#include "src/spi_display/Config/DEV_Config.h"

/*******************************************************************************
function:
			Hardware reset
*******************************************************************************/
static void OLED_Reset(void)
{
    OLED_RST_1;
    DEV_Delay_ms(100);
    OLED_RST_0;
    DEV_Delay_ms(100);
    OLED_RST_1;
    DEV_Delay_ms(100);
}

/*******************************************************************************
function:
			Write register address and data
*******************************************************************************/
static void OLED_WriteReg(uint8_t Reg)
{
#if USE_SPI
    OLED_DC_0;
    OLED_CS_0;
    DEV_SPI_WriteByte(Reg);
    OLED_CS_1;
#elif USE_IIC
    I2C_Write_Byte(Reg,IIC_CMD);
#endif
}

static void OLED_WriteData(uint8_t Data)
{	
#if USE_SPI
    OLED_DC_1;
    OLED_CS_0;
    DEV_SPI_WriteByte(Data);
    OLED_CS_1;
#elif USE_IIC
    I2C_Write_Byte(Data,IIC_RAM);
#endif
}

/*******************************************************************************
function:
			Common register initialization
*******************************************************************************/
// static void OLED_InitReg(void)
// {
//     OLED_WriteReg(0xae);	/*turn off OLED display*/
//
//     OLED_WriteReg(0x00);	/*set lower column address*/ 
//     OLED_WriteReg(0x10);	/*set higher column address*/ 
//
//     OLED_WriteReg(0xB0);	/*set page address*/ 
// 	
//     OLED_WriteReg(0xdc);	/*set display start line*/ 
//     OLED_WriteReg(0x00);  
//
//     OLED_WriteReg(0x81);	/*contract control*/ 
//     OLED_WriteReg(0x6f);	/*128*/ 
//     OLED_WriteReg(0x21);  	/* Set Memory addressing mode (0x20/0x21) */ 
// 	
//     OLED_WriteReg(0xa0); 	/*set segment remap*/ 
//     OLED_WriteReg(0xc0);  	/*Com scan direction*/ 
//     OLED_WriteReg(0xa4);	/*Disable Entire Display On (0xA4/0xA5)*/ 
//
//     OLED_WriteReg(0xa6);	/*normal / reverse*/
//     OLED_WriteReg(0xa8);	/*multiplex ratio*/ 
//     OLED_WriteReg(0x3f);  	/*duty = 1/64*/ 
//   
//     OLED_WriteReg(0xd3);  	/*set display offset*/ 
//     OLED_WriteReg(0x60);
//
//     OLED_WriteReg(0xd5);	/*set osc division*/ 
//     OLED_WriteReg(0x41);
// 		
//     OLED_WriteReg(0xd9); 	/*set pre-charge period*/ 
//     OLED_WriteReg(0x22);   
//
//     OLED_WriteReg(0xdb); 	/*set vcomh*/ 
//     OLED_WriteReg(0x35);  
//
//     OLED_WriteReg(0xad); 	/*set charge pump enable*/ 
//     OLED_WriteReg(0x8a);	/*Set DC-DC enable (a=0:disable; a=1:enable) */
// }

static void OLED_InitReg(void)
{
    printf("Writing 0xae (turn off OLED display)...\r\n");
    OLED_WriteReg(0xae);	/*turn off OLED display*/
    
    printf("Writing 0x00 (set lower column address)...\r\n");
    OLED_WriteReg(0x00);	/*set lower column address*/ 
    
    printf("Writing 0x10 (set higher column address)...\r\n");
    OLED_WriteReg(0x10);	/*set higher column address*/ 
    
    printf("Writing 0xB0 (set page address)...\r\n");
    OLED_WriteReg(0xB0);	/*set page address*/ 
	
    printf("Writing 0xdc (set display start line)...\r\n");
    OLED_WriteReg(0xdc);	/*set display start line*/ 
    
    printf("Writing 0x00 (display start line value)...\r\n");
    OLED_WriteReg(0x00);  
    
    printf("Writing 0x81 (contract control)...\r\n");
    OLED_WriteReg(0x81);	/*contract control*/ 
    
    printf("Writing 0x6f (contrast value 128)...\r\n");
    OLED_WriteReg(0x6f);	/*128*/ 
    
    printf("Writing 0x21 (Set Memory addressing mode)...\r\n");
    OLED_WriteReg(0x21);  	/* Set Memory addressing mode (0x20/0x21) */ 
	
    printf("Writing 0xa0 (set segment remap)...\r\n");
    OLED_WriteReg(0xa0); 	/*set segment remap*/ 
    
    printf("Writing 0xc0 (Com scan direction)...\r\n");
    OLED_WriteReg(0xc0);  	/*Com scan direction*/ 
    
    printf("Writing 0xa4 (Disable Entire Display On)...\r\n");
    OLED_WriteReg(0xa4);	/*Disable Entire Display On (0xA4/0xA5)*/ 
    
    printf("Writing 0xa6 (normal/reverse)...\r\n");
    OLED_WriteReg(0xa6);	/*normal / reverse*/
    
    printf("Writing 0xa8 (multiplex ratio)...\r\n");
    OLED_WriteReg(0xa8);	/*multiplex ratio*/ 
    
    printf("Writing 0x3f (duty = 1/64)...\r\n");
    OLED_WriteReg(0x3f);  	/*duty = 1/64*/ 
  
    printf("Writing 0xd3 (set display offset)...\r\n");
    OLED_WriteReg(0xd3);  	/*set display offset*/ 
    
    printf("Writing 0x60 (display offset value)...\r\n");
    OLED_WriteReg(0x60);
    
    printf("Writing 0xd5 (set osc division)...\r\n");
    OLED_WriteReg(0xd5);	/*set osc division*/ 
    
    printf("Writing 0x41 (osc division value)...\r\n");
    OLED_WriteReg(0x41);
		
    printf("Writing 0xd9 (set pre-charge period)...\r\n");
    OLED_WriteReg(0xd9); 	/*set pre-charge period*/ 
    
    printf("Writing 0x22 (pre-charge value)...\r\n");
    OLED_WriteReg(0x22);   
    
    printf("Writing 0xdb (set vcomh)...\r\n");
    OLED_WriteReg(0xdb); 	/*set vcomh*/ 
    
    printf("Writing 0x35 (vcomh value)...\r\n");
    OLED_WriteReg(0x35);  
    
    printf("Writing 0xad (set charge pump enable)...\r\n");
    OLED_WriteReg(0xad); 	/*set charge pump enable*/ 
    
    printf("Writing 0x8a (Set DC-DC enable)...\r\n");
    OLED_WriteReg(0x8a);	/*Set DC-DC enable (a=0:disable; a=1:enable) */
    
    printf("OLED_InitReg() COMPLETED SUCCESSFULLY!\r\n");
}


/********************************************************************************
function:
			initialization
********************************************************************************/
void OLED_1in3_C_Init()
{
    printf("Starting OLED_Reset()...\r\n");
    //Hardware reset
    OLED_Reset();
    printf("OLED_Reset() completed\r\n");
    
    printf("Starting OLED_InitReg()...\r\n");
    //Set the initialization register
    OLED_InitReg();
    printf("OLED_InitReg() completed\r\n");
    
    printf("Starting 200ms delay...\r\n");
    DEV_Delay_ms(200);
    printf("Delay completed\r\n");
    
    printf("Starting OLED_WriteReg(0xaf)...\r\n");
    //Turn on the OLED display
    OLED_WriteReg(0xaf);
    printf("OLED_WriteReg(0xaf) completed\r\n");
    
    printf("OLED_1in3_C_Init() FULLY COMPLETED\r\n");
}


/********************************************************************************
function:
			Clear screen
********************************************************************************/
void OLED_1in3_C_Clear()
{
	UWORD Width, Height, column;
	Width = (OLED_1in3_C_WIDTH % 8 == 0)? (OLED_1in3_C_WIDTH / 8 ): (OLED_1in3_C_WIDTH / 8 + 1);
	Height = OLED_1in3_C_HEIGHT;  
	OLED_WriteReg(0xb0); 	//Set the row  start address
	for (UWORD j = 0; j < Height; j++) {
		column = 63 - j;
		OLED_WriteReg(0x00 + (column & 0x0f));  //Set column low start address
		OLED_WriteReg(0x10 + (column >> 4));  //Set column higt start address
		for (UWORD i = 0; i < Width; i++) {
			OLED_WriteData(0x00);
		 }
	}
}

/********************************************************************************
function:   
            reverse a byte data
********************************************************************************/
static UBYTE reverse(UBYTE temp)
{
    temp = ((temp & 0x55) << 1) | ((temp & 0xaa) >> 1);
    temp = ((temp & 0x33) << 2) | ((temp & 0xcc) >> 2);
    temp = ((temp & 0x0f) << 4) | ((temp & 0xf0) >> 4);  
    return temp;
}

/********************************************************************************
function:	
			Update all memory to OLED
********************************************************************************/
void OLED_1in3_C_Display(const UBYTE *Image)
{		
    UWORD Width, Height, column, temp;
    Width = (OLED_1in3_C_WIDTH % 8 == 0)? (OLED_1in3_C_WIDTH / 8 ): (OLED_1in3_C_WIDTH / 8 + 1);
    Height = OLED_1in3_C_HEIGHT;   
    OLED_WriteReg(0xb0); 	//Set the row  start address
    for (UWORD j = 0; j < Height; j++) {
        column = 63 - j;
        OLED_WriteReg(0x00 + (column & 0x0f));  //Set column low start address
        OLED_WriteReg(0x10 + (column >> 4));  //Set column higt start address
        for (UWORD i = 0; i < Width; i++) {
            temp = Image[i + j * Width];
            // printf("0x%x \r\n",temp);
            temp = reverse(temp);	//reverse the buffer
            OLED_WriteData(temp);
         }
    }   
}


