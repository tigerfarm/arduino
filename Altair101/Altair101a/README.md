--------------------------------------------------------------------------------
# Altair 8800 Simulator Software used in the Altair-Duino

--------------------------------------------------------------------------------
I can load the 4K basic program into memory.
But not sure how to start it.
Not sure if interrupts are required or input as normal.

https://www.youtube.com/watch?v=8InWiihlIQw

$ ls -l /var/folders/07/0fdddwrn42lc27mprqg359b40w1snn/T/arduino_build_142320/
/var/folders/07/0fdddwrn42lc27mprqg359b40w1snn/T/arduino_build_142320/arduinoTest.ino.bin

/Users/dthurston/Library/Arduino15/packages/arduino/tools/bossac/1.7.0-arduino3/bossac

/Users/dthurston/Library/Arduino15/packages/arduino/tools/bossac/1.7.0-arduino3/bossac -i -d --port=tty.usbmodem1421 -U true -i -e -w -v /var/folders/07/0fdddwrn42lc27mprqg359b40w1snn/T/arduino_build_142320/arduinoTest.ino.bin -R

$ ls -l /dev/tty.*
crw-rw-rw-  1 root  wheel   20,   0 Dec 14 16:07 /dev/tty.Bluetooth-Incoming-Port
crw-rw-rw-  1 root  wheel   20, 0x00000304 Dec 29 17:27 /dev/tty.usbmodem1421

--------------------------------------------------------------------------------
All output to port#2 which outputs to Serial2 (port# 2) if enabled, else to the default Serial port (port# 3).

00000001.BIN    CLI
+++ Welcome to the machine.
+ Press keys. Ctrl+c to exit.
> 

00000010.BIN    List printable characters.
: :!:":#:$:%:&:':(:):*:+:,:-:.:/:0:1:2:3:4:5:6:7:8:9:
...

00000011.BIN    Print bytes as decimal digits and as a binary number string.
++ Number, 001 = 00000001
++ Number, 200 = 11001000
...
++ Number, 255 = 11111111

00000101.BIN    Kill the Bit serial version
INTE MEMR INP M1 OUT HLTA STACK WO INT        D7  D6   D5  D4  D3   D2  D1  D0
 .    .    *  .   .   .    .    *   .         .   .    .   .   .    *   .   *
WAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8   A7  A6   A5  A4  A3   A2  A1  A0
 *    .      .   .   .   .   .   .   .   *    .   .    .   .   .    .   .   *
            S15 S14 S13 S12 S11 S10  S9  S8   S7  S6   S5  S4  S3   S2  S1  S0
             v   v   v   v   v   ^   v   ^    v   v    v   v   v    v   v   v
 ------ 


--------------------------------------------------------------------------------
My goal is to use the Altair-Duino code to process machine byte code
in my Altair 101 Processor.ino program.
This will save me finishing the writing and testing of my byte code interpreter.
And, I will know that the interpreter works properly for all the opcodes.

Steps to use the code for running opcode instructions.

Create a minimum code base to run machine code:
+ 1. Get the simulator to run on Mega, maybe the Due. I have it running on Mega.
+ 2. Didn't work for me: Get the serial output to work properly. I'm using MacWise VT100 emulator.
+ 3. I found the documentation. I can interact with the Simulator over serial command line.
+ 4. I can enter and run simple programs. Load and run CPM?
+ 5. Remove programs not require to run a machine code programs, such as: dazzler.*, disassembler.*, and others.

After the above, I chose to modify Altair101b to not use VT100 emulation
but to simply output text to the serial port which works well in the Arduino IDE monitor.

Modify the minimum code base to work with Altair 101:
+ 6. Manage Altair 101 memory size.
+ 7. Starting with a simple Serial control program, Add Altair 880 Simulator functions.
        Get memory manage to work (EXAMINE and DEPOSIT).
+ 8. Get a Program to Run and Add monitor/debug functions.
+ 9. Consolidate the program files (*.h and *.cpp).
+ 10. Get single step to work, with number of opcode cycles: memory read and write, stack push and pop.

