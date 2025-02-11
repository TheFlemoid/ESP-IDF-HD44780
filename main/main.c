#include "HD44780.h"
#include "freertos/FreeRTOS.h"

void app_main(void)
{
    HD44780_FOUR_BIT_BUS bus = { 18, 19, 21, 22, 16, 17 }; 

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
    //bool pattern = false;

    //while (true) {
    //    HD44780_clear();

    //    for (int i = 0; i < 16; i+=2) {
    //        HD44780_setCursorPos(i, (pattern == false));
    //        HD44780_write("*");
    //    }

    //    for (int i = 1; i < 16; i+=2) {
    //        HD44780_setCursorPos(i, (pattern == true));
    //        HD44780_write("*");
    //    }

    //    pattern = !pattern;

    //    vTaskDelay(2000 / portTICK_PERIOD_MS);
    //}
}
