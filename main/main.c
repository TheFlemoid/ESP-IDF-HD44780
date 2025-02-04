#include "HD44780.h"
#include "freertos/FreeRTOS.h"

void app_main(void)
{
    HD44780_FOUR_BIT_BUS bus = { GPIO_NUM_18, GPIO_NUM_19, GPIO_NUM_21, GPIO_NUM_22, 
                                 GPIO_NUM_16, GPIO_NUM_17 };

    HD44780_initFourBitBus(&bus);

    HD44780_setCursorPos(3,0);
    HD44780_print("This is a");
    HD44780_setCursorPos(6,1);
    HD44780_print("test");

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
