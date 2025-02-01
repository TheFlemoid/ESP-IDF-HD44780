#include <stdio.h>
#include "HD44780.h"
#include "freertos/FreeRTOS.h"

void app_main(void)
{
    const uint32_t twentyMilliDelay = 20 / portTICK_PERIOD_MS;
    const uint32_t fiveMilliDelay = 5 / portTICK_PERIOD_MS;

    //HD44780_FOUR_BIT_BUS bus = { GPIO_NUM_33, GPIO_NUM_32, GPIO_NUM_18, GPIO_NUM_19, 
    //                            GPIO_NUM_26, GPIO_NUM_25 };

    //HD44780_InitFourBitBus(&bus);

    HD44780_EIGHT_BIT_BUS bus = { GPIO_NUM_18, GPIO_NUM_19, GPIO_NUM_21, GPIO_NUM_22, 
                                  GPIO_NUM_23, GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27,
                                  GPIO_NUM_16, GPIO_NUM_17 };
    
    HD44780_InitEightBitBus(&bus);

    vTaskDelay(150 / portTICK_PERIOD_MS);

    printf("Sending reset and set mode sent.\n");
    HD44780_SendInstruction(0x30);
    vTaskDelay(twentyMilliDelay);
    HD44780_SendInstruction(0x30);
    vTaskDelay(twentyMilliDelay);
    HD44780_SendInstruction(0x30);
    vTaskDelay(twentyMilliDelay);
    HD44780_SendInstruction(0x30);
    vTaskDelay(twentyMilliDelay);
    printf("Reset and set mode sent.\n");

    HD44780_SendInstruction(0x08);
    HD44780_SendInstruction(0x01);
    vTaskDelay(twentyMilliDelay);
    HD44780_SendInstruction(0x06);

    HD44780_SendInstruction(0x0F);
    //HD44780_SendInstruction(HD44780_CURSOR_BLINK);

    HD44780_SendData('T');
    HD44780_SendData('h');
    HD44780_SendData('i');
    HD44780_SendData('s');
    HD44780_SendData(' ');
    HD44780_SendData('a');
    HD44780_SendData(' ');
    HD44780_SendData('t');
    HD44780_SendData('r');
    HD44780_SendData('i');
    HD44780_SendData('u');
    HD44780_SendData('m');
    HD44780_SendData('p');
    HD44780_SendData('h');
}