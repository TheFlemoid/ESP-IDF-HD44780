#include <stdio.h>
#include "HD44780.h"
#include "freertos/FreeRTOS.h"

void app_main(void)
{
    HD44780_FOUR_BIT_BUS bus = { GPIO_NUM_23, GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27, 
                                 GPIO_NUM_16, GPIO_NUM_17 };

    HD44780_InitFourBitBus(&bus);

    HD44780_SendData('F');
    HD44780_SendData('o');
    HD44780_SendData('U');
    HD44780_SendData('r');
    HD44780_SendData(' ');
    HD44780_SendData('B');
    HD44780_SendData('i');
    HD44780_SendData('T');
    HD44780_SendData(' ');
    HD44780_SendData('m');
    HD44780_SendData('O');
    HD44780_SendData('d');
    HD44780_SendData('E');
    HD44780_SendData('!');
    HD44780_SendData('!');
}