Get opcode processing to work with the goal to integrate Altair101a into Processor.ino:
+ 11. Sync Altair101a.ino status light bits with values in Processor.ino.
+ 12. Implement Mac terminal VT100 output to display panel lights, labels and toggles.
+ 13. Get Kill the Bit, to run which requires input (opcode IN).
+ 13. Implement lightsStatusAddressData(Status,Address,Data) in Altair101a.ino.

````
void lightsStatusAddressData( byte status8bits, unsigned int address16bits, byte data8bits) {
    ...
}
````
+ Run Altair101a.ino on the Altair 101 desktop machine.
+ Output to Altair 101 panel LED.
+ Input from Altair 101 panel switches.

--------------------------------------------------------------------------------
### Connecting to the Arduino serial ports Using Mac VT100 Terminals.

Following is how to find the serial ports.
Then connect using the screen command, in a VT100 Mac terminal.
And how to disconnect and close the connection.
````
$ ls /dev/tty.*
/dev/tty.Bluetooth-Incoming-Port	/dev/tty.wchusbserial14110
$ screen /dev/tty.wchusbserial14110

$ screen /dev/tty.wchusbserial14120 <baud rate>
$ screen /dev/tty.wchusbserial14120 57600
...

To exit and close the connection: Control-A followed by Control-\.
Then, answer "y" to exit and close the connection.

If screen is messed up, run reset:
$ reset
````

##### Sequence to upload a file:

Confirm that the Mega and second serial component are plugged into the USB hub.
Confirm that the USB ports are turned on.

In a terminal window,
Connect to Mega's default serial port.
The Mega is running Altair101a.ino.
Put the program into Download mode.
````
$ ls /dev/tty.*
/dev/tty.Bluetooth-Incoming-Port	/dev/tty.wchusbserial14230		/dev/tty.wchusbserial14240
$ screen /dev/tty.wchusbserial14240

... screen clears, displayed on top:
+++ Setup.
+++ Altair 101a initialized.
?- + D, Download mode.
?- + Download mode: ready to receive a program. Enter, x, to exit.
````

Run asm.
If required, select a program file, assemble it, and upload the byte code.
Set to use the Mega's second serial port.
Upload the byte code.
````
$ java -jar asm.jar
> list ports
...
> set port tty.wchusbserial14230
+ Set the serial port name: tty.wchusbserial14230
+ Serial port set to: tty.wchusbserial14230
+ Serial port set to system name: /dev/tty.wchusbserial14230
> upload
...
````
If the upload failed, confirm that serial program is not connected to the port.

In a terminal window, connect to the second serial port.
````
$ screen /dev/tty.wchusbserial14230 57600
... screen clears with the cursor at the home position.
````

Set second serial port for output.
Run the program.
````
++ Byte#    207, Byte: 00001101 00d 015    13
++ Byte#    208, Byte: 00001010 00a 012    10
++ Byte#    209, Byte: 11111111 0ff 377   255
+ Download complete.
?- + Y, Serial2 on (begin), baud rate: 57600.
?- + r, RUN.
?- + runProcessor()

Start...

Hello

Again3.
++ HALT, host_read_status_led_WAIT() = 0
?- 
````
View the output in the second serial terminal window.
````
------------
+ regA:   1 = 001 = 00000001
+ regB:   2 = 002 = 00000010  regC:   3 = 003 = 00000011
+ regD:   4 = 004 = 00000100  regE:   5 = 005 = 00000101
+ regH:   6 = 006 = 00000110  regL:   7 = 007 = 00000111
------------
....
````
--------------------------------------------------------------------------------
##### Links

Chris Davis of Altairduino.com has set up a Google Group for discussion of Altair-Duino related questions:
https://groups.google.com/forum/#!forum/altair-duino 

