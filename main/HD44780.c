#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "rom/ets_sys.h"
#include "HD44780.h"

HD44780_DISPLAY_MODE displayMode;
HD44780_FOUR_BIT_BUS *fourBus;
HD44780_EIGHT_BIT_BUS *eightBus;
gpio_num_t enablePin;
gpio_num_t rsPin;

static uint32_t ONE_HUNDRED_MILLI_DELAY = (100 / portTICK_PERIOD_MS);
static uint32_t TWENTY_MILLI_DELAY = (20 / portTICK_PERIOD_MS);
static uint32_t VOLTAGE_CHANGE_DELAY_US = 5;
static uint32_t INSTRUCTION_DELAY_US = 100;

/** 
 * HD44780 init steps for either 8 or 4 bit mode are as follows:
 * 1. Wait 100ms (actual startup delay is >40ms, but this is only 
 *    done once so erring on the side of caution.)
 * 2. Send HD44780_INIT_SEQ (0x30), then delay >4ms.  This is a special
 *    case of the function set instruction where only the upper nibble
 *    (D4-D7) is considered by the driver.  Three of these instructions 
 *    together will cause the driver to reset, which is the goal here.
 * 3. Send HD44780_INIT_SEQ (0x30) again, then delay >100us.
 * 4. Send HD44780_INIT_SEQ (0x30) a third time, then delay >100us.
 *    This will cause the display to do a soft reset.
 * 5. For 4 bit initialization, send instruction HD44780_FOUR_BIT_MODE
 *    (0x20) and delay >100us.  This tells the controller that all
 *    commands going forward are on D4-D7 only, and to ignore D0-3 entirely.
 *    This is not necessary for 8 bit.
 * 6. Send the actual function set instruction then wait >50us.  
 *    The function set instruction is as follows: 
 *      0b001(DL)(N)(F)00 where:
 *      - DL is 1 for 8 bit interface, 0 for 4 bit interface
 *      - N is 0 for one line, or 1 for two line (this is the logical
 *        number of lines as perceived by the LCD controller, which 
 *        would still be two for four line displays).
 *      - F is 0 for 5x8 dot character font, or 1 for 5x10 dot character
 *        font (this will almost always be 0, or 5x8 font).
 *      For 8 bit, this will usually be 0x38
 *      For 4 bit, this will usually be 0x28
 *      In practice, we OR together the DL, N, and F bitmasks here.
 * 7. Send HD44780_DISP_OFF and delay >50us.  The lower three bits 
 *    of this instruction control whether the display itself (D), 
 *    the cursor(C), and the cursor blink (B) should be on or off, 
 *    but for now we turn all of them off.
 * 8. Send HD44780_DISP_CLEAR and delay >3ms.  This clears all 80 DDRAM
 *    addresses on the display, hence the longer wait time.
 * 9. Send HD44780_ENTRY_MODE and delay >50us.  The lower two bits of 
 *    this command control whether the cursor should move (I/D) left 
 *    to right (1) or right to left (0), and whether the display should 
 *    shift (S) (1) or not shift (0).  We typically want the cursor to 
 *    move left to right and for the display to not shift, so 0x06.
 * 10. The display is not initialized, but it was turned off in step 7,
 *     so we have to turn it on and set our display mode.
 * 11. Send HD44780_DISP_ON and delay >50us. Set the lower two bits 
 *     based on whether the cursor should be on (C) and the cursor 
 *     should blink (B). For example, sending HD44780_DISP_ON would 
 *     just turn the display on, HD44780_CUSOR_ON would turn the 
 *     display on and display the cursor, etc.
 */

/**
 * Initializes the param four bit bus, and initialized the display
 * in four bit mode.
 * 
 * @param fourBitBus HD44780_FOUR_BIT_BUS to drive the display
 */
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

    HD44780_InitDisplay();
}

/**
 * Initializes the param eight bit bus, and initialized the display
 * in eight bit mode.
 * 
 * @param eightBitBus HD44780_EIGHT_BIT_BUS to drive the display
 */
void HD44780_InitEightBitBus(HD44780_EIGHT_BIT_BUS *eightBitBus) {
    displayMode = HD44780_EIGHT_BIT_MODE;
    eightBus = eightBitBus;

    // Set all pins on bus to OUTPUTs
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

    // Keep E and RS as globals separately to make other functions easier
    enablePin = eightBitBus->E;
    rsPin = eightBitBus->RS;

    HD44780_InitDisplay();
}

