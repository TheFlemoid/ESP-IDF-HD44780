## ESP-IDF HD44780 Scroll Example

An example application showing some text scrolling on a two row, 16 column HD44780 based
character display.  Mostly here to prove that this libary works.

In order to build this project, it must be build with esp-idf from the same directory that
this README is in.  If, like me, you typically compile esp-idf projects in Visual Studio
Code, then you need to open the folder that this README is in from the initial "Open Folder"
dialog, not the root of this repo.  Otherwise, the CMakeList infrastructure of esp-idf
won't work out properly, and you'll end up with a weird precompile error.
