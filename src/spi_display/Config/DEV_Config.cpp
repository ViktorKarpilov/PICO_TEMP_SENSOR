/*****************************************************************************
* | File      	:   DEV_Config.c
* | Author      :   
* | Function    :   Hardware underlying interface
* | Info        :
*----------------
* |	This version:   V1.1
* | Date        :   2024-09-15
* | Info        :   
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of theex Software, and to permit persons to  whom the Software is
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
#include "DEV_Config.h"

#define SPI_PORT spi1
#define I2C_PORT spi1

uint slice_num;
/**
 * GPIO read and write
**/
void DEV_Digital_Write(UWORD Pin, UBYTE Value)
{
    gpio_put(Pin, Value);
}

UBYTE DEV_Digital_Read(UWORD Pin)
{
    return gpio_get(Pin);
}

/**
 * SPI
**/
void DEV_SPI_WriteByte(uint8_t Value)
{
    spi_write_blocking(SPI_PORT, &Value, 1);
}

void DEV_SPI_Write_nByte(uint8_t pData[], uint32_t Len)
{
    spi_write_blocking(SPI_PORT, pData, Len);
}

/**
 * GPIO Mode
**/
void DEV_GPIO_Mode(UWORD Pin, UWORD Mode)
{
    gpio_init(Pin);
    if(Mode == 0 || Mode == GPIO_IN) {
        gpio_set_dir(Pin, GPIO_IN);
    } else {
        gpio_set_dir(Pin, GPIO_OUT);
    }
}

/**
 * KEY Config
**/
void DEV_KEY_Config(UWORD Pin)
{
    gpio_init(Pin);
	gpio_pull_up(Pin);
    gpio_set_dir(Pin, GPIO_IN);
}

/**
 * delay x ms
**/
void DEV_Delay_ms(UDOUBLE xms)
{
    uint32_t start = to_ms_since_boot(get_absolute_time());
    while (to_ms_since_boot(get_absolute_time()) - start < xms);
}

void DEV_Delay_us(UDOUBLE xus)
{
    uint32_t start = to_us_since_boot(get_absolute_time());
    while (to_us_since_boot(get_absolute_time()) - start < xus);
}



void DEV_GPIO_Init(void)
{
    DEV_GPIO_Mode(LCD_RST_PIN, 1);
    DEV_GPIO_Mode(LCD_DC_PIN, 1);
    DEV_GPIO_Mode(LCD_CS_PIN, 1);
    DEV_GPIO_Mode(LCD_BL_PIN, 1);
    
    
    DEV_GPIO_Mode(LCD_CS_PIN, 1);
    DEV_GPIO_Mode(LCD_BL_PIN, 1);

    DEV_Digital_Write(LCD_CS_PIN, 1);
    DEV_Digital_Write(LCD_DC_PIN, 0);
    DEV_Digital_Write(LCD_BL_PIN, 1);
}
/******************************************************************************
function:	Module Initialize, the library and initialize the pins, SPI protocol
parameter:
Info:
******************************************************************************/
// UBYTE DEV_Module_Init(void)
// {
//     stdio_init_all();   
//     // SPI Config
//     spi_init(SPI_PORT, 10000 * 1000);
//     gpio_set_function(LCD_CLK_PIN, GPIO_FUNC_SPI);
//     gpio_set_function(LCD_MOSI_PIN, GPIO_FUNC_SPI);
//     
//     // GPIO Config
//     DEV_GPIO_Init();
//     
//     
//     // PWM Config
//     gpio_set_function(LCD_BL_PIN, GPIO_FUNC_PWM);
//     slice_num = pwm_gpio_to_slice_num(LCD_BL_PIN);
//     pwm_set_wrap(slice_num, 100);
//     pwm_set_chan_level(slice_num, PWM_CHAN_B, 1);
//     pwm_set_clkdiv(slice_num,50);
//     pwm_set_enabled(slice_num, true);
//     
//     
//     //I2C Config
//     // i2c_init(i2c1,300*1000);
//     // gpio_set_function(LCD_SDA_PIN,GPIO_FUNC_I2C);
//     // gpio_set_function(LCD_SCL_PIN,GPIO_FUNC_I2C);
//     // gpio_pull_up(LCD_SDA_PIN);
//     // gpio_pull_up(LCD_SCL_PIN);
//     
//     printf("DEV_Module_Init OK \r\n");
//     return 0;
// }

UBYTE DEV_Module_Init(void)
{
    stdio_init_all();   
    
    // SPI Config with diagnostics
    printf("Initializing SPI1 at 10MHz...\r\n");
    uint actual_baudrate = spi_init(SPI_PORT, 10000 * 1000);
    printf("SPI actual baudrate: %d Hz\r\n", actual_baudrate);
    
    printf("Setting GPIO functions...\r\n");
    gpio_set_function(LCD_CLK_PIN, GPIO_FUNC_SPI);   // GP10
    gpio_set_function(LCD_MOSI_PIN, GPIO_FUNC_SPI);  // GP11
    printf("SPI pins configured\r\n");
    
    // Test SPI by sending a dummy byte
    printf("Testing SPI communication...\r\n");
    uint8_t test_byte = 0xAA;
    spi_write_blocking(SPI_PORT, &test_byte, 1);
    printf("SPI test write completed\r\n");
    
    // GPIO Config
    DEV_GPIO_Init();
    
    printf("DEV_Module_Init COMPLETED\r\n");
    return 0;
}

void DEV_SET_PWM(uint8_t Value){
    if(Value<0 || Value >100){
        printf("DEV_SET_PWM Error \r\n");
    }else {
        pwm_set_chan_level(slice_num, PWM_CHAN_B, Value);
    }
        
    
    
}

/******************************************************************************
function:	Module exits, closes SPI and BCM2835 library
parameter:
Info:
******************************************************************************/
void DEV_Module_Exit(void)
{

}
