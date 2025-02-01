#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "rom/ets_sys.h"
#include "HD44780.h"

HD44780_DISPLAY_MODE displayMode;
HD44780_FOUR_BIT_BUS *fourBus;
HD44780_EIGHT_BIT_BUS *eightBus;
gpio_num_t enablePin;
gpio_num_t rsPin;

static uint32_t oneMilliDelay = 1 / portTICK_PERIOD_MS;
static uint32_t VOLTAGE_CHANGE_DELAY_US = 5;
static uint32_t INSTRUCTION_DELAY_US = 100;

void HD44780_InitFourBitBus(HD44780_FOUR_BIT_BUS *fourBitBus) {
    displayMode = HD44780_FOUR_BIT_MODE;
    fourBus = fourBitBus;

    gpio_set_direction(fourBitBus->D4, GPIO_MODE_OUTPUT);
    gpio_set_direction(fourBitBus->D5, GPIO_MODE_OUTPUT);
    gpio_set_direction(fourBitBus->D6, GPIO_MODE_OUTPUT);
    gpio_set_direction(fourBitBus->D7, GPIO_MODE_OUTPUT);
    gpio_set_direction(fourBitBus->E, GPIO_MODE_OUTPUT);
    gpio_set_direction(fourBitBus->RS, GPIO_MODE_OUTPUT);

    enablePin = fourBitBus->E;
    rsPin = fourBitBus->RS;
}

void HD44780_InitEightBitBus(HD44780_EIGHT_BIT_BUS *eightBitBus) {
    displayMode = HD44780_EIGHT_BIT_MODE;
    eightBus = eightBitBus;

    gpio_set_direction(eightBitBus->D0, GPIO_MODE_OUTPUT);
    gpio_set_direction(eightBitBus->D1, GPIO_MODE_OUTPUT);
    gpio_set_direction(eightBitBus->D2, GPIO_MODE_OUTPUT);
    gpio_set_direction(eightBitBus->D3, GPIO_MODE_OUTPUT);
    gpio_set_direction(eightBitBus->D4, GPIO_MODE_OUTPUT);
    gpio_set_direction(eightBitBus->D5, GPIO_MODE_OUTPUT);
    gpio_set_direction(eightBitBus->D6, GPIO_MODE_OUTPUT);
    gpio_set_direction(eightBitBus->D7, GPIO_MODE_OUTPUT);
    gpio_set_direction(eightBitBus->E, GPIO_MODE_OUTPUT);
    gpio_set_direction(eightBitBus->RS, GPIO_MODE_OUTPUT);

    enablePin = eightBitBus->E;
    rsPin = eightBitBus->RS;
}

/**
 * Pulses the 'E' clock pin on param HD44780_FOUR_BIT_BUS
 */
void HD44780_Pulse_E() {
    gpio_set_level(enablePin, 1);
    ets_delay_us(10);
    gpio_set_level(enablePin, 0);
    ets_delay_us(10);
}

/**
 * Set the bits on the four bit bus to the four upper bits of the param data byte.
 * 
 * @param data Byte to set
 */
void HD44780_SetUpperNibble(unsigned short int data) {
    HD44780_ClearUpperNibble();

    gpio_num_t D7;
    gpio_num_t D6;
    gpio_num_t D5;
    gpio_num_t D4;

    if (displayMode == HD44780_EIGHT_BIT_MODE) {
        D7 = eightBus->D7;
        D6 = eightBus->D6;
        D5 = eightBus->D5;
        D4 = eightBus->D4;
    } else {
        D7 = fourBus->D7;
        D6 = fourBus->D6;
        D5 = fourBus->D5;
        D4 = fourBus->D4;
    }

    printf("In SetUpperNibble, data received is %d : %04x\n", data, data);
    // Set D4-D7 if corresponding bit is set in the data
    gpio_set_level(D7, (data & 0x80));
    printf("Setting D7 to %d\n", (data & 0x80));
    gpio_set_level(D6, (data & 0x40));
    printf("Setting D6 to %d\n", (data & 0x40));
    gpio_set_level(D5, (data & 0x20));
    printf("Setting D5 to %d\n", (data & 0x20));
    gpio_set_level(D4, (data & 0x10));
    printf("Setting D4 to %d\n", (data & 0x10));

    ets_delay_us(VOLTAGE_CHANGE_DELAY_US);
}

