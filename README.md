ESP-32 drivers for common Hitachi HD44780 character LCD displays.

Currently supports HD44780 in either 8 bit and 4 bit mode.

Work in progress.

---

<p align="center">
    <img src="./resources/lcd_test.gif" alt="Test Display Example"/>
</p>

Usage of this library is incredibly simple.  First you have to pick your mode of operation.

HD44780 displays are driven in one of three modes:
 - **8-bit mode:** This is the standard way that these displays have been driven for decades.  All 8 bits of the
   bus are utilized for communications with the LCD driver.  This has numerous obvious disadvantages, most
   especially that no less then 10 (and sometimes 11) GPIOs are required to drive a simple display.  Regardless,
   if you want to drive a display in 8-bit mode for some (presumably legacy) reason, this library fully 
   supports that use case, we've got you covered.
 - **4-bit mode:** This is one of the two 'modern' ways of driving this type of display.  4-bit mode is supported
   by the HD44780 LCD controller locally (with no external circuitry), and can achieve all of the features that 
   8-bit mode can achieve with almost half the pins (usually 6 pins).  The only requirement is that the MCU 
   driving the display is "fast", when compared to the HD44780 controller design from the 80s, an easy
   bar to hurdle.
 - **I2C mode:** This is a ***relatively*** new method of controlling these displays, and it is the most common method
   of display control that you'll see in modern drivers.  This method relies on an external I2C driver, which is
   typically shipped  with character LCDs of this variety in the modern day.  Most HD44780 drivers you see around
   GitHub today are for displays of this variety.  Advantages of this mode are obvious, you only need two pins 
   (SCL and SDA) to support any number of displays/I2C devices.  And, with any modern type of MCU, you can drive
   the I2C bus faster then you could in 8-bit mode parallel operation in the 90s.  I am not currently actively 
   supporting I2C display control, since I mostly wrote this in preparation for a different project (in which I was 
   using an HD44780 in 4-bit mode), and since I don't actually have any of these new displays to test with.  If this 
   erks you and you would like to use this library with an I2C character LCD, please drop an issue on this repo and 
   I'll look into adding support for such a thing.

Once you've determined the display mode you want, you need to make a struct specific to that mode to initialize the display.  For 4-bit mode, create an HD44780_FOUR_BIT_BUS or for 8-bit mode, create an HD44780_EIGHT_BIT_BUS struct.  The structs are as follows:
    - HD44870_FOUR_BIT_BUS has six parameters
        1. GPIO pin number connected to D4
        2. GPIO pin number connected to D5
        3. GPIO pin number connected to D6
        4. GPIO pin number connected to D7
        5. GPIO pin number connected to RS
        6. GPIO pin number connected to E

    - HD44870_EIGHT_BIT_BUS has ten parameters
        1. GPIO pin number connected to D0
        2. GPIO pin number connected to D1
        3. GPIO pin number connected to D2
        4. GPIO pin number connected to D3
        5. GPIO pin number connected to D4
        6. GPIO pin number connected to D5
        7. GPIO pin number connected to D6
        8. GPIO pin number connected to D7
        9. GPIO pin number connected to RS
        10. GPIO pin number connected to E
---

## HD44780 Initialization:
Initializing the HD44780 controller is a little tricky, as the datasheet is somewhat
obtuse about the process and there are two modes that the controller
can be initialized to (4-bit and 8-bit mode).  Outside of the 
[HD44780 datasheet](resources/HD44780.pdf) 
(pdf warning) I got the information to initialize the display from an [Alfred State
College blog](https://web.alfredstate.edu/faculty/weimandn/lcd/lcd_initialization/lcd_initialization_index.html).

HD44780 init steps for either 8 or 4 bit mode are as follows:
1. Wait 100ms (actual startup delay is >40ms, but this is only 
   done once so erring on the side of caution.)
2. Send HD44780_INIT_SEQ (0x30), then delay >4ms.  This is a special
   case of the function set instruction where only the upper nibble
   (D4-D7) is considered by the driver.  Three of these instructions 
   together will cause the driver to reset, which is the goal here.
3. Send HD44780_INIT_SEQ (0x30) again, then delay >100us.
4. Send HD44780_INIT_SEQ (0x30) a third time, then delay >100us.
   This will cause the display to do a soft reset.
5. For 4-bit mode initialization, send instruction HD44780_FOUR_BIT_MODE
   (0x20) and delay >100us.  This tells the controller that all
   commands going forward are on D4-D7 only, and to ignore D0-3 entirely.
   This step should be skipped for 8-bit mode operation.
6. Send the actual function set instruction then wait >50us.  
   The function set instruction is as follows: 
     0b001(DL)(N)(F)00 where:
     - DL is 1 for 8-bit mode or 0 for 4-bit mode
     - N is 0 for one line, or 1 for two line (this is the logical
       number of lines as perceived by the LCD controller, which 
       would still be two for four line displays).
     - F is 0 for 5x8 dot character font, or 1 for 5x10 dot character
       font (this will almost always be 0, or 5x8 font).
     - For 8-bit operation, this will usually be 0x38.  For 4-bit operation, this will 
       usually be 0x28.  In practice, we OR together the DL, N, and F bitmasks here.
7. Send HD44780_DISP_OFF and delay >50us.  The lower three bits 
   of this instruction control whether the display itself (D), 
   the cursor (C), and the cursor blink (B) should be on or off, 
   but for now we turn all of them off.
8. Send HD44780_DISP_CLEAR and delay >3ms.  This clears all 80 DDRAM
   addresses on the display, hence the longer wait time.
9. Send HD44780_ENTRY_MODE and delay >50us.  The lower two bits of 
   this command control whether the cursor should move (I/D) left 
   to right (1) or right to left (0), and whether the display should 
   shift (S) (1) or not shift (0).  We typically want the cursor to 
   move left to right and for the display to not shift, so 0x06.
10. The display is now initialized, but it was turned off in step 7,
    so we have to turn it on and set our display mode.
11. Send HD44780_DISP_ON and delay >50us. Set the lower two bits 
    based on whether the cursor should be on (C) and the cursor 
    should blink (B). For example, sending HD44780_DISP_ON would 
    just turn the display on, HD44780_CUSOR_ON would turn the 
    display on and display the cursor, and HD44780_CURSOR_BLINK would
    turn the cursor on and cause it to blink.

