--------------------------------------------------------------------------------
# Altair 101 Processor Major Version

I have reach my goal an Altair 8800 emulator that is programmable and runs programs.

This version fully integrates the 1602 LCD module.
The integration include "out 1" to the LCD.
+ If register A is 0, "out 1" will cause the backlight to turn off.
+ If register A is 1, "out 1" will cause the backlight to turn on.
+ If register A is 2, "out 1" will cause the LCD screen to clear.
+ If register A is 3, "out 1" will cause the LCD to display a splash message.

The status lights are now displayed correctly.

Many of the opcodes were tested using assembler programs.

I integrated a second serial port for upload machine code from my laptop to the Altair 101.

Read and write updates. Examples:
+ 00000000 doesn't read from the disk, it auto sets all the memory bytes to zero.
+ 11111111 write, puts the machine in ready to receive bytes over the second serial port.
+ Upload of programs over the second serial port works great.

--------------------------------------------------------------------------------
Cheers