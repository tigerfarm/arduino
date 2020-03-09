--------------------------------------------------------------------------------
# Altair 101 Processor Major Version

I have reach my goal an Altair 8800 emulator that is programmable and runs programs.
+ Processor.ino emulates the basic interactivity of the Altair 8800.
+ The toggles and LED lights work as expected.
+ The test program, Kill the Bit, runs great!
+ The emulator's memory can be wrote to the micro SD card, and then later, read back into memory.
+ The Java assembler program works to generate and upload machine code to the Altair 101 dev machine.
+ The opcode programs I wrote and ran, work fine.

Features,
+ I can use toggles to load and run programs.
+ The status lights display correctly.
+ I can save and load machine code programs to/from an micro SD card.
+ Many of the opcodes have been tested using assembler programs.

In this version:
+ I cleaned up the data processing function: processData(), and the functions it calls.
+ The status lights now display properly when running or stepping through a program.
+ I confirmed that status lights worked for the opcodes LDA and STA, as seen in the Altair 8800 clone front panel video.
+ I added a function to display the current program values in the panel LED lights:
++  processDataLights() use statusByte, curProgramCounter, and dataByte.

The control switch functions were updated to work as expected:
++ STOP, RUN, STEP, EXAMINE, EXAMINE-NEXT, DEPOSIT, DEPOSIT-NEXT.
++ Can STOP a program. Then flip RUN and the program will continue.
++ EXAMINE, EXAMINE-NEXT, DEPOSIT, DEPOSIT-NEXT, all use and set the program counter properly.

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