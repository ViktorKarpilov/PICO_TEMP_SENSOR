#include "SSD1306.h"
#include <cstdio>
#include <hardware/i2c.h>

const uint8_t initiated_simple[] = {
    // Border
    0xFF, 0x00,
    
    // I-n-i-t-i-a-t-e-d (compact 8-pixel font)
    0x7F, 0x00,             // I
    0x7C, 0x04, 0x78, 0x00, // n
    0x7D, 0x00,             // i
    0x04, 0x7F, 0x44, 0x00, // t
    0x7D, 0x00,             // i
    0x24, 0x54, 0x78, 0x00, // a
    0x04, 0x7F, 0x44, 0x00, // t
    0x3C, 0x54, 0x5C, 0x00, // e
    0x38, 0x44, 0x7F,       // d
    
    // Border
    0x00, 0xFF
};

void SSD1306::initiate() const
{
    printf("Displaying simple 'Initiated' text\n");
    
    // Set page 3 (middle of screen)
    this->sendCommand(0xB0 + 3);
    this->sendCommand(0x00 | (8 & 0x0F));     // Start at column 8
    this->sendCommand(0x10 | (8 >> 4));
    
    this->sendData(initiated_simple, sizeof(initiated_simple));
}

int SSD1306::draw_vertical_line() const
{
    if (!this->is_turned_on)
    {
        printf("Display wasn't turned on \n");
        return 1;
    }
    
    printf("Drawing vertical line\n");
    this->sendCommand(0x20);  // Set memory addressing mode  
    this->sendCommand(0x02);  // Page addressing mode

    for(int page = 0; page < 8; page++) {
        // Set page address
        this->sendCommand(0xB0 + page);

        // Set column address  
        this->sendCommand(0x00 | (10 & 0x0F));
        this->sendCommand( 0x10 | (10 >> 4));
        
        // Send pixel data (all 8 pixels ON)
        constexpr uint8_t pixel_data[] = {0xFF};
        this -> sendData(pixel_data, 1);
    }

    return 0;
}

void SSD1306::turn_on()
{
    this->sendCommand(0xAF);  // Display ON
    this->sendCommand(0x81);  // Set contrast
    this->sendCommand(0xFF);  // Maximum contrast

    // Fix the horizontal flip
    this->sendCommand(0xA1);  // Set segment remap (0xA0 = normal, 0xA1 = flipped)
    // While we're at it, let's also fix vertical orientation
    this->sendCommand(0xC8);  // Set COM output scan direction (0xC0 = normal, 0xC8 = flipped)

    this->is_turned_on = true;

    initiate();
}

void SSD1306::debug_addressing() const
{
    printf("Testing addressing modes\n");
    
    // FORCE page addressing mode
    this->sendCommand(0x20);  // Set Memory Addressing Mode
    this->sendCommand(0x02);  // 0x02 = Page addressing mode
    
    // Set column address range (just to be sure)
    this->sendCommand(0x21);  // Set column address
    this->sendCommand(0x00);  // Start column = 0
    this->sendCommand(0x7F);  // End column = 127
    
    // Set page address range
    this->sendCommand(0x22);  // Set page address  
    this->sendCommand(0x00);  // Start page = 0
    this->sendCommand(0x07);  // End page = 7
    
    // Now try a simple test: fill just page 2, columns 10-20
    this->sendCommand(0xB0 + 2);  // Set page 2
    this->sendCommand(0x00 | (10 & 0x0F));   // Low nibble of column 10
    this->sendCommand(0x10 | (10 >> 4));     // High nibble of column 10
    
    // Send 10 bytes of 0xFF (should create a solid rectangle)
    for(int i = 0; i < 10; i++) {
        uint8_t pixel = 0xFF;
        this->sendData(&pixel, 1);
    }
}

// Test individual pixel placement
void SSD1306::test_pixel_addressing() const
{
    // Clear everything first
    this->sendCommand(0x20);  // Page addressing mode
    this->sendCommand(0x02);
    
    // Test: put one pixel at exact coordinates
    // Page 3, Column 64 (middle of screen)
    this->sendCommand(0xB0 + 3);
    this->sendCommand(0x00 | (64 & 0x0F));   // Column 64 low
    this->sendCommand(0x10 | (64 >> 4));     // Column 64 high
    
    uint8_t single_pixel = 0x10;  // Just bit 4 (middle of the 8-pixel column)
    this->sendData(&single_pixel, 1);
}

void SSD1306::test_simple_line() const
{
    printf("Testing simple horizontal line\n");
    
    // Clear everything first
    for(int page = 0; page < 8; page++) {
        this->sendCommand(0xB0 + page);
        this->sendCommand(0x00);
        this->sendCommand(0x10);
        for(int col = 0; col < 128; col++) {
            uint8_t clear = 0x00;
            this->sendData(&clear, 1);
        }
    }
    
    // Now draw ONE horizontal line in page 3
    this->sendCommand(0x20);  // Set addressing mode
    this->sendCommand(0x02);  // Page mode
    
    this->sendCommand(0xB0 + 3);  // Page 3
    this->sendCommand(0x00);      // Column 0
    this->sendCommand(0x10);
    
    // Draw 50 pixels in a row
    for(int i = 0; i < 50; i++) {
        uint8_t pixel = 0xFF;  // All 8 bits on
        this->sendData(&pixel, 1);
    }
}
