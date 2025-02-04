ESP-32 drivers for common Hitachi HD44780 character LCD displays.

Currently supports HD44780 in either 8 bit and 4 bit mode.

Work in progress.

---

## HD44780 Initialization:
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
5. For 4 bit initialization, send instruction HD44780_FOUR_BIT_MODE
   (0x20) and delay >100us.  This tells the controller that all
   commands going forward are on D4-D7 only, and to ignore D0-3 entirely.
   This is not necessary for 8 bit.
6. Send the actual function set instruction then wait >50us.  
   The function set instruction is as follows: 
     0b001(DL)(N)(F)00 where:
     - DL is 1 for 8 bit interface, 0 for 4 bit interface
     - N is 0 for one line, or 1 for two line (this is the logical
       number of lines as perceived by the LCD controller, which 
       would still be two for four line displays).
     - F is 0 for 5x8 dot character font, or 1 for 5x10 dot character
       font (this will almost always be 0, or 5x8 font).
     For 8 bit, this will usually be 0x38
     For 4 bit, this will usually be 0x28
     In practice, we OR together the DL, N, and F bitmasks here.
7. Send HD44780_DISP_OFF and delay >50us.  The lower three bits 
   of this instruction control whether the display itself (D), 
   the cursor(C), and the cursor blink (B) should be on or off, 
   but for now we turn all of them off.
8. Send HD44780_DISP_CLEAR and delay >3ms.  This clears all 80 DDRAM
   addresses on the display, hence the longer wait time.
9. Send HD44780_ENTRY_MODE and delay >50us.  The lower two bits of 
   this command control whether the cursor should move (I/D) left 
   to right (1) or right to left (0), and whether the display should 
   shift (S) (1) or not shift (0).  We typically want the cursor to 
   move left to right and for the display to not shift, so 0x06.
10. The display is not initialized, but it was turned off in step 7,
    so we have to turn it on and set our display mode.
11. Send HD44780_DISP_ON and delay >50us. Set the lower two bits 
    based on whether the cursor should be on (C) and the cursor 
    should blink (B). For example, sending HD44780_DISP_ON would 
    just turn the display on, HD44780_CUSOR_ON would turn the 
    display on and display the cursor, etc.

