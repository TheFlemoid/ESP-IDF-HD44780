## ESP-IDF HD44780 Snow Example

An example application showing a rudimentary snow effect on a two row, 16 column HD44780 based
character display.  Mostly here to prove that this libary works.

In order to build this project, it must be build with esp-idf from the same directory that
this README is in.  If, like me, you typically compile esp-idf projects in Visual Studio
Code, then you need to open the folder that this README is in from the initial "Open Folder"
dialog, not the root of this repo.  Otherwise, the CMakeList infrastructure of esp-idf
won't work out properly, and you'll end up with a weird precompile error.

In terms of physical connection, this project is by default designed to be run in HD44780 
four bit mode, and should be set up as follows.

| ESP-32 | HD44780 Pin |
| :---: | :---: |
| GPIO 18  | D4 |
| GPIO 19  | D5 |
| GPIO 21  | D6 |
| GPIO 22  | D7 |
| GPIO 16  | RS |
| GPIO 17  | E |

On the display: 
- Pin 1 should be connected to ground and pin 2 connected to 5V.  
- Pin 3 is the contrast control, and needs to be connected to a voltage divider for tuning.  
- Typically, connect pin 3 to the center (wiper) pin of a 10K potentiometer, and connect 
one side to 5V and the other to ground (making an adjustable voltage divider).  
- Connect pin 5 (RW) to ground, to ensure that
the HD44780 is in write mode for all operations.
