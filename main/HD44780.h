#pragma once

#include "driver/gpio.h"

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

void HD44780_InitDisplay();

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

// HD44780 Instruction Definitions
#define HD44780_INIT_SEQ        0x30
#define HD44780_DISP_CLEAR      0x01
#define HD44780_DISP_OFF        0x08
#define HD44780_DISP_ON         0x0C
#define HD44780_CURSOR_ON       0x0E
#define HD44780_CURSOR_BLINK    0x0F
#define HD44780_RETURN_HOME     0x02
#define HD44780_ENTRY_MODE      0x06
#define HD44780_FOUR_BIT_MODE   0x20
#define HD44780_EIGHT_BIT_MODE  0x30

// Bitmasks for various instructions
#define HD44780_TWO_ROWS        0x08
#define HD44780_FONT_5X8        0x00
#define HD44780_FONT_5X10       0x40