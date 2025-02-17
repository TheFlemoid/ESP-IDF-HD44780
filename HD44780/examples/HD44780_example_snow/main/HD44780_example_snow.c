/**
 * File:       HD44780_example_snow.c
 * Author:     Franklyn Dahlberg
 * Created:    10 February, 2025
 * Copyright:  2025  Franklyn Dahlberg
 * License:    MIT License (see https://choosealicense.com/licenses/mit/)
 */

/**
 * Prints two lines of inverted * characters to make a crude snow animation.
 * Designed to show basic HD44780 functionality.
 */
#include "HD44780.h"
#include "freertos/FreeRTOS.h"

void app_main(void)
{
    HD44780_FOUR_BIT_BUS bus = { 2, 16, 18, 19, 21, 22, 16, 17 }; 

    HD44780_initFourBitBus(&bus);

    bool pattern = false;

    while (true) {
        HD44780_clear();

        for (int i = 0; i < 20; i+=2) {
            HD44780_setCursorPos(i, (pattern == false));
            HD44780_print("*");
        }

        for (int i = 1; i < 20; i+=2) {
            HD44780_setCursorPos(i, (pattern == true));
            HD44780_print("*");
        }

        pattern = !pattern;

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

