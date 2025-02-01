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
    HD44780_SendData('u');
    HD44780_SendData('r');
    HD44780_SendData(' ');
    HD44780_SendData('B');
    HD44780_SendData('i');
    HD44780_SendData('t');
    HD44780_SendData(' ');
    HD44780_SendData('M');
    HD44780_SendData('o');
    HD44780_SendData('d');
    HD44780_SendData('e');
    HD44780_SendData('!');
}