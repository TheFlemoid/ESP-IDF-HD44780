#pragma once

#include "driver/gpio.h"

// Instruction definitions
#define HD44780_CURSOR_BLINK 0x0F
#define HD44780_CLEAR_DISPLAY 0x01

typedef enum _displayMode {
    HD44780_FOUR_BIT_MODE,
    HD44780_EIGHT_BIT_MODE
} HD44780_DISPLAY_MODE;

typedef struct _fourBitBus {
    gpio_num_t D4;
    gpio_num_t D5;
    gpio_num_t D6;
    gpio_num_t D7;
    gpio_num_t RS;
    gpio_num_t E;
} HD44780_FOUR_BIT_BUS;

typedef struct _eightBitBus {
    gpio_num_t D0;
    gpio_num_t D1;
    gpio_num_t D2;
    gpio_num_t D3;
    gpio_num_t D4;
    gpio_num_t D5;
    gpio_num_t D6;
    gpio_num_t D7;
    gpio_num_t RS;
    gpio_num_t E;
} HD44780_EIGHT_BIT_BUS;

// Method declarations
void HD44780_InitFourBitBus(HD44780_FOUR_BIT_BUS *bus);

void HD44780_InitEightBitBus(HD44780_EIGHT_BIT_BUS *bus);

void HD44780_Pulse_E();

void HD44780_SetUpperNibble(unsigned short int data);

void HD44780_SetLowerNibble(unsigned short int data);

void HD44780_ClearUpperNibble();

void HD44780_ClearLowerNibble();

void HD44780_Send4BitsIn4BitMode(unsigned short int data);

void HD44780_Send8BitsIn4BitMode(unsigned short int data);

void HD44780_SendFourBitStartInstruction(unsigned short int data);

void HD44780_SendInstruction(unsigned short int data);

void HD44780_SendData(unsigned short int data);