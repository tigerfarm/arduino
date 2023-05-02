--------------------------------------------------------------------------------
# Altair 101 Computer User Guide

This document is a guide for users to run and manage an Altair 101 computer.

------------------------------------------------------------------------------
## User Guide Sections

[Startup Initialization](#Startup-Initialization): initialize components and show the status during reboot.

[Processor](#Processor): Manage processor memory.
Manage the running of programs.

[Download](#Download-from-the-Serial-Port) a program from the Serial Port.

[Clock](#Clock): Display time and date.

[Clock Timer](#Clock-Timer): Set and run timers.

Not implemented: [Clock Counter](#Clock-Counter): Manage counters that are stored on the SD card.

[MP3 Player](#MP3-Player): Manage the playing of MP3 files.

Not implemented: [MP3 Player Sound Effects](#MP3-Player-Sound-Effects): 
Manage which MP3 files are played for each sound effect, such as a beep when flipping switches to change modes.

[Documentation](#Documentation): development documentation rather than user guide.

[Assembly programing](asm/README.md#how-to-assemble-and-run-programs-on-the-altair-101):
Steps to assemble, upload, and run an assembler program is in another document.

##### Front Panel Indicator Mode Lights
````
Processor:      WAIT:ON/OFF HLDA:OFF
Clock:          WAIT:OFF    HLDA:ON
Clock Timer:    WAIT:ON     HLDA:ON
Clock Counter:  WAIT:ON     HLDA:ON
Player:         WAIT:OFF    HLDA:ON
Player files:   WAIT:ON     HLDA:ON
````

------------------------------------------------------------------------------
## Startup Initialization

Initialization and system check function: setup().

### Initialization User Guide

##### To set a startup program to run on reboot:
````
+ Enter your program into the processor's memory. Run and test.
+ Set all Data switches down.
+ Flip AUX2 up twice to write the processor's memory to 00000000.bin.
````
Now, 00000000.bin has your program. The program will run on reboot.

##### To remove a startup program from running:
````
+ Flip CLR twice to clear the processor's memory and registers to zero.
+ Set all Data switches down.
+ Flip AUX2 up twice to write the processor's memory to 00000000.bin.
````
Now, 00000000.bin is all zeros. No program will run on reboot.

##### Initialization sequence during reboot.
````
+ Init LCD, if using one.
+ Front panel toggle switches and PCF modules settings initialized.
+ Front panel LED lights: Wait and HLDA set on, and status light byte is set for processor mode.
++ Flash LED Success(1). Status lights count the number of times flash success is called.
+ MP3 player module for playing MP3 files.
++ Flash LED Success or error.
+ SD card module, used for read and write.
++ Flash LED Success(2) or error.
+ Clock module, used when in clock mode.
++ Flash LED Success(3) or error.
+ Sound effects are initialized: read sound effect file information into an arrary.
-----------
If 00000000.bin exists and any of the first 32 bytes are not zero,
+ Set, programState = PROGRAM_RUN, which runs 00000000.bin when loop() starts.
Else,
+ programState = PROGRAM_WAIT (default start state), don't run 00000000.bin when loop() starts.
-----------
+ programLights();// Uses: statusByte, curProgramCounter, dataByte
````

------------------------------------------------------------------------------
## Processor

#### Front Panel Lights and Toggles Functions
````
-----------
+ Status        Current instruction cycle status.
+ Data          Data byte at the current program counter address.
+ Address       Current program counter value.
+ Indicator     WAIT : On, when program is not running. Off, when program is running.
+ Indicator     HLDA : Off.
-----------
+ STOP          Pause running of a program: exist RUN mode and enter WAIT mode.
+ RUN           Enter run mode and run the processor memory program from the program counter address.
+ RESET         If in RUN mode, set the program counter to zero and continue running from there.

If in WAIT mode, do the following.

+ SINGLE up     Run one machine instruction cycle.
+ SINGLE down   Decrement the program counter and display the program counter address and data byte.
+ EXAMINE       Examine a data byte specified by the Address toggles.
                  1. Use the Address toggles to set the address.
                  2. Flip EXAMINE sets the program counter to Address toggles.
                  3. Displays the address in the Address lights.
                  4. Displays the address data byte in the Data lights.
+ EXAMINE NEXT  Increment the program counter and display the program counter address and the data byte at that address.
+ DEPOSIT       Deposit Data toggle values into the current address.
+ DEPOSIT NEXT  Increment the program counter and deposit Data toggle values into the program counter address.
+ RESET         Set the program counter to zero, and display the zero address and address data byte.
+ CLR           Double flip to set processor memory to zeros and set program counter to 0.
+ PROTECT       Decrease MP3 player volume.
+ UNPROTECT     Increase MP3 player volume.
+ AUX1 up       Enter Clock mode: show the time: hours and minutes.
+ AUX1 down     Enter MP3 player mode
+ AUX2 up       Write processor memory to the SD card.
                  1. Set the Sense switches to program filename.
                  2. Double flip the switch. WAIT and HLDA indicators are on.
                  3. Write processor memory to SD drive program filename.
+ AUX2 down     Read SD card file bytes into processor memory.
                  1. Set the Sense switches to an SD drive program filename value.
                  2. Double flip the switch.
                  3. Read file bytes into processor memory.
                  4. Automatically return to WAIT mode once download is complete.
+ AUX2 down     Enter receive mode to receive bytes into processor memory from the serial port.
                  1. Set the Sense switches to 0.
                  2. Double flip the switch.
                  3. Enter receive mode. HLDA indicator light is on.
                  4. Wait to receive bytes from the serial port. Optional: flip RESET to return to WAIT mode.
                  5. Automatically return to WAIT mode once download is complete.
````
-----------
### Download from the Serial Port

#### Front Panel Lights and Toggles
````
-----------
+ Status        INP  : Timer indicator, ready for timer value input.
+ Status        WO   : Processor logic is: WO on when not writing to memory.
+ Data          Off.
+ Address       Off.
+ Indicator     WAIT : Off.
+ Indicator     HLDA : On.
-----------
+ RESET         Exit download mode, return to processor WAIT mode.

--------------
Serial port mode function: runDownloadProgram().
````

### User Guide of Processor Tasks

How to save a program to the SD card.
+ Set the Sense switches to the file number value.
+ Double flip AUX2 up to confirm and write processor memory to file.

How to load and run a program from the SD card.
+ Set the Sense switches to the file number value.
+ Double flip AUX2 down. File bytes are read into processor memory. LED lights flash success with a sound bite message.
+ Flip RUN, to start the program running from program counter 0.

How to assemble, upload, and run an assembler program.
+ See Project: [Altair101/asm/README.md](asm/README.md#how-to-assemble-and-run-programs-on-the-altair-101)

------------------------------------------------------------------------------
### Clock

#### Front Panel Lights and Toggles
+ Start by showing the time of day hours and minutes.
+ To do: If clock timer mode was set, return to timer mode or reset timer mode values.
+ Function: runClock().
````
-----------
Clock mode,
+ Status        Display the minutes tens digit,    day tens,   or year tens
+ Address       Display the hour: A1 ... A12,      month,      or century
+ Data          Display the minutes single digit,  day single, or year single
+ Indicator     WAIT : Off.
+ Indicator     HLDA : On to indicate controlled by another process, other than the program emulator.
-----------
+ STOP          Not implemented.
+ RUN           Not implemented.
+ SINGLE up     1) First flip, show month and day.
                2) Next flip, show: year.
                3) Next flip, return to show time of day: hour and minutes.
+ SINGLE down   Not implemented.
+ EXAMINE       Set mode: flip EXAMINE to decrement the minutes.
+ EXAMINE NEXT  Set mode: flip EXAMINE NEXT to increment the minutes.
+ DEPOSIT       Toggle from clock set mode to clock mode. Set the time based on the display.
+ DEPOSIT NEXT  Not implemented.
+ RESET         Toggle from clock mode to clock set mode, to change the hours and minutes.
                   In clock set mode, the time flashes.
                Toggle from clock set mode to clock mode. Don't change the time.
+ PROTECT       Decrease MP3 player volume. To do: Decrement value to set.
+ UNPROTECT     Increase MP3 player volume. To do: Increment value to set.
+ AUX1 Up       Toggle clock mode off, return to processor mode.
+ AUX1 down     MP3 player mode
+ AUX2 up       Toggle clock counter mode.
+ AUX2 Down     Toggle clock timer mode on.
````

Steps to set the time hours and minutes using the front panel.
````
+ In clock mode, flip RESET. Time flashes once a second.
+ Flip EXAMINE to decrement the minutes.
+ Flip EXAMINE NEXT to increment the minutes.
+ Flip RESET to return to clock mode, time not changed.
+ Flip DEPOSIT to return to clock mode, time is set based on the displayed hours and minutes.
++ Seconds are set to zero, so flip DEPOSIT when the second hand hits 12 and the minute changes.
````
------------------------------------------------------------------------------
### Clock Timer

#### Front Panel Lights and Toggles

Mode function: clockRunTimer().
````
-----------
Clock timer mode,
+ Status        INP  : Timer indicator, ready to input timer value.
+ Status        M1   : Timer running.
+ Status        HLTA : Timer run is completed.
+ Address       Displays the timer minute: A1 ... A15. And flashing counter minute when running.
+ Data          If using an array of times, displays which array time is running.
+ Indicator     WAIT : On.
+ Indicator     HLDA : On, non-processor mode, clock timer mode.
-----------
+ STOP          Stop clock timer.
+ RUN           Run a timer sequence. Before using RUN, use EXAMINE to set the timer minutes.
                1. Set the timer minutes using the address toggles: A1 is 1 minute, A2 is 2, ... A15 is 15.
                2. To do: Start the timer using the timer minutes value.
+ SINGLE up     Not implemented.
+ SINGLE down   Not implemented.
+ EXAMINE       1. Set Address toggle to timer array value.
                2. Flip EXAMINE.
                3. Timer array index is displayed in the Data lights.
                4. Timer array value is displayed in the Address lights: A1:1 minute, ..., A15:15 minutes.
+ EXAMINE NEXT  Not implemented.
                1. Timer array index is incremented.
                2. Timer array index is displayed in the Data lights.
                3. Timer array value is displayed in the Address lights: A1:1 minute, ..., A15:15 minutes.
+ DEPOSIT       Not implemented.
                1. Set Address lights to timer array value: A1:1 minute, ..., A15:15 minutes.
                2. Flip DEPOSIT.
                3. Timer value is displayed in the Address lights.
                4. Timer array counter value is displayed in the Data lights.
                5. Timer array value is stored into the timer array at the array counter value.
+ DEPOSIT NEXT  Not implemented.
                1. Set Address lights to timer array value: A1:1 minute, ..., A15:15 minutes.
                2. Flip DEPOSIT NEXT.
                3. Timer array index is incremented.
                4. Timer array index is displayed in the Data lights.
                5. Timer array value is displayed in the Address lights.
                6. Timer time value is stored into the memory array, at the incremented array index value.
+ RESET         Not implemented.
                1. Set timer array index to 1 (D0).
                2. Timer array index is displayed in the Data lights.
                3. Timer array value is displayed in the Address lights.
+ PROTECT       Decrease MP3 player volume.
+ UNPROTECT     Increase MP3 player volume.
+ AUX1 Up       Toggle clock mode off, return to processor mode.
+ AUX1 down     MP3 player mode
+ AUX2 up       Enter clock mode.
+ AUX2 Down     Not implemented. Enter clock counter mode.
````
#### Clock Timer Mode User Guide

Set and run a clock Timer,
+ Set to clock mode.
+ Flip AUX2 toggle up to enter clock timer mode.
+ Select the minutes by toggling a single address switch.
++ For example A10 is for 10 minutes.
+ Flip EXAMINE to set the minutes based on the address switches.
++ The LED above the toggled on switch will turn on. For example A10 will turn on.
+ Flip RUN to start the timer.
++ Flash counter minute LED on/off each second.
++ Increment counter minute, each minute, starting with A0.
++ Play a sound bite as each minute passes.
++ Sound and flash when time is reached.

Not implemented. Enter a sequence of timer values,
+ Must be in clock mode.
+ Flip AUX2 toggle Down to enter clock timer mode.
+ Set Data buttons to one (A0).
+ Flip EXAMINE, to set the timer array value.
++ Timer array index is displayed in the Data lights (D0 is on, value of 1).
++ Timer array value is displayed in the Address lights. No lights are lit because the value is zero.
+ Set Address Set Address toggles to the first timer value, for example 3.
+ Flip DEPOSIT.
++ Timer array index is displayed in the Data lights (D0 is on).
++ Timer array value is displayed in the Address lights(A3, for 3 minutes).
+ Set Address lights to the next timer array value, for example, A5, for 5 minutes.
+ Flip DEPOSIT NEXT.
++ Timer array index is displayed in the Data lights (D1 is on, value of 2).
++ Timer array value is displayed in the Address lights (A5:5 minutes).
+ Set Address lights to the next timer array value, for example, A7, for 7 minutes.
+ Flip DEPOSIT NEXT.
++ Timer array index is displayed in the Data lights (D0 and D1 are on, value of 3).
++ Timer array value is displayed in the Address lights (A7:7 minutes).

Run a sequence of timer values,
+ Flip RESET. Or Set Address toggle A0:1 up(on). Flip EXAMINE.
++ Timer array index is displayed in the Data lights (D0 is on).
++ Timer array value is displayed in the Address lights(A3:3 minutes).
+ Flip RUN, and the timer starts.

------------------------------------------------------------------------------
### Not Implemented: Clock Counter

#### Front Panel Lights and Toggles

Mode function: clockCounterControls().
````
-----------
Clock counter mode,
First time entering counter mode: display 0 Address and Data values in the lights.
+ Status        MEMR  : Counter indicator.
+ Status        STACK : Counter indicator.
+ Status        INP   : On after a read, for example, an EXAMINE flip. Else off.
+ Status        WO    : On after a write, for example, a STOP, RUN, or DEPOSIT flip. Else off.
+ Address       Counter index value.
+ Data          Counter data value.
+ Indicator     WAIT : On.
+ Indicator     HLDA : On.
-----------
+ STOP          Decrement counter value at current address, and update in display.
+ RUN           Increment counter value at current address, and update in display.
+ SINGLE up     Not implemented.
+ SINGLE dn     Decrement counter address, and display counter address and file data byte.
+ EXAMINE       1. Set address toggles (Data toggles) to the counter file number.
                  2 Flip EXAMINE, read and display the address toggle (Data toggle) value into data value.
+ EXAMINE NEXT  Increment counter address, get the file data byte and display it in the Data lights.
+ DEPOSIT       1. Set Data switches to a counter file number.
                  2. Flip DEPOSIT writes the data byte to the counter address file.
+ DEPOSIT NEXT  1. Set Data switches to an MP3 file number.
                  2. Flip DEPOSIT NEXT increments counter address, and writes the data byte to the counter address file.
+ RESET         Reset counter address to 0, get the file data byte and display it in the Data lights.
+ CLR           Not implemented.
+ PROTECT       To do: Decrease volume
+ UNPROTECT     To do: Increase volume
+ AUX1 up       Clock mode: show hour and mintues time.
+ AUX1 down     MP3 player mode
+ AUX2 up       Not implemented.
+ AUX2 Down     Toggle clock timer mode off, return to clock mode.
````
View and set a counter file value,
+ Must be in clock mode.
+ Flip AUX2 toggle Down to enter clock counter mode.
+ Set address toggles (Data toggles) to the counter file number.
+ Flip EXAMINE to display the value.
+ Set Data switches to a counter file number.
+ Flip DEPOSIT writes the data byte to the counter address file.

------------------------------------------------------------------------------
### MP3 Player

#### Front Panel Lights and Toggles

Mode function: runPlayer().
````
+ MP3 addressable files is 255.
-----------
+ Data          Displays the song number that is playing.
+ Address       Displays the volume using A0 to A15 as the volume level.
+ Status        M1   : Loop single MP3 is on.
+ Status        OUT  : On indicates player mode, since HLDA is on as well.
+ Status        HLTA : pause, light is on, else off.
+ Indicator     HLDA : On to indicate controlled by other than the program emulator.
+ Indicator     WAIT : Off.
-----------
+ STOP          Pause play
+ RUN           Play MP3. And loop all.
+ SINGLE up     Toogle on and off, of the playing/looping of a single MP3.
+ SINGLE dn     Flip to loop a single MP3. Song plays when in other modes such as clock timer.
+ EXAMINE       Play previous MP3. And loop all.
+ EXAMINE NEXT  Play next MP3. And loop all.
+ DEPOSIT       Play previous folder. During first MP3, loop directory. After first song, will loop all.
+ DEPOSIT NEXT  Play next folder.     During first MP3, loop directory. After first song, will loop all.
+ RESET         Play first MP3. And loop all.
+ CLR           1. Set address toggles to the value of an MP3 file.
                  2. Play toggle address value MP3 file. If HLTA is on, only play once, then pause.
+ PROTECT       Decrease volume
+ UNPROTECT     Increase volume
+ AUX1 up       Clock mode: show hour and mintues time.
+ AUX1 Down     Toggle MP3 player mode off, return to processor mode.
+ AUX2 up       Not implemented.
+ AUX2 down     Player file mode.
````
------------------------------------------------------------------------------
### Not Implemented: MP3 Player Sound Effects

#### Front Panel Lights and Toggles

Mode function: checkPlayerFileControls().
````
-----------
+ Status        MEMR : Player file mode indicator.
+ Status        INP  : On after a read, for example, an EXAMINE flip. Else off.
+ Status        OUT  : MP3 playing/played.
+ Status        HLTA : pause, MP3 is not playing.
+ Status        WO   : On after a write, for example, DEPOSIT flipped. Else off.
+ Address       Displays the song number that is playing.
+ Data          Displays the volume.
+ Indicator     WAIT : On.
+ Indicator     HLDA : On.
-----------
+ STOP          Pause play
+ RUN           Play MP3 file based on the Data value filename.
+ SINGLE up     Not implemented.
+ SINGLE dn     Not implemented.
+ EXAMINE       1. Set address toggles (Data toggles) to the MP3 data file number.
                  2 Flip EXAMINE, which reads the address toggle (Data toggle) value into playerFileAddress, which is displayed.
+ EXAMINE NEXT  Increment playerFileAddress, get the file data byte and display it in the Data lights.
+ DEPOSIT       1. Set Data switches to an MP3 file number.
                  2. Flip DEPOSIT will write the data byte to the playerFileAddress file.
+ DEPOSIT NEXT  1. Set Data switches to an MP3 file number.
                  2. Flip DEPOSIT NEXT will increment playerFileAddress, and write the data byte to the playerFileAddress file.
+ RESET         Load sound effect index array from files.
+ CLR           Not implemented.
+ PROTECT       Decrease volume
+ UNPROTECT     Increase volume
+ AUX1 up       Clock mode: show hour and mintues time.
+ AUX1 Down     Toggle MP3 player mode off, return to processor mode.
+ AUX2 up       Not implemented.
+ AUX2 down     Return to player mode.
````
### User Guide for MP3 Player Sound Effect Files

##### Actions that have sound effects,
````
+ Constant          Array index value. Value is a file name. Value 1, is filename: 0001.mp3.
  int READ_FILE       = 1;
  int TIMER_COMPLETE  = 2;
  int CLOCK_ON        = 3;
  int CLOCK_OFF       = 4;
  int PLAYER_ON       = 3;
  int PLAYER_OFF      = 4;
  int KR5             = 5;
  int CLOCK_CUCKOO    = 6;
  int TIMER_MINUTE    = 7;
````
##### Steps to hear which sound effect matches action:
````
+ Flip AUX1 down to put the computer in player mode.
+ Flip AUX2 down to put the computer in player file mode.
+ Flip the address switches to match the above number of the array index.
++ For example, flip to 2, to hear the TIMER_COMPLETE sound.
+ Flip EXAMINE.
++ The Address LED lights now display the array index value, for example, 2.
++ The Data LED lights now display tge MP3 file number for that action.
+ Flip RUN, to hear the sound file.
+ Flip EXAMINE NEXT to increment the Address, and display the next file number in the Data lights.
+ Flip RUN, to hear the sound file.
Notes,
+ If data byte is zero, no sound file assigned.
+ Search for "soundEffects", to get the above action list of sound effect constants and array index values.
````
##### Steps to Update Altair 101 SD Card MP3 Files
Prepare the SD card for files.
````
Remove the Altair 101 SD MP3 card and put it into the MacBook.
Run Applications/Utilities/Disk Utility.app
Select the SD card, example: MUSICSD.
Select option, Erase.
Set, Name: MUSICSD, Format: MS-DOS (FAT).
Click Erase. Click Done. Close the Disk Utility.
````
Prepare MP3 files to be copied to the SD card.
````
Copy MP3 files into a working directory, for example: mp3player0.
Notes,
+ You can organize the files into subdirectories. The subdirectories allow file play separations.
+ The file order, is the play order: 
++ Play each file in the first subdirectory, in the name order in the directory.
````
Copy the prepared MP3 files to the SD card in a format that the DFPlayer can use.
````
Create an empty directory, for example: mp3player1.
Use mp3player.jar to copy files from the working directory into the empty directory,

$ java -jar mp3player.jar copy mp3player0 mp3player1
+++ Start, DFPlayer module copy program.
++ Copy into DFPlayer formatted file and directory names, the <IN: directory> (mp3player1) to the <OUT: directory> (mp3player0).
++ From directory: mp3player1, Create to-directory: 01
+ doCopyFile, copied from: mp3player0/00-computer/01-operational.mp3 to mp3player1/01/0001.mp3
+ doCopyFile, copied from: mp3player0/00-computer/02-chirpChrip.mp3 to mp3player1/01/0002.mp3
+ doCopyFile, copied from: mp3player0/00-computer/03-computerbeep_17.mp3 to mp3player1/01/0003.mp3
...
++ From directory: mp3player1, Create to-directory: 02
+ doCopyFile, copied from: mp3player0/02-top/Jimmy Buffet - Margaritaville.mp3 to mp3player1/02/0034.mp3
...

Copy the prepared directories of files onto the clean SD card.
Put the card back into the Altair 101.
````

------------------------------------------------------------------------------
## Documentation

More for development rather than user guide.

#### Processor program options.

From OUT opcode (B11100011), when timer's time is complete,
clockRunTimerControlsOut(getMinuteValue(regA), <true|false>);
+ Plays the timer completed MP3.
+ If true to return when completed, return to process the next opcode.
+ If false to return when completed, play the timer completed MP3 each minute as a continuous reminder.

MP3 player options,
+ OUT 10 or 11 with regA=B00000000, pause the player. Don't start when RUN is flipped.
+ OUT 10 or 11 with regA=B11111111, start the player. Also, start when RUN is flipped.
+ OUT 10 or 11 with regA > 0,
+ OUT 69 regA=1 Play sound effects, regA=0 Don't play sound effects.
++ Set processorPlayerCounter=regA, play/loop the MP3. Also, start when RUN is flipped.
+ STOP, mp3player.pause();
+ RUN, if (processorPlayerLoop) mp3player.start()

````
--------------
Inputs:
+ dataByte == 000, regA = 0;              // Input is not implemented on this port.
+ dataByte == 001, regA = hwStatus;       // hwStatus = 0, a number to indicate a hardware issue.
+ dataByte == 255, regA = toggleSense();
+ Else, input port not implimented: error.

--------------
Outputs, opcode OUT B11100011: OUT port#(dataByte)
--------------
Terminal
+ dataByte == 3, Serial terminal output of the contents of register A :"));
++ asciiChar = regA; Serial.print(asciiChar);
--------------
LCD
+ dataByte == 1, Print register A value to the LCD module.
++ regA == 0, lcdBacklight( false );     // LCD back light off.
++ regA == 1, lcdBacklight( true );      // LCD back light on.
++ regA == 2, lcdClearScreen();
++ regA == 3, lcdSplash();
++ else, char charA = regA; lcdPrintChar((String)charA);
--------------
MP3 Player
+ dataByte == 10, mp3player.play(regA); // Play once, the MP3 file named in register A.
+ dataByte == 11, mp3player.loop(regA); // Play in a loop, the MP3 file named in register A.
+ dataByte == 69, Play sound effects: on(regA=1) or off(regA=0). OUT 69. See sample program: pPlaySoundEffects.asm.
--------------
Flash light messages
+ dataByte == 13, 015oct Error happened, flash the LED light error sequence.
+ dataByte == 42, 052oct Success happened, flash the LED light success sequence.
--------------
Debug messages
+ dataByte == 30, printData(regB);
+ dataByte == 31, printData(regC);
+ dataByte == 32, printData(regD);
+ dataByte == 33, printData(regE);
+ dataByte == 34, printData(regH);
+ dataByte == 35, printData(regL);
+ dataByte == 36, Print data at H:L address: Serial.print(memoryData[regH * 256 + regL]);
+ dataByte == 37, printData(regA);
+ dataByte == 38, printRegisters();
+ dataByte == 39, printRegisters(); printOther();
--------------
````

#### Mode Selection

AUX switches for setting program modes.
````
+ Default mode is the Altair 8800 emulator processor mode.
When in processor mode,
+ AUX1 up     Enter clock mode.
+ AUX1 Down   Enter MP3 player mode.
+ AUX2 up     Double flip to write processor memory to file.
+ AUX2 Down   Double flip to read a file of bytes into processor memory.
When in clock mode,
+ AUX1 up     Enter processor mode.
+ AUX1 Down   Enter MP3 player mode.
+ AUX2 up     Enter timer mode.
+ AUX2 Down   Enter counter mode.
When in clock timer mode,
+ AUX1 up     Enter processor mode.
+ AUX1 Down   Enter MP3 player mode.
+ AUX2 up     Enter clock mode.
+ AUX2 Down   Enter counter mode.
When in player mode,
+ AUX1 up     Enter clock mode.
+ AUX1 Down   Enter processor mode.
+ AUX2 up     Not implemented.
+ AUX2 Down   Enter player file mode to manage sound effect array values.
When in player file mode,
+ AUX1 up     Enter clock mode.
+ AUX1 Down   Not implemented.
+ AUX2 up     Not implemented.
+ AUX2 Down   Enter MP3 player mode.
````
------------------------------------------------------------------------------
## Sample Programs Stored on the SD Card



--------------------------------------------------------------------------------
Cheers