----------------------------------------------------------------------------------
# Running MITS Altair 4K Basic on the Altair101a

I found a de-assembled listing of 4K Basic which is
the assembler program Bill Gates, Paul Allen, and another wrote. It's the first Microsoft software product.
It was written in Intel 8080 assembler language.
There was also a binary file for download.
After a few updates to Altair101a.ino, the binary ran.

Download [link](https://github.com/option8/Altair-BASIC).

[BASIC Manual 75.pdf](https://altairclone.com/downloads/manuals/)

Upload from assembler using:
````
> set byte p4kBasic/basic4k.bin
> upload
````
Then run it. On a Mega, 2246 BYTES FREE for programs.
````
?- + r, RUN.
?- + runProcessor()

MEMORY SIZE? 5370
TERMINAL WIDTH? 120
WANT SIN? N
WANT RND? N
WANT SQR? N

2246 BYTES FREE

BASIC VERSION 3.2
[4K VERSION]

OK
````
Then run it. On a Due, When set to about 56K, 52630 BYTES FREE for programs.
````
?- + r, RUN.
?- + runProcessor()

MEMORY SIZE? 56000
TERMINAL WIDTH? 120
WANT SIN? Y

52630 BYTES FREE

BASIC VERSION 3.2
[4K VERSION]

OK
````
Since 4K Basic is programed to use an 88-2SIO interface,
Altair101a was updated to use 88-2SIO serial port requirements.
I found the requirements in the 4K Basic de-assembled source code.
Altair101a now uses the ports, 0 and 1, which are hard coded into Altair101a.

88-2SIO has double input ports with the following specs:
+ Port 00 indicates input: 0 for input, 1 for no input.
+ Port 01 is for receiving the actual input character.
+ Both ports are input from one Arduino serial input port, either Serial or Serial2 (Arduino port #0 or #2).
+ The Altair101a has an option to switch between using Serial or Serial2 for 4K Basic I/O.

Since 4K Basic only works with CR,
on port 1 Altair101a converts LF(ASCII 10) from my Linux based terminal to CR(ASCII 13).
Since I was coding for one conversion, I added conversions from lowercase letters to uppercase, which 4K Basic requires.

On the output ports (0 and 1), I output to Arduino Serial2.
Altair101a also sets the output byte's 8th bit to 0, which is another requirement.
````
regAdata = regAdata & B01111111;
````

----------------------------------------------------------------------------------
### Working with 4K Basic

Load and run 4K Basic. While the Altair101a is running, connect to the default serial port (Arduino Serial).
I prefer using a terminal window, however the Arduino IDE works too.
````
+++ setup
+ SD card module is initialized.
+++ Altair101a initialized.
?- ?- ?- + m, Read file into program memory.
++ Program filename: 00001001.bin
++ Confirm, y/n: 
+ Confirmed.
+ Program file loaded and ready to use.
?- + R, RESET.
?- + T, Terminal output VT100 escape codes is enabled. Use Crtl+d(or Crtl+c) to STOP, Crtl+Z to RESET.
?- + R, RESET.
?- + r, RUN.
?- + runProcessor()

MEMORY SIZE? 5370
TERMINAL WIDTH? 120
WANT SIN? N
WANT RND? N
WANT SQR? N

2246 BYTES FREE

BASIC VERSION 3.2
[4K VERSION]

OK
````

I download the [BASIC Manual 75.pdf](https://altairclone.com/downloads/manuals/) and worked through some of it.

Following are sample Basic commands I entered from the manual, which ran on the Altair101a.
````
OK
LIST

10 PRINT 3+7
20 PRINT 4+8
OK
NEW

OK
LIST

OK
PI_RINT 1/3
 .333333 

OK
PRINT "1/3 =";1/3
1/3 = .333333 

OK
N=6

OK
PRINT N
 6 
````
----------------------------------------------
Generate random numbers between A and B.
````
OK
A=3

OK
B=6

OK
PRINT (B-A)*RND(1)+A
 4.51314 

OK
10 PRINT (B-A)*RND(1)+A
RUN
 0 

OK
2 A=6
3 B=9
RUN
 7.16428 

OK
RUN
 7.70737 

OK
RUN
 8.16006 

OK
2 A=20
3 B=30
RUN
 22.0905 

OK
````
--------------------------------------------------------------------------------
### How to Upload BAS file and Save Programs.

Upload works from asm upload option. Need to do more test with delay settings:
+ set baudst 100
+ set baudcr 200

To save: LIST the program, then copy and paste it into a BAS text file on the laptop.

Upload the following.
````
REM 4K Basic listing.
REM
10  PRINT "ENTER A NUMBER, 0 OR GREATER:"
12  INPUT  B
20  IF B=0 THEN  50  
30  PRINT  "NON-ZERO"  
40  GOTO  10  
50  PRINT  "ZERO"  
60  GOTO  10
````

--------------------------------------------------------------------------------
### Switching Basic I/O Serial Ports

To set Basic's I/O Arduino port, your must be in WAIT mode.
Either switch before starting Basic or stop Basic from running.

From WAIT mode,
+ Press "Y" to enable Serial2 for Basic I/O.
+ Press "y" to disable Serial2 for Basic I/O. Which means that Serial2 is disabled and that Serial is enabled for Basic I/O.

When Serial2 is enable for Basic I/O, use a serial terminal window or the 
Arduino IDE serial monitor to connect to the Serial port.

Altair101a messages when pressing "Y" and "y".
````
?- + y, Serial2 off (end).
?- + Y, Serial2 on (begin), baud rate: 115200.
````

--------------------------------------------------------------------------------

Altair 8800 - Video #7 - Loading 4K BASIC from a Paper Tape Image
https://www.youtube.com/watch?v=8InWiihlIQw

The Paper Tape Image, is loaded into the Altair 8800 clone from a laptop over a serial port.
At 8:45, Basic is loaded and running.


--------------------------------------------------------------------------------
Cheers
