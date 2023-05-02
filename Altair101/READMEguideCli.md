--------------------------------------------------------------------------------
# Altair 101 Computer Command Line Quickstart

This document is a guide start using an Altair 101 computer that is setup and turned on.
It does not require a front panel.

------------------------------------------------------------------------------
## Mac Serial Terminal Commands

On my Mac laptop terminal, I use the screen command for serial communications. 
The method should work from Linux computer. 
To determine the serial port name use the ls command, or from the Arduino IDE menu: Tools/Port. 
Then use the screen command.

Note, the tty values will vary based on the Arduino version or USB serial component you are using.

For example where I'm using Ardiuno Due clone,
````
$ ls /dev/tty.*
... /dev/tty.wchusbserial14210  /dev/tty.wchusbserial14220
$ screen /dev/tty.wchusbserial14210 115200
````
Or, another example where I'm using Ardiuno Mega,
````
$ ls /dev/tty.*
/dev/tty.Bluetooth-Incoming-Port	/dev/tty.usbmodem1112401		/dev/tty.usbserial-111230		/dev/tty.wlan-debug
$ screen /dev/tty.usbmodem1112401 115200
...
+++ setup()
+ Front panel toggle switches are configured for input.
+ Initialized: MP3 player infrared receiver.
+ Initialized: MP3 player, number of MP3 files = 128
+ Initialized: SD card module.
+ Initialized: clock, Date: 2022/09/07 (YYYY/MM/DD) Time: Wednesday 11am 11:19:00 (HH:MM:SS)
+++ Altair101f initialized, version 1.66.f.

+ File: 00000000.bin, number of bytes: 21, number of bytes read: 21
++ Since 00000000.bin, has non-zero bytes in the first 32 bytes, run it.
+ runProcessor()
...
+ s, STOP, regPC = 12
?-
````

The window will clear and the emulator output begins:
````
+++ Setup.
+++ Altair 101a initialized.
?-
````
To exit the serial terminal, Ctrl+A+D. 
This detaches from the session. Note, session is still live. To re-attach, use the following.
````
$ screen -r
````

Other screen commands:
````
Ctrl-a ?        Help screen
Ctrl-a-d        Detach from screen
Ctrl-a-k        Kill the session. Cannot restart.
````

If the connection quits unexpectly, use "reset" to reset the window.
````
$ reset
````
------------------------------------------------------------------------------
## Once Serial Port Terminal is Connected

You will see the following startup message.
````
+++ setup()
+ Front panel toggle switches are configured for input.
+ Initialized: MP3 player infrared receiver.
+ Initialized: MP3 player, number of MP3 files = 128
+ Initialized: SD card module.
+ Initialized: clock, Date: 2165/165/165 (YYYY/MM/DD) Time: Monday 153pm 165:18:30 (HH:MM:SS)
+++ Altair101f initialized, version 1.66.f.

+ File: 00000000.bin, number of bytes: 16, number of bytes read: 16
++ Since 00000000.bin, has non-zero bytes in the first 32 bytes, run it.
+ runProcessor()
++ HALT, host_read_status_led_WAIT() = 0
?- 
````

Hit enter-key to view the virtual front panel.
It you have an actual front panel, the should match.
````
?- 
INTE MEMR INP M1 OUT HLTA STACK WO INT        D7  D6   D5  D4  D3   D2  D1  D0
 .    *    .  .   .   *    .    *   .         .   *    *   *   .    *   *   .
WAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8   A7  A6   A5  A4  A3   A2  A1  A0
 *    .      .   .   .   .   .   .   .   .    .   .    .   .   .    *   .   .
            S15 S14 S13 S12 S11 S10  S9  S8   S7  S6   S5  S4  S3   S2  S1  S0
             v   v   v   v   v   v   v   v    v   v    v   v   v    v   v   v
 ------ 
+ Ready to receive command.
?- 
````

Hit h, to display the help options.
````
?- + h, Print help information.
----------------------------------------------------
+++ Altair 8800 Virtual Front Panel Switch Controls
-------------
+ s, STOP         When in RUN mode, change to WAIT mode.
+ r, RUN mode     When in WAIT mode, change to RUN mode.
+ s, SINGLE STEP  When in WAIT mode, SINGLE STEP.
+ x, EXAMINE      Address of the address switches.
+ X, EXAMINE NEXT Next address, current address + 1.
+ p, DEPOSIT      Deposit into current address.
+ P, DEPOSIT NEXT Deposit into current address + 1.
+ R, RESET        Set program counter address to zero.
+ C, CLR          Clear memory, set registers and program counter address to zero.
-------------
+ 0...9, a...f    Toggle sense/address/data switches:  A0...A9, A10...A15.
----------------------------------------------------
+ Enter key       Refresh virtual front panel display.
+ t/T VT100 panel Disable/enable VT100 virtual front panel.
+ k/K Terminal    Disable/enable VT100 terminal command line escape codes.
-------------
+ l, Load sample  Load a sample program.
+ i, info         CPU information print of registers.
+ j, settings     Settings information.
-------------
+ H Help          Help for Altair101b module extension commands, and other commands.
----------------------------------------------------
````

