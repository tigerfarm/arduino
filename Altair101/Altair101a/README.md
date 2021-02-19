--------------------------------------------------------------------------------
# Altair101a is an Altair 8800 Simulator

This simulator was built to run Intel 8080 machine code programs.
It has a simplistic OS to set configurations and manage programs: run, stop, save, and load.
Altair101a uses the processing control code from my first simulator, Processor.ino.
Altair101a uses David Hansel's Altair 8800 simulator code to execute machine instructions.
Hansel's code is proven to work, as it's the software engine of the Altair-Duino.

Altair101a runs Kill the Bit, sample assembler programs I wrote, and MITS 4K Basic.
My assembler program is great for assembling programs into byte code and uploading to the Altair101a.
I used the assembler to assemble and run a modified version the program GALAXY80,
a variation of the classic Star Trek game.

The next step is to build a new hardware machine using an Arduino Due.
My current test machine is based on the Arduino Mega 2560.
The Altair101a simulator program is already tested a Due.
It will be fun building and running a Due Altair101a machine.

The long term software goal for Altair101a, is to run CP/M.
Hansel's code is capable of running CP/M, and CP/M programs.
Other goals are to implement the hardware front panel on my Altair101 desktop,
and add the MP3 player.

--------------------------------------------------------------------------------
## Altair101a Setup and Startup

No wiring required.
Download the Altair101a project program files into an Arduino project directory of the name Altair101a.
````
Altair101a.ino
Altair101a.h
AltairSamples.cpp
AltairSamples.h
cpucore_i8080.cpp
cpucore_i8080.h
````

Have your Arduino Mega 2560 or Due connect to your computer using a serial cable.
Load Altair101a.ino into your Arduino IDE.
All the supporting program files are loaded as well.
Click Run.

The program files are compiled and uploaded to your Arduino.
The following message was displayed for uploading to my Mega.
Your exact numbers and percentages may vary because I continue to update the program.
````
Sketch uses 70800 bytes (27%) of program storage space. Maximum is 253952 bytes.
Global variables use 7631 bytes (93%) of dynamic memory, leaving 561 bytes for local variables. Maximum is 8192 bytes.
Low memory available, stability problems may occur.
````
Note, ignore the Low memory available warning.
There is enough memory to run the program.

Open the Arduino IDE Serial Monitor, IDE menu: Tools/Serial Monitor.
The following will be displayed.
````
+++ setup
- Error initializing SD card module.
+++ Altair101a initialized.
?- 
````
Ignore the error message about initializing the SD card
because you don't have one connected. The program will work fine without it.
If you did happen to connect one before running the program, you will get a
connection message.
````
+++ setup
+ SD card module is initialized
+++ Altair101a initialized.
?- 
````
Either way, your Altair101a is ready to be tested.

--------------------------------------------------------------------------------
## Running the Altair101a

In the top bar of the Serial Monitor, enter "h" and click the Send button.
The help information will be displayed followed by a print of the virtual front panel.
````
?- + h, Print help information.
----------------------------------------------------
+++ Virtual Front Panel Commands
-------------
+ v/V Front panel Disable(use CLI)/enable VT100 virtual front panel.
-------------
+ 0...9, a...f    Toggle sense/address/data switches:  A0...A9, A10...A15.
-------------
+ r, RUN mode     When in WAIT mode, change to RUN mode.
+ s, STOP         When in RUN mode, change to WAIT mode.
+ s, SINGLE STEP  When in WAIT mode, SINGLE STEP.
+ x, EXAMINE      sense switch address.
+ X, EXAMINE NEXT address, current address + 1.
+ p, DEPOSIT      into current address.
+ P, DEPOSIT NEXT address, deposit into current address + 1.
+ R, RESET        Set program counter address to zero.
+ C, CLR          Clear memory, set registers and program counter address to zero.
+ D, Download     Download mode, from serial port (Serial2).
----------------------------------------------------
+++ Operations
-------------
+ m, Read         Memory: Read an SD card file into program memory.
+ M, Write        Memory: Write program memory to an SD card file.
+ n, Directory    Directory file listing of the SD card.
+ l, Load sample  Load a sample program.
+ L, Load hex     Load hex code from the serial port.
-------------
+ i, info         Information print of registers.
+ j, settings     Settings information.
-------------
+ t/T Terminal    Disable/enable VT100 terminal commandline (cli) escape codes.
+ w/W USB serial  Disable/enable USB serial output.
+ y/Y Serial2     Disable/enable Serial2 for I/O.
+ o/O LEDs        Disable/enable LED light output.
-------------
+ Enter key       Refresh USB serial output front panel display.
+ u/U Log msg     Log messages off/on.
+ z/Z cursor      VT100 block cursor off/on.
----------------------------------------------------
?- 
INTE MEMR INP M1 OUT HLTA STACK WO INT        D7  D6   D5  D4  D3   D2  D1  D0
 .    *    .  *   .   .    .    *   .         .   .    .   .   .    .   .   .
WAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8   A7  A6   A5  A4  A3   A2  A1  A0
 *    .      .   .   .   .   .   .   .   .    .   .    .   .   .    .   .   .
            S15 S14 S13 S12 S11 S10  S9  S8   S7  S6   S5  S4  S3   S2  S1  S0
             v   v   v   v   v   v   v   v    v   v    v   v   v    v   v   v
 ------ 
+ Ready to receive command.
?- 
````
In the above, note that the virtual data LEDs (D0..D7) are all off (.), zeros.
That is the initial value of the first memory byte of memory, zero.

In the top bar of the Serial Monitor, enter "X" and click Send.
X will EXAMINE NEXT address, current address + 1.
````
?- + X, EXAMINE NEXT: 1
````
The virtual front panel memory address LEDs (S0..S15), now show address one.
The memory data byte for address, is also zero.
````
INTE MEMR INP M1 OUT HLTA STACK WO INT        D7  D6   D5  D4  D3   D2  D1  D0
 .    *    .  *   .   .    .    *   .         .   .    .   .   .    .   .   .
WAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8   A7  A6   A5  A4  A3   A2  A1  A0
 *    .      .   .   .   .   .   .   .   .    .   .    .   .   .    .   .   *
            S15 S14 S13 S12 S11 S10  S9  S8   S7  S6   S5  S4  S3   S2  S1  S0
             v   v   v   v   v   v   v   v    v   v    v   v   v    v   v   v
````
In the Serial Monitor, enter "123" and click Send.
The toggles, S1, S2, S3 are now up (on).
````
            S15 S14 S13 S12 S11 S10  S9  S8   S7  S6   S5  S4  S3   S2  S1  S0
             v   v   v   v   v   v   v   v    v   v    v   v   ^    ^   ^   v
````
Enter "p" and click Send.
The data LEDs now show D1, D2, and D3 on.
The data byte value of 1110 was deposited into memory address 1.
````
INTE MEMR INP M1 OUT HLTA STACK WO INT        D7  D6   D5  D4  D3   D2  D1  D0
 .    *    .  *   .   .    .    *   .         .   .    .   .   *    *   *   .
WAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8   A7  A6   A5  A4  A3   A2  A1  A0
 *    .      .   .   .   .   .   .   .   .    .   .    .   .   .    .   .   *
            S15 S14 S13 S12 S11 S10  S9  S8   S7  S6   S5  S4  S3   S2  S1  S0
             v   v   v   v   v   v   v   v    v   v    v   v   ^    ^   ^   v
````
Enter "R", for RESET, and click Send.
The memory address is set back to zero. The data byte is zero.
````
 *    .      .   .   .   .   .   .   .   .    .   .    .   .   .    .   .   .
            S15 S14 S13 S12 S11 S10  S9  S8   S7  S6   S5  S4  S3   S2  S1  S0
````
Enter "X" to examine the next byte in memory, and click Send.
The memory address is set to one. The data byte is 1110, which it was set to, above.
````
?- + X, EXAMINE NEXT: 1
?- 
INTE MEMR INP M1 OUT HLTA STACK WO INT        D7  D6   D5  D4  D3   D2  D1  D0
 .    *    .  *   .   .    .    *   .         .   .    .   .   *    *   *   .
WAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8   A7  A6   A5  A4  A3   A2  A1  A0
 *    .      .   .   .   .   .   .   .   .    .   .    .   .   .    .   .   *
            S15 S14 S13 S12 S11 S10  S9  S8   S7  S6   S5  S4  S3   S2  S1  S0
             v   v   v   v   v   v   v   v    v   v    v   v   v    v   v   v
