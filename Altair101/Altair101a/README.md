--------------------------------------------------------------------------------
# Altair101a is an Altair 8800 Simulator

This simulator was built to run Intel 8080 machine code programs.
My first simulator, Processor.ino, was wrote while building and designing hardware
based on goals to manage an Altair 8800 front panel
and add other components such as an MP3 player and real time clock.

Altair101a uses the processing control code from Processor.ino.
To execute machine instructions,
it uses David Hansel's Altair 8800 simulator code.
Hansel's code is proven to work, as it is runs on the Altair-Duino.
It runs Kill the Bit, versions of Basic, CP/M, and CP/M programs.

Altair101a is a success in that it runs a modified version of the assembler program GALAXY80,
a version of the classic Star Trek game, sample assembler programs I wrote, and
4K Basic. My development Altair101a runs an Arduino Mega 2560.
The next step is to build a new hardware machine using an Arduino Due.
The Altair101a simulator program is already testing on a Due,
should be fun building and running the Due machine.

The long term software goal for Altair101a, is to run CP/M.
Other goals are to implement the hardware front panel and add the MP3 player.

--------------------------------------------------------------------------------
## To Run Altair101a

Download the Altair101a project program files into an Arduino project directory of the name Altair101a.
````
Altair101a.ino
Altair101a.h
AltairSamples.cpp
AltairSamples.h
cpucore_i8080.cpp
cpucore_i8080.h
````

Using your serial cable, connect your Arduino Mega 2560 or Due to your computer.
Load Altair101a.ino from the Arduino IDE.
All the supporting program files are loaded as well.
Click Run.
The program files are compiled and uploaded to your Arduino.
The following message was displayed for uploading to my Mega.
Your exact numbers and percentages may vary because I continue to update the program.
You can safely ignore the Low memory available warning.
There is enough memory to run the program.
````
Sketch uses 70800 bytes (27%) of program storage space. Maximum is 253952 bytes.
Global variables use 7631 bytes (93%) of dynamic memory, leaving 561 bytes for local variables. Maximum is 8192 bytes.
Low memory available, stability problems may occur.
````

Open the Arduino IDE Serial Monitor (menu: Tools/Serial Monitor).
The following will be displayed.
At this time, you will get an error message initializing the SD card
because you don't have one connected.
If you did happen to connect one before running the program, you will get a
connection message(+ SD card module is initialized).
Either way, your Altair101a is ready to be tested.
````
+++ setup
- Error initializing SD card module.
+++ Altair101a initialized.
?- 
````

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
In the above, note that the virtual data LEDs are all off (.), zeros.
That is the initial values of the memory bytes, zero.

In the top bar of the Serial Monitor, enter "l" and click the Send button.
A list of programs are list that are built in.

For example, enter "m" and click Send to load the program to Test the setting of registers.
````
?- + l, load a sample program.
+ Sample programs.
----------
++ k, Kill the Bit version for serial VT100 input.
++ m, Test the setting of registers.
----------
++ K, Kill the Bit front panel fast version.
----------
+ x, Exit: don't load a program.
````

Enter "m" and click Send.
A load confirmation is displayed followed by a print of the virtual front panel.
This time the first byte of the program is displayed in the virtual data LEDs.
````
?- + m, load: a program that uses MVI to set the registers with values from 1 to 7.
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

--------------------------------------------------------------------------------
My goal is to use the Altair-Duino code to process machine byte code
in my Altair 101 Processor.ino program.
This will save me finishing the writing and testing of my byte code interpreter.
And, I will know that the interpreter works properly for all the opcodes.

--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
Cheers