Hit l, to load the sample program menu.
````
?- + Load a sample program.
----------------------------------------
+ Sample programs to load and run:
----------
++ r, Test the setting of registers.
++ b, MITS Altair Basic 4K.
++ f, Original Galaxy 80 game which I changed for my Star Wars Attack game.
----------
++ k, Virtual front panel version of Kill the Bit.
++ K, Load the original version of virtual Kill the Bit.
----------
++ x, Exit: don't load a program

Hit r, to load the register test program.
?- r

Hit i, to view the register settings before running the program.
?- i
...
++ regA:  10 = 012 = 00001010
++ regB:   0 = 000 = 00000000  regC:   0 = 000 = 00000000
++ regD:   0 = 000 = 00000000  regE:   0 = 000 = 00000000
++ regH:   0 = 000 = 00000000  regL:   0 = 000 = 00000000
...

Run the program by hitting r, or flipping RUN on the front panel.
?- + r, RUN.
?- + runProcessor()
++ HALT, host_read_status_led_WAIT() = 0
?-

Hit i, to view the changed register settings.
?- i
...
++ regA:   1 = 001 = 00000001
++ regB:   2 = 002 = 00000010  regC:   3 = 003 = 00000011
++ regD:   4 = 004 = 00000100  regE:   5 = 005 = 00000101
++ regH:   6 = 006 = 00000110  regL:   7 = 007 = 00000111
...
````

To run the Star Wars Attack game.
````
From the prompt, "l" to get the default program list.
"g" to load the game.
Set the program start address to 1280.
  Toggle S8(key 8) and S10(key A) on and EXAMINE(key x) the byte at that address, 1280.
Then run the program."
````
--------------------------------------------------------------------------------
## Set the Date and Time

````
?- H
...
+ F, SD Card      SD Card CLI, memory manage with the SD card.
...
-------------
+ J, MP3 Player   PLAYER mode, run the MP3 player.
+ g/G Play        Pause/Play MP3 song.
+ v/V Volume      Down/Up player volume.
-------------
+ Q, Clock        CLOCK CLI, interact with the clock.
+ q, Time         Show clock date and time.
-------------
...
?- q
+ Current Date: 2165/165/165 (YYYY/MM/DD) Time: Monday 153pm 165:165:85 (HH:MM:SS)
?- Q
+ Run in CLOCK mode.
````
Display help.
````
CLOCK ?- h
----------------------------------------------------
+++ Real Time Clock Controls
--------------------------
+ r, RUN time     Show date and time.
+ R, RESET        Toggle between clock SET mode to CLOCK view mode.
+ M, TIMER        Switch to CLOCK TIMER mode.
--------------------------
+ t/T VT100 panel Disable/enable VT100 virtual front panel.
+ Ctrl+L          Clear screen.
+ K, Scanner      Play Knight Rider Scanner.
--------------------------
+ X, Exit CLOCK   Return to program WAIT mode.
----------------------------------------------------
````
Toggle into clock set mode, display help, display each date-time value.
````
CLOCK ?- R
+ RESET, enter clock set mode.

Clock SET ?- h
----------------------------------------------------
+++ SET Clock Time
-------------
+ x, EXAMINE      Rotate through the clock values that can be set.
                  Rotate order: seconds to year.
+ X, EXAMINE NEXT Rotate through the clock values that can be set.
                  Rotate order: year to seconds.
+ S, Set UP       Increment date or time value.
+ s, Set Down     Decrement date or time value.
-------------
+ r, RUN          CLOCK mode: show date and time.
                  Clock SET mode: show date and time, and the set data and time value.
+ p, DEPOSIT      Set the time based on the clock SET values.
+ R, RESET        Toggle between clock SET mode to CLOCK mode.
                  Reset the clock SET values when taggling into clock SET mode.
                  Doesn't set/change the time.
-------------
+ Ctrl+L          Clear screen.
----------------------------------------------------
Clock SET ?- 
````
Display and set the time.
````
Clock SET ?- r  (note, the "set" values are not set)
+ Current Date: 2022/09/07 (YYYY/MM/DD) Time: Wednesday 11am 11:27:08 (HH:MM:SS)
+ Set     Date: ----/--/--               Time: --:27:00

Clock SET ?- x (use "x" to cycle through the values)
Clock SET ?- + Clock SET, Previous value: seconds, current set value = 0
Clock SET ?- + Clock SET, Previous value: minute, current set value = 27
Clock SET ?- + Clock SET, Previous value: hour, current set value = 11
Clock SET ?- + Clock SET, Previous value: day, current set value = 7
Clock SET ?- + Clock SET, Previous value: month, current set value = 9
Clock SET ?- + Clock SET, Previous value: year, current set value = 2022
Clock SET ?- + Clock SET, Previous value: seconds, current set value = 0

Clock SET ?- r (note, the "set" values are now set)
+ Current Date: 2022/09/07 (YYYY/MM/DD) Time: Wednesday 11am 11:28:56 (HH:MM:SS)
+ Set     Date: 2022/09/07               Time: 11:27:00
````
Return to processor mode.
````
Clock SET ?- + RESET, return to CLOCK mode.
CLOCK ?- + Exit CLOCK mode. Return to Processor WAIT mode.
?- 
````

--------------------------------------------------------------------------------
Cheers