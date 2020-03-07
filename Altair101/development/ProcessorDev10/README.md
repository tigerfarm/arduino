--------------------------------------------------------------------------------
# Altair 101 Processor Major Version

I have reach my goal an Altair 8800 emulator that is programmable and runs programs.

Features,
+ I can use toggles to load and run programs.
+ The status lights display correctly.
+ Many of the opcodes were tested using assembler programs.
+ I can save and load machine code programs to/from an micro SD card.

Read and write updates,
+ I integrated a second serial port for upload machine code from my laptop to the Altair 101.
+ 11111111 write, puts the machine in ready to receive bytes over the second serial port.
+ 00000000 doesn't read from the disk, it auto sets all the memory bytes to zero.

This version fully integrates the 1602 LCD module.
The integration uses opcode, "out", address, 1, to write register A to the LCD.
+ If register A is 0, "out 1" will cause the backlight to turn off.
+ If register A is 1, "out 1" will cause the backlight to turn on.
+ If register A is 2, "out 1" will cause the LCD screen to clear.
+ If register A is 3, "out 1" will cause the LCD to display a splash message.
+ Other register A values are written as characters such as "a" (ascii value 97).

--------------------------------------------------------------------------------
Cheers