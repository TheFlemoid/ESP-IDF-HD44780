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
| --- | --- |
| GPIO 18  | D4 |
| GPIO 19  | D5 |
| GPIO 21  | D6 |
| GPIO 22  | D7 |
| GPIO 16  | RS |
| GPIO 17  | E |

