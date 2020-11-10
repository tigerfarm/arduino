--------------------------------------------------------------------------------
# Altair 8800 Simulator Software used in the Altair-Duino.

Steps to use the code for running opcode instructions.

Create a minimum code base to run machine code:
+ 1. Get the simulator to run on Mega, maybe the Due. Runs on Mega.
+ 2. Get the serial output to work properly.
+ 3. Document, or find documentation, to interact with the Simulator over serial command line (Arduino IDE Monitor).
+ 4. Load and run simple programs.
+ 5. Remove programs not require to run a machine code program, such as: dazzler.*, disassembler.*, and others.

Modify the minimum code base to work with Altair 101:
+ Use Altair 101 memory array.
+ Output to Altair 101 panel LED.
+ Input from Altair 101 panel switches.

##### Discussion Forum

Chris Davis of Altairduino.com has set up a Google Group for discussion of Altair-Duino related questions:

https://groups.google.com/forum/#!forum/altair-duino 

MacWise terminal program
http://carnationsoftware.com/domains/MacWise/Download_MacWise.html
Serial apps
https://pbxbook.com/other/mac-ser.html

--------------------------------------------------------------------------------
### 2. Serial Terminal for the Standalone Simulator

Download MacWise, which has VT100 terminal emulation option.
http://carnationsoftware.com/domains/MacWise/Download_MacWise.html
````
Download, install and start the app.
By default, it's VT100 terminal emulation ready.
Stop the Arduino IDE monitor if it's running.
On MacWise, set: Settings/Baud 15200
On MacWise, select: Settings/Serial Port, for the Arduino.
It connects and starts.
````

Sample program:
````
Address Byte        Comment
000     11 000 11   jmp 3
001     00 000 11   lb = 3
002     00 000 00   hb
003     11 000 11   jmp 0
004     00 000 00   lb
005     00 000 00   hb

````
--------------------------------------------------------------------------------
### 3. Command Line Simulator

From the download zip file, Documentation.pdf:
+ Page 27, has the documented commands, also listed below.
+ Page 28, has how to DEPOSIT the program into memory, and then run it.
    The instructions also have save to memory and re-load.

Run the Simulator.
````
[K[u[s[0;0HINTE PROT MEMR INP M1 OUT HLTA STACK WO INT  D7  D6  D5  D4  D3  D2  D1  D0
 .    .    *    .  *   .   .     .   *   .   *   *   *   *   *   *   *   *
WAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8  A7  A6  A5  A4  A3  A2  A1  A0
 *    .      .   *   .   *   *   *   *   .   *   *   *   .   *   *   *   .
            S15 S14 S13 S12 S11 S10  S9  S8  S7  S6  S5  S4  S3  S2  S1  S0
             v   v   v   v   v   v   v   v   v   v   v   v   v   v   v   v
            Stop  Step  Examine  Deposit  Reset  Protect   Aux  Aux
             ^     o      o        o       o        o       ^    o  
            Run         E.Next   D.Next    CLR   Unprotect
[K
````

Use the IDE serial monitor to respond with "C",
which echos data and brings up the command prompt.
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
U AUX1 up
u AUX1 down
s STANDALONE only: Capture serial data (AUX2 up)
l STANDALONE only: Play back captured serial data or BASIC example (AUX2 down)
m STANDALONE only: Mount (hard) disk image (AUX2 down)
Q Protect
q Unprotect
> Run from address
B Add breakpoint (only if breakpoints enabled in setup.h)
V Delete last breakpoint
D Disassemble (will prompt for start address, space bar continues, any other key exits) M Dump memory (will prompt for start address, space bar continues, any other key exits) n change number system (hexadecimal/octal/decimal)
C Enter configuration menu
L Load a program or data through serial input into simulated memory
  First value is start address, second value is length, followed by data bytes
  (all values separated by spaces). Easier to enter data this way than using the switches. 
H Load a program in Intel HEX format through serial input.
  https://en.wikipedia.org/wiki/Intel_HEX
  Useful to deposit programs and/or data directly into the simulated memory.
h Dump memory in Intel HEX format through serial output.
````

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

To reach the configuration menu, type C in the terminal window.

````

David Hansel Altair 8800 Simulator serial

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
Cheers