````

Now that you have the basics to manage the virtual front panel,
you can follow along in the Deramp video introducing the Altair 8800 front panel.

https://www.youtube.com/watch?v=suyiMfzmZKs

Note, enter a..f to toggle the virtual front panel toggles S10..S15.

--------------------------------------------------------------------------------
## Running Programs on the Altair101a

In the top bar of the Serial Monitor, enter "l" and click the Send button.
A list of programs are list that are built in.
````
?- + l, load a sample program.
+ Sample programs.
----------
++ k, Kill the Bit version for serial VT100 input.
++ r, Test the setting of registers.
----------
++ K, Kill the Bit front panel fast version.
----------
+ x, Exit: don't load a program.
````
Enter "r" and click Send to load the program to Test the setting of registers.
A load confirmation message is displayed followed by a print of the virtual front panel.
This time the first byte of the loaded program is displayed in the virtual data LEDs (00111110).
````
?- + r, load: a program that uses MVI to set the registers with values from 1 to 7.
     Use option 'i' to list the register values.
?- 
INTE MEMR INP M1 OUT HLTA STACK WO INT        D7  D6   D5  D4  D3   D2  D1  D0
 .    *    .  *   .   .    .    *   .         .   .    *   *   *    *   *   .
WAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8   A7  A6   A5  A4  A3   A2  A1  A0
 *    .      .   .   .   .   .   .   .   .    .   .    .   .   .    .   .   .
            S15 S14 S13 S12 S11 S10  S9  S8   S7  S6   S5  S4  S3   S2  S1  S0
             v   v   v   v   v   v   v   v    v   v    v   v   v    v   v   v
 ------ 
+ Ready to receive command.
?- 
````
Enter "i" and click Send. Register information and other system information is displayed.
````
?- + i: Information.
+++ Software: Altair101a
++ Program version: 1.00
++ CPU: Mega 2560, Oscillator frequency: 16000000
++ Memory size: 5376
++ host_read_status_led_WAIT()=1
++ host_read_status_led_HLDA()=0
++ host_read_status_led_INTE()=0
++ host_read_status_led_HLTA()=0
++ PC      0 = 00000000:00000000 = 0000 Program pointer
++ SP      0 = 00000000:00000000 = 0000 Stack pointer
++ regS:             00000000 :Sign=0:Zero=0:.:HalfCarry=0:.:Parity=0:.:Carry=0:
++ regA:   0 = 000 = 00000000
++ regB:   0 = 000 = 00000000  regC:   0 = 000 = 00000000
++ regD:   0 = 000 = 00000000  regE:   0 = 000 = 00000000
++ regH:   0 = 000 = 00000000  regL:   0 = 000 = 00000000
++ Front panel display Status byte,  fpStatusByte:    162 = 242 = 10100010
++ Front panel display Data byte,    fpDataByte:        0 = 000 = 00000000
++ Front panel display Address word, fpAddressWord:     0 = 00000000:00000000

?- 
````
Enter "r" and click Send. The program will run.
````
?- + r, RUN.
?- + runProcessor()
++ HALT, host_read_status_led_WAIT() = 0
````

Enter "i" and click Send. Register information is displayed.
This time, the register values are set.
````
++ regA:   1 = 001 = 00000001
++ regB:   2 = 002 = 00000010  regC:   3 = 003 = 00000011
++ regD:   4 = 004 = 00000100  regE:   5 = 005 = 00000101
++ regH:   6 = 006 = 00000110  regL:   7 = 007 = 00000111
````

You have confirmed that your Altair101a is working.

Now that you have the basics to run program from the virtual front panel,
you can follow along in the Deramp video introducing Altair 8800 Front Panel Programming
where you enter and run the Kill the Bit program.

https://www.youtube.com/watch?v=EV1ki6LiEmg

Note, since this is a serial port interface to the LED lights,
change byte ??? to ???, to adjust the speed.

--------------------------------------------------------------------------------
My goal is to use the Altair-Duino code to process machine byte code
in my Altair 101 Processor.ino program.
This will save me finishing the writing and testing of my byte code interpreter.
And, I will know that the interpreter works properly for all the opcodes.

--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
Cheers