void HD44780_InitDisplay() {
    // NOTE: The SendInstruction method has a delay built in, but it
    //       is not long enough for some of the initialization steps.
    //       That is why some of these commands have a delay after, and
    //       some don't.
    vTaskDelay(ONE_HUNDRED_MILLI_DELAY);
    HD44780_Send4BitStartInstruction(HD44780_INIT_SEQ);
    vTaskDelay(TWENTY_MILLI_DELAY);
    HD44780_Send4BitStartInstruction(HD44780_INIT_SEQ);
    vTaskDelay(TWENTY_MILLI_DELAY);
    HD44780_Send4BitStartInstruction(HD44780_INIT_SEQ);
    vTaskDelay(TWENTY_MILLI_DELAY);

    // TODO: FLD 01FEB25 - Assuming here that all displays are two row and 5x8
    //                     add support for one row and 5x10 displays later.
    if (displayMode == HD44780_FOUR_BIT_MODE) {
        HD44780_Send4BitStartInstruction(HD44780_FOUR_BIT_MODE);
        HD44780_SendInstruction(HD44780_FOUR_BIT_MODE | HD44780_TWO_ROWS | 
                                HD44780_FONT_5X8);
    } else {
        HD44780_SendInstruction(HD44780_EIGHT_BIT_MODE | HD44780_TWO_ROWS | 
                                HD44780_FONT_5X8);
    }

    HD44780_SendInstruction(HD44780_DISP_OFF);
    HD44780_SendInstruction(HD44780_DISP_CLEAR);
    vTaskDelay(TWENTY_MILLI_DELAY);
    HD44780_SendInstruction(HD44780_ENTRY_MODE);
    HD44780_SendInstruction(HD44780_DISP_ON);
}

/**
 * Pulses the 'E' clock pin
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

    // Set D4-D7 if corresponding bit is set in the data
    gpio_set_level(D7, (data & 0x80));
    gpio_set_level(D6, (data & 0x40));
    gpio_set_level(D5, (data & 0x20));
    gpio_set_level(D4, (data & 0x10));

    ets_delay_us(VOLTAGE_CHANGE_DELAY_US);
}

void HD44780_SetLowerNibble(unsigned short data) {
    if (displayMode != HD44780_EIGHT_BIT_MODE) {
        return;
    }


    gpio_set_level(eightBus->D3, (data & 0x08));
    gpio_set_level(eightBus->D2, (data & 0x04));
    gpio_set_level(eightBus->D1, (data & 0x02));
    gpio_set_level(eightBus->D0, (data & 0x01));

    ets_delay_us(VOLTAGE_CHANGE_DELAY_US);
}

/**
 * Sends the four upper bits from the param data byte.
 * 
 * @param data byte containing the 4 MSB to send
 */
void HD44780_Send4BitsIn4BitMode(unsigned short int data) {
    HD44780_SetUpperNibble(data);
    HD44780_Pulse_E();
    ets_delay_us(INSTRUCTION_DELAY_US);
}

/**
 * Sends the param data byte in 8 bit mode.  In 8 bit mode the
 * entire byte is sent at once on D7-D0.
 * 
 * @param data byte to send
 */
void HD44780_Send8BitsIn8BitMode(unsigned short int data) {
    HD44780_SetUpperNibble(data);
    HD44780_SetLowerNibble(data);
    HD44780_Pulse_E();
    ets_delay_us(INSTRUCTION_DELAY_US);
}

/**
 * Sends the param byte of data in 4 bit mode.  In 4 bit mode
 * the byte is sent only on D7-D4, and is sent in two steps
 * (4 MSB followed by 4 LSB).
 * 
 * @param data byte to send
 */
void HD44780_Send8BitsIn4BitMode(unsigned short int data) {
    // Send upper nibble
    HD44780_SetUpperNibble(data);
    HD44780_Pulse_E();

    // Send lower nibble
    HD44780_SetUpperNibble(data << 4);
    HD44780_Pulse_E();
    ets_delay_us(INSTRUCTION_DELAY_US);
}

/**
 * Sends the param instruction to the HD44780 as a special four bit start
 * instruction.  This is used on initialization of the display to set the 
 * mode (4 or 8 bit).  Only the top nibble of the instruction is actually 
 * sent, and it's sent on D7-D4.
 * 
 * @param data Instruction to send
 */
void HD44780_Send4BitStartInstruction(unsigned short int data) {
    // RS low to write to instruction register
    gpio_set_level(rsPin, 0);
    HD44780_Send4BitsIn4BitMode(data);
    ets_delay_us(INSTRUCTION_DELAY_US);
}

/**
 * Sends the param instruction to the HD44780.
 * 
 * @param data Instruction to send
 */
void HD44780_SendInstruction(unsigned short int data) {
    // RS low to write to instruction register
    gpio_set_level(rsPin, 0);

    if (displayMode == HD44780_FOUR_BIT_MODE) {
        HD44780_Send8BitsIn4BitMode(data);
    } else {
        HD44780_Send8BitsIn8BitMode(data);
    }
}

/**
 * Sends the param character to the HD44780.
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
