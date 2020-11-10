--------------------------------------------------------------------------------
# Altair 8800 Simulator Software used in the Altair-Duino

Steps to use the code for running opcode instructions.

Create a minimum code base to run machine code:
+ 1. Get the simulator to run on Mega, maybe the Due. I have it running on Mega.
+ 2. Get the serial output to work properly. I'm using MacWise VT100 emulator.
+ 3. I found the documentation. I can interact with the Simulator over serial command line.
+ 4. I can enter and run simple programs. Load and run CPM?
+ 5. Remove programs not require to run a machine code programs, such as: dazzler.*, disassembler.*, and others.

Modify the minimum code base to work with Altair 101:
+ Use Altair 101 memory array.
+ Output to Altair 101 panel LED.
+ Input from Altair 101 panel switches.

##### Links

Chris Davis of Altairduino.com has set up a Google Group for discussion of Altair-Duino related questions:
https://groups.google.com/forum/#!forum/altair-duino 

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
### 2. Get the serial output to work properly.

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
00000010:   00111010 : 3A:072 > opcode: lda 96
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
1       Up switch A1.
2       Up switch A2.
3       Up switch A3.
4       Up switch A4.
5       Up switch A5.
P       DEPOSIT the opcode into memory address 0.
        ---
        Toggle switches to enter the MVI immediate value, 6: 00 000 110
3       Down switch A3.
4       Down switch A4.
5       Down switch A5.
p       DEPOSIT NEXT the immediate value into memory address 1.
        ---
        Toggle switches to enter the LDA opcode: 00 111 010
2       Down switch A2.
3       Up switch A3.
4       Up switch A4.
5       Up switch A5.
p       DEPOSIT NEXT the opcode into memory address 2.
        ---
        Toggle switches to enter low address byte 96: 01 100 000
2       Down switch A2.
3       Down switch A3.
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
r       Run the program
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