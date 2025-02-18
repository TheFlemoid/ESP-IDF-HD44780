/**
 * File:       HD44780_example_scroll.c
 * Author:     Franklyn Dahlberg
 * Created:    10 February, 2025
 * Copyright:  2025 (c) Franklyn Dahlberg
 * License:    MIT License (see https://choosealicense.com/licenses/mit/)
 */

/**
 * Example for a 2x16 HD44780 display that shows a two line string with two 
 * special characters that moves left to right on a loop.  Designed to 
 * show basic HD44780 functionality.
 */
#include "HD44780.h"
#include "freertos/FreeRTOS.h"

void app_main(void)
{
    HD44780_FOUR_BIT_BUS bus = { 2, 16, 18, 19, 21, 22, 16, 17 }; 

    HD44780_initFourBitBus(&bus);

    uint8_t smileyChar[8] = {
        0b00000,
        0b01010,
        0b01010,
        0b00000,
        0b10001,
        0b10001,
        0b01110,
        0b00000
    };

    uint8_t invertSmileyChar[8] = {
        0b11111,
        0b10101,
        0b10101,
        0b11111,
        0b01110,
        0b01110,
        0b10001,
        0b11111
    };
    
    HD44780_createChar(0, smileyChar);
    HD44780_createChar(1, invertSmileyChar);

    HD44780_setCursorPos(3,0);
    HD44780_print("This is a");
    HD44780_setCursorPos(3,1);
    HD44780_writeChar(0);
    HD44780_print("  test ");
    HD44780_writeChar(1);

    const uint32_t shiftDelay = 500 / portTICK_PERIOD_MS;

    for (int i = 0; i < 4; i++) {
        HD44780_shiftDispRight();
        vTaskDelay(shiftDelay);
    }

    for (int i = 0; i < 7; i++) {
        HD44780_shiftDispLeft();
        vTaskDelay(shiftDelay);
    }

    while (true) {
        for (int i = 0; i < 7; i++) {
            HD44780_shiftDispRight();
            vTaskDelay(shiftDelay);
        }

        for (int i = 0; i < 7; i++) {
            HD44780_shiftDispLeft();
            vTaskDelay(shiftDelay);
        }
    }
}