void HD44780_SetLowerNibble(unsigned short data) {
    if (displayMode != HD44780_EIGHT_BIT_MODE) {
        return;
    }

    printf("In SetLowerNibble, data received is %d : %04x\n", data, data);

    gpio_set_level(eightBus->D3, (data & 0x08));
    printf("Setting D3 to %d\n", (data & 0x08));
    gpio_set_level(eightBus->D2, (data & 0x04));
    printf("Setting D2 to %d\n", (data & 0x04));
    gpio_set_level(eightBus->D1, (data & 0x02));
    printf("Setting D1 to %d\n", (data & 0x02));
    gpio_set_level(eightBus->D0, (data & 0x01));
    printf("Setting D0 to %d\n", (data & 0x01));

    ets_delay_us(VOLTAGE_CHANGE_DELAY_US);
}

/**
 * Clears the four upper bits of the bus.
 */
void HD44780_ClearUpperNibble() {
    gpio_num_t D7;
    gpio_num_t D6;
    gpio_num_t D5;
    gpio_num_t D4;

    if (displayMode == HD44780_EIGHT_BIT_MODE) {
        D7 = eightBus->D7;
        D6 = eightBus->D6;
        D5 = eightBus->D5;
        D4 = eightBus->D4;
    } else {
        D7 = fourBus->D7;
        D6 = fourBus->D6;
        D5 = fourBus->D5;
        D4 = fourBus->D4;
    }

    // Clear bits D4-D7
    gpio_set_level(D4, 0);
    gpio_set_level(D5, 0);
    gpio_set_level(D6, 0);
    gpio_set_level(D7, 0);

    ets_delay_us(VOLTAGE_CHANGE_DELAY_US);
}

void HD44780_ClearLowerNibble() {
    if (displayMode != HD44780_EIGHT_BIT_MODE) {
        return;
    }

    gpio_set_level(eightBus->D3, 0);
    gpio_set_level(eightBus->D2, 0);
    gpio_set_level(eightBus->D1, 0);
    gpio_set_level(eightBus->D0, 0);

    ets_delay_us(VOLTAGE_CHANGE_DELAY_US);
}

/**
 * Sends the four upper bits from the param data byte over the param bus.
 * 
 * @param data Byte containing four upper bits to send on the bus
 */
void HD44780_Send4BitsIn4BitMode(unsigned short int data) {
    HD44780_SetUpperNibble(data);
    HD44780_Pulse_E();
    ets_delay_us(INSTRUCTION_DELAY_US);

    HD44780_ClearUpperNibble();
}

void HD44780_Send8BitsIn8BitMode(unsigned short int data) {
    HD44780_SetUpperNibble(data);
    HD44780_SetLowerNibble(data);
    HD44780_Pulse_E();
    ets_delay_us(INSTRUCTION_DELAY_US);

    HD44780_ClearUpperNibble();
    HD44780_ClearLowerNibble();
}

void HD44780_Send8BitsIn4BitMode(unsigned short int data) {
    // Send upper nibble
    HD44780_SetUpperNibble(data);
    HD44780_Pulse_E();

    // Send lower nibble
    HD44780_SetUpperNibble(data << 4);
    HD44780_Pulse_E();
    ets_delay_us(INSTRUCTION_DELAY_US);

    HD44780_ClearUpperNibble();
}

void HD44780_SendFourBitStartInstruction(unsigned short int data) {
    // RS low to write to instruction register
    gpio_set_level(rsPin, 0);
    HD44780_Send4BitsIn4BitMode(data);
    ets_delay_us(INSTRUCTION_DELAY_US);

    HD44780_ClearUpperNibble();
}

/**
 * Sends the param instruction to the HD44780.
 * NOTE: Like everything else here, this currently only works for four bit mode.
 *       Will need to implement 8 bit mode later if desired.  Also, instead of checking
 *       the busy flag like civilized people, we're just waiting a safety factored
 *       amount of time and assuming the display is done by the time we exit here.
 * 
 * @param data Instruction to send
 */
void HD44780_SendInstruction(unsigned short int data) {
    // RS low to write to instruction register
    gpio_set_level(rsPin, 0);

    if (displayMode == HD44780_FOUR_BIT_MODE) {
        printf("Sending instruction in 4 bit mode.\n");
        HD44780_Send8BitsIn4BitMode(data);
    } else {
        printf("Sending instruction in 8 bit mode.\n");
        HD44780_Send8BitsIn8BitMode(data);
    }
}

/**
 * Sends the param character to the HD44780.
 * NOTE: Like everything else here, this currently only works for four bit mode.
 *       Will need to implement 8 bit mode later if desired.
 * 
 * @param data Character to send
 */
void HD44780_SendData(unsigned short int data) {
    // RS high to write to data register
    gpio_set_level(rsPin, 1);

    if (displayMode == HD44780_FOUR_BIT_MODE) {
        HD44780_Send8BitsIn4BitMode(data);
    } else {
        HD44780_Send8BitsIn8BitMode(data);
    }
}