[dhansel / Altair8800](https://github.com/dhansel/Altair8800)
    
[Sample programs](https://altairclone.com/downloads/)

[Arduino Due](https://www.adwaterandstir.com)
    
Download front panel full-sized [images and files](https://www.vintagecomputer.net/MITS/software/)

Kill the Bit [hex byte](https://altairclone.com/downloads/front_panel/KILLBITS.HEX)
program [listing](https://altairclone.com/downloads/front_panel/KILLBITS.ASM),
Assembler listing](https://altairclone.com/downloads/front_panel/KILLBITS.PRN)

Hex of the bytes
````
      :100000002100001680010E001A1A1A1A09D20800DF
      :08001000DBFFAA0F57C3080033
      :0000000000
````

--------------------------------------------------------------------------------
## Steps I used to run David Hansel's Altair 8800 Simulator code


##### Preparation work

Arduino IDE Tools settings for Mega:
````
Board: "Arduino Mega or Mega 2560"
Processor: "ATMega2560 (Mega2560)"
Port: ...14110
````
Arduino IDE Tools settings for Due:
````
Connect to the programming USB port which is the one closest to the power plug.
Board: "Arduino Due" (Programming port)
Port: ...14210

From: https://www.adwaterandstir.com/install/
Click Sketch > Include Library > Manage Libraries…
Search and install SdFat by Bill Greiman.
Search and install, “DueFlashStorage” by Sebastian Nilsson.
````

Note, the Simulator program uses hardware specific PORT values.
I've changed them to be memory bytes which are not hardware specific.
````
// Microcontroller Port Registers:
// Port registers allow for lower-level and faster manipulation of the i/o pins.
// https://www.arduino.cc/en/Reference/PortManipulation
//    D PORTD maps to digital pins 0 to 7.
//    B PORTB maps to digital pins 8 to 13.
//    C PORTC maps to analog input pins 0 to 5.
// Requires including Arduino.h.
//    A statusByteA maps to digital pins 22 to 29 (physical pins).
//                  Digital pin 50 (MISO), Digital pin 10, 11, 12, 13 (PWM)?
//                  Serial.println(bitRead(statusByteA.IN, 0)); // print the state of pin 2 (statusByteA, bit 0)
// https://forum.arduino.cc/index.php?topic=52534.0
````

--------------------------------------------------------------------------------
### Other Notes

From
[Documentation](https://www.hackster.io/david-hansel/arduino-altair-8800-simulator-3594a6).

Before uploading the sketch to the Arduino Due, 
be sure to switch the Arduino compiler's optimization setting to "performance". 
By default it is set to "size" (not sure why since the Due has 512k flash memory). To do so, load file
c:\Users\[user]\AppData\Local\Arduino15\packages\arduino\hardware\sam\1.6.9\platform.txt
into a text editor and change any occurrence of "-Os" to "-O3". 
You can skip this step but then the simulator will run significantly slower.


--------------------------------------------------------------------------------
### 1. Get the simulator to run on Mega

Get started with the software 11:50 into the [video](https://www.youtube.com/watch?v=zqRILp6srBk).

Download the software from David Hansel's [Altair 8800 Simulator](https://github.com/dhansel/Altair8800) repository.

[Documentation](https://www.hackster.io/david-hansel/arduino-altair-8800-simulator-3594a6).
The simulator software can also run on a bare-bones Arduino (Mega or Due) without any front panel controls connected.
This does allow to run quite a few of the included programs.
To do so, edit the config.h source file and set #define STANDALONE 1 (instead of 0).

Getting it to run on a Mega.
Emulated RAM size is 64KB (Due) or 6K (Mega)
````
Edit config.h, set:
#define STANDALONE 1
drive.cpp:48:2: error: #error Disk drive emulation requires host filesystem

#error Arduino MEGA port does not support disk drives. Set NUM_DRIVES, NUM_TDRIVES and NUM_CDRIVES to 0 in config.h
// #define NUM_DRIVES 4
#define NUM_DRIVES 0        // Stacy, for standalone test.
#define NUM_HDSK_UNITS 0    // Stacy, for standalone test.

#error Throttling neither supported nor necessary for Arduino MEGA. Set USE_THROTTLE to 0 in config.h
// #define USE_THROTTLE 1
#define USE_THROTTLE 0        // Stacy, for standalone test.

#error Either set USE_PRINTER to 0 in config.h or reduce MEMSIZE in host_mega.h to 4096+1024, otherwise stability problems may occur
// #define USE_PRINTER 1
#define USE_PRINTER 0    // Stacy, for standalone test.

#error MAX_NUM_ROMS>1 is NOT recommended due to memory constraints. See comment in host_mega.h
// #define MAX_NUM_ROMS 8
#define MAX_NUM_ROMS 0    // Stacy, for standalone test.

Sketch uses 158087 bytes (62%) of program storage space. Maximum is 253952 bytes.
Global variables use 7796 bytes (95%) of dynamic memory, leaving 396 bytes for local variables. Maximum is 8192 bytes.
Low memory available, stability problems may occur.

PC   = 34F8 = [FF FF FF] = RST 0038
 SP   = 0D42 = [82 92 E7 01 5D F2 0A 32]
 regA = D6 regS = 40 = .Z......
 regB = 1D regC = 8B regD = 77 regE = 99 regH = AF regL = CB
[s[0;0HINTE PROT MEMR INP M1 OUT HLTA STACK WO INT  D7  D6  D5  D4  D3  D2  D1  D0
 .    .    *    .  *   .   .     .   *   .   *   *   *   *   *   *   *   *
WAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8  A7  A6  A5  A4  A3  A2  A1  A0
 *    .      .   .   *   *   .   *   .   .   *   *   *   *   *   .   .   .
            S15 S14 S13 S12 S11 S10  S9  S8  S7  S6  S5  S4  S3  S2  S1  S0
             v   v   v   v   v   v   v   v   v   v   v   v   v   v   v   v
            Stop  Step  Examine  Deposit  Reset  Protect   Aux  Aux
             o     o      o        o       o        o       o    o  
            Run         E.Next   D.Next    CLR   Unprotect
[K
[K
[K
[K
[K[uStart address: e
End address: 
:0000000000
````
To reach the configuration menu, type C in the terminal window.

--------------------------------------------------------------------------------
### 3. Interact with the Simulator over Serial command line.

From the Simulator download zip file, Documentation.pdf:
+ Page 27, has the documented commands, also listed below.
+ Page 28, has how to DEPOSIT the program into memory, and then run it.
    The instructions also have save to memory and re-load.

Run the Simulator in MacWise.
````
INTE PROT MEMR INP M1 OUT HLTA STACK WO INT  D7  D6  D5  D4  D3  D2  D1  D0
 .    .    *    .  *   .   .     .   *   .   *   *   *   *   *   *   *   *
WAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8  A7  A6  A5  A4  A3  A2  A1  A0
 *    .      .   *   .   *   *   *   *   .   *   *   *   .   *   *   *   .
            S15 S14 S13 S12 S11 S10  S9  S8  S7  S6  S5  S4  S3  S2  S1  S0
             v   v   v   v   v   v   v   v   v   v   v   v   v   v   v   v
            Stop  Step  Examine  Deposit  Reset  Protect   Aux  Aux
             ^     o      o        o       o        o       ^    o  
            Run         E.Next   D.Next    CLR   Unprotect
````

Use the IDE serial monitor to send "C" which echos data and brings up the command prompt.
````
Enable pro(f)iling          : no
Enable serial (p)anel       : yes
Enable serial (d)ebug       : yes
Configure (m)emory          : [4;31H[K6 KB RAM
Aux1 shortcut program (u/U) : 16k ROM Basic
Configure host (s)erial     : 115200 baud

(E) Configure serial cards  : SIO,2SIO-P1,2SIO2-P1,2SIO2-P2 mapped
(I) Configure interrupts    : Interrupts connected directly to CPU

(M)anage Filesystem     
(S)ave configuration    (L)oad configuration
(R)eset to defaults     E(x)it

Command:
````

Commands:
````
0-9,a-f STANDALONE only: Toggle SW0-15
/ STANDALONE only: Prompt for value to set SW0-15
r Run
o Stop
t Step
R Reset
! Hard reset (STOP+RESET)
X/x Examine/examine next
P/p Deposit/deposit next
> Run from address

U AUX1 up
u AUX1 down
````

--------------------------------------
To run Basic 4K:
1. S0 S2    00000101
2. u        AUX1 down
PC   = 003C = [64 6F 0A] = MOV H,H
 SP   = FC81 = [D3 37 C9 08 00 00 80 00]
 regA = 4F regS = 00 = ........
 regB = E0 regC = A7 regD = D2 regE = D2 regH = 55 regL = FF
[Running 4k Basic]

MEMORY SIZE? 
TERMINAL WIDTH? 
WANT SIN? N
WANT RND? N
WANT SQR? N

3021 BYTES FREE

BASIC VERSION 3.2
[4K VERSION]

OK
PRINT "HELLO"
HELLO

OK

--------------------------------------
Enter and run a program.
````
To enter the kill-the-bit game below into the ALTAIR:
1. Activate RESET
2. Set SW7-0 to first byte of program (041 octal, 00 100 001 binary)
3. Activate DEPOSIT
4. Set SW7-0 to next byte of program
5. Activate DEPOSIT NEXT
6. Repeat steps 4-5 until all bytes have been entered
7. If running the simulator on an Arduino MEGA, the game runs slow.
To compensate, change the content of memory location 006 from 016 to 056 (all octal).

To run the game:
1. Activate RESET
2. Set SW15-8 switches to 0 Activate RUN
3. Activate RUN
````
To save the game to Arduino storage:
````
1. Set SW15-8 to 0 (selects memory page 0)
2. Set SW7 to 1 (selects memory page operation)
3. Set SW6 to 1 (selects memory page save)
4. Set SW5-0 to 0 (selects file number 0)
5. Activate AUX1 down
````
To load the game from Arduino storage:
````
1. Set SW15-0 as above, except SW6 to 0 (selects memory page load)
2. Activate AUX1 down
````

The simulator provides a quick and easy way to load 4k BASIC by setting SW0-7 to 00000101
and pressing AUX1 down (see “Aux1 down” section above).

--------------------------------------------------------------------------------
### 4. Enter and Run Simple Programs

Exercise steps:
````
Enter a program to load a byte into an address and then halt.
RUN the program.
EXAMINE the address where the byte was loaded.
````
Program listing:
````
Address(lb):databyte :hex:oct > description
00000000:   00111110 : 3E:076 > opcode: mvi a,6
00000001:   00000110 : 06:006 > immediate: 6 : 6
00000010:   00110010 : 32:062 > opcode: sta 96
00000011:   01100000 : 60:140 > lb: 96
00000100:   00000000 : 00:000 > hb: 0
00000101:   01110110 : 76:166 > opcode: hlt
````
Enter the program bytes base on the above listing.
````
Enter   Action
R       RESET to memory address to zero.
        The following assumes all address switches are in the down position (v).
        ---
        Toggle switches to enter the MVI opcode: 00 111 110
1       Up switch A1 (^).
2       Up switch A2.
3       Up switch A3.
4       Up switch A4.
5       Up switch A5.
P       DEPOSIT the opcode into memory address 0.
        ---
        Toggle switches to enter the MVI immediate value, 6: 00 000 110
3       Down switch A3 (v).
4       Down switch A4.
5       Down switch A5.
p       DEPOSIT NEXT the immediate value into memory address 1.
        ---
        Toggle switches to enter the STA opcode: 00 110 010
2       Down switch A2.
4       Up switch A4.
5       Up switch A5.
p       DEPOSIT NEXT the opcode into memory address 2.
        ---
        Toggle switches to enter low address byte 96: 01 100 000
1       Down switch A1.
4       Down switch A4.
6       Up switch A6.
p       DEPOSIT NEXT to deposit the address low byte into memory address 3.
        ---
        Toggle switches to enter high address byte 0: 00 000 000
5       Down switch A5.
6       Down switch A6.
p       DEPOSIT NEXT to deposit the address high byte into memory address 4.
        ---
        Toggle switches to enter the HLT opcode: 01 110 110
1       Up switch A1.
2       Up switch A2.
4       Up switch A4.
5       Up switch A5.
6       Up switch A6.
p       DEPOSIT NEXT to deposit the address high byte into memory address 5.
````
Run the program which will halt after loading the byte into a memory address.
````
Enter   Action
R       RESET to memory address to zero.
r       Run the program. Runs to memory address 5, and halts.
````
View the address where the byte was loaded.
````
Enter   Action
        Toggle switches to enter the memory address, 96: 01 100 000
1       Down switch A1.
2       Down switch A2.
4       Down switch A4.
X       EXAMINE to deposit the address high byte into memory address 5.
````
The data value will be, 6: 00 000 110.

--------------------------------------------------------------------------------
Enter and run the jump-loop program.
````
Enter   Action
R       RESET to memory address to zero.
        The following assumes all address switches are in the down position (v).
        ---
        Toggle switches to enter the jump opcode: 11 000 011
0       Up switch A0.
1       Up switch A1.
6       Up switch A6.
7       Up switch A7.
P       DEPOSIT the opcode into memory address 0.
        ---
        Toggle switches to enter low address byte 6: 00 000 110
0       Down switch A0.
2       Up switch A2.
6       Down switch A6.
7       Down switch A7.
p       DEPOSIT NEXT to deposit the address low byte into memory address 1.
        ---
        Toggle switches to enter high address byte 0: 00 000 000
1       Down switch A1.
2       Down switch A2.
p       DEPOSIT NEXT to deposit the address high byte into memory address 2.
        ---
p       DEPOSIT NEXT to deposit NOP opcode into memory address 3.
p       DEPOSIT NEXT to deposit NOP opcode into memory address 4.
p       DEPOSIT NEXT to deposit NOP opcode into memory address 5.
        ---
        Toggle switches to enter the jump opcode: 11 000 011
0       Up switch A0.
1       Up switch A1.
6       Up switch A6.
7       Up switch A7.
p       DEPOSIT NEXT the opcode into memory address 6.
        ---
        Toggle switches to enter low address byte 0: 00 000 000
0       Down switch A0.
1       Down switch A1.
6       Down switch A6.
7       Down switch A7.
p       DEPOSIT NEXT to deposit the address low byte into memory address 7.
p       DEPOSIT NEXT to deposit the address high byte into memory address 8.
R       RESET to memory address to zero.

Address(lb):databyte :hex:oct > description
00000000:   11000011 : C3:303 > opcode: jmp 6
00000001:   00000110 : 05:005 > lb: 6
00000010:   00000000 : 00:000 > hb: 0
00000011:   00000000 : 00:000 > NOP
00000100:   00000000 : 00:000 > NOP
00000101:   00000000 : 00:000 > NOP
00000110:   11000011 : C3:303 > opcode: jmp 0
00000111:   00000110 : 05:005 > lb: 6
00001000:   00000000 : 00:000 > hb: 0
````
Jump-loop program is entered and ready to run.

--------------------------------------------------------------------------------
### 5. Remove programs not require to run a machine code programs

Removed the following, and other files.
````
// #include "breakpoint.h"
// #include "disassembler.h"
// #include "filesys.h"
// #include "drive.h"
// #include "tdrive.h"
// #include "cdrive.h"
// #include "hdsk.h"
// #include "printer.h"
// #include "prog.h"
// #include "dazzler.h"
// #include "vdm1.h"
````

Removed all SD card and EEPROM file functions and programs.

--------------------------------------------------------------------------------
### 6. Manage Altair 101 memory size.

Need to reduce current memory usage.
````
Sketch uses 63164 bytes (24%) of program storage space. Maximum is 253952 bytes.
Global variables use 7709 bytes (94%) of dynamic memory, leaving 483 bytes for local variables. Maximum is 8192 bytes.
````
Memory size definition: MEMSIZE in host_mega.h or host_due.h.

After reducing MEMSIZE in host_mega.h, Global variables use dropped from 7709 to 2589 bytes
````
Sketch uses 63072 bytes (24%) of program storage space. Maximum is 253952 bytes.
Global variables use 2589 bytes (31%) of dynamic memory, leaving 5603 bytes for local variables. Maximum is 8192 bytes.
````

From host_mega.h
````
// Mega2650: 8k SRAM, use 6k for emulated RAM
// NOTE: Using too much emulated RAM can cause stability issues when
//       running the emulator. Modify settings in config.h and the MEMSIZE
//       setting here to make sure Arduino IDE says (after compiling) that
//       AT LEAST 310 bytes of RAM are left for local variables!
//       If you run into weird emulation issues, consider (temporarily)
//       using only 5K MEMSIZE here to see if that resolves the problem.
// #define MEMSIZE (4096+2048)
#define MEMSIZE (1024)
````
From host_due.h
````
#define MEMSIZE 0x10000         // 16K
````

From mem.h
````
extern byte Mem[MEMSIZE];
````

--------------------------------------------------------------------------------
### 7. Starting with a simple Serial control program, Add Altair 880 Simulator functions.

````
Start with my program, serialRead.ino.
Add function to display the text version of the front panel: print_panel_serial().
Create switch case statement to handle serial commands to toggle the address/data switches.
Sending a serial character from 0-9 and a-f, toggles the address/data switches and are displayed.
Create switch case statement to handle serial commands such as EXAMINE, EXAMINE NEXT, DEPOSIT, DEPOSIT NEXT
Add functionality to manage memory.
````
--------------------------------------------------------------------------------
### 8. Get a Program to Run.

Implement a method to run sample programs.
````
Add a serial command option to load a simple program into memory.
Use EXAMINE and EXAMINE NEXT to view the program.
Run the program.
````
Add monitor/debug functions.
````
Run one opcode at a time, with a serial input requirement between instructions.
````
--------------------------------------------------------------------------------
### 9. Consolidate the program files (*.h and *.cpp).
````
Altair101a.h
Altair101a.ino
cpucore_i8080.cpp
cpucore_i8080.h         Clear extern and duplicate definitions
host_mega.h
````
--------------------------------------------------------------------------------
### 10. Prepare the program files to be used in Processor.ino.

````
Use Processor.ino Loop flow.
Initialize front panel status lights.
Get SINGLE STEP to work.
Confirm front panel LED functions.
Confirm front panel switch and toggle functions.
````
File list:
````
Altair101a.h
Altair101a.ino
cpucore_i8080.cpp
cpucore_i8080.h         Clear extern and duplicate definitions
````
Load and run a program.
````
 ------ 
+ Ready to receive command.
+ l, loaded a simple program.
...
+++ Setup.
+++ Altair 101a initialized.
INTE PROT MEMR INP M1 OUT HLTA STACK WO INT  D7  D6  D5  D4  D3  D2  D1  D0
 .    .    *    .  *   .   .     .   *   .   .   .   .   .   .   .   .   .
WAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8  A7  A6  A5  A4  A3  A2  A1  A0
 .    .      .   .   .   .   .   .   .   .   .   .   .   .   .   .   .   .
            S15 S14 S13 S12 S11 S10  S9  S8  S7  S6  S5  S4  S3  S2  S1  S0
             v   v   v   v   v   v   v   v   v   v   v   v   v   v   v   v
 ------ 
+ Ready to receive command.
+ runProcessorWait()
+ h, Help.
-------------
+ r, RUN.
+ x, EXAMINE switch address.
+ x, EXAMINE NEXT address, current address + 1.
+ p, DEPOSIT at current address
+ P, DEPOSIT NEXT address, current address + 1
+ R, RESET, set address to zero.
-------------

+ Ready to receive command.
+ r, RUN.
+ runProcessor()
++ regPC:0: data: 62 = 076 = 00111110
++ regPC:2: data: 50 = 062 = 00110010
++ regPC:5: data:118 = 166 = 01110110
INTE PROT MEMR INP M1 OUT HLTA STACK WO INT  D7  D6  D5  D4  D3  D2  D1  D0
 .    .    *    .  .   .   *     .   *   .   .   *   *   *   .   *   *   .
WAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8  A7  A6  A5  A4  A3  A2  A1  A0
 .    .      .   .   .   .   .   .   .   .   .   .   .   .   .   *   .   *
            S15 S14 S13 S12 S11 S10  S9  S8  S7  S6  S5  S4  S3  S2  S1  S0
             v   v   v   v   v   v   v   v   v   ^   ^   v   v   v   v   v
 ------ 
+ Ready to receive command.
````

Running from command line.
````
+ Initialized: statusByte, programCounter & curProgramCounter, dataByte.
+ Front panel LED lights are initialized.
INTE MEMR INP M1 OUT HLTA STACK WO INT        D7  D6   D5  D4  D3   D2  D1  D0
 .    *    .  *   .   .    .    *   .         .   .    .   .   .    .   .   .
WAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8   A7  A6   A5  A4  A3   A2  A1  A0
 *    .      .   .   .   .   .   .   .   .    .   .    .   .   .    .   .   .
            S15 S14 S13 S12 S11 S10  S9  S8   S7  S6   S5  S4  S3   S2  S1  S0
             v   v   v   v   v   v   v   v    v   v    v   v   v    v   v   v
 ------ 
+ Ready to receive command.
+++ Altair 101a initialized.
+ Starting the processor loop.

+ v, VT100 escapes are disabled and block cursor on.
+ w, USB serial output is disabled.

+ l, load a sample program.
+ m, load: MVI testing to the setting of registers.
+ r, RUN.

+ runProcessor()
++ HALT, host_read_status_led_WAIT() = 0

+ i: Information.
++ CPU: Mega 2560
++ Memory size: 2048
++ host_read_status_led_WAIT()=1
++ host_read_status_led_HLDA()=0
++ host_read_status_led_INTE()=0
++ host_read_status_led_HLTA()=8
++ PC     14 = 00000000:00001110 = 000E = [76 C3 00] Program pointer
++ SP      0 = 00000000:00000000 = 0000 = [3E 01 06 02 0E 03 16 04] Stack pointer
++ regS  = 00 = ........ Status byte
+ Front panel display Status byte,  fpStatusByte:    138 = 212 = 10001010
+ Front panel display Data byte,    fpDataByte:      118 = 166 = 01110110
+ Front panel display Address word, fpAddressWord:    14 = 00000000:00001110
+ regA:   1 = 001 = 00000001
+ regB:   2 = 002 = 00000010  regC:   3 = 003 = 00000011
+ regD:   4 = 004 = 00000100  regE:   5 = 005 = 00000101
+ regH:   6 = 006 = 00000110  regL:   7 = 007 = 00000111
------------
++ LED_IO = 0 SERIAL_IO_VT100 = 0 SERIAL_IO_IDE = 0

+ D, Download mode.
+ Download mode: ready to receive a program.
...
+ -------------------------------------
+ Write to the serial port, the program file: p1.bin:
+ Serial port is open.
+ Write to serial port. Number of bytes: 85 in the file: p1.bin
00110001 11111111 00000000 00111110 00000000 00110010 01010100 00000000 11001101 00101111 
00000000 00000110 00000000 00001110 00000000 00001010 11010011 00100101 00010110 00000000 
00011110 00001111 00011010 11010011 00100101 11001101 00101111 00000000 00010110 00000100 
00011110 00000000 00011010 11010011 00100101 00111110 00001101 11010011 00000011 00111110 
00001010 11010011 00000011 01110110 11000011 00000011 00000000 00111110 00001101 11010011 
00000011 00111110 00001010 11010011 00000011 00111110 00101011 11010011 00000011 00111110 
00101011 11010011 00000011 00111110 00100000 11010011 00000011 00111010 01010100 00000000 
00111100 00110010 01010100 00000000 11001101 01001110 00000000 11001001 00000110 00110000 
10000000 11010011 00000011 11001001 11111111 
+ Serial port is closed.

+ Write completed.
> 
+ -------------------------------------
...
++ Byte#     79, Byte: 00110000 030 060    48
++ Byte#     80, Byte: 10000000 080 200   128
++ Byte#     81, Byte: 11010011 0d3 323   211
++ Byte#     82, Byte: 00000011 003 003     3
++ Byte#     83, Byte: 11001001 0c9 311   201
++ Byte#     84, Byte: 11111111 0ff 377   255
+ Download complete.
+ r, RUN.
+ runProcessor()

++ 1
 > Register A =  49 = 061 = 00110001
 > Register A =  10 = 012 = 00001010
++ 2
 > Register A =   0 = 000 = 00000000
++ HALT, host_read_status_led_WAIT() = 0
````
--------------------------------------------------------------------------------
--------------------------------------------------------------------------------
Cheers
