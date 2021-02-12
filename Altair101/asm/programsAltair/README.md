--------------------------------------------------------------------------------
# Running Programs on the Altair101a

--------------------------------------------------------------------------------
### Working with 4K Basic

Load and run 4K Basic.
````
+++ setup
+ SD card module is initialized.
+++ Altair 101a initialized.
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

Write to serial port. Number of bytes: 1551 in the file: p1.bin
00110101 00100000 01010000 01010010 01001001 01001110 01010100 0010
...
````

I download the [BASIC Manual 75.pdf](https://altairclone.com/downloads/manuals/) and worked through some of it.

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
PIN__RINT 1/3
 .333333 

OK
PRINT "1/3=";1/3
1/3= .333333 

OK
````

--------------------------------------------------------------------------------
### Working to run Galaxy80.asm
````
; ----------------------------------------------
; Programing challenge:
; + Done: Compile using my assembler.
; + Done: Interactive basics: input and output.
; + Done: Run the start to get the initial prompt and "n" to halt the program.
; + Get the complete program to run by adding section and section.
;
; ------------------------------------------------
````
--------------------------------------------------------------------------------
Cheers
