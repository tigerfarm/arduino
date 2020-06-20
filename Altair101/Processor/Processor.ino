// -----------------------------------------------------------------------------
/*
  Altair 101 Processor program

  This is an Altair 8800 emulator program that runs on an Arduino Mega microcontroller.
  Component additions to the emulator:
  + An MP3 player controlled by using the front panel toggles with the lights displaying status.
  + The clock displays the current hours, minutes, month, day, and year, on the front panel lights.

  The Altair 8800 emulator program,
  + Emulates the basic Altair 8800 computer processes from 1975.
  + The Altair 8800 was built around the Intel 8080 CPU chip.
  + The 8080's machine instructions(opcodes) are the same for the 8085.
  + This program implements more than enough opcodes to run the classic program, Kill the Bit.

  -----------------------------------------------------------------------------
  Work to do,

  Should auto exit after bytes are downloaded from the serial port.

  -----------------------------------------------------------------------------
  -----------------------------------------------------------------------------
  Program sections,

  ----------------------------
  Application definitions
  #define                     Defines and variable definitions
  byte statusByte             Front Panel Status LEDs definitions
  #include                    Includes and Arduino pin definitions.
  printClockDate()            DS3231 Clock definitions and functions
  lcdSetup()                  1602 LCD definitions and functions
  ----------------------------
  Processor Definitions and Functions:
  memoryData[memoryBytes]     Memory and process definitions
  Process a subset of the Intel 8080/8085 opcode instructions:
  processData()               Control processing instruction set of opcodes.
  processOpcode()             Process opcode instruction machine cycle one (M1): fetch opcode and process it.
  processOpcodeData()         Process opcode machine cycles greater than 1: immediate byte or address.
  ----------------------------
  Front Panel toggle switch controls and data entry events
  initSdcard()                SD card module functions
  controlResetLogic()         Processor Switch Functions
  checkClockControls()        Clock Front Panel Switch Functions.
  checkConfirmUploadSwitch()  SD card processor memory read/write from/into a file.
  checkPlayerControls()       Player Front Panel Control Switch Functions.
  ----------------------------
  DownloadProgram()           Receive bytes through serial port. The bytes are loaded into processorr memory.
  setup()                     Computer initialization.
  loop()                      Based on state, clock cycling through memory, show the time, or other state processing.

  -----------------------------------------------------------------------------
  SD card programs:
  -----------------
  + 0000    NOPs. When AUX2 is flipped down, with sense switches all down, memory is all set to zeros (NOPs).
  + 0001    Simple jump
  + 0010    More lights, jump program
  + 0011    Kill the Bit
  + 0100    Kill the Bit with an MP3 playing.
  + 0101    Add program, x + y: x in address 1, y in address 3, and answer in A6(64).
  -----------------
  + 0110    Loop a sound bite with NOPs. NOPs.
            Address 1 is the MP3 file number.
  + 0111    Loop a sound bite with NOPs. HLT at A8. RUN and JMP back to address 0 to start all over.
  -----------------
  + 1110    Program list, requires LCD
  + 1111    Start up program. Play MP3 while NOPs are processing,then HLT when the MP3 is finished.
  -----------------
  + 10000   (A12 switch) Current test program.

  Common opcodes:
  + B00000000 NOP, my NOP instruction has a delay: delay(100).
  + B01110110 HLT
  + B11000011 JMP <address, low byte followed by high byte>
  +       076 MVI A,<immediate value>
  -----------------
  +       343 OUT <port#>
  +       012 10  Port# to play the MP3 file# in register A.
  ++              If regA==0, pause the player
  +       013 11  Port# to continuously (loop) play the MP3 file# in register A.
  ++              If regA==0, pause the player
  +       015 13  Port# to flash error light sequence.
  +       052 42  Port# to flash success light sequence.
  -----------------
  + Example to add an OUT opcode option to play an MP3 file.
          // MVI A, <file#>
          // OUT 11   ; Use out 11 is for looping. 10 is for single play.
          //  ++ Address:oct > description
          //  ++       0:076 > opcode: mvi a,2  : Set to play sound bite file, 0002.mp3.
          //  ++       1:002 > immediate: 2
          //  ++       2:343 > opcode: out 11   : 11 is for looping. 10 is for single play.
          //  ++       3:013 > immediate: 11
          //  ++       ... NOPs ...

  ------------------------------------------------------------------------------
  ------------------------------------------------------------------------------
  Processor
  Application Documentation and User Guide

  ----------------------------------------
  Startup initialization, setup()
  -----------
  Init and check:
  + Serial1 for receiving programs, for download mode.
  + MP3 player module for playing MP3 files.
  + Front panel toggle settings.
  + Clock module, used when in clock mode.
  + SD card module, used for read and write.
  -----------
  If 00000000.bin exists and any of the first 32 bytes are not zero,
  + Set, programState = PROGRAM_RUN, which runs 00000000.bin when loop() starts.
  Else,
  + programState = PROGRAM_WAIT (default start state), don't run 00000000.bin when loop() starts.

  ----------------------------------------
  Modes: AUX switches for setting program modes,
  + Default mode is the Altair 8800 emulator processor mode.
  When in processor mode,
  + AUX1 up     Toggle clock mode on.
  + AUX1 Down   Toggle MP3 player mode on.
  + AUX2 up     Double flip to write processor memory to file.
  + AUX2 Down   Read file into processor memory.
  When in clock mode,
  + AUX1 up     Toggle clock mode off, return to processor mode.
  + AUX1 Down   Toggle MP3 player mode on.
  + AUX2 up     Not implemented.
  + AUX2 Down   Toggle clock timer mode on.
  When in clock timer mode,
  + AUX1 up     Toggle clock mode off, return to processor mode.
  + AUX1 Down   Toggle MP3 player mode on.
  + AUX2 up     Not implemented.
  + AUX2 Down   Toggle clock timer mode off, return to clock mode.
  When in player mode,
  + AUX1 up     Toggle clock mode on.
  + AUX1 Down   Toggle MP3 player mode off, return to processor mode.
  + AUX2 up     Not implemented.
  + AUX2 Down   Toggle player file mode to manage sound effect array values.
  When in player file mode,
  + AUX1 up     Toggle clock mode on.
  + AUX1 Down   Not implemented.
  + AUX2 up     Not implemented.
  + AUX2 Down   Toggle player file mode off, return to player mode.

  ------------------------------------------------------------------------------
  Processor
  -----------
  + Status        Current instruction cycle status.
  + Data          Data byte at the programCounter address.
  + Address       ProgramCounter value.
  + Indicator     WAIT : On when program is not running. Off when program is running.
  + Indicator     HLDA : Off.
  -----------
  + STOP          Pause running of a program, exist run mode and enter wait mode.
  + RUN           Run the program in processor memory, from the programCounter address. Enter run mode.
  + SINGLE up     Run one machine instruction cycle at a time.
  + SINGLE dn     Display the previous address and address data byte, programCounter - 1.
  + EXAMINE       1. Use the Address switches to set the adderess.
                  2.1 Flip EXAMINE sets the programCounter to address toggles.
                  2.2 Display the address in the address lights.
                  2.3 Display the address data byte in the data lights.
  + EXAMINE NEXT  Display the next address and address data byte: programCounter + 1.
  + DEPOSIT       Deposit Data switch values into the current address.
  + DEPOSIT NEXT  Deposit Data switch values into the next address.
  + RESET         Set the programCounter to zero, and display the zero address and address data byte.
  + CLR           Double flip to set processor memory to zeros, and program counter to 0.
  + PROTECT       If wait mode, decrease MP3 player volume.
  + UNPROTECT     If wait mode, increase MP3 player volume.
  + AUX1 up       Clock mode: show hour and mintues time.
  + AUX1 down     MP3 player mode
  + AUX2 up       1. Write to file: Use the Sense switches to set an SD drive program filename.
                  2. Double flip the switch. WAIT and HLDA indicators are on.
                  3. Write processor memory to SD drive.
  + AUX2 down     1. Set the Sense switches to read type, or to an SD drive program filename value.
                  2.1 Flip the switch. WAIT and HLDA indicators are on.
                  2.1.1 If Sense switches all up, enter receive mode. Wait to receive bytes from the serial port.
                  2.1.2 When received, the bytes are loaded into the processor memory.
                  2.1.3 Flip RESET to exit receive mode.
                  2.2 Double flip the switch.
                  2.2.1 If Sense switches are all down (off), reset memory to zeros.
                  2.2.2 Else, read the file bytes into processor memory.

  --------------
  User guide,

  For the processor, option to set MP3 play off.
  + When OUT 10 or 11 with regA=0, pause the player.
  + Set playerCounter=0, and don't play MP3 files when playerCounter=0.
  + Maybe have processerPlay value instead of using playerCounter.

  How to save a program to the SD card.
  + Set the Sense switches to the file number value.
  + Double flip AUX2 up to confirm and write processor memory to file.

  How to load and run a program from the SD card.
  + Set the Sense switches to the file number value.
  + Flip AUX2 down. File bytes are read into processor memory. LED ligts flash success with a sound bite message.
  + Flip RUN, to start the program running from program counter 0.

  How to assemble, upload, and run an assembler program.
  ++ See Project: Altair101/asm/README.md

  --------------
  Inputs:
  + dataByte == 000, regA = 0;              // Input is not implemented on this port.
  + dataByte == 001, regA = hwStatus;       // hwStatus = 0, a number to indicate a hardware issue.
  + dataByte == 255, regA = toggleSense();
  + Else, input port not implimented: error.

  --------------
  Outputs:
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

  ------------------------------------------------------------------------------
  Clock, clockRun(),
  + Start by showing the time of day hours and minutes.
  + To do: If clock timer mode was set, return to timer mode or reset timer mode values.
  -----------
  Clock mode,
  + Status        Display the minutes tens digit,    day tens,   or year tens
  + Address       Display the hour: A1 ... A12,      month,      or century
  + Data          Display the minutes single digit,  day single, or year single
  + Indicator     WAIT : Off.
  + Indicator     HLDA : On to indicate controlled by other than the program emulator.
  -----------
  + STOP          Not implemented.
  + RUN           Not implemented.
  + SINGLE up     1) month and day, 2) year, 3) Return to show time of day: hour and minutes
  + SINGLE dn     Not implemented.
  + EXAMINE       To do: Select the flashing date or time value.
  + EXAMINE NEXT  To do: Move through time and date values, making the selected value flash.
  + DEPOSIT       To do: After selecting and inc/dec a value, set the value.
  + DEPOSIT NEXT  Not implemented.
  + RESET         Knight Rider sounds and lights, then show time of day.
  + PROTECT       Decrease MP3 player volume. To do: Decrement value to set.
  + UNPROTECT     Increase MP3 player volume. To do: Increment value to set.
  + AUX1 Up       Toogle clock mode off, return to processor mode.
  + AUX1 down     MP3 player mode
  + AUX2 up       Not implemented.
  + AUX2 Down     Toggle clock timer mode on.
  -----------
  Clock Timer mode,
  + Status        MEMR : Timer values set.
  + Status        INP  : Ready for timer value inputs.
  + Status        M1   : Timer running.
  + Address       Displays the timer minite: A1 ... A15.
  + Data          Displays the time step 1) Set timer mode, 2) Set timer minute, 3. Run the timer
  -----------
  + STOP          Stop clock timer.
  + RUN           1. Set the timer minutes using the address toggles: A1 is 1 minute, A2 is 2 minutes, ... 15 minutes.
                  2. To do: Start the timer using the timer minutes value.
  + SINGLE up     Not implemented.
  + SINGLE dn     Not implemented.
  + EXAMINE       1. Set Address toggles to timer array value.
                  2. Flip EXAMINE.
                  3. Timer array index is displayed in the Data lights.
                  4. Timer array value is displayed in the Address lights: A1:1 minute, ..., A15:15 minutes.
  + EXAMINE NEXT  1. Timer array index is incremented.
                  2. Timer array index is displayed in the Data lights.
                  3. Timer array value is displayed in the Address lights: A1:1 minute, ..., A15:15 minutes.
  + DEPOSIT       1. Set Address lights to timer array value: A1:1 minute, ..., A15:15 minutes.
                  2. Flip DEPOSIT.
                  3. Timer value is displayed in the Address lights.
                  4. Timer array counter value is displayed in the Data lights.
                  5. Timer array value is stored into the timer array at the array counter value.
  + DEPOSIT NEXT  1. Set Address lights to timer array value: A1:1 minute, ..., A15:15 minutes.
                  2. Flip DEPOSIT NEXT.
                  3. Timer array index is incremented.
                  4. Timer array index is displayed in the Data lights.
                  5. Timer array value is displayed in the Address lights.
                  6. Timer time value is stored into the memory array, at the incremented array index value.
  + RESET         1. Set timer array index to 1.
                  2. Timer array index is displayed in the Data lights.
                  3. Timer array value is displayed in the Address lights.
  + PROTECT       Decrease MP3 player volume. To do: Decrement value to set.
  + UNPROTECT     Increase MP3 player volume. To do: Increment value to set.
  + AUX1 Up       Toogle clock mode off, return to processor mode.
  + AUX1 down     MP3 player mode
  + AUX2 up       Not implemented.
  + AUX2 Down     Toggle clock timer mode off, return to clock mode.
  -----------

  User guide, Clock Timer,
  + Must be in clock mode.
  + Flip AUX2 toggle Down to enter clock timer mode.
  + Set the minutes by toggling a single address switch. For example A10 is for 10 minutes.
  + Flip RUN to start the timer.
  ++ Flash counter minute LED on/off each second.
  ++ Increment counter minute, each minute, starting with A0.
  ++ Play a sound bite as each minute passes.
  ++ Sound and flash when time is reached, and return to displaying the time of day hours and minutes.

  Enter a sequence of timer values,
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
  MP3 Player mode, playerRun(),
  + MP3 addressable files is 255.
  -----------
  + Address       Displays the song number that is playing.
  + Data          Displays the volume.
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
  -----------
  Player file mode, checkPlayerFileControls()
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
  + PROTECT       To do: Decrease volume
  + UNPROTECT     To do: Increase volume
  + AUX1 up       Clock mode: show hour and mintues time.
  + AUX1 down     Not implemented.
  + AUX2 up       Not implemented.
  + AUX2 down     Exit player file mode, return to player MP3 mode.

  -----------
  Player consistancy with processor.
  + If HLTA_ON, don't start playing for EXAMINE, or other switches.
  + If HLTA_OFF, continue playing when flipping EXAMINE, or other switches.

  -----------
  User guide, Actions that have sound effects,
  + Constant          Array index value.
  int READ_FILE       = 1;
  int TIMER_COMPLETE  = 2;
  int CLOCK_ON        = 3;
  int CLOCK_OFF       = 4;
  int PLAYER_ON       = 3;
  int PLAYER_OFF      = 4;
  int KR5             = 5;
  int CLOCK_CUCKOO    = 6;
  int TIMER_MINUTE    = 7;
  To hear which sound effect matches action, do the following.
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

  ------------------------------------------------------------------------------
  ------------------------------------------------------------------------------
  Current/Next Work

  Clock currently requires an LCD to set the time.
  + I should add inc/dec hours and minutes using toggles. This would also work for my other clock.

  --------------
  + Implement a processor error function, such as:
  #ifdef LOG_MESSAGES
        Serial.print(F(" Error, IN not implemented on this port."));
  #endif
        ledFlashError();
        controlStopLogic();
        statusByte = HLTA_ON;
        digitalWrite(WAIT_PIN, HIGH);
  + Add a sound when there is a program error.

  --------------
  + hwStatus should have status, like processor statuses.
             Hardware  OK or Not Online.
  // hwStatus = 1;  // SD_OK, SD_NO, SD card
  // hwStatus = 2;  // CL_OK, CL_NO, Clock module
  // hwStatus = 4;  // PL_OK, PL_NO, MP3 Player

  Update the SD card logic: initSdcard()

  Test write memory issue,
  + Seems to be a CALL opcode issue.
  + If byte count over 276, characters no longer are displayed.
  + Fails at the address: 00010100 (276)

  Time to generate videos.
  + Done: Steampunk tablet running Kill the Bit.
  + Emulate Star Trek computer using, Kill the Bit flashing lights and playing a Star Trek computer sound bite.
  + Run NOP program without and with a sound bite.
  + Demo entering and running the following program:
   ; Add content of address 1 and 3, and store the answer in address 64.
  ++ Address:oct > description
  ++       0:076 > opcode: mvi a,2
  ++       1:002 > immediate: 2 : 2
  ++       2:306 > opcode: adi 3
  ++       3:003 > immediate: 3 : 3
  ++       4:062 > opcode: sta 64
  ++       5:100 > lb: 64
  ++       6:000 > hb: 0
  ++       7:166 > opcode: hlt
  ++       8:303 > opcode: jmp Start
  ++       9:000 > lb: 0
  ++      10:000 > hb: 0
  + End of list.

  Now, can set the LCD backlight as on when prompting, and reset after.
  + LCD backlight on/off status boolean: LcdBacklight.

  ---------------------------------------------
  Desktop Box:
  ------------
  + Mount, connect, and test an LCD, such as a 1602 LCD.
  + Add internal 120AC socket for 3 devices: 1) Mega 5V wall adapter, 2) MP3 5V wall adapter, 3) stearo amp plug,
  + Use the Mega to control an On/off relay switch for 120AC socket.
  + Use the Mega to control an On/off relay switch for the stearo amp's 120AC adapter.
  + Later, add the stearo amp power supply inside the case.
  ------------
  + Done: Cut a glue Spider-Man paper to panels: 2 sides, bar top, and top panel.
  + Done: Cut separation on the top for easy internal access.
  + Done: Install, wire, and test the front panel.
  + Done: Panel LED lights all display correctly.
  + Done: Toggle functions all work for Altair 8800 emulation.
  + Done: Test new serial module using the tablet. Then install it in the box.
  + Done: Panel LED lights and toggle functions all work for the clock.
  + Done: Panel LED lights and toggle functions all work for the MP3 player.
  + Done: Wire up the MP3 player using multiple USB wall plugs to reduce static noise.
  + Done: Add RCA female plugs for L/R external output to an amp.

  ---------------------------------------------
  ---------------------------------------------
  Connect the Mega to the desktop front panel:

  Total = 12 Mega pin wire connections to the front panel, 4 of which are for power (+ and -).

  5 Mega pins to PCF8574 daisy chain, toggle inputs:
  + I2C SDA to Mega SDA
  + I2C SCL to Mega SCL
  + I2C +5V
  + I2C Ground
  + 1 wrie from the Mega pin 2 to the front panel breadboard. Then 2 wires to PCF8574 module interrupts for Control and AUX.

  5 Mega pins to 74HC595 daisy chain, LED output:
  + Mega pin A14 to 74HC595 14 Data pin.
  + Mega pin A12 to 74HC595 12 Latch pin.
  + Mega pin A11 to 74HC595 11 Clock pin.
  --- Plus ---
  + 2 wires for +5V and Ground to the front panel breadboard.

  2 pins for the WAIT and HLDA LED lights.
  + const int WAIT_PIN = A9;
  + const int HLDA_PIN = A10;

  ---------------------------------------------
  Connect the Mega to the MP3 player:
  + Mega pin 18(RX) to player pin 3(TX)
  + Mega pin 19(TX) to external resister to player pin 2(RX)

*/
// -----------------------------------------------------------------------------
// Code compilation options.

// Define for desktop module.
// Comment out for tablet model.
#define DESKTOP_MODULE 1

#define SETUP_SDCARD 1
#define SETUP_CLOCK 1
// #define SETUP_LCD 1

#define INCLUDE_AUX 1
#define INCLUDE_CLOCK 1
#define INCLUDE_SDCARD 1
#define INCLUDE_LCD 1

// -----------------------------------
// #define LOG_MESSAGES 1         // Has large memory requirements.
#define SWITCH_MESSAGES 1

// -----------------------------------------------------------------------------
// Program states

#define PROGRAM_WAIT 0
#define PROGRAM_RUN 1
#define CLOCK_RUN 2
#define PLAYER_RUN 3
#define SERIAL_DOWNLOAD 4
int programState = PROGRAM_WAIT;  // Intial, default.

// MP3 player has a sub-state for managing sound effect byte values in files.
#define PLAYER_FILE 0
#define PLAYER_MP3 1
int playerState = PLAYER_MP3;     // Intial, default.

int theCounterHours = 0;
int theCounterMinutes = 0;

// -----------------------------------------------------------------------
// DFPlayer Mini MP3 play

#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
DFRobotDFPlayerMini mp3player;

// Front panel display light values:
uint16_t playerCounter = 1;       // First song played when player starts up. Then incremented when next is played.
uint8_t playerStatus = 0;
uint8_t playerVolume = 0;
//
uint16_t playerCounterProcessor = 0; // Indicator for the processor to play an MP3, or not (if zero).
//
uint16_t playerCounterTop = 0;
uint16_t playerDirectoryTop = 0;
uint8_t playerDirectory = 1;      // File directory name on the SD card. Example 1 is directory name: /01.
boolean playPause = false;        // For toggling pause.
boolean loopSingle = false;       // For toggling single song.

void playerLights() {
  lightsStatusAddressData(playerStatus, playerCounter, playerVolume);
}

// Front panel display light values:
uint8_t playerFileStatus = B10000000;   // MEMR_ON
uint8_t playerFileData = 0;
uint16_t playerFileAddress = 0;       // First song played when player starts up. Then incremented when next is played.

void playerFileLights() {
  lightsStatusAddressData(playerFileStatus, playerFileAddress, playerFileData);
}

// ---------------------------
// Sound bites for sound effects

// Example: playerPlaySound(READ_FILE) plays 05-transfercomplete.mp3
// Arrary matching the above value, to the soundEffects array, file number.
// Example: READ_FILE=1 which is array value = 5, soundEffects[READ_FILE]=5 or soundEffects[1]=5.
const int maxSoundEffects = 16;
int soundEffects[maxSoundEffects];
//  Variable      soundEffects[] Array value
int READ_FILE       = 1;
int TIMER_COMPLETE  = 2;
int CLOCK_ON        = 3;
int CLOCK_OFF       = 4;
int PLAYER_ON       = 3;
int PLAYER_OFF      = 4;
int KR5             = 5;
int CLOCK_CUCKOO    = 6;
int TIMER_MINUTE    = 7;
//
// Function to play a sound file using the above mapping.
void playerPlaySound(int theFileNumber) {
  // Serial.print(F("+ playerPlaySound("));
  // Serial.print(theFileNumber);
  // Serial.print(F(") "));
  if ((playerStatus & B00001000) && (theFileNumber > 0)) {
    mp3player.play(soundEffects[theFileNumber]);
    // Serial.print(F("mp3player.play("));
    // Serial.print(soundEffects[theFileNumber]);
    // Serial.print(F(")"));
  }
  // Serial.println("");
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Front Panel Status LEDs

// ------------------------------
// Status Indicator LED lights

// Program wait status.
const int WAIT_PIN = A9;      // Processor program wait state: off/LOW or wait state on/HIGH.

// HLDA : 8080 processor goes into a hold state because of other hardware running.
const int HLDA_PIN = A10;     // Emulator processing (off/LOW) or clock/player processing (on/HIGH).

// ------------------------------
// Added this to identify hardware status.
// if hardware has an error, or hardware is not initialized, hwStatus > 0.
// Else hwStatus = 0.
byte hwStatus = 0;
// hwStatus = 1;  // 0001 SD card
// hwStatus = 2;  // 0010 Clock module
// hwStatus = 4;  // 0100 MP3 Player
const byte SD_ON  =    B00000001;
const byte CL_ON  =    B00000010;
const byte PL_ON  =    B00000100;
const byte SD_OFF =    B11111110;
const byte CL_OFF =    B11111101;
const byte PL_OFF =    B11111011;

// ------------------------------
// Status LEDs
//
// Info: page 33 of the Altair 8800 oprator's manaul.
// ------------
// Not in use:
//  INTE : On, interrupts enabled.
//  INT : An interrupt request has been acknowledged.
//  PROT : Useful only if RAM has page protection impliemented. I'm not implementing PROT.
// ------------
// HLDA : 8080 processor go into a hold state because of other hardware such as the clock.

// ------------
// Bit patterns for the status shift register (SN74HC595N):

byte statusByte = B00000000;        // By default, all are OFF.

// ------------
// Order on the desktop module, seems to match the tablet module.
// Desktop module,
// Status lights: INT(B000000001), WO, STACK, HLTA, OUT, MI, INP, MEMR(B10000000)
// ------------
// Tablet module:
// MEMR - Bar LED #1
// M1   - Bar LED #3
// HLTA - Bar LED #5
// WO   - Bar LED #7
// WAIT - Green LED on the tablet
// HLDA - Red LED
// ------------

// Use OR to turn ON. Example:
//  statusByte = statusByte | MEMR_ON;
const byte MEMR_ON =    B10000000;  // MEMR   The memory bus will be used for memory read data.
const byte INP_ON =     B01000000;  // INP    The address bus containing the address of an input device. The input data should be placed on the data bus when the data bus is in the input mode
const byte M1_ON =      B00100000;  // M1     Machine cycle 1, fetch opcode.
const byte OUT_ON =     B00010000;  // OUT    The address contains the address of an output device and the data bus will contain the out- put data when the CPU is ready.
const byte HLTA_ON =    B00001000;  // HLTA   Machine opcode hlt, has halted the machine.
const byte STACK_ON =   B00000100;  // STACK  Stack process
const byte WO_ON =      B00000010;  // WO     Write out (inverse logic)
const byte INT_ON =     B00000001;  // INT    Interrupt

// Use AND to turn OFF. Example:
//  statusByte = statusByte & M1_OFF;
const byte MEMR_OFF =   B01111111;
const byte INP_OFF =    B10111111;
const byte M1_OFF =     B11011111;
const byte OUT_OFF =    B11101111;
const byte HLTA_OFF =   B11110111;
const byte STACK_OFF =  B11111011;
const byte WO_OFF =     B11111101;
const byte INT_OFF =    B11111110;
// const byte WAIT_OFF =   B11111110;   // WAIT   Changed to a digital pin control.

// Video demonstrating status lights:
//    https://www.youtube.com/watch?v=3_73NwB6toY
// MEMR & M1 & WO are on when fetching an op code, example: jmp(303) or lda(072).
// MEMR & WO are on when fetching a low or high byte of an address.
// MEMR & WO are on when fetching data from an address.
// All status LEDs are off when storing a value to a memory address.
// When doing a data write, data LEDs are all on.
//
// Halt: MEMR & hltA & WO are on. All address and data lights are on.
//  System is locked. To get going again, hard reset: flip stop and reset same time.
//
// STACK is the only status LED on when making a stack push (write to the stack).
// MEMR & STACK & WO are on when reading from the stack.
//
// INP & WO are on when reading from an input port.
// out is on when send an output to a port.
//
// INTE is on when interrupts are enabled.
// INTE is off when interrupts are disabled.

// -----------------------------------------------------------------------------
void ledFlashKnightRider(int times, boolean NotUsed) {
  int delayTime = 66;
  int theDelayTime = 0;
  unsigned int riderByte;
  byte flashByte;
  int iStart = 0;
  lightsStatusAddressData(0, 0, 0);
  delay(delayTime);
  for (int j = 0; j < times; j++) {
    flashByte = B10000000;
    for (int i = iStart; i < 8; i++) {
      // A15 to A8.
      riderByte = flashByte * 256;
      lightsStatusAddressData(0, riderByte, 0);
      flashByte = flashByte >> 1;
      if (j > 0 && i == 0) {
        theDelayTime = delayTime / 2;
      } else {
        theDelayTime = delayTime;
      }
      delay(theDelayTime);
    }
    flashByte = B10000000;
    for (int i = 0; i < 7; i++) {
      // A7 to A0.
      lightsStatusAddressData(0, flashByte, 0);
      flashByte = flashByte >> 1;
      delay(delayTime);
    }
    flashByte = B00000001;
    for (int i = 0; i < 8; i++) {
      // A0 to A7.
      lightsStatusAddressData(0, flashByte, 0);
      flashByte = flashByte << 1;
      delay(delayTime);
    }
    flashByte = B00000001;
    for (int i = 0; i < 8; i++) {
      // A8 to A14.
      // Only 7, as not to repeat flashing the LED twice.
      riderByte = flashByte * 256;
      lightsStatusAddressData(0, riderByte, 0);
      flashByte = flashByte << 1;
      if (j < times && i == 0) {
        theDelayTime = delayTime / 2;
      } else {
        theDelayTime = delayTime;
      }
      delay(theDelayTime);
    }
  }
  //
  // Reset the panel lights to program values.
}

void KnightRiderScanner() {
  // Knight Rider scanner lights and sound.
  if (playerStatus & HLTA_ON) {
    mp3player.play(CLOCK_CUCKOO);
    ledFlashKnightRider(1, true);
    mp3player.play(CLOCK_CUCKOO);
    ledFlashKnightRider(1, false);
    mp3player.play(CLOCK_CUCKOO);
    ledFlashKnightRider(1, false);
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Switch definitions

#include <PCF8574.h>
#include <Wire.h>

// -------------------------------------------
// Desktop version.
// Address for the PCF8574 module being tested.
PCF8574 pcfControl(0x020);  // Control: STOP, RUN, SINGLE STEP, EXAMINE, EXAMINE NEXT, DEPOSIT, DEPOSIT NEXT, REST
PCF8574 pcfData(0x021);     // Low bytes, data byte
PCF8574 pcfSense(0x022);    // High bytes, sense switch byte
PCF8574 pcfAux(0x023);      // AUX switches and others: Step down, CLR, Protect, Unprotect, AUX1 up, AUX1 down,  AUX2 up, AUX2 down

//                   Mega pin for control toggle interrupt. Same pin for Nano.
const int INTERRUPT_PIN = 2;

// Interrupt setup: interrupt pin to use, interrupt handler routine.
boolean pcfControlinterrupted = false;
void pcfControlinterrupt() {
  pcfControlinterrupted = true;
}

// -----------------------------------------------------------------------------
// Tablet version pin definitions.
// Desktop uses a PCF8574 controller.
//                              Mega pins
const int CLOCK_SWITCH_PIN =    8;  // Also works: pins 4 and A11. Doesn't work: 24, 33.
const int PLAYER_SWITCH_PIN =   9;
const int UPLOAD_SWITCH_PIN =   10;
const int DOWNLOAD_SWITCH_PIN = 11;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// SD Card module is an SPI bus slave device.
#ifdef INCLUDE_SDCARD

#include <SPI.h>
#include <SD.h>

//     Mega Nano - SPI module pins
// Pin 53   10   - CS   : chip/slave select (SS pin). Can be any master(Nano) digital pin to enable/disable this device on the SPI bus.
// Pin 52   13   - SCK  : serial clock, SPI: accepts clock pulses which synchronize data transmission generated by Arduino.
// Pin 51   11   - MOSI : master out slave in, SPI: input to the Micro SD Card Module.
// Pin 50   12   - MISO : master in slave Out, SPI: output from the Micro SD Card Module.
// Pin 5V+  - VCC  : can use 3.3V or 5V
// Pin GND  - GND  : ground
// Note, Nano pins are declared in the SPI library for SCK, MOSI, and MISO.

// The CS pin is the only one that is not really fixed as any of the Arduino digital pin.
// const int csPin = 10;  // SD Card module is connected to Nano pin 10.
const int csPin = 53;  // SD Card module is connected to Mega pin 53.

File myFile;

#endif

// -----------------------------------------------------------------------------
// Add another serial port settings, to connect to the new serial hardware module.
#include <SoftwareSerial.h>
// Connections:
// Since not transmiting, the second parameter pin doesn't need to be connected.
// Parameters: (receive, transmit).
// Receive needs to be an interrupt pin.
// Computer USB >> serial2 module TXD >> RX pin for the Arduino to receive the bytes.
//                                TXD transmits received bytes to the Arduino receive (RX) pin.
const int PIN_RX = 12;  // Arduino receive is connected to TXD on the serial module.
const int PIN_TX = 11;  // Arduino transmit is not used, and therefore notconnected to RXD pin on the serial module.
SoftwareSerial serial2(PIN_RX, PIN_TX);
// Then, to read from the new serial port, use:
//    serial2.begin(9600);
//    serial2.available()
//    serial2.read();

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Output LED lights shift register(SN74HC595N) pins

//           Mega/Nano pins        74HC595 Pins
const int dataPinLed  = A14;    // pin 14 Data pin. Also tested with pin 7.
const int latchPinLed = A12;    // pin 12 Latch pin.
const int clockPinLed = A11;    // pin 11 Clock pin.

// -----------------------------------------------------------------------------
// Clock setting values using in toggle switch functions.

#ifdef INCLUDE_CLOCK

// For the clock board.
#include "RTClib.h"
RTC_DS3231 rtc;
DateTime now;

// Clock has a sub-state for managing the clock timer.
#define CLOCK_SET 0
#define CLOCK_TIME 1
#define CLOCK_TIMER 2
int clockState = CLOCK_TIME;     // Intial, default.

int setClockValue = 0;
// rtc.adjust(DateTime(2019, 10, 9, 16, 22, 3));   // year, month, day, hour, minute, seconds
int theCounterYear = 0;
int theCounterMonth = 0;
int theCounterDay = 0;
// int theCounterHours = 0;     // Declared above.
// int theCounterMinutes = 0;
int theCounterSeconds = 0;

int theSetRow = 1;
int theSetCol = 0;
int theSetMin = 0;
int theSetMax = 59;
int setValue = 0;

int theCursor;
const int printRowClockDate = 0;
const int printColClockDate = 0;
const int printRowClockPulse = 0;
const int thePrintColMonth = 5;
const int thePrintColDay = 5;
const int thePrintColHour = thePrintColDay + 3;
const int thePrintColMin = thePrintColHour + 3;
const int thePrintColSec = thePrintColMin + 3;

// -----------------------------------------------------------------------------
// 1602 LCD
/*
  1602 LCD serial connections pins,
    LCD - Nano - Mega - Mega optional
    SCL - A5   - SCL  - 21
    SDA - A4   - SDA  - 20
    VCC - 5V   - 5V
    GND - GND  - GND
*/
#ifdef INCLUDE_LCD

#include<Wire.h>
#include<LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

// ----------------------------------------------
// Toggle the LCD backlight on/off.
boolean lcdBacklightIsOn = true;
void lcdBacklight(boolean setOn) {
  if (setOn) {
    Serial.println(F("++ Backlight on."));
    lcd.backlight();
    lcdBacklightIsOn = true;
    return;
  }
  Serial.println(F("++ Backlight off."));
  lcd.noBacklight();
  lcdBacklightIsOn = false;
}
void toggleLcdBacklight() {
  if (lcdBacklightIsOn) {
    lcdBacklightIsOn = false;
    // Serial.println("+ Toggle: off.");
    lcd.noBacklight();
  } else {
    lcdBacklightIsOn = true;
    // Serial.println("+ Toggle: on.");
    lcd.backlight();
  }
}

void printLcdClockValue(int theColumn, int theRow, int theInt) {
  lcd.setCursor(theColumn, theRow);    // Column, Row
  if (theInt < 10) {
    lcd.print("0");
    lcd.setCursor(theColumn + 1, theRow);
  }
  lcd.print(theInt);
}

// -------------------------
char dayOfTheWeek[7][1] = {"S", "M", "T", "W", "T", "F", "S"};

void printClockDate() {
  now = rtc.now();
  theCounterYear = now.year();
  theCounterMonth = now.month();
  theCounterDay = now.day();
  //
  theCursor = printColClockDate;
  lcd.setCursor(theCursor, printRowClockDate);    // Column, Row
  lcd.print(dayOfTheWeek[now.dayOfTheWeek()]);
  // ---
  lcd.setCursor(++theCursor, printRowClockDate);    // Column, Row
  lcd.print(":");
  printLcdClockValue(++theCursor, printRowClockDate, theCounterMonth);
  // ---
  theCursor = theCursor + 2;
  lcd.print("/");
  printLcdClockValue(++theCursor, printRowClockDate, theCounterDay);
}

void setClockMenuItems() {
  if (!lcdBacklightIsOn) {
    // Don't make clock setting changes when the LCD is off.
    return;
  }
  switch (setClockValue) {
    case 0:
      // Serial.print("Cancel set");
      lcdPrintln(theSetRow, "");
      break;
    case 1:
      // Serial.print("seconds");
      // Stacy, need to clear minute.
      lcdPrintln(theSetRow, "Set:");
      theSetMax = 59;
      theSetMin = 0;
      theSetCol = thePrintColSec;
      setValue = theCounterSeconds;
      printLcdClockValue(theSetCol, theSetRow, setValue);
      break;
    case 2:
      // Serial.print("minutes");
      // Stacy, need to clear hour.
      lcdPrintln(theSetRow, "Set:");
      theSetMax = 59;
      theSetMin = 0;
      theSetCol = thePrintColMin;
      setValue = theCounterMinutes;
      printLcdClockValue(theSetCol, theSetRow, setValue);
      break;
    case 3:
      // Serial.print("hours");
      // Stacy, need to clear month.
      //                     1234567890123456
      lcdPrintln(theSetRow, "Set:");
      theSetMax = 24;
      theSetMin = 0;
      theSetCol = thePrintColHour;
      setValue = theCounterHours;
      printLcdClockValue(theSetCol, theSetRow, setValue);
      break;
    case 4:
      // Serial.print("day");
      lcdPrintln(theSetRow, "Set day:");
      theSetMax = 31;
      theSetMin = 1;
      theSetCol = thePrintColMin;
      setValue = theCounterDay;
      printLcdClockValue(theSetCol, theSetRow, setValue);
      break;
    case 5:
      // Serial.print("month");
      lcdPrintln(theSetRow, "Set month:");
      theSetMax = 12;
      theSetMin = 1;
      theSetCol = thePrintColMin;
      setValue = theCounterMonth;
      printLcdClockValue(theSetCol, theSetRow, setValue);
      break;
    case 6:
      // Serial.print("year");
      lcdPrintln(theSetRow, "Set year:");
      theSetMax = 2525; // In the year 2525, If man is still alive, If woman can survive...
      theSetMin = 1795; // Year John Keats the poet was born.
      theSetCol = thePrintColMin;
      setValue = theCounterYear;
      printLcdClockValue(theSetCol, theSetRow, setValue);
      break;
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// 1602 LCD print functions.

const int displayColumns = 16;  // LCD has 16 columns for printing to.

int lcdColumn = 0;    // Current print column position.
int lcdRow = 0;       // Current print row/line.
String lcdRow0 = "";  // Buffer for row 0.
String lcdRow1 = "";  // Buffer for row 1.

//                        1234567890123456
String clearLineString = "                ";

// ------------------------------------------------
void lcdSetup() {
  lcd.init();
  lcdBacklight(true);  // LCD Backlight on.
  lcd.cursor();
  // lcdSplash();
  lcd.clear();
  //         1234567890123456
  lcdRow0 = "Altair 101";
  lcdPrintln(0, lcdRow0);
  lcdRow = 1;
  lcdColumn = 0;
  lcd.setCursor(lcdColumn, lcdRow);
}
void lcdSplash() {
  lcd.clear();
  //         1234567890123456
  lcdRow0 = "Altair 101";
  //         0123456789012345
  lcdRow1 = "Ready...";
  lcdPrintln(0, lcdRow0);
  lcdPrintln(1, lcdRow1);
  lcdRow = 1;
  lcdColumn = 8;
  lcd.setCursor(lcdColumn, lcdRow);
}

void lcdClearScreen() {
  Serial.println(F("++ Clear screen."));
  lcd.clear();
  lcd.home();     // Set cursor home (0,0).
  lcdColumn = 0;
  lcdRow = 0;
  lcdRow0 = "";
  lcdRow1 = "";
}

// ----------------------------------------------
// Print line.

void lcdPrintln(int theRow, String theString) {
  // To overwrite anything on the current line.
  String printString = theString;
  int theRest = displayColumns - theString.length();
  if (theRest < 0) {
    // Shorten to the display column length.
    printString = theString.substring(0, displayColumns);
  } else {
    // Buffer with spaces to the end of line.
    while (theRest > 0) {
      printString = printString + " ";
      theRest--;
    }
  }
  lcd.setCursor(0, theRow);
  lcd.print(printString);
}

// ----------------------------------------------
void lcdScroll() {
  //Serial.println(F("+ lcdScroll()"));
  // -----------------------
  // Place cursor at start of the second line.
  lcdColumn = 0;
  if (lcdRow == 0) {
    // If the first time printing to the second row, no need to scroll the data.
    lcd.setCursor(0, 1);
    lcdRow = 1;
    return;
  }
  // -----------------------
  // Scroll the data.
  //
  // Clear the screen befor moving row 1 to 0, and row 1 needs to be clear.
  lcd.clear();
  // Screen buffers: scroll row 1, up to row 0.
  lcdRow0 = lcdRow1;
  lcdPrintln(0, lcdRow0);
  lcdRow1 = "";  // Clear row 1 buffer.
  lcd.setCursor(0, 1);
}

// ----------------------------------------------
// Print character.

void lcdPrintChar(String theChar) {
  Serial.print(F("+ lcdPrintChar :"));
  Serial.print(theChar);
  Serial.print(F(":"));
  // ----------------------------------------------
  // New line character
  if (theChar == "\n") {
    lcdScroll();
    // delay(1000);
    return;
  }
  // ----------------------------------------------
  // Print character to the display.
  //
  // Characters are dropped, if print past the 16 characters in a line.
  if (lcdColumn >= displayColumns) {
    return;
  }
  //
  if (lcdRow == 0) {
    lcdRow0 = lcdRow0 + theChar;
  } else {
    lcdRow1 = lcdRow1 + theChar;
  }
  lcd.print(theChar);
  lcdColumn++;
  if (lcdColumn < displayColumns) {
    // Move the cursor forward.
    lcd.setCursor(lcdColumn, lcdRow);
  }
}

#endif

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Memory definitions

const int memoryBytes = 1024;  // When using Mega: 1024 or 2048, for Nano: 256
byte memoryData[memoryBytes];
unsigned int curProgramCounter = 0;     // Current program address value
unsigned int programCounter = 0;        // When processing opcodes, the next program address value.

const int stackBytes = 32;
int stackData[stackBytes];
unsigned int stackPointer = stackBytes;

// -----------------------------------------------------------------------------
// Memory Functions

char charBuffer[17];

void zeroOutMemory() {
  // Clear memory option.
  // Example, clear memory before loading a new program.
  Serial.println(F("+ Initialize all memory bytes to zero."));
  for (int i = 0; i < memoryBytes; i++) {
    memoryData[i] = 0;
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Process Definitions

// -----------------------------------------------------------
// Source registers, Destination registers, and register pairs.
byte regA = 0;   // 111=A  a  register A, or Accumulator
//                            --------------------------------
//                            Register pair 'RP' fields:
byte regB = 0;   // 000=B  b  00=BC   (B:C as 16 bit register)
byte regC = 0;   // 001=C  c
byte regD = 0;   // 010=D  d  01=DE   (D:E as 16 bit register)
byte regE = 0;   // 011=E  e
byte regH = 0;   // 100=H  h  10=HL   (H:L as 16 bit register)
byte regL = 0;   // 101=L  l
//                            11=SP   (Stack pointer, refers to PSW (FLAGS:A) for PUSH/POP)
byte regM = 0;   // 110=M  m  Memory reference for address in H:L

// -----------------------------------
// Process flags and values.

boolean flagCarryBit = false; // Set by dad. Used jnc.
boolean flagZeroBit = true;   // Set by cpi. Used by jz.

byte aByteBit;                    // For capturing a bit when doing bitwise calculations.
char asciiChar;  // For printing ascii characters, example 72 is the letter "H".

// For calculating 16 bit values.
// uint16_t bValue;           // Test using uint16_t instead of "unsigned int".
unsigned int bValue = 0;
unsigned int cValue = 0;
unsigned int dValue = 0;
unsigned int eValue = 0;
unsigned int hValue = 0;
unsigned int lValue = 0;
unsigned int bcValue = 0;
unsigned int deValue = 0;
unsigned int hlValue = 0;
unsigned int hlValueNew = 0;

// -----------------------------------------------------------------------------
// Output: log messages and Front Panel LED data lights.

void printAddress(unsigned int theAddress) {
  byte aByte = theAddress / 256;
  printByte(aByte);
  aByte = theAddress - theAddress * 256;
  Serial.print(":");
  printByte(aByte);
}
void printByte(byte b) {
  for (int i = 7; i >= 0; i--)
    Serial.print(bitRead(b, i));
}
void printWord(unsigned int theValue) {
  String sValue = String(theValue, BIN);
  for (int i = 1; i <= 16 - sValue.length(); i++) {
    Serial.print("0");
  }
  Serial.print(sValue);
}

void printHex(byte b) {
  String sValue = String(b, HEX);
  for (int i = 1; i <= 3 - sValue.length(); i++) {
    Serial.print("0");
  }
  Serial.print(sValue);
}

void printOctal(byte b) {
  String sValue = String(b, OCT);
  for (int i = 1; i <= 3 - sValue.length(); i++) {
    Serial.print("0");
  }
  Serial.print(sValue);
}

void printData(byte theByte) {
  sprintf(charBuffer, "%3d = ", theByte);
  Serial.print(charBuffer);
  printOctal(theByte);
  Serial.print(F(" = "));
  printByte(theByte);
}

void displayCmp(String theRegister, byte theRegValue) {
  Serial.print(F(" > cmp, compare register "));
  Serial.print(theRegister);
  Serial.print(F(" to A. "));
  Serial.print(theRegister);
  Serial.print(F(":"));
  Serial.print(theRegValue);
  if (flagZeroBit) {
    Serial.print(F(" == "));
  } else {
    if (flagCarryBit) {
      Serial.print(F(" > "));
    } else {
      Serial.print(F(" < "));
    }
  }
  Serial.print(F(" A:"));
  Serial.print(regA);
}

// -----------------------------------------------------------------------------
// Processor: Processing opcode instructions

byte opcode = 0;            // Opcode being processed
int instructionCycle = 0;   // Opcode process cycle

// Instruction parameters:
byte dataByte = 0;           // db = Data byte (8 bit)

byte lowOrder = 0;           // lb: Low order byte of 16 bit value.
byte highOrder = 0;          // hb: High order byte of 16 bit value.

// ------------------------------------------------
// Data LED lights displayed using shift registers.

void programLights() {
  // Use the current program values: statusByte, curProgramCounter, and dataByte.
  digitalWrite(latchPinLed, LOW);
#ifdef DESKTOP_MODULE
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, statusByte);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, dataByte);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, lowByte(curProgramCounter));
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, highByte(curProgramCounter));
#else
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, dataByte);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, lowByte(curProgramCounter));
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, highByte(curProgramCounter));
  shiftOut(dataPinLed, clockPinLed, MSBFIRST, statusByte); // MSBFIRST matches the bit to LED mapping.
#endif
  digitalWrite(latchPinLed, HIGH);
}

void lightsStatusAddressData( byte status8bits, unsigned int address16bits, byte data8bits) {
  digitalWrite(latchPinLed, LOW);
#ifdef DESKTOP_MODULE
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, status8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, data8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, lowByte(address16bits));
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, highByte(address16bits));
#else
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, data8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, lowByte(address16bits));
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, highByte(address16bits));
  shiftOut(dataPinLed, clockPinLed, MSBFIRST, status8bits);
#endif
  digitalWrite(latchPinLed, HIGH);
}

void processData() {
  //
  // Get the next opcode data to be processed.
  dataByte = memoryData[programCounter];
  // Update curProgramCounter to the address being processed.
  curProgramCounter = programCounter;
  //
#ifdef LOG_MESSAGES
  Serial.print(F("Addr: "));
  sprintf(charBuffer, "%4d:", programCounter);
  Serial.print(charBuffer);
  Serial.print(F(" Data: "));
  printData(dataByte);
  Serial.print(" > ");
#endif
  if (opcode == 0) {
    // Instruction machine cycle 1 (M1 cycle), fetch the opcode.
    instructionCycle = 1;
    // statusByte = MEMR_ON | M1_ON | WO_ON;
    statusByte = statusByte | MEMR_ON;
    statusByte = statusByte | M1_ON;
    statusByte = statusByte | WO_ON;
    // If no parameter bytes (immediate data byte or address bytes), process the opcode.
    // Else, the opcode variable is set to the opcode byte value.
    processOpcode();
    // If not a RET opcode, programCounter remains the same.
    // If a RET opcode processed, programCounter is set(from the stack) to the call-from address.
    //    Then set to the address after the call-from address, by this statement:
    programCounter++;
  } else {
    // Machine cycles 2, 3, or 4.
    instructionCycle++;
    statusByte = statusByte & M1_OFF;
    //
    // Processing opcode data: an immediate data byte or an address(2 bytes: lb,hb).
    // If not a jump, programCounter incremented.
    // Else, programCounter is the jump-to address.
    // dataByte maybe changed in the case of displaying the read/write value.
    // statusByte maybe updated, example for IN or OUT.
    processOpcodeData();
  }
  // Now, programCounter holds the next address to process.
  // curProgramCounter holds the currently processed address.
  //
  // -------------
  // Display the currently processed: status, program counter, and data byte.
#ifdef LOG_MESSAGES
  Serial.print("+ ");
#endif
  programLights();  // Uses: statusByte, curProgramCounter, dataByte.
#ifdef LOG_MESSAGES
  Serial.println("");
#endif
  // -------------
}

// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
void processOpcode() {

  // Working variables.
  byte workingByte = 0;
  unsigned int anAddress = 0;

  switch (dataByte) {

    // ---------------------------------------------------------------------
    // ++ ADD SSS  10 000 SSS  1  Add source register to register A. To do, set: ZSCPA.
    //    10000SSS
    case B10000000:
#ifdef LOG_MESSAGES
      Serial.print(F(" > add, Add register B:"));
      printByte(dataByte);
      Serial.print(F(" to register A:"));
      printByte(regB);
#endif
      regA = regA + regB;
#ifdef LOG_MESSAGES
      Serial.print(F(" = "));
      printByte(regA);
#endif
      break;
    case B10000001:
#ifdef LOG_MESSAGES
      Serial.print(F(" > add, Add register C:"));
      printByte(regC);
      Serial.print(F(" to register A:"));
      printByte(regA);
#endif
      regA = regA + regC;
#ifdef LOG_MESSAGES
      Serial.print(F(" = "));
      printByte(regA);
#endif
      break;
    case B10000010:
#ifdef LOG_MESSAGES
      Serial.print(F(" > add, Add register D:"));
      printByte(regD);
      Serial.print(F(" to register A:"));
      printByte(regA);
#endif
      regA = regA + regD;
#ifdef LOG_MESSAGES
      Serial.print(F(" = "));
      printByte(regA);
#endif
      break;
    case B10000011:
#ifdef LOG_MESSAGES
      Serial.print(F(" > add, Add register E:"));
      printByte(regE);
      Serial.print(F(" to register A:"));
      printByte(regA);
#endif
      regA = regA + regE;
#ifdef LOG_MESSAGES
      Serial.print(F(" = "));
      printByte(regA);
#endif
      break;
    case B10000100:
#ifdef LOG_MESSAGES
      Serial.print(F(" > add, Add register H:"));
      printByte(regH);
      Serial.print(F(" to register A:"));
      printByte(regA);
#endif
      regA = regA + regH;
#ifdef LOG_MESSAGES
      Serial.print(F(" = "));
      printByte(regA);
#endif
      break;
    case B10000101:
#ifdef LOG_MESSAGES
      Serial.print(F(" > add, Add register L:"));
      printByte(regL);
      Serial.print(F(" to register A:"));
      printByte(regA);
#endif
      regA = regA + regL;
#ifdef LOG_MESSAGES
      Serial.print(F(" = "));
      printByte(regA);
#endif
      break;
    case B10000110:
      hlValue = regH * 256 + regL;
      workingByte = memoryData[hlValue];
#ifdef LOG_MESSAGES
      Serial.print(F(" > add, Add content at address H:L(M):"));
      printByte(workingByte);
      Serial.print(F(" to register A:"));
      printByte(regA);
#endif
      regA = regA + workingByte;
#ifdef LOG_MESSAGES
      Serial.print(F(" = "));
      printByte(regA);
#endif
      break;
    case B10000111:
#ifdef LOG_MESSAGES
      Serial.print(F(" > add, Add register A:"));
      printByte(regA);
      Serial.print(F(" to register A:"));
      printByte(regA);
#endif
      regA = regA + regA;
#ifdef LOG_MESSAGES
      Serial.print(F(" = "));
      printByte(regA);
#endif
      break;
    // ---------------------------------------------------------------------
    // ++ ADI #     11000110 db       ZSCPA   Add immediate to A
    case B11000110:
      opcode = B11000110;
#ifdef LOG_MESSAGES
      Serial.print(F(" > ani, Add immedite number to register A."));
#endif
      break;
    //-----------------------
    // ++ SUI #     11010110 db       ZSCPA   Subtract immediate from A
    case B11010110:
      opcode = B11010110;
#ifdef LOG_MESSAGES
      Serial.print(F(" > sui, Subtract immedite number from register A."));
#endif
      break;
    // ---------------------------------------------------------------------
    //ANI #     11100110 db       ZSPCA   AND immediate with A
    case B11100110:
      opcode = B11100110;
#ifdef LOG_MESSAGES
      Serial.print(F(" > ani, AND db with register A."));
#endif
      break;
    // ---------------------------------------------------------------------
    // CMP: Compare register to A, then set Carry and Zero bit flags.
    // If #=A, set Zero bit to 1. If #>A, Carry bit = 1. If #<A, Carry bit = 0.
    //    10111SSS
    case B10111000:
      workingByte = regB;
      flagZeroBit = workingByte == regA;
      flagCarryBit = workingByte > regA;
#ifdef LOG_MESSAGES
      displayCmp("B", regB);
#endif
      break;
    // ------------------------
    //    10111SSS
    case B10111001:
      workingByte = regC;
      flagZeroBit = workingByte == regA;
      flagCarryBit = workingByte > regA;
#ifdef LOG_MESSAGES
      displayCmp("C", regC);
#endif
      break;
    // ------------------------
    //    10111SSS
    case B10111010:
      workingByte = regD;
      flagZeroBit = workingByte == regA;
      flagCarryBit = workingByte > regA;
#ifdef LOG_MESSAGES
      displayCmp("D", regD);
#endif
      break;
    // ------------------------
    //    10111SSS
    case B10111011:
      workingByte = regE;
      flagZeroBit = workingByte == regA;
      flagCarryBit = workingByte > regA;
#ifdef LOG_MESSAGES
      displayCmp("E", regE);
#endif
      break;
    // ------------------------
    //    10111SSS
    case B10111100:
      workingByte = regH;
      flagZeroBit = workingByte == regA;
      flagCarryBit = workingByte > regA;
#ifdef LOG_MESSAGES
      displayCmp("H", regH);
#endif
      break;
    // ------------------------
    //    10111SSS
    case B10111101:
      workingByte = regL;
      flagZeroBit = workingByte == regA;
      flagCarryBit = workingByte > regA;
#ifdef LOG_MESSAGES
      displayCmp("L", regH);
#endif
      break;
    // ------------------------
    //    10111SSS  Register M, is memory data, H:L.
    case B10111110:
      hlValue = regH * 256 + regL;
      workingByte = memoryData[hlValue];
      flagZeroBit = workingByte == regA;
      flagCarryBit = workingByte > regA;
#ifdef LOG_MESSAGES
      displayCmp("M", regB);
#endif
      break;
    // ---------------------------------------------------------------------
    case B11111110:
      opcode = B11111110;
#ifdef LOG_MESSAGES
      Serial.print(F(" > cpi, compare next data byte to A, and set Zero bit and Carry bit flags."));
#endif
      break;
    // ---------------------------------------------------------------------
    // Stack opcodes.
    // stacy
    // -----------------
    // CALL a    11001101 lb hb   Unconditional subroutine call. 3 cycles.
    case B11001101:
      opcode = B11001101;
#ifdef LOG_MESSAGES
      Serial.print(F(" > call, call a subroutine."));
#endif
      break;
    // -----------------
    // RET  11001001  Unconditional return from subroutine. 1 cycles.
    case B11001001:
#ifdef LOG_MESSAGES
      Serial.print(F(" > ret, Return from a subroutine to the original CALL address: "));
#endif
      stackPointer++;
      highOrder = stackData[stackPointer];
      stackPointer++;
      lowOrder = stackData[stackPointer];
      programCounter = highOrder * 256 + lowOrder;
#ifdef LOG_MESSAGES
      Serial.print(programCounter);
      Serial.print(F(". stackPointer = "));
      Serial.print(stackPointer);
#endif
      break;
    // --------------------------------------------------------------------
    //    11RP0101 Push    register pair B onto the stack. 1 cycles.
    case B11000101:
#ifdef LOG_MESSAGES
      Serial.print(F(" > push, Push register pair B:C onto the stack."));
#endif
      stackData[stackPointer--] = regC;
      stackData[stackPointer--] = regB;
      break;
    case B11010101:
#ifdef LOG_MESSAGES
      Serial.print(F(" > push, Push register pair D:E onto the stack."));
#endif
      stackData[stackPointer--] = regE;
      stackData[stackPointer--] = regD;
      break;
    case B11100101:
#ifdef LOG_MESSAGES
      Serial.print(F(" > push, Push register pair H:L onto the stack."));
#endif
      stackData[stackPointer--] = regL;
      stackData[stackPointer--] = regH;
      break;
    case B11110101:
      Serial.print(F(" > push, Push flags is not implemented. Push the flags onto the stack."));
      printData(workingByte);
      Serial.println(F(""));
      Serial.print(F("- Error, at programCounter: "));
      printData(programCounter);
      Serial.println(F(""));
      ledFlashError();
      controlStopLogic();
      statusByte = HLTA_ON;
      digitalWrite(WAIT_PIN, HIGH);
      break;
    // --------------------------------------------------------------------
    //    11RP0001 Pop    register pair B from the stack. 1 cycles.
    case B11000001:
#ifdef LOG_MESSAGES
      Serial.print(F(" > pop, Pop register pair B:C from the stack."));
#endif
      stackPointer++;
      regB = stackData[stackPointer];
      stackPointer++;
      regC = stackData[stackPointer];
#ifdef LOG_MESSAGES
      Serial.print(F(" regB:regC "));
      Serial.print(regB);
      Serial.print(F(":"));
      Serial.print(regC);
#endif
      break;
    // ----------------------------------
    //    11RP0001
    case B11010001:
#ifdef LOG_MESSAGES
      Serial.print(F(" > pop, Pop register pair D:E from the stack."));
#endif
      stackPointer++;
      regD = stackData[stackPointer];
      stackPointer++;
      regE = stackData[stackPointer];
#ifdef LOG_MESSAGES
      Serial.print(F(" regD:regE "));
      Serial.print(regD);
      Serial.print(F(":"));
      Serial.print(regE);
#endif
    // -----------------
    //    11RP0001
    case B11100001:
#ifdef LOG_MESSAGES
      Serial.print(F(" > pop, Pop register pair H:L from the stack."));
#endif
      stackPointer++;
      regH = stackData[stackPointer];
      stackPointer++;
      regL = stackData[stackPointer];
#ifdef LOG_MESSAGES
      Serial.print(F(" regH:regL "));
      Serial.print(regH);
      Serial.print(F(":"));
      Serial.print(regL);
#endif
      break;
    case B11110001:
      Serial.print(F(" > pop, Pop flags is not implemented. Pop the flags from the stack."));
      printData(workingByte);
      Serial.println(F(""));
      Serial.print(F("- Error, at programCounter: "));
      printData(programCounter);
      Serial.println(F(""));
      ledFlashError();
      controlStopLogic();
      statusByte = HLTA_ON;
      digitalWrite(WAIT_PIN, HIGH);
      break;
    // ---------------------------------------------------------------------
    // dad RP   00RP1001  Add register pair(RP) to H:L (16 bit add). And set carry bit.
    // ----------
    //    00RP1001
    case B00001001:
      // dad b  : Add B:C to H:L (16 bit add). Set carry bit.
      bValue = regB;
      cValue = regC;
      hValue = regH;
      lValue = regL;
#ifdef LOG_MESSAGES
      Serial.print(F(" > dad, 16 bit add: B:C("));
      Serial.print(bValue);
      Serial.print(":");
      Serial.print(cValue);
      Serial.print(")");
      Serial.print(F(" + H:L("));
      Serial.print(hValue);
      Serial.print(":");
      Serial.print(lValue);
      Serial.print(")");
#endif
      bcValue = bValue * 256 + cValue;
      hlValue = hValue * 256 + lValue;
      hlValueNew = bcValue + hlValue;
      if (hlValueNew < bcValue || hlValueNew < hlValue) {
        flagCarryBit = true;
      } else {
        flagCarryBit = false;
      }
      regH = highByte(hlValueNew);
      regL = lowByte(hlValueNew);
#ifdef LOG_MESSAGES
      Serial.print(F(" = "));
      Serial.print(bcValue);
      Serial.print(F(" + "));
      Serial.print(hlValue);
      Serial.print(F(" = "));
      Serial.print(hlValueNew);
      Serial.print("(");
      Serial.print(regH);
      Serial.print(":");
      Serial.print(regL);
      Serial.print(")");
      Serial.print(F(", Carry bit: "));
      if (flagCarryBit) {
        Serial.print(F("true. Sum over: 65535."));
      } else {
        Serial.print(F("false. Sum <= 65535."));
      }
#endif
      break;
    // ----------
    //    00RP1001
    case B00011001:
      // dad d  : Add D:E to H:L (16 bit add). Set carry bit.
      dValue = regD;
      eValue = regE;
      hValue = regH;
      lValue = regL;
#ifdef LOG_MESSAGES
      Serial.print(F(" > dad, 16 bit add: D:E"));
      Serial.print("(");
      Serial.print(dValue);
      Serial.print(":");
      Serial.print(eValue);
      Serial.print(")");
      Serial.print(F(" + H:L "));
      Serial.print("(");
      Serial.print(hValue);
      Serial.print(":");
      Serial.print(lValue);
      Serial.print(")");
#endif
      deValue = dValue * 256 + eValue;
      hlValue = hValue * 256 + lValue;
      hlValueNew = deValue + hlValue;
      if (hlValueNew < deValue || hlValueNew < hlValue) {
        flagCarryBit = true;
      } else {
        flagCarryBit = false;
      }
      regH = highByte(hlValueNew);
      regL = lowByte(hlValueNew);
#ifdef LOG_MESSAGES
      Serial.print(F(" = "));
      Serial.print(deValue);
      Serial.print(F(" + "));
      Serial.print(hlValue);
      Serial.print(F(" = "));
      Serial.print(hlValueNew);
      Serial.print("(");
      Serial.print(regH);
      Serial.print(":");
      Serial.print(regL);
      Serial.print(")");
      if (flagCarryBit) {
        Serial.print(F("true. Sum over: 65535."));
      } else {
        Serial.print(F("false. Sum <= 65535."));
      }
#endif
      break;
    // ---------------------------------------------------------------------
    // dcr d : decrement a register. To do: update the flags.
    //    00DDD101    Flags:ZSPA
    case B00111101:
#ifdef LOG_MESSAGES
      Serial.print(F(" > dcr register A: "));
      Serial.print(regA);
#endif
      regA--;
      if (regA == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("-- = ");
      Serial.print(regA);
#endif
      break;
    case B00000101:
#ifdef LOG_MESSAGES
      Serial.print(F(" > dcr register B: "));
      Serial.print(regB);
#endif
      regB--;
      if (regB == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("-- = ");
      Serial.print(regB);
#endif
      break;
    case B00001101:
#ifdef LOG_MESSAGES
      Serial.print(F(" > dcr register C: "));
      Serial.print(regC);
#endif
      regC--;
      if (regC == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("-- = ");
      Serial.print(regC);
#endif
      break;
    case B00010101:
#ifdef LOG_MESSAGES
      Serial.print(F(" > dcr register D: "));
      Serial.print(regD);
#endif
      regD--;
      if (regD == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("-- = ");
      Serial.print(regD);
#endif
      break;
    case B00011101:
#ifdef LOG_MESSAGES
      Serial.print(F(" > dcr register E: "));
      Serial.print(regE);
#endif
      regE--;
      if (regE == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("-- = ");
      Serial.print(regE);
#endif
      break;
    case B00100101:
#ifdef LOG_MESSAGES
      Serial.print(F(" > dcr register H: "));
      Serial.print(regH);
#endif
      regH--;
      if (regH == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("-- = ");
      Serial.print(regH);
#endif
      break;
    case B00101101:
#ifdef LOG_MESSAGES
      Serial.print(F(" > dcr register L: "));
      Serial.print(regL);
#endif
      regL--;
      if (regL == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("-- = ");
      Serial.print(regL);
#endif
      break;
    case B00110101:
#ifdef LOG_MESSAGES
      Serial.print(F(" > dcr M, memory address, register pair H:L: "));
      Serial.print(regH);
      Serial.print(":");
      Serial.print(regL);
#endif
      regL--;
      if (regL == 0) {
        regH--;
        regL = 255;
      }
#ifdef LOG_MESSAGES
      Serial.print("-- = ");
      Serial.print(regH);
      Serial.print(":");
      Serial.print(regL);
#endif
      break;
    // ---------------------------------------------------------------------
    case B01110110:
#ifdef LOG_MESSAGES
      Serial.print(F("+ HLT, program halted."));
#else
      Serial.println(F("\n+ HLT, program halted."));
#endif
      controlStopLogic();
      statusByte = HLTA_ON;
      digitalWrite(WAIT_PIN, HIGH);
      break;
    case B11011011:
      opcode = B11011011;
      // INP status light is on when reading from an input port.
      statusByte = statusByte | INP_ON;
#ifdef LOG_MESSAGES
      Serial.print(F(" > IN, If input value is available, get the input byte."));
#endif
      break;
    // ---------------------------------------------------------------------
    // inr d : increment a register. Stacy, to do: update the flags. See CMP for example.
    //    00DDD100    Flags:ZSPA
    case B00111100:
#ifdef LOG_MESSAGES
      Serial.print(F(" > inr register A: "));
      Serial.print(regA);
#endif
      regA++;
      if (regA == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("++ = ");
      Serial.print(regA);
#endif
      break;
    case B00000100:
#ifdef LOG_MESSAGES
      Serial.print(F(" > inr register B: "));
      Serial.print(regB);
#endif
      regB++;
      if (regB == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("++ = ");
      Serial.print(regB);
#endif
      break;
    case B00001100:
#ifdef LOG_MESSAGES
      Serial.print(F(" > inr register C: "));
      Serial.print(regC);
#endif
      regC++;
      if (regC == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("++ = ");
      Serial.print(regC);
#endif
      break;
    case B00010100:
#ifdef LOG_MESSAGES
      Serial.print(F(" > inr register D: "));
      Serial.print(regD);
#endif
      regD++;
      if (regD == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("++ = ");
      Serial.print(regD);
#endif
      break;
    case B00011100:
#ifdef LOG_MESSAGES
      Serial.print(F(" > inr register E: "));
      Serial.print(regE);
#endif
      regE++;
      if (regE == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("++ = ");
      Serial.print(regE);
#endif
      break;
    case B00100100:
#ifdef LOG_MESSAGES
      Serial.print(F(" > inr register H: "));
      Serial.print(regH);
#endif
      regH++;
      if (regH == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("++ = ");
      Serial.print(regH);
#endif
      break;
    case B00101100:
#ifdef LOG_MESSAGES
      Serial.print(F(" > inr register L: "));
      Serial.print(regL);
#endif
      regL++;
      if (regL == 0) {
        // Set flag.
      }
#ifdef LOG_MESSAGES
      Serial.print("++ = ");
      Serial.print(regL);
#endif
      break;
    case B00110100:
      // stacy
#ifdef LOG_MESSAGES
      Serial.print(F(" > inr address M (H:L): "));
      Serial.print(regH);
      Serial.print(":");
      Serial.print(regL);
#endif
      regL++;
      if (regL == 0) {
        regH++;
      }
#ifdef LOG_MESSAGES
      Serial.print("++ = ");
      Serial.print(regH);
      Serial.print(":");
      Serial.print(regL);
#endif
      break;
    // ---------------------------------------------------------------------
    // inx : increment a register pair.
    // ------------
    case B00000011:
      // inx b
      regC++;
      if (regC == 0) {
        regB++;
      }
#ifdef LOG_MESSAGES
      Serial.print(F(" > inx, increment the 16 bit register pair B:C."));
      Serial.print(F(", B:C = "));
      Serial.print(regB);
      Serial.print(":");
      Serial.print(regC);
      Serial.print(" = ");
      printOctal(regB);
      Serial.print(":");
      printOctal(regC);
#endif
      break;
    // ------------
    case B00010011:
      // inx d
      regE++;
      if (regE == 0) {
        regD++;
      }
#ifdef LOG_MESSAGES
      Serial.print(F(" > inx, increment the 16 bit register pair D:E."));
      Serial.print(F(", regD:regE = "));
      Serial.print(regD);
      Serial.print(":");
      Serial.print(regE);
      Serial.print(" = ");
      printOctal(regD);
      Serial.print(":");
      printOctal(regE);
#endif
      break;
    // ------------
    case B00100011:
      // inx h (h:l)
      regL++;
      if (regL == 0) {
        regH++;
      }
#ifdef LOG_MESSAGES
      Serial.print(F(" > inx, increment the 16 bit register pair H:L."));
      Serial.print(F(", regH:regL = "));
      Serial.print(regH);
      Serial.print(":");
      Serial.print(regL);
      Serial.print(" = ");
      printOctal(regH);
      Serial.print(":");
      printOctal(regL);
#endif
      break;
    // ---------------------------------------------------------------------
    // Jump options.

    // ----------------
    //    110CC010
    case B11000010:
      opcode = B11000010;
#ifdef LOG_MESSAGES
      Serial.print(F(" > jnz, Jump if zero bit flag is not set(false)."));
#endif
      break;
    // ----------------
    //    110CC010
    case B11001010:
      opcode = B11001010;
#ifdef LOG_MESSAGES
      Serial.print(F(" > jz, Jump if Zero bit flag is set(true)."));
#endif
      break;
    // ----------------
    //    110CC010
    case B11010010:
      opcode = B11010010;
#ifdef LOG_MESSAGES
      Serial.print(F(" > jnc, Jump if carry bit is not set(false)."));
#endif
      break;
    // ----------------
    //    110CC010
    case B11011010:
      opcode = B11011010;
#ifdef LOG_MESSAGES
      Serial.print(F(" > jc, Jump if carry bit is set(true)."));
#endif
      break;
    // ----------------
    case B11000011:
      opcode = B11000011;
#ifdef LOG_MESSAGES
      Serial.print(F(" > jmp, get address low and high order bytes."));
#endif
      break;
    // ---------------------------------------------------------------------
    // ldax RP  00RP1010 - Load indirect through BC(RP=00) or DE(RP=01)
    // ---------------
    case B00001010:
      opcode = B00001010;
#ifdef LOG_MESSAGES
      Serial.print(F(" > ldax, Into register A, load data from B:C address: "));
      printOctal(regB);
      Serial.print(F(" : "));
      printOctal(regC);
#endif
      break;
    // ---------------
    case B00011010:
      opcode = B00011010;
#ifdef LOG_MESSAGES
      Serial.print(F(" > ldax, Into register A, load data from D:E address: "));
      printOctal(regD);
      Serial.print(F(" : "));
      printOctal(regE);
#endif
      break;
    // ---------------------------------------------------------------------
    // lxi                                Load register pair immediate
    // lxi RP,a 00RP0001                  Move a(hb:lb) into register pair RP, example, B:C = hb:lb.
    case B00000001:
      opcode = B00000001;
#ifdef LOG_MESSAGES
      Serial.print(F(" > lxi, Move the lb hb data, into register pair B:C = hb:lb."));
#endif
      break;
    case B00010001:
      opcode = B00010001;
#ifdef LOG_MESSAGES
      Serial.print(F(" > lxi, Move the lb hb data, into register pair D:E = hb:lb."));
#endif
      break;
    case B00100001:
      opcode = B00100001;
#ifdef LOG_MESSAGES
      Serial.print(F(" > lxi, Move the lb hb data, into register pair H:L = hb:lb."));
#endif
      break;
    case B00110001:
      // stacy
      // opcode = B00110001;
#ifdef LOG_MESSAGES
      Serial.print(F(" > lxi, Stacy, to do: move the lb hb data, to the stack pointer address."));
#endif
      Serial.print(F(" - Error, unhandled instruction."));
      ledFlashError();
      controlStopLogic();
      statusByte = HLTA_ON;
      digitalWrite(WAIT_PIN, HIGH);
      break;
    // ------------------------------------------------------------------------------------------
    /*
      B01DDDSSS         // mov d,S  ; Move from one register to another.
    */
    // ---------------------------------------------------------------------
    case B01111110:
      // B01DDDSSS
      anAddress = word(regH, regL);
      regA = memoryData[anAddress];
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov"));
      Serial.print(F(", Move data from Address: "));
      sprintf(charBuffer, "%4d:", anAddress);
      Serial.print(charBuffer);
      Serial.print(F(", to Accumulator = "));
      printData(regA);
#endif
      break;
    case B01000110:
      // B01DDDSSS
      anAddress = word(regH, regL);
      regB = memoryData[anAddress];
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov"));
      Serial.print(F(", Move data from Address: "));
      sprintf(charBuffer, "%4d:", anAddress);
      Serial.print(charBuffer);
      Serial.print(F(", to Accumulator = "));
      printData(regA);
#endif
      break;
    case B01001110:
      // B01DDDSSS
      anAddress = word(regH, regL);
      regC = memoryData[anAddress];
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov"));
      Serial.print(F(", Move data from Address: "));
      sprintf(charBuffer, "%4d:", anAddress);
      Serial.print(charBuffer);
      Serial.print(F(", to Accumulator = "));
      printData(regA);
#endif
      break;
    case B01010110:
      // B01DDDSSS
      anAddress = word(regH, regL);
      regD = memoryData[anAddress];
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov"));
      Serial.print(F(", Move data from Address: "));
      sprintf(charBuffer, "%4d:", anAddress);
      Serial.print(charBuffer);
      Serial.print(F(", to Accumulator = "));
      printData(regA);
#endif
      break;
    case B01011110:
      // B01DDDSSS
      anAddress = word(regH, regL);
      regE = memoryData[anAddress];
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov"));
      Serial.print(F(", Move data from Address: "));
      sprintf(charBuffer, "%4d:", anAddress);
      Serial.print(charBuffer);
      Serial.print(F(", to Accumulator = "));
      printData(regA);
#endif
      break;
    case B01100110:
      // B01DDDSSS
      anAddress = word(regH, regL);
      regH = memoryData[anAddress];
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov"));
      Serial.print(F(", Move data from Address: "));
      sprintf(charBuffer, "%4d:", anAddress);
      Serial.print(charBuffer);
      Serial.print(F(", to Accumulator = "));
      printData(regA);
#endif
      break;
    case B01101110:
      // B01DDDSSS
      anAddress = word(regH, regL);
      regL = memoryData[anAddress];
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov"));
      Serial.print(F(", Move data from Address: "));
      sprintf(charBuffer, "%4d:", anAddress);
      Serial.print(charBuffer);
      Serial.print(F(", to Accumulator = "));
      printData(regA);
#endif
      break;
    case B01000111:
      regB = regA;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register a to b = "));
      printData(regB);
#endif
      break;
    case B01001111:
      regC = regA;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register a to c = "));
      printData(regC);
#endif
      break;
    case B01010111:
      regD = regA;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register a to d = "));
      printData(regD);
#endif
      break;
    case B01011111:
      regE = regA;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register a to e = "));
      printData(regE);
#endif
      break;
    case B01100111:
      regH = regA;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register a to h = "));
      printData(regH);
#endif
      break;
    case B01101111:
      regL = regA;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register a to l = "));
      printData(regL);
#endif
      break;
    // ---------------------------------------------------------------------
    case B01111000:
      regA = regB;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register b to a = "));
      printData(regL);
#endif
      break;
    case B01001000:
      regC = regB;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register b to c = "));
      printData(regL);
#endif
      break;
    case B01010000:
      regD = regB;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register b to d = "));
      printData(regD);
#endif
      break;
    case B01011000:
      regE = regB;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register b to e = "));
      printData(regE);
#endif
      break;
    case B01100000:
      regH = regB;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register b to h = "));
      printData(regH);
#endif
      break;
    case B01101000:
      regL = regB;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register b to l = "));
      printData(regL);
#endif
      break;
    // ---------------------------------------------------------------------
    case B01111001:
      regA = regC;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register c to a = "));
      printData(regA);
#endif
      break;
    case B01000001:
      regB = regC;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register c to b = "));
      printData(regB);
#endif
      break;
    case B01010001:
      regD = regC;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register c to d = "));
      printData(regD);
#endif
      break;
    case B01011001:
      regE = regC;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register c to e = "));
      printData(regE);
#endif
      break;
    case B01100001:
      regH = regC;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register c to h = "));
      printData(regH);
#endif
      break;
    case B01101001:
      regL = regC;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register c to l = "));
      printData(regL);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B01111010:
      regA = regD;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register d to a = "));
      printData(regA);
#endif
      break;
    case B01000010:
      regB = regD;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register d to b = "));
      printData(regB);
#endif
      break;
    case B01001010:
      regC = regD;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register d to c = "));
      printData(regC);
#endif
      break;
    case B01011010:
      regE = regD;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register d to e = "));
      printData(regE);
#endif
      break;
    case B01100010:
      regH = regD;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register d to h = "));
      printData(regH);
#endif
      break;
    case B01101010:
      regL = regD;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register d to l = "));
      printData(regL);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B01111011:
      regA = regE;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register e to a = "));
      printData(regA);
#endif
      break;
    case B01000011:
      regB = regE;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register e to b = "));
      printData(regB);
#endif
      break;
    case B01001011:
      regC = regE;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register e to c = "));
      printData(regC);
#endif
      break;
    case B01010011:
      regD = regE;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register e to d = "));
      printData(regD);
#endif
      break;
    case B01100011:
      regH = regE;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register e to h = "));
      printData(regH);
#endif
      break;
    case B01101011:
      regL = regE;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register e to l = "));
      printData(regL);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B01111100:
      regA = regH;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register h to a = "));
      printData(regA);
#endif
      break;
    case B01000100:
      regB = regH;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register h to b = "));
      printData(regB);
#endif
      break;
    case B01001100:
      regC = regH;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register h to c = "));
      printData(regC);
#endif
      break;
    case B01010100:
      regD = regH;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register h to d = "));
      printData(regD);
#endif
      break;
    case B01011100:
      regE = regH;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register h to e = "));
      printData(regE);
#endif
      break;
    case B01101100:
      regL = regH;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register h to l = "));
      printData(regL);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B01111101:
      regA = regL;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register l to a = "));
      printData(regA);
#endif
      break;
    case B01000101:
      regB = regL;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register l to b = "));
      printData(regB);
#endif
      break;
    case B01001101:
      regC = regL;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register l to c = "));
      printData(regC);
#endif
      break;
    case B01010101:
      regD = regL;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register l to d = "));
      printData(regD);
#endif
      break;
    case B01011101:
      regE = regL;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register l to e = "));
      printData(regE);
#endif
      break;
    case B01100101:
      regH = regL;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mov register l to h = "));
      printData(regH);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    // mvi R,#  00 RRR 110  Move a number (#), which is the next db, to register RRR.
    // mvi a,#  00 111 110  0036
    // mvi b,#  00 000 110  0006
    // mvi c,#  00 001 110  0016
    // mvi d,#  00 010 110  0026
    // mvi e,#  00 011 110  0036
    // mvi h,#  00 100 110  0046
    // mvi l,#  00 101 110  0056
    case B00111110:
      opcode = B00111110;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mvi, move db address into register A."));
#endif
      break;
    case B00000110:
      opcode = B00000110;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mvi, move db address into register B."));
#endif
      break;
    case B00001110:
      opcode = B00001110;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mvi, move db address into register C."));
#endif
      break;
    case B00010110:
      opcode = B00010110;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mvi, move db address into register D."));
#endif
      break;
    case B00011110:
      opcode = B00011110;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mvi, move db address into register E."));
#endif
      break;
    case B00100110:
      opcode = B00100110;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mvi, move db address into register H."));
#endif
      break;
    case B00101110:
      opcode = B00101110;
#ifdef LOG_MESSAGES
      Serial.print(F(" > mvi, move db address into register L."));
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B00000000:
#ifdef LOG_MESSAGES
      Serial.print(F(" > nop ---------------------------"));
#endif
      delay(100);
      break;
    // ------------------------------------------------------------------------------------------
    // ORA10110SSS
    case B10110000:
#ifdef LOG_MESSAGES
      Serial.print(F(" > ora, OR register B: "));
      printByte(regB);
      Serial.print(F(", with register A: "));
      printByte(regA);
      Serial.print(F(" = "));
#endif
      regA = regA | regB;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ---------------------
    // ORA10110SSS
    case B10110001:
#ifdef LOG_MESSAGES
      Serial.print(F(" > ora, OR register C: "));
      printByte(regC);
      Serial.print(F(", with register A: "));
      printByte(regA);
      Serial.print(F(" = "));
#endif
      regA = regA | regC;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ---------------------
    // ORA10110SSS
    case B10110010:
#ifdef LOG_MESSAGES
      Serial.print(F(" > ora, OR register D: "));
      printByte(regD);
      Serial.print(F(", with register A: "));
      printByte(regC);
      Serial.print(F(" = "));
#endif
      regA = regA | regD;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ---------------------
    // ORA10110SSS
    case B10110011:
#ifdef LOG_MESSAGES
      Serial.print(F(" > ora, OR register E: "));
      printByte(regE);
      Serial.print(F(", with register A: "));
      printByte(regA);
      Serial.print(F(" = "));
#endif
      regA = regA | regE;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ---------------------
    // ORA10110SSS
    case B10110100:
#ifdef LOG_MESSAGES
      Serial.print(F(" > ora, OR register H: "));
      printByte(regH);
      Serial.print(F(", with register A: "));
      printByte(regA);
      Serial.print(F(" = "));
#endif
      regA = regA | regH;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ---------------------
    // ORA10110SSS
    case B10110101:
#ifdef LOG_MESSAGES
      Serial.print(F(" > ora, OR register L: "));
      printByte(regL);
      Serial.print(F(", with register A: "));
      printByte(regA);
      Serial.print(F(" = "));
#endif
      regA = regA | regL;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ---------------------
    // ORA10110SSS
    case B10110110:
      hlValue = regH * 256 + regL;
      workingByte = memoryData[hlValue];
#ifdef LOG_MESSAGES
      Serial.print(F(" > ora, OR data from address register M(H:L): "));
      printByte(workingByte);
      Serial.print(F(", with register A: "));
      printByte(regA);
      Serial.print(F(" = "));
#endif
      regA = regA | workingByte;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B11100011:
      opcode = B11100011;
      statusByte = statusByte & WO_OFF;  // Inverse logic: off writing out. On when not.
#ifdef LOG_MESSAGES
      Serial.print(F(" > OUT, Write output to the port address(following db)."));
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B00000111:
#ifdef LOG_MESSAGES
      Serial.print(F(" > rlc"));
      Serial.print(F(", Shift register A: "));
      printByte(regA);
      Serial.print(F(", left 1 bit: "));
#endif
      // # 0x07  1  CY       RLC        A = A << 1; bit 0 = prev bit 7; CY = prev bit 7
      // Get bit 0, and use it to set bit 7, after the shift.
      aByteBit = regA & B10000000;
      regA = regA << 1;
      if (aByteBit > 0) {
        regA = regA | B00000001;
      }
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B00001111:
#ifdef LOG_MESSAGES
      Serial.print(F(" > rrc"));
      Serial.print(F(", Shift register A: "));
      printByte(regA);
      Serial.print(F(", right 1 bit: "));
#endif
      // # 0x0f RRC 1 CY  A = A >> 1; bit 7 = prev bit 0; CY = prev bit 0
      // Get bit 7, and use it to set bit 0, after the shift.
      aByteBit = regA & B00000001;
      regA = regA >> 1;
      if (aByteBit == 1) {
        regA = regA | B10000000;
      }
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    // shld a    00100010 lb hb    -  Store register L to memory address hb:lb. Store register H to hb:lb + 1.
    // The contents of register L, are stored in the memory address specified in bytes lb and hb (hb:lb).
    // The contents of register H, are stored in the memory at the next higher address (hb:lb + 1).
    case B00100010:
      opcode = B00100010;
#ifdef LOG_MESSAGES
      Serial.print(F(" > shld, Store register L to memory address hb:lb. Store register H to hb:lb + 1."));
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B00110010:
      opcode = B00110010;
#ifdef LOG_MESSAGES
      Serial.print(F(" > sta, Store register A to the hb:lb address."));
#endif
      break;
    case B00111010:
      opcode = B00111010;
#ifdef LOG_MESSAGES
      Serial.print(F(" > lda, Load register A with data from the hb:lb address."));
#endif
      break;
    // ------------------------------------------------------------------------------------------
    // XRA S     10101SSS          ZSPCA   ExclusiveOR register with A.
    //                             ZSPCA   Stacy, need to set flags.
    // ---------------
    case B10101000:
      // xra b
#ifdef LOG_MESSAGES
      Serial.print(F(" > xra"));
      Serial.print(F(", Register B: "));
      printByte(regB);
      Serial.print(F(", Exclusive OR with register A: "));
      printByte(regA);
      Serial.print(F(" = "));
      // If one but not both of the variables has a value of one, the bit will be one, else zero.
#endif
      regA = regB ^ regA;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ---------------
    case B10101001:
      // xra c
#ifdef LOG_MESSAGES
      Serial.print(F(" > xra"));
      Serial.print(F(", Register C: "));
      printByte(regC);
      Serial.print(F(", Exclusive OR with register A: "));
      printByte(regA);
      Serial.print(F(" = "));
#endif
      regA = regC ^ regA;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ---------------
    case B10101010:
      // xra d
#ifdef LOG_MESSAGES
      Serial.print(F(" > xra"));
      Serial.print(F(", Register D: "));
      printByte(regD);
      Serial.print(F(", Exclusive OR with register A: "));
      printByte(regA);
      Serial.print(F(" = "));
#endif
      regA = regD ^ regA;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ---------------
    case B10101011:
      // xra e
#ifdef LOG_MESSAGES
      Serial.print(F(" > xra"));
      Serial.print(F(", Register E: "));
      printByte(regE);
      Serial.print(F(", Exclusive OR with register A: "));
      printByte(regA);
      Serial.print(F(" = "));
#endif
      regA = regE ^ regA;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ---------------
    case B10101100:
      // xra h
#ifdef LOG_MESSAGES
      Serial.print(F(" > xra"));
      Serial.print(F(", Register H: "));
      printByte(regH);
      Serial.print(F(", Exclusive OR with register A: "));
      printByte(regA);
      Serial.print(F(" = "));
#endif
      regA = regH ^ regA;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ---------------
    case B10101101:
      // xra l
#ifdef LOG_MESSAGES
      Serial.print(F(" > xra"));
      Serial.print(F(", Register L: "));
      printByte(regL);
      Serial.print(F(", Exclusive OR with register A: "));
      printByte(regA);
      Serial.print(F(" = "));
#endif
      regA = regL ^ regA;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // -------------------------------------------------------------------------------------
    default:
#ifdef LOG_MESSAGES
      Serial.print(F(" - Error, unknown opcode instruction."));
#else
      Serial.print(F("- Error, unknown opcode instruction: "));
      printData(workingByte);
      Serial.println(F(""));
      Serial.print(F("- Error, at programCounter: "));
      printData(programCounter);
      Serial.println(F(""));
#endif
      ledFlashError();
      controlStopLogic();
      statusByte = HLTA_ON;
      digitalWrite(WAIT_PIN, HIGH);
  }
}

// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
void processOpcodeData() {

  // Process data types:
  // 1) Immediate data byte:
  // + Instruction cycle 1 (M1 above), get the opcode in processOpcode().
  // + Instruction cycle 2, get the immediate data byte and process it.
  // 2) Address bytes:
  // + Instruction cycle 1 (M1 above), get the opcode in processOpcode().
  // + Instruction cycle 2, get the lower data byte (lb: lowByte).
  // + Instruction cycle 3, get the higher data byte (hb: lowByte), and process the address, hb:lb.
  // + Instruction cycle 4, read/write the data and display the byte in the data LED lights.

  // Note,
  //    if not jumping, increment programCounter.
  //    if jumping, programCounter is set to an address, don't increment it.

  switch (opcode) {

    // ---------------------------------------------------------------------
    //ADI #     11000110 db   ZSCPA Add immediate number to register A.
    case B11000110:
      // instructionCycle == 2

#ifdef LOG_MESSAGES
      Serial.print(F(" > adi, Add immediate number:"));
      printByte(dataByte);
      Serial.print(F(" to register A:"));
      printByte(regA);
#endif
      regA = regA + dataByte;
#ifdef LOG_MESSAGES
      Serial.print(F(" = "));
      printByte(regA);
#endif
      programCounter++;
      break;
    // ---------------------------------------------------------------------
    //SUI #     11010110 db   ZSCPA Subtract immediate number from register A.
    case B11010110:
      // instructionCycle == 2
#ifdef LOG_MESSAGES
      Serial.print(F(" > adi, Subtract immediate number:"));
      printByte(dataByte);
      Serial.print(F(" from register A:"));
      printByte(regA);
#endif
      regA = regA - dataByte;
#ifdef LOG_MESSAGES
      Serial.print(F(" = "));
      printByte(regA);
#endif
      programCounter++;
      break;
    // ---------------------------------------------------------------------
    // ---------------------------------------------------------------------
    //ANI #     11100110 db       ZSPCA   AND immediate with A
    case B11100110:
      // instructionCycle == 2
#ifdef LOG_MESSAGES
      Serial.print(F(" > ani, AND db:"));
      printByte(dataByte);
      Serial.print(F(" with register A:"));
      printByte(regA);
#endif
      regA = regA & dataByte;
#ifdef LOG_MESSAGES
      Serial.print(F(" = "));
      printByte(regA);
#endif
      programCounter++;
      break;
    // ---------------------------------------------------------------------
    // CALL a    11001101 lb hb   Unconditional subroutine call. 3 cycles.
    // stacy
    case B11001101:
      if (instructionCycle == 2) {
        lowOrder = dataByte;
#ifdef LOG_MESSAGES
        Serial.print(F("< call, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 3
      highOrder = dataByte;
#ifdef LOG_MESSAGES
      Serial.print(F("< call, hb: "));
      sprintf(charBuffer, "%4d:", highOrder);
      Serial.print(charBuffer);
#endif
      // instructionCycle == 3
#ifdef LOG_MESSAGES
      Serial.print(F(" > call, jumping from address: "));
      Serial.print(programCounter);
#endif
      // Push the next memory location onto the stack. This will be used by the RET opcode.
      stackData[stackPointer--] = lowByte(programCounter);
      stackData[stackPointer--] = highByte(programCounter);
      // Jump to the subroutine.
      programCounter = highOrder * 256 + lowOrder;
#ifdef LOG_MESSAGES
      Serial.print(F(", to the subroutine address: "));
      Serial.print(programCounter);
      Serial.print(F(". stackPointer = "));
      Serial.print(stackPointer);
      /*
        Serial.print(F(", stackData[stackPointer+1] = "));
        Serial.print(stackData[stackPointer+1]);
        Serial.print(F(", stackData[stackPointer+2] = "));
        Serial.print(stackData[stackPointer+2]);
      */
#endif
      break;
    // ---------------------------------------------------------------------
    case B11111110:
      // instructionCycle == 2
      // Compare #(dataByte) to A, then set Carry and Zero bit flags.
      // If #=A, set Zero bit to 1. If #>A, Carry bit = 1. If #<A, Carry bit = 0.
      flagZeroBit = dataByte == regA;
      flagCarryBit = dataByte > regA;
#ifdef LOG_MESSAGES
      Serial.print(F(" > cpi, compare the result db: "));
      Serial.print(dataByte);
      if (flagZeroBit) {
        Serial.print(F(" == "));
      } else {
        if (flagCarryBit) {
          Serial.print(F(" > "));
        } else {
          Serial.print(F(" < "));
        }
      }
      Serial.print(F(" A: "));
      Serial.print(regA);
#endif
      programCounter++;
      break;
    // ---------------------------------------------------------------------
    case B11011011:
      // stacy
      // instructionCycle == 2
      // INP & WO are on when reading from an input port.
      // IN p      11011011 pa       -       Read input for port a, into A
      //                    pa = 0ffh        Check toggle sense switches for non-zero input.
      //                         0ffh = B11111111 = 255
      //
#ifdef LOG_MESSAGES
      Serial.print(F("< IN, input port: "));
      Serial.print(dataByte);
      Serial.print(F("."));
#endif
      if (dataByte == 255) {
        regA = toggleSense();
      } else if (dataByte == 1) {
        regA = hwStatus;
      } else if (dataByte == 0) {
        regA = 0; // Input not implemented on this port.
      } else {
        // stacy1
#ifdef LOG_MESSAGES
        Serial.print(F(" Error, IN not implemented on this port."));
#endif
        ledFlashError();
        controlStopLogic();
        statusByte = HLTA_ON;
        digitalWrite(WAIT_PIN, HIGH);
      }
#ifdef LOG_MESSAGES
      Serial.print(F(" Register A = "));
      Serial.print(regA);
      Serial.print(F(" = "));
      printByte(regA);
      Serial.print(F("."));
#endif
      statusByte = statusByte & INP_OFF;
      programCounter++;
      break;
    // ---------------------------------------------------------------------
    // Jump opcodes

    //-----------------------------------------
    case B11000011:
      if (instructionCycle == 2) {
        lowOrder = dataByte;
#ifdef LOG_MESSAGES
        Serial.print(F(" > jmp, lb:"));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 3
      highOrder = dataByte;
      programCounter = word(highOrder, lowOrder);
#ifdef LOG_MESSAGES
      Serial.print(F(" > jmp, hb:"));
      sprintf(charBuffer, "%4d:", highOrder);
      Serial.print(charBuffer);
      Serial.print(F(", jmp, jump to:"));
      sprintf(charBuffer, "%4d = ", programCounter);
      Serial.print(charBuffer);
      printByte((byte)programCounter);
#endif
      break;
    //-----------------------------------------
    case B11000010:
      if (instructionCycle == 2) {
        lowOrder = dataByte;
#ifdef LOG_MESSAGES
        Serial.print(F("< jnz, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 3
      highOrder = dataByte;
#ifdef LOG_MESSAGES
      Serial.print(F("< jnz, hb: "));
      sprintf(charBuffer, "%4d:", highOrder);
      Serial.print(charBuffer);
#endif
      if (!flagZeroBit) {
        programCounter = word(highOrder, lowOrder);
#ifdef LOG_MESSAGES
        Serial.print(F(" > jnz, Zero bit flag is false, jump to:"));
        Serial.print(programCounter);
#endif
      } else {
#ifdef LOG_MESSAGES
        Serial.print(F(" - Carry Zero flag is true, don't jump."));
#endif
        programCounter++;
      }
      break;
    //-----------------------------------------
    case B11001010:
      if (instructionCycle == 2) {
        lowOrder = dataByte;
#ifdef LOG_MESSAGES
        Serial.print(F("< jz, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 3
      highOrder = dataByte;
#ifdef LOG_MESSAGES
      Serial.print(F("< jz, hb: "));
      sprintf(charBuffer, "%4d:", highOrder);
      Serial.print(charBuffer);
#endif
      if (flagZeroBit) {
        programCounter = word(highOrder, lowOrder);
#ifdef LOG_MESSAGES
        Serial.print(F(" > jz, jump to:"));
        Serial.print(programCounter);
#endif
      } else {
#ifdef LOG_MESSAGES
        Serial.print(F(" - Zero bit flag is false, don't jump."));
#endif
        programCounter++;
      }
      break;
    //-----------------------------------------
    case B11010010:
      if (instructionCycle == 2) {
        lowOrder = dataByte;
#ifdef LOG_MESSAGES
        Serial.print(F("< jnc, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 3
      highOrder = dataByte;
#ifdef LOG_MESSAGES
      Serial.print(F("< jnc, hb: "));
      sprintf(charBuffer, "%4d:", highOrder);
      Serial.print(charBuffer);
#endif
      if (!flagCarryBit) {
        programCounter = word(highOrder, lowOrder);
#ifdef LOG_MESSAGES
        Serial.print(F(" > jnc, Carry bit flag is false, jump to:"));
        Serial.print(programCounter);
#endif
      } else {
#ifdef LOG_MESSAGES
        Serial.print(F(" - Carry bit flag is true, don't jump."));
#endif
        programCounter++;
      }
      break;
    //-----------------------------------------
    case B11011010:
      if (instructionCycle == 2) {
        lowOrder = dataByte;
#ifdef LOG_MESSAGES
        Serial.print(F("< jc, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 3
      highOrder = dataByte;
#ifdef LOG_MESSAGES
      Serial.print(F("< jc, hb: "));
      sprintf(charBuffer, "%4d:", highOrder);
      Serial.print(charBuffer);
#endif
      if (flagCarryBit) {
        programCounter = word(highOrder, lowOrder);
#ifdef LOG_MESSAGES
        Serial.print(F(" > jc, jump to:"));
        Serial.print(programCounter);
#endif
      } else {
#ifdef LOG_MESSAGES
        Serial.print(F(" - Carry bit flag is false, don't jump."));
#endif
        programCounter++;
      }
      break;
    // ---------------------------------------------------------------------
    // ldax RP  00RP1010 - Load indirect through BC(RP=00) or DE(RP=01)
    // ---------------
    case B00001010:
      bValue = regB;
      cValue = regC;
      bcValue = bValue * 256 + cValue;
      if (deValue < memoryBytes) {
        regA = memoryData[bcValue];
      } else {
        regA = 0;
      }
      lightsStatusAddressData(statusByte, deValue, regA);
#ifdef LOG_MESSAGES
      Serial.print(F("< ldax, the data moved into Accumulator is "));
      printData(regA);
#endif
      break;
    // ---------------
    case B00011010:
      dValue = regD;
      eValue = regE;
      deValue = dValue * 256 + eValue;
      if (deValue < memoryBytes) {
        regA = memoryData[deValue];
      } else {
        regA = 0;
      }
      lightsStatusAddressData(statusByte, deValue, regA);
#ifdef LOG_MESSAGES
      Serial.print(F("< ldax, the data moved into Accumulator is "));
      printData(regA);
#endif
      break;
    // ---------------------------------------------------------------------
    case B00000001:
      // lxi b,16-bit-address
      if (instructionCycle == 2) {
        regC = dataByte;
#ifdef LOG_MESSAGES
        Serial.print(F("< lxi, lb data: "));
        sprintf(charBuffer, "%4d:", regC);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 3
      regB = dataByte;
#ifdef LOG_MESSAGES
      Serial.print(F("< lxi, hb data: "));
      sprintf(charBuffer, "%4d:", regB);
      Serial.print(charBuffer);
      Serial.print(F(" > B:C = "));
      printOctal(regB);
      Serial.print(F(":"));
      printOctal(regC);
#endif
      programCounter++;
      break;
    // ---------------------------------------------------------------------
    case B00010001:
      // lxi d,16-bit-address
      if (instructionCycle == 2) {
        regE = dataByte;
#ifdef LOG_MESSAGES
        Serial.print(F("< lxi, lb data: "));
        sprintf(charBuffer, "%4d:", regE);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 3
      regD = dataByte;
#ifdef LOG_MESSAGES
      Serial.print(F("< lxi, hb data: "));
      sprintf(charBuffer, "%4d:", regD);
      Serial.print(charBuffer);
      Serial.print(F(" > D:E = "));
      printOctal(regD);
      Serial.print(F(":"));
      printOctal(regE);
#endif
      programCounter++;
      break;
    // -------------------
    case B00100001:
      // lxi h,16-bit-address
      if (instructionCycle == 2) {
        regL = dataByte;
#ifdef LOG_MESSAGES
        Serial.print(F("< lxi, lb data: "));
        sprintf(charBuffer, "%4d:", regL);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 3
      regH = dataByte;
#ifdef LOG_MESSAGES
      Serial.print(F("< lxi, hb data: "));
      sprintf(charBuffer, "%4d:", regH);
      Serial.print(charBuffer);
      Serial.print(F(" > H:L = "));
      printOctal(regH);
      Serial.print(F(":"));
      printOctal(regL);
#endif
      programCounter++;
      break;
    // ------------------------------------------------------------------------------------------
    // mvi R,#  00 RRR 110  Move a number (#), which is the next db, to register RRR.
    // mvi a,#  00 111 110  0036
    // mvi b,#  00 000 110  0006
    // mvi c,#  00 001 110  0016
    // mvi d,#  00 010 110  0026
    // mvi e,#  00 011 110  0036
    // mvi h,#  00 100 110  0046
    // mvi l,#  00 101 110  0056
    case B00111110:
      regA = dataByte;
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register A: "));
      printData(regA);
#endif
      programCounter++;
      break;
    case B00000110:
      regB = dataByte;
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register B: "));
      printData(regB);
#endif
      programCounter++;
      break;
    case B00001110:
      regC = dataByte;
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register C: "));
      printData(regC);
#endif
      programCounter++;
      break;
    case B00010110:
      regD = dataByte;
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register D: "));
      printData(regD);
#endif
      programCounter++;
      break;
    case B00011110:
      regE = dataByte;
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register E: "));
      printData(regE);
#endif
      programCounter++;
      break;
    case B00100110:
      regH = dataByte;
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register H: "));
      printData(regH);
#endif
      programCounter++;
      break;
    case B00101110:
      regL = dataByte;
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register L: "));
      printData(regL);
#endif
      programCounter++;
      break;
    // -------------------------------------------------------------------------------------------
    case B11100011:
      // instructionCycle == 2
#ifdef LOG_MESSAGES
      Serial.print(F("< OUT, input port: "));
      Serial.print(dataByte);
#else
      // -----------------------------------------
      // Special case of output to serial monitor.
      if (dataByte == 3) {
#ifdef LOG_MESSAGES
        Serial.print(F(", Serial terminal output of the contents of register A :"));
        Serial.print(regA);
        Serial.print(":");
#endif
        asciiChar = regA;
        Serial.print(asciiChar);
        opcode = 0;
        return;
      }
      // -----------------------------------------
      Serial.println("");
#endif
      switch (dataByte) {
        // ---------------------------------------
        case 1:
          Serial.print(F("+ Print register A to the LCD screen."));
          if (regA == 0) {
            lcdBacklight( false );     // LCD back light off.
          } else if (regA == 1) {
            lcdBacklight( true );      // LCD back light on.
          } else if (regA == 2) {
            lcdClearScreen();
          } else if (regA == 3) {
            lcdSplash();
          } else {
            char charA = regA;
            lcdPrintChar((String)charA);
          }
          break;
        case 3:
          // Handled before this switch statement.
          break;
        // ---------------------------------------
        case 10:
          Serial.print(F(" > Play once, the MP3 file named in register A."));
          // mvi a, <file#>
          // out 10   ; Use out 11, to have the sound looped.
          //  ++ Address:oct > description
          //  ++       0:076 > opcode: mvi a,2
          //  ++       1:002 > immediate: 2 ; use other sounds such as 027.
          //  ++       2:343 > opcode: out 10
          //  ++       3:013 > immediate: 10
          if (regA > 0) {
            mp3player.play(regA);
            playerCounterProcessor = regA;
          } else {
            // If an MP3 was playing, this is how to stop it.
            mp3player.pause();
            playerCounterProcessor = 0;
          }
          break;
        case 11:
          Serial.print(F(" > Play in a loop, the MP3 file named in register A."));
          if (regA > 0) {
            mp3player.loop(regA);
            playerCounterProcessor = regA;
            // The following will allow looping after a STOP and RUN.
            playerStatus = playerStatus | M1_ON;
            playerCounter = regA;
          } else {
            // If an MP3 was playing, this is how to stop it.
            mp3player.pause();
            playerCounterProcessor = 0;
          }
          break;
        // ---------------------------------------
        case 30:
          Serial.print(F(" > Register B = "));
          printData(regB);
          break;
        case 31:
          Serial.print(F(" > Register C = "));
          printData(regC);
          break;
        case 32:
          Serial.print(F(" > Register D = "));
          printData(regD);
          break;
        case 33:
          Serial.print(F(" > Register E = "));
          printData(regE);
          break;
        case 34:
          Serial.print(F(" > Register H = "));
          printData(regH);
          break;
        case 35:
          Serial.print(F(" > Register L = "));
          printData(regL);
          break;
        case 36:
          Serial.print(F(" > Register H:L = "));
          Serial.print(regH);
          Serial.print(F(":"));
          Serial.print(regL);
          Serial.print(F(", Data: "));
          hlValue = regH * 256 + regL;
          Serial.print(memoryData[hlValue]);
          break;
        case 37:
          Serial.print(F(" > Register A = "));
          printData(regA);
          break;
        case 38:
#ifdef LOG_MESSAGES
          Serial.println("");
#endif
          Serial.println(F("------------"));
          printRegisters();
          Serial.print(F("------------"));
          break;
        case 39:
#ifdef LOG_MESSAGES
          Serial.println("");
#endif
          Serial.println(F("------------"));
          printRegisters();
          printOther();
          Serial.print(F("------------"));
          break;
        // ---------------------------------------
        case 13:
          ledFlashError();
          Serial.print(F(" > Error happened, flash the LED light error sequence."));
          break;
        case 42:
          ledFlashSuccess();
          Serial.print(F(" > Success happened, flash the LED light success sequence."));
          break;
        // ---------------------------------------
        default:
          Serial.print(F(". - Error, OUT not implemented on this port address: "));
          Serial.println(regA);
      }
      statusByte = statusByte | WO_ON;  // Inverse logic: off writing out. On when not.
      programCounter++;
      break;
    // ------------------------------------------------------------------------------------------
    case B00110010:
      // STA a : Store register A's content to the address (a).
      if (instructionCycle == 2) {
        lowOrder = dataByte;
#ifdef LOG_MESSAGE
        Serial.print(F("< sta, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      if (instructionCycle == 3) {
        highOrder = dataByte;
#ifdef LOG_MESSAGES
        Serial.print(F("< sta, hb: "));
        sprintf(charBuffer, "%4d:", highOrder);
        Serial.print(charBuffer);
#endif
        // programCounter increments during the next cycle.
        return;
      }
      // instructionCycle == 4, an action display cycle.
      hlValue = highOrder * 256 + lowOrder;
      memoryData[hlValue] = regA;
      //
      // Turn all the status lights off, during this step.
      statusByte = 0;
      // The write status light is inverse logic, hence, it is turned off.
      // Turn status light MEMR and WO are on, MI is off, during this step.
      statusByte = MEMR_ON | WO_ON;
      //
      // Set address lights to hb:lb. Set data lights to regA.
      dataByte = regA;
      // lightsStatusAddressData(statusByte, hlValue, regA);
#ifdef LOG_MESSAGES
      Serial.print(F(" > sta, register A data: "));
      Serial.print(regA);
      Serial.print(F(", is stored to the hb:lb address."));
#endif
      programCounter++;
      break;
    // -----------------------------------------
    case B00111010:
      // LDA a : Load register A with data from the address, a(hb:lb).
      if (instructionCycle == 2) {
        lowOrder = dataByte;
#ifdef LOG_MESSAGES
        Serial.print(F("< lda, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      if (instructionCycle == 3) {
        highOrder = dataByte;
#ifdef LOG_MESSAGES
        Serial.print(F("< lda, hb: "));
        sprintf(charBuffer, "%4d:", highOrder);
        Serial.print(charBuffer);
#endif
        // programCounter increments during the next cycle.
        return;
      }
      // instructionCycle == 4
      //  Status lights, MEMR and WO are on, MI is off, during this step.
      statusByte = 0;
      statusByte = (MEMR_ON | WO_ON) & M1_OFF;
      //
      hlValue = highOrder * 256 + lowOrder;
      regA = memoryData[hlValue];
      dataByte = regA;
      // lightsStatusAddressData(statusByte, hlValue, regA);
#ifdef LOG_MESSAGES
      Serial.print(F(" > lda, load data at hb:lb address, into register A: "));
      Serial.print(regA);
#endif
      programCounter++;
      break;
    // ------------------------------------------------------------------------------------------
    case B00100010:
      // shld a
      if (instructionCycle == 2) {
        lowOrder = dataByte;
#ifdef LOG_MESSAGES
        Serial.print(F("< shld, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      if (instructionCycle == 3) {
        highOrder = dataByte;
#ifdef LOG_MESSAGES
        Serial.print(F("< shld, hb: "));
        sprintf(charBuffer, "%4d:", highOrder);
        Serial.print(charBuffer);
#endif
        return;
      }
      hlValue = highOrder * 256 + lowOrder;
      memoryData[hlValue] = regL;
      memoryData[hlValue + 1] = regH;
#ifdef LOG_MESSAGES
      Serial.print(F(" > shld, Store register L:"));
      Serial.print(regL);
      Serial.print(F(" to memory address hb:lb :"));
      Serial.print(hlValue);
      Serial.print(F(". Store register H:"));
      Serial.print(regH);
      Serial.print(F(" to hb:lb + 1 = "));
      Serial.print(hlValue + 1);
#endif
      programCounter++;
      break;
    // ------------------------------------------------------------------------------------------
    default:
      Serial.print(F(" -- Error, unknow instruction."));
      ledFlashError();
      controlStopLogic();
      statusByte = HLTA_ON;
      digitalWrite(WAIT_PIN, HIGH);
  }
  // The opcode cycles are complete.
  opcode = 0;
}

// -----------------------------------------------------------------------------
//  Output Functions

void printOther() {
  Serial.print(F("+ Zero bit flag: "));
  Serial.print(flagZeroBit);
  Serial.print(F(", Carry bit flag: "));
  Serial.print(flagCarryBit);
  Serial.println("");
  Serial.print(F("+ Stack pointer: "));
  Serial.print(stackPointer);
  Serial.println("");
}

void printRegisters() {
  Serial.print(F("+ regA: "));
  printData(regA);
  Serial.println("");
  // ---
  Serial.print(F("+ regB: "));
  printData(regB);
  Serial.print(F("  regC: "));
  printData(regC);
  Serial.println("");
  // ---
  Serial.print(F("+ regD: "));
  printData(regD);
  Serial.print(F("  regE: "));
  printData(regE);
  Serial.println("");
  // ---
  Serial.print(F("+ regH: "));
  printData(regH);
  Serial.print(F("  regL: "));
  printData(regL);
  // ---
  Serial.println("");
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// SD card module functions

// Handle the case if the card is not inserted. Once inserted, the module will be re-initialized.
boolean sdcardInitiated = false;
void initSdcard() {
  if (SD.begin(csPin)) {
    // Serial.println(F("+ SD card initialized."));
    sdcardInitiated = true;
    return;
  }
  sdcardInitiated = false;
  Serial.println(F("- Error initializing SD card."));
  Serial.println(F("-- Check that SD card is inserted"));
  Serial.println(F("-- Check that SD card adapter is wired properly."));
  //
  // Optionally, retry for a period of time.
}

void ledFlashSuccess() {
  lcdPrintln(1, "+ Success");
  int delayTime = 60;
  lightsStatusAddressData(0, 0, B00000000);
  delay(delayTime);
  for (int i = 0; i < 1; i++) {
    byte flashByte = B10000000;
    for (int i = 0; i < 8; i++) {
      lightsStatusAddressData(0, 0, flashByte);
      flashByte = flashByte >> 1;
      delay(delayTime);
    }
    flashByte = B00000001;
    for (int i = 0; i < 8; i++) {
      lightsStatusAddressData(0, 0, flashByte);
      flashByte = flashByte << 1;
      delay(delayTime);
    }
  }
  // Reset the panel lights to program values.
  switch (programState) {
    case CLOCK_RUN:
    case PLAYER_RUN:
      playerLights();
      break;
    default:
      programLights();
      break;
  }
}
void ledFlashError() {
  lcdPrintln(1, "- Error");
  int delayTime = 300;
  for (int i = 0; i < 3; i++) {
    lightsStatusAddressData(0, 0, B11111111);
    delay(delayTime);
    lightsStatusAddressData(0, 0, B00000000);
    delay(delayTime);
  }
  // Reset the panel lights to program values.
  switch (programState) {
    case CLOCK_RUN:
    case PLAYER_RUN:
      playerLights();
      break;
    default:
      programLights();
      break;
  }
}

// -------------------------------------
// Write Program memory to a file.

void writeProgramMemoryToFile(String theFilename) {
  digitalWrite(HLDA_PIN, HIGH);
  if (!sdcardInitiated) {
    initSdcard();
  }
  // Serial.print(F("+ Write program memory to a new file named: "));
  // Serial.println(theFilename);
  // Serial.println("+ Check if file exists. ");
  if (SD.exists(theFilename)) {
    SD.remove(theFilename);
    // Serial.println("++ Exists, so it was deleted.");
  } else {
    // Serial.println("++ Doesn't exist.");
  }
  myFile = SD.open(theFilename, FILE_WRITE);
  if (!myFile) {
    Serial.print(F("- Error opening file: "));
    Serial.println(theFilename);
    ledFlashError();
    sdcardInitiated = false;
    digitalWrite(HLDA_PIN, LOW);
    return; // When used in setup(), causes jump to loop().
  }
  // Serial.println("++ New file opened.");
  // Serial.println("++ Write binary memory to the file.");
  for (int i = 0; i < memoryBytes; i++) {
    myFile.write(memoryData[i]);
  }
  myFile.close();
  Serial.println(F("+ Write completed, file closed."));
  ledFlashSuccess();
  digitalWrite(HLDA_PIN, LOW);
}

// -------------------------------------
// Read program memory from a file.

boolean readProgramFileIntoMemory(String theFilename) {
  digitalWrite(HLDA_PIN, HIGH);
  if (!sdcardInitiated) {
    initSdcard();
  }
  // Serial.println("+ Read a file into program memory, file named: ");
  // Serial.print(theFilename);
  // Serial.println("+ Check if file exists. ");
  if (!SD.exists(theFilename)) {
    Serial.print(F("- Read ERROR, file doesn't exist: "));
    Serial.println(theFilename);
    ledFlashError();
    sdcardInitiated = false;
    digitalWrite(HLDA_PIN, LOW);
    return (false);
  }
  myFile = SD.open(theFilename);
  if (!myFile) {
    Serial.print(F("- Read ERROR, cannot open file: "));
    Serial.println(theFilename);
    ledFlashError();
    sdcardInitiated = false;
    digitalWrite(HLDA_PIN, LOW);
    return (false);
  }
  int i = 0;
  while (myFile.available()) {
    // Reads one character at a time.
    memoryData[i] = myFile.read();
#ifdef LOG_MESSAGES
    // Print Binary:Octal:Decimal values.
    Serial.print("B");
    printByte(memoryData[i]);
    Serial.print(":");
    printOctal(memoryData[i]);
    Serial.print(":");
    Serial.println(memoryData[i], DEC);
#endif
    i++;
    if (i > memoryBytes) {
      Serial.println(F("-+ Warning, file contains more data bytes than available memory."));
      break;
    }
  }
  myFile.close();
  Serial.println(F("+ Read completed, file closed."));
  digitalWrite(HLDA_PIN, LOW);
  controlResetLogic();
  return (true);
}

// -------------------------------------

boolean writeFileByte(String theFilename, byte theByte) {
  if (!sdcardInitiated) {
    initSdcard();
  }
  if (SD.exists(theFilename)) {
    SD.remove(theFilename);
  }
  myFile = SD.open(theFilename, FILE_WRITE);
  if (!myFile) {
    Serial.print(F("- Error opening file: "));
    Serial.println(theFilename);
    ledFlashError();
    sdcardInitiated = false;
    return (false);
  }
  myFile.write(theByte);
  myFile.close();
  // Serial.println(F("+ Byte write completed, file closed."));
  // ledFlashSuccess();
  return (true);
}

// Read a byte from a file.
int readFileByte(String theFilename) {
  if (!sdcardInitiated) {
    initSdcard();
  }
  if (!SD.exists(theFilename)) {
    // Serial.print(F("- Read ERROR, file doesn't exist: "));
    // Serial.println(theFilename);
    // ledFlashError();
    sdcardInitiated = false;
    return (0);
  }
  myFile = SD.open(theFilename);
  if (!myFile) {
    // Serial.print(F("- Read ERROR, cannot open file: "));
    // Serial.println(theFilename);
    // ledFlashError();
    sdcardInitiated = false;
    return (0);
  }
  int returnByte = 0;
  if (myFile.available()) {
    returnByte = myFile.read();
#ifdef LOG_MESSAGES
    Serial.print("+ Byte read = ");
    Serial.println(returnByte, DEC);
    Serial.print(":B");
    printByte(returnByte);
#endif
  }
  myFile.close();
  // Serial.println(F("+ Read byte completed, file closed."));
  return (returnByte);
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Front Panel Switches

// Notes regarding switch logic,
// + Only do the action once, don't repeat if the button is held down.
// + Don't repeat action if the button is not pressed.

// --------------------------------------------------
// Get Front Panel address/data/sense toggle values.

// Invert byte bits using bitwise not operator: "~";
// Bitwise "not" operator to invert bits:
//  int a = 103;  // binary:  0000000001100111
//  int b = ~a;   // binary:  1111111110011000 = -104

int toggleData() {
  byte toggleByte = ~pcfData.read8();
  return toggleByte;
}
// --------------------------
int toggleSense() {
#ifdef DESKTOP_MODULE
  byte toggleByte = ~pcfSense.read8();
#else
  byte toggleByte = ~pcfData.read8();     // Only 8 toggle bits on the tablet, so Sense and Data are shared.
#endif
  return toggleByte;
}
// --------------------------
unsigned int toggleAddress() {
  byte byteLow = ~pcfData.read8();
#ifdef DESKTOP_MODULE
  byte byteHigh = ~pcfSense.read8();
#else
  byte byteHigh = 0;                    // No high byte toggles on the tablet.
#endif
  return byteHigh * 256 + byteLow;
}

// --------------------------------------------------

#ifdef DESKTOP_MODULE
const int pinStop = 7;
const int pinRun = 6;
const int pinStep = 5;
const int pinExamine = 4;
const int pinExamineNext = 3;
const int pinDeposit = 2;
const int pinDepositNext = 1;
const int pinReset = 0;
#else
// TABLET_MODULE
const int pinStop = 0;
const int pinRun = 1;
const int pinStep = 2;
const int pinExamine = 3;
const int pinExamineNext = 4;
const int pinDeposit = 5;
const int pinDepositNext = 6;
const int pinReset = 7;
#endif

const int pinAux1up = 3;
const int pinAux1down = 2;
const int pinAux2up = 1;
const int pinAux2down = 0;
const int pinProtect = 5;
const int pinUnProtect = 4;
const int pinClr = 6;
const int pinStepDown = 7;

boolean switchStop = false;
boolean switchRun = false;
boolean switchStep = false;
boolean switchExamine = false;
boolean switchExamineNext = false;
boolean switchDeposit = false;;
boolean switchDepositNext = false;;
boolean switchReset = false;
boolean switchProtect = false;
boolean switchUnProtect = false;
boolean switchClr = false;
boolean switchStepDown = false;

boolean switchAux1up = false;
boolean switchAux1down = false;
boolean switchAux2up = false;
boolean switchAux2down = false;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Processor Switch Functions

void controlResetLogic() {
  programCounter = 0;
  curProgramCounter = 0;
  stackPointer = 0;
  opcode = 0;  // For the case when the processing cycle 2 or more.
  statusByte = 0;
  if (programState != PROGRAM_RUN) {
    programState = PROGRAM_WAIT;
    digitalWrite(WAIT_PIN, HIGH);
  }
  // Fetch the first opcode, which display values to the panel.
  processData();
}

void pausePlayer() {
  // Twice because sometimes, once doesn't work.
  mp3player.pause();
  delay(100);
  mp3player.pause();
  playerStatus = playerStatus | HLTA_ON;
}
void controlStopLogic() {
  programState = PROGRAM_WAIT;
  // Create a STOP statusByte, to display on stopping.
  // When the program continues, the orginal statusByte should be used.
  int stopStatusByte = HLTA_ON;
  digitalWrite(WAIT_PIN, HIGH);
  // Display values to the panel lights.
  lightsStatusAddressData(stopStatusByte, programCounter, dataByte);
  pausePlayer();
}

// --------------------------------------------------
// Toggle switch functions

void checkExamineButton() {
  // Read PCF8574 input for this switch.
  if (pcfControl.readButton(pinExamine) == 0) {
    if (!switchExamine) {
      switchExamine = true;
    }
  } else if (switchExamine) {
    switchExamine = false;
    //
    // Switch logic, based on programState.
    //
    switch (programState) {
      // -------------------
      case PROGRAM_WAIT:
        programCounter = toggleAddress();
        curProgramCounter = programCounter;     // Synch for control switches.
        dataByte = memoryData[programCounter];
        programLights();
#ifdef SWITCH_MESSAGES
        Serial.print(F("+ Control, Examine address: "));
        sprintf(charBuffer, "%5d", programCounter); // A10 = 1024, A11 = 2048, ..., A14 = 16K, A15 = 32K (32768).
        Serial.print(charBuffer);
        Serial.print("=");
        printAddress(programCounter);
        Serial.print(", data byte value = ");
        // printByte(dataByte);
        // Serial.print(" = ");
        printData(dataByte);
        Serial.println("");
#endif
        break;
      case CLOCK_RUN:
#ifdef SWITCH_MESSAGES
        Serial.print(F("+ Clock, Examine, set clock values, "));
        Serial.println(F("one at a time: year, month, day, hour, minutes, seconds."));
#endif
        // Serial.print("+ Key OK");
        // setClockValue is indicator of which clock value is being processed, to be set.
        if (setClockValue) {
          // Serial.print(" ");
          switch (setClockValue) {
            case 1:
              // Serial.print("seconds");
              theCounterSeconds = setValue;
              printLcdClockValue(theSetCol, printRowClockPulse, setValue);
              break;
            case 2:
              // Serial.print("minutes");
              theCounterMinutes = setValue;
              printLcdClockValue(theSetCol, printRowClockPulse, setValue);
              break;
            case 3:
              // Serial.print("hours");
              theCounterHours = setValue;
              printLcdClockValue(theSetCol, printRowClockPulse, setValue);
              break;
            case 4:
              // Serial.print("day");
              theCounterDay = setValue;
              break;
            case 5:
              // Serial.print("month");
              theCounterMonth = setValue;
              break;
            case 6:
              // Serial.print("year");
              theCounterYear = setValue;
              break;
          }
          // The following offsets the time to make the change.
          // Else, the clock looses about second each time a setting is made.
          theCounterSeconds ++;
          delay(100);
          //
          rtc.adjust(DateTime(theCounterYear, theCounterMonth, theCounterDay, theCounterHours, theCounterMinutes, theCounterSeconds));
          lcdPrintln(theSetRow, "Value is set.");
          printClockDate();
          delay(2000);
          lcdPrintln(theSetRow, "");
        }
        break;
    }
  }
}
void checkExamineNextButton() {
  // Read PCF8574 input for this switch.
  if (pcfControl.readButton(pinExamineNext) == 0) {
    if (!switchExamineNext) {
      switchExamineNext = true;
    }
  } else if (switchExamineNext) {
    switchExamineNext = false;
    //
    // Switch logic, based on programState.
    //
    switch (programState) {
      // -------------------
      case PROGRAM_WAIT:
        if (curProgramCounter != programCounter) {
          // Synch for control switches: programCounter and curProgramCounter.
          // This handles the first switch...Next, after a STEP or HLT.
          programCounter = curProgramCounter;
        }
        curProgramCounter++;
        programCounter++;
        dataByte = memoryData[programCounter];
        programLights();
#ifdef SWITCH_MESSAGES
        Serial.print(F("+ Control, Examine Next, programCounter: "));
        printByte(programCounter);
        Serial.print(", byte = ");
        printByte(dataByte);
        Serial.print(" = ");
        printData(dataByte);
        Serial.println("");
#endif
        break;
      case CLOCK_RUN:
#ifdef SWITCH_MESSAGES
        Serial.println(F("+ Clock, Examine Next clock value: year, month, day, hour, minutes, seconds."));
#endif
        setClockValue--;
        if (setClockValue < 0) {
          setClockValue = 6;
        }
        setClockMenuItems();
        break;
    }
  }
}

void checkDepositButton() {
  // Read PCF8574 input for this switch.
  if (pcfControl.readButton(pinDeposit) == 0) {
    if (!switchDeposit) {
      switchDeposit = true;
    }
  } else if (switchDeposit) {
    switchDeposit = false;
    //
    // Switch logic, based on programState.
    //
    switch (programState) {
      // -------------------
      case PROGRAM_WAIT:
#ifdef SWITCH_MESSAGES
        Serial.println(F("+ Control, Deposit."));
#endif
        if (curProgramCounter != programCounter) {
          // Synch for control switches: programCounter and curProgramCounter.
          // This handles the first switch, after a STEP or HLT.
          programCounter = curProgramCounter;
        }
        dataByte = toggleData();
        memoryData[programCounter] = dataByte;
        programLights();
        break;
      case CLOCK_RUN:
#ifdef SWITCH_MESSAGES
        Serial.println(F("+ Clock, Deposit, increment the clock value being set(Y,M,D,H,M, or S)."));
#endif
        setValue++;
        if (setValue > theSetMax) {
          setValue = theSetMin;
        }
        printLcdClockValue(theSetCol, theSetRow, setValue);
        break;
    }
  }
}
void checkDepositNextButton() {
  // Read PCF8574 input for this switch.
  if (pcfControl.readButton(pinDepositNext) == 0) {
    if (!switchDepositNext) {
      switchDepositNext = true;
    }
  } else if (switchDepositNext) {
    switchDepositNext = false;
    //
    // Switch logic, based on programState.
    //
    switch (programState) {
      // -------------------
      case PROGRAM_WAIT:
#ifdef SWITCH_MESSAGES
        Serial.println(F("+ Control, Deposit Next."));
#endif
        // Switch logic...
        if (curProgramCounter != programCounter) {
          // Synch for control switches: programCounter and curProgramCounter.
          // This handles the first switch...Next, after a STEP or HLT.
          programCounter = curProgramCounter;
        }
        curProgramCounter++;
        programCounter++;
        dataByte = toggleData();
        memoryData[programCounter] = dataByte;
        programLights();
        break;
      case CLOCK_RUN:
#ifdef SWITCH_MESSAGES
        Serial.println(F("+ Clock, Deposit Next: decrement the clock value being set(Y,M,D,H,M, or S)."));
#endif
        setValue--;
        if (setValue < theSetMin) {
          setValue = theSetMax;
        }
        printLcdClockValue(theSetCol, theSetRow, setValue);
        break;
    }
  }
}

// --------------------------------------------------------
// Front Panel Control Switches, when a program is running.
// Switches: STOP and RESET.
void checkRunningButtons() {
  // -------------------
  // Read PCF8574 input for this switch.
  if (pcfControl.readButton(pinStop) == 0) {
    if (!switchStop) {
      switchStop = true;
    }
  } else if (switchStop) {
    switchStop = false;
    // Switch logic
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ Running, Stop."));
#endif
    controlStopLogic();
    // -------------------
    pausePlayer();
  }
  // -------------------
  if (pcfControl.readButton(pinReset) == 0) {
    if (!switchReset) {
      switchReset = true;
    }
  } else if (switchReset) {
    switchReset = false;
    // Switch logic
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ Running, Reset."));
#endif
    controlResetLogic();
  }
  // -------------------
}

// -------------------------------------------------------------------
// Front Panel Control Switches, when a program is not running (WAIT).
// Switches: RUN, RESET, STEP, EXAMINE, EXAMINE NEXT, DEPOSIT, DEPOSIT NEXT,
void checkControlButtons() {
  // -------------------
  // Read PCF8574 input for this switch.
  if (pcfControl.readButton(pinRun) == 0) {
    if (!switchRun) {
      switchRun = true;
    }
  } else if (switchRun) {
    switchRun = false;
    // Switch logic
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ Control, Run."));
#endif
    // Switch logic...
    programState = PROGRAM_RUN;
    digitalWrite(WAIT_PIN, LOW);
    // statusByte = statusByte & WAIT_OFF;
    statusByte = statusByte & HLTA_OFF;
    // -------------------
    if (playerStatus & M1_ON) {
      if (playerCounterProcessor > 0) {
        // mp3player.start();
        mp3player.loop(playerCounter);
        playerStatus = playerStatus & HLTA_OFF;
      }
    }
  }
  // -------------------
  if (pcfControl.readButton(pinReset) == 0) {
    if (!switchReset) {
      switchReset = true;
    }
  } else if (switchReset) {
    switchReset = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ Control, Reset."));
#endif
    controlResetLogic();
  }
  // -------------------
  if (pcfControl.readButton(pinStep) == 0) {
    if (!switchStep) {
      switchStep = true;
    }
  } else if (switchStep) {
    switchStep = false;
    // Switch logic
#ifdef SWITCH_MESSAGES
    Serial.println("+ Control, Step.");
#endif
    statusByte = statusByte & HLTA_OFF;
    processData();
  }
  // -------------------
  checkExamineButton();
  checkExamineNextButton();
  checkDepositButton();
  checkDepositNextButton();
  // -------------------
  if (pcfAux.readButton(pinStepDown) == 0) {
    if (!switchStepDown) {
      switchStepDown = true;
    }
  } else if (switchStepDown) {
    switchStepDown = false;
    // Switch logic
    if (curProgramCounter != programCounter) {
      // Synch for control switches: programCounter and curProgramCounter.
      // This handles the first switch...Next, after a STEP or HLT.
      programCounter = curProgramCounter;
    }
    if (programCounter > 0) {
      curProgramCounter--;
      programCounter--;
      dataByte = memoryData[programCounter];
      programLights();
#ifdef SWITCH_MESSAGES
      Serial.print(F("+ Control, Examine Next, programCounter: "));
      printByte(programCounter);
      Serial.print(", byte = ");
      printByte(dataByte);
      Serial.print(" = ");
      printData(dataByte);
      Serial.println("");
#endif
    }
  }
  // -------------------
  if (pcfAux.readButton(pinClr) == 0) {
    if (!switchClr) {
      switchClr = true;
    }
  } else if (switchClr) {
    switchClr = false;
    // Switch logic
    // -------------------------------------------------------
    // Double flip confirmation.
    switchClr = false;      // Required to reset the switch state for confirmation.
    boolean confirmChoice = false;
    unsigned long timer = millis();
    while (!confirmChoice && (millis() - timer < 1000)) {
      if (pcfAux.readButton(pinClr) == 0) {
        if (!switchClr) {
          switchClr = true;
        }
      } else if (switchClr) {
        switchClr = false;
        // Switch logic.
        confirmChoice = true;
      }
      delay(100);
    }
    if (!confirmChoice) {
      return;
    }
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ Choice confirmed."));
#endif
    // -------------------------------------------------------
    zeroOutMemory();
    controlResetLogic();
  }
  // -------------------
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Clock Section

void syncCountWithClock() {
  now = rtc.now();
  theCounterHours = now.hour();
  theCounterMinutes = now.minute();
  theCounterSeconds = now.second();
  //
  theCursor = thePrintColHour;
  printLcdClockValue(theCursor, printRowClockPulse, theCounterHours);  // Column, Row
  theCursor = theCursor + 3;
  lcd.print(":");
  printLcdClockValue(theCursor, printRowClockPulse, theCounterMinutes);
  theCursor = theCursor + 3;
  lcd.print(":");
  printLcdClockValue(theCursor, printRowClockPulse, theCounterSeconds);
  //
  Serial.print("+ syncCountWithClock,");
  Serial.print(" theCounterHours=");
  Serial.print(theCounterHours);
  Serial.print(" theCounterMinutes=");
  Serial.print(theCounterMinutes);
  Serial.print(" theCounterSeconds=");
  Serial.println(theCounterSeconds);
  //
  printClockDate();
}

// -----------------------------------------------------------------------------
void processClockNow() {
  //
  now = rtc.now();
  //
  if (now.second() != theCounterSeconds) {
    // When the clock second value changes, that's a clock second pulse.
    theCounterSeconds = now.second();
    clockPulseSecond();
    if (theCounterSeconds == 0) {
      // When the clock second value changes to zero, that's a clock minute pulse.
      theCounterMinutes = now.minute();
      clockPulseMinute();
      if (theCounterMinutes == 0) {
        // When the clock minute value changes to zero, that's a clock hour pulse.
        theCounterHours = now.hour();
        clockPulseHour();

        // -------------------------
        // Date pulses.
        if (now.hour() == 0) {
          // When the clock hour value changes to zero, that's a clock day pulse.
          printClockDate(); // Prints and sets the values for day, month, and year.
          clockPulseDay();
          if (theCounterDay == 1) {
            // When the clock day value changes to one, that's a clock month pulse.
            clockPulseMonth();
            if (theCounterMonth == 1) {
              // When the clock Month value changes to one, that's a clock year pulse.
              clockPulseYear();
            }
          }
        }
        // -------------------------
      }
    }
  }
}

void clockPulseYear() {
  Serial.print("+++++ clockPulseYear(), theCounterYear= ");
  Serial.println(theCounterYear);
}
void clockPulseMonth() {
  Serial.print("++++ clockPulseMonth(), theCounterMonth= ");
  Serial.println(theCounterMonth);
}
void clockPulseDay() {
  Serial.print("+++ clockPulseDay(), theCounterDay= ");
  Serial.println(theCounterDay);
}
int theHour = 0;
void clockPulseHour() {
  Serial.print("++ clockPulseHour(), theCounterHours= ");
  Serial.println(theCounterHours);
  Serial.println(theCounterHours);
  // Use AM/PM rather than 24 hours.
  if (theCounterHours > 12) {
    theHour = theCounterHours - 12;
  } else if (theCounterHours == 0) {
    theHour = 12; // 12 midnight, 12am
  } else {
    theHour = theCounterHours;
  }
  KnightRiderScanner();
  //
  displayTheTime( theCounterMinutes, theCounterHours );
  printLcdClockValue(thePrintColHour, printRowClockPulse, theHour);
}
void clockPulseMinute() {
  Serial.print("+ clockPulseMinute(), theCounterMinutes= ");
  Serial.println(theCounterMinutes);
  displayTheTime( theCounterMinutes, theCounterHours );
  printLcdClockValue(thePrintColMin, printRowClockPulse, theCounterMinutes);
  if (theCounterMinutes == 15 || theCounterMinutes == 30 || theCounterMinutes == 45) {
    if (playerStatus & HLTA_ON) {
      mp3player.play(CLOCK_CUCKOO);
    }
  }
}
void clockPulseSecond() {
  // Serial.print("+ theCounterSeconds = ");
  // Serial.println(theCounterSeconds);
  printLcdClockValue(thePrintColSec, printRowClockPulse, theCounterSeconds);  // Column, Row
}

// ------------------------------------------------------------------------
// Display hours and minutes on LED lights.

void displayTheTime(byte theMinute, byte theHour) {
  byte theMinuteOnes = 0;
  byte theMinuteTens = 0;
  // byte theBinaryMinute = 0;  // not used anymore.
  byte theBinaryHour1 = 0;
  byte theBinaryHour2 = 0;

  // ----------------------------------------------
  // Convert the minute into binary for display.
  if (theMinute < 10) {
    // theBinaryMinute = theMinute;
    theMinuteOnes = theMinute;
  } else {
    // There are 3 bits for the tens: 0 ... 5 (00, 10, 20, 30, 40, or 50).
    // There are 4 bits for the ones: 0 ... 9.
    // LED diplay lights: ttt mmmm
    // Example:      23 = 010 0011
    //      Clock: Tens(t) & Minutes(m): B-tttmmmm
    //                                   B00001111 = 2 ^ 4 = 16
    // Altair 101: Tens(t) & Minutes(m): Bttt-mmmm
    //                                   B00011111 = 2 ^ 5 = 32
    // theMinute = 10, theBinaryMinute =  00100000
    theMinuteTens = theMinute / 10;
    theMinuteOnes = theMinute - theMinuteTens * 10;
    // theBinaryMinute = 32 * theMinuteTens + theMinuteOnes;
  }

  // ----------------------------------------------
  // Convert the hour into binary for display.
  // Use a 12 hour clock value rather than 24 value.
  if (theHour > 12) {
    theHour = theHour - 12;
  } else if (theHour == 0) {
    theHour = 12; // 12 midnight, 12am
  }
  switch (theHour) {
    case 1:
      theBinaryHour1 = B00000010;   // Use A1 as 1 o'clock.
      theBinaryHour2 = 0;
      break;
    case 2:
      theBinaryHour1 = B00000100;
      theBinaryHour2 = 0;
      break;
    case 3:
      theBinaryHour1 = B00001000;
      theBinaryHour2 = 0;
      break;
    case 4:
      theBinaryHour1 = B00010000;
      theBinaryHour2 = 0;
      break;
    case 5:
      theBinaryHour1 = B00100000;
      theBinaryHour2 = 0;
      break;
    case 6:
      theBinaryHour1 = B01000000;
      theBinaryHour2 = 0;
      break;
    case 7:
      theBinaryHour1 = B10000000;
      theBinaryHour2 = 0;
      break;
    case 8:
      theBinaryHour1 = 0;
      theBinaryHour2 = B00000001;
      break;
    case 9:
      theBinaryHour1 = 0;
      theBinaryHour2 = B00000010;
      break;
    case 10:
      theBinaryHour1 = 0;
      theBinaryHour2 = B00000100;
      break;
    case 11:
      theBinaryHour1 = 0;
      theBinaryHour2 = B00001000;
      break;
    case 12:
      theBinaryHour1 = 0;
      theBinaryHour2 = B00010000;
      break;
  }
  // ----------------------------------------------
  // Front panel clock time display, stacy
  //
  // Need enter: theBinaryMinute, theBinaryHour1, theBinaryHour2, into the following:
  // Data byte LED lights: theBinaryMinute.
  // Address word LED lights: theBinaryHour2 & theBinaryHour1.
  unsigned int hourWord = theBinaryHour2 * 256 + theBinaryHour1;
  //
  // lightsStatusAddressData(OUT_ON, hourWord, theBinaryMinute);
  // This option is easier to read on the desktop module:
  lightsStatusAddressData(theMinuteTens, hourWord, theMinuteOnes);
}

// -----------------------------------------------------------------------
// Menu items to set the clock date and time values.

void cancelSet() {
  if (setClockValue) {
    // Serial.println("Cancel set.");
    lcdPrintln(theSetRow, "");
    setClockValue = false;
  }
}

// ------------------------
String currentLcdRow0;
String currentLcdRow1;
int currentLcdRow;
int currentLcdColumn;
void saveClearLcdScreenData() {
  // Save the current LCD screen information.
  currentLcdRow0 = lcdRow0;
  currentLcdRow1 = lcdRow1;
  currentLcdRow = lcdRow;
  currentLcdColumn = lcdColumn;
  lcdClearScreen();
}
void restoreLcdScreenData() {
  // Restore the LCD screen.
  lcdRow0 = currentLcdRow0;
  lcdRow1 = currentLcdRow1;
  lcdPrintln(0, lcdRow0);
  lcdPrintln(1, lcdRow1);
  lcdRow = currentLcdRow;
  lcdColumn = currentLcdColumn;
  lcd.cursor();
  lcd.setCursor(lcdColumn, lcdRow);
}
#endif

// -----------------------------------------------------------------------
// Clock Timer

const int timerTop = 8;
unsigned int timerData[timerTop];
unsigned int timerCounter = 0;
unsigned int timerDataAddress = 0;

byte timerStatus = INP_ON;         // Clock timer is ready for timer value input.
byte timerStep = 0;
unsigned int timerMinute = 0;
unsigned int clockTimerAddress = 0;
unsigned long clockTimer;
int clockTimerCount = 0;

unsigned long clockTimerSeconds;
boolean clockTimerSecondsOn = false;
int clockTimerCountBit;

void clockSetTimer(int timerMinute) {
  //
  // Set parameters before starting the timere.
  //    timerMinute is the amount of minutes to be timed.
  //
  timerStatus = timerStatus | M1_ON;  // Timer is running (M1_ON).
  clockTimer = millis();              // Initialize the clock timer milliseconds.
  clockTimerCount = 0;                // Start counting from 0. Timer is done when it equals timerMinute.
  clockTimerAddress = 0;              // Used to display Address lights: timerMinute and clockTimerCount.
  // Put the timerMinute into the display value: clockTimerAddress.
  clockTimerAddress = bitWrite(clockTimerAddress, timerMinute, 1);
}
void clockRunTimer() {
  // -----------------------------------------
  if ((millis() - clockTimerSeconds >= 500)) {
    //
    // Each half second, toggle the tracking minute counter LED on/off.
    //
    clockTimerSeconds = millis();
    if (clockTimerSecondsOn) {
      clockTimerSecondsOn = false;
      clockTimerCountBit = 1;
      clockTimerAddress = bitWrite(clockTimerAddress, clockTimerCount, clockTimerCountBit);
    } else {
      clockTimerSecondsOn = true;
      clockTimerCountBit = 0;
      clockTimerAddress = bitWrite(clockTimerAddress, clockTimerCount, clockTimerCountBit);
    }
  }
  // -----------------------------------------
  if ((millis() - clockTimer >= 60000)) {
    //
    // Minute process.
    //
    // 60 x 1000 = 60000, which is one minute.
    clockTimer = millis();
    clockTimerCount++;
    if (clockTimerCount >= timerMinute ) {
      // -----------------------------------------
      // When the timer is complete, play a sound, flash some lights and return to clock mode.
      //
#ifdef SWITCH_MESSAGES
      Serial.print(F("+ clockTimerCount="));
      Serial.print(clockTimerCount);
      Serial.print(F(" timerMinute="));
      Serial.print(clockTimerCount);
      Serial.println(F(" Timer timed."));
#endif
      // Force playing the sound. If a song was playing, it doesn't restart.
      mp3player.play(soundEffects[TIMER_COMPLETE]);
      delay(1200);  // Delay time for the sound to play.
      // KnightRiderScanner();
      //
      if (timerCounter < timerTop && timerCounter > 0) {
        // Check if another timer time needs to happen.
        timerCounter++;
        timerMinute = getMinuteValue(timerData[timerCounter]);
        if (timerMinute > 0) {
#ifdef SWITCH_MESSAGES
          Serial.print(F("+ Run next timer event. timerCounter="));
          Serial.print(timerCounter);
          Serial.print(F(" timerData[timerCounter]="));
          Serial.println(timerData[timerCounter]);
#endif
          clockTimerAddress = 1;
          clockTimerAddress = bitWrite(clockTimerAddress, timerMinute, 1);
          // Start the timer and count.
          clockTimer = millis();
          clockTimerCount = 0;
        } else {
          // End the timer run state, stay in clock timer mode.
          timerStatus = timerStatus & M1_OFF;
          clockTimerAddress = bitWrite(clockTimerAddress, timerMinute, 1);
        }
      } else {
        // End the timer run state, stay in clock timer mode.
        timerStatus = timerStatus & M1_OFF;
        clockTimerAddress = bitWrite(clockTimerAddress, timerMinute, 1);
      }
    } else {
      // -----------------------------------------
      // Each minute, increment the minute counter and LED light, and play a cuckoo sound.
      //
#ifdef SWITCH_MESSAGES
      Serial.print(F("+ clockTimerCount="));
      Serial.print(clockTimerCount);
      Serial.print(F(" timerMinute="));
      Serial.println(clockTimerCount);
#endif
      clockTimerAddress = 0;
      clockTimerAddress = bitWrite(clockTimerAddress, timerMinute, 1);
      clockTimerAddress = bitWrite(clockTimerAddress, clockTimerCount, 1);
      playerPlaySound(TIMER_MINUTE);
      delay(1200);  // Delay time for the sound to play.
    }
  }
  lightsStatusAddressData(timerStatus, clockTimerAddress, timerCounter);
}

// -----------------------------------------------------------------------------
// Clock Front Panel Switch Functions.

int clockData = 0;

String getSfbFilename(byte fileByte) {
  // SFB: Sound File Byte.
  String sfbFilename = ".sfb";
  if (fileByte < 10) {
    sfbFilename = "000" + String(fileByte) + sfbFilename;
  } else if (fileByte < 100) {
    sfbFilename = "00" + String(fileByte) + sfbFilename;
  } else if (fileByte < 1000) {
    sfbFilename = "0" + String(fileByte) + sfbFilename;
  } else {
    sfbFilename = String(fileByte) + sfbFilename;
  }
  return sfbFilename;
}
String getDataFilename() {
  // byte fileByte = toggleData();
  return getSfbFilename(toggleData());
}

int getMinuteValue(unsigned int theWord) {
  int theMinute = 0;
  for (int i = 15; i >= 0; i--) {
    if (bitRead(theWord, i) > 0) {
      theMinute = i;
      // Serial.print("\n+ Minute = ");
      // Serial.println(theMinute);
    }
  }
  return (theMinute);
}

void checkClockControls() {
  // ---------------------------------------------------------
  if (pcfControl.readButton(pinStep) == 0) {
    if (!switchStep) {
      switchStep = true;
    }
  } else if (switchStep) {
    switchStep = false;
    // Switch logic.
    Serial.print(F("+ STEP, clock, "));
    if (clockData == 0) {
      clockData = 1;
      Serial.println(F("Show month and day."));
      displayTheTime(theCounterDay, theCounterMonth);
    } else if (clockData == 1) {
      clockData = 2;
      Serial.println(F("Show year."));
      int theCentury = theCounterYear / 100;
      int theYear = theCounterYear - theCentury * 100;
      int theYearTens = theYear / 10;
      int theYearOnes = theYear - theYearTens * 10;
      lightsStatusAddressData(theYearTens, theCentury, theYearOnes);
    } else if (clockData == 2) {
      clockData = 0;
      Serial.println(F("Show minutes and hours."));
      displayTheTime(theCounterMinutes, theCounterHours);
    }
  }
  // ------------------------
  if (pcfControl.readButton(pinReset) == 0) {
    if (!switchReset) {
      switchReset = true;
    }
  } else if (switchReset) {
    switchReset = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ Control, clock Reset."));
#endif
    //
    KnightRiderScanner();
    //
    clockData = 0;
    Serial.println(F("Show minutes and hours."));
    displayTheTime(theCounterMinutes, theCounterHours);
  }
  // ------------------------
#ifdef DESKTOP_MODULE
  if (pcfAux.readButton(pinAux2down) == 0) {
#else
  // Tablet:
  if (digitalRead(DOWNLOAD_SWITCH_PIN) == LOW) {
#endif
    if (!switchAux2down) {
      switchAux2down = true;
      // Serial.print(F("+ Clock, AUX2 down switch pressed..."));
    }
  } else if (switchAux2down) {
    switchAux2down = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Clock, AUX2 up, switched. Enter clock timer mode."));
    Serial.println("");
#endif
    clockState = CLOCK_TIMER;
    timerStatus = timerStatus & M1_OFF;
    lightsStatusAddressData(timerStatus, timerDataAddress, timerCounter);
  }
}

// ---------------------------------------------------------
void clockTimerControls() {
  // Timer options
  // -------------------
  if (pcfControl.readButton(pinStop) == 0) {
    if (!switchStop) {
      switchStop = true;
    }
  } else if (switchStop) {
    switchStop = false;
    // Switch logic
    timerStatus = timerStatus & M1_OFF;
    // Set the timer bit on, so that it is displayed.
    clockTimerAddress = bitWrite(clockTimerAddress, clockTimerCount, 1);
    lightsStatusAddressData(timerStatus, clockTimerAddress, timerCounter);
  }
  // -------------------
  if (pcfControl.readButton(pinRun) == 0) {
    if (!switchRun) {
      switchRun = true;
    }
  } else if (switchRun) {
    switchRun = false;
    // Switch logic
    //
    // Check if there are any timer array values set.
    int timerDataTotal = 0;
    for (int i = 1; i < timerTop; i++) {
      timerDataTotal = timerDataTotal + timerData[i];
    }
    if (timerDataTotal == 0) {
      // Since no timer array values set, use quick timer option.
      timerMinute = getMinuteValue(toggleAddress());
      timerCounter = 0;
    } else {
      timerMinute = getMinuteValue(timerData[timerCounter]);
    }
    clockSetTimer(timerMinute);
    //
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Clock Timer, RUN. Timer minutes="));
    Serial.print(timerMinute);
    Serial.println("");
#endif
  }
  // -------------------
  if (pcfControl.readButton(pinExamine) == 0) {
    if (!switchExamine) {
      switchExamine = true;
    }
  } else if (switchExamine) {
    switchExamine = false;
    // Switch logic
    timerCounter = toggleData();                      // Timer array index counter.
    timerDataAddress = timerData[timerCounter];       // Timer array index data.
    lightsStatusAddressData(timerStatus, timerDataAddress, timerCounter);
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Clock Timer, Examine: timerCounter="));
    Serial.print(timerCounter);
    Serial.print(F(", timerDataAddress="));
    Serial.println(timerDataAddress);
#endif
  }
  // -------------------
  if (pcfControl.readButton(pinExamineNext) == 0) {
    if (!switchExamineNext) {
      switchExamineNext = true;
    }
  } else if (switchExamineNext) {
    switchExamineNext = false;
    // Switch logic
    timerCounter++;                                   // Timer array index counter.
    timerDataAddress = timerData[timerCounter];       // Timer array index data.
    lightsStatusAddressData(timerStatus, timerDataAddress, timerCounter);
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Clock Timer, Examine Next: timerCounter="));
    Serial.print(timerCounter);
    Serial.print(F(", timerDataAddress="));
    Serial.println(timerDataAddress);
#endif
  }
  // -------------------
  if (pcfControl.readButton(pinDeposit) == 0) {
    if (!switchDeposit) {
      switchDeposit = true;
    }
  } else if (switchDeposit) {
    switchDeposit = false;
    // Switch logic
    timerDataAddress = toggleAddress();               // Timer array index data value.
    timerData[timerCounter] = timerDataAddress;       // Set data value into the Timer array.
    lightsStatusAddressData(timerStatus, timerDataAddress, timerCounter);
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Clock Timer, Deposit: timerCounter="));
    Serial.print(timerCounter);
    Serial.print(F(", timerDataAddress="));
    Serial.println(timerDataAddress);
#endif
  }
  // -------------------
  if (pcfControl.readButton(pinDepositNext) == 0) {
    if (!switchDepositNext) {
      switchDepositNext = true;
    }
  } else if (switchDepositNext) {
    switchDepositNext = false;
    // Switch logic
    timerCounter++;                                   // Timer array index counter.
    timerDataAddress = toggleAddress();               // Timer array index data value.
    timerData[timerCounter] = timerDataAddress;       // Set data value into the Timer array.
    lightsStatusAddressData(timerStatus, timerDataAddress, timerCounter);
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Clock Timer, Deposit Next: timerCounter="));
    Serial.print(timerCounter);
    Serial.print(F(", timerDataAddress="));
    Serial.println(timerDataAddress);
#endif
  }
  // -------------------
  if (pcfControl.readButton(pinReset) == 0) {
    if (!switchReset) {
      switchReset = true;
    }
  } else if (switchReset) {
    switchReset = false;
    // Switch logic
    timerCounter = 1;                                 // Timer array index counter.
    timerDataAddress = timerData[timerCounter];       // Timer array index data.
    lightsStatusAddressData(timerStatus, timerDataAddress, timerCounter);
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Clock Timer, RESET: timerCounter="));
    Serial.print(timerCounter);
    Serial.print(F(", timerDataAddress="));
    Serial.println(timerDataAddress);
#endif
  }
  // -------------------
  if (pcfAux.readButton(pinClr) == 0) {
    if (!switchClr) {
      switchClr = true;
    }
  } else if (switchClr) {
    switchClr = false;
    // Switch logic
    // -------------------------------------------------------
    // Double flip confirmation.
    digitalWrite(WAIT_PIN, HIGH);
    switchClr = false;      // Required to reset the switch state for confirmation.
    boolean confirmChoice = false;
    unsigned long timer = millis();
    while (!confirmChoice && (millis() - timer < 1000)) {
      if (pcfAux.readButton(pinClr) == 0) {
        if (!switchClr) {
          switchClr = true;
        }
      } else if (switchClr) {
        switchClr = false;
        // Switch logic.
        confirmChoice = true;
      }
      delay(100);
    }
    if (!confirmChoice) {
      digitalWrite(WAIT_PIN, LOW);
      return;
    }
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ Choice confirmed."));
#endif
    digitalWrite(WAIT_PIN, LOW);
    // -------------------------------------------------------
    // Zero out timer array.
    int timerDataTotal = 0;
    for (int i = 1; i < timerTop; i++) {
      timerData[i] = 0;
    }
    // Reset
    timerCounter = 1;                                 // Timer array index counter.
    timerDataAddress = timerData[timerCounter];       // Timer array index data.
    lightsStatusAddressData(timerStatus, timerDataAddress, timerCounter);
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Clock Timer, CLR: timerCounter="));
    Serial.print(timerCounter);
    Serial.print(F(", timerDataAddress="));
    Serial.println(timerDataAddress);
#endif
  }
  // ------------------------
#ifdef DESKTOP_MODULE
  if (pcfAux.readButton(pinAux2up) == 0) {
#else
  // Tablet:
  if (digitalRead(UPLOAD_SWITCH_PIN) == LOW) {
#endif
    if (!switchAux2up) {
      switchAux2up = true;
      // Serial.print(F("+ Clock Timer, AUX2 up switch pressed..."));
    }
  } else if (switchAux2up) {
    switchAux2up = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Clock Timer, AUX2 up, switched."));
    Serial.println("");
#endif
  }
  // ------------------------
#ifdef DESKTOP_MODULE
  if (pcfAux.readButton(pinAux2down) == 0) {
#else
  // Tablet:
  if (digitalRead(DOWNLOAD_SWITCH_PIN) == LOW) {
#endif
    if (!switchAux2down) {
      switchAux2down = true;
      // Serial.print(F("+ Clock Timer, AUX2 down switch pressed..."));
    }
  } else if (switchAux2down) {
    switchAux2down = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Clock Timer, AUX2 up, switched. Exit clock timer mode, back to clock mode."));
    Serial.println("");
#endif
    clockState = CLOCK_TIME;
    displayTheTime(theCounterMinutes, theCounterHours);
  }
}

// ------------------------------------------------
void checkClockSwitch() {
#ifdef DESKTOP_MODULE
  if (pcfAux.readButton(pinAux1up) == 0) {
#else
  // Tablet:
  if (digitalRead(CLOCK_SWITCH_PIN) == LOW) {
#endif
    if (!switchAux1up) {
      switchAux1up = true;
      // Serial.print(F("+ pinAux1up switch pressed..."));
    }
  } else if (switchAux1up) {
    switchAux1up = false;
    // Switch logic.
    if (programState == CLOCK_RUN) {
      Serial.println(F("+ Return to the Altair 8800 emulator."));
      programState = PROGRAM_WAIT;
      digitalWrite(HLDA_PIN, LOW);
      digitalWrite(WAIT_PIN, HIGH);
      programLights();                // Restore the front panel lights.
    } else {
      Serial.println(F("+ Clock mode."));
      programState = CLOCK_RUN;
      digitalWrite(HLDA_PIN, HIGH);
      digitalWrite(WAIT_PIN, LOW);
      lightsStatusAddressData(0, 0, 0);  // Clear the front panel lights.
    }
  }
}
String getSenseSwitchValue() {
  byte bValue = toggleSense();
  String sValue = String(bValue, BIN);
  int addZeros = 8 - sValue.length();
  for (int i = 0; i < addZeros; i++) {
    sValue = "0" + sValue;
  }
  return sValue;
}

// -----------------------------------------------------
void clockRun() {
  Serial.println(F("+ clockRun()"));
  playerPlaySound(CLOCK_ON);
  saveClearLcdScreenData();
  lcd.noCursor();
  syncCountWithClock();
  displayTheTime(theCounterMinutes, theCounterHours);
  while (programState == CLOCK_RUN) {
    switch (clockState) {
      // ----------------------------
      case CLOCK_TIME:
        processClockNow();
        checkClockControls();     // Clock and timer controls.
        // Control buttons for setting the time.
        // checkExamineButton();
        // checkExamineNextButton();
        // checkDepositButton();
        // checkDepositNextButton();
        break;
      case CLOCK_TIMER:
        clockTimerControls();     // Clock and timer controls.
        if (timerStatus & M1_ON) {
          // Timer is running when timerStatus: M1_ON.
          clockRunTimer();
        }
        break;
      case CLOCK_SET:
        break;
    }
    checkClockSwitch();       // Option to exit clock mode.
    checkPlayerSwitch();      // If AUX1 is flipped down, switch to player mode.
    checkProtectSetVolume();
    delay(100);
  }
  playerPlaySound(CLOCK_OFF);
  restoreLcdScreenData();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// SD card processor memory read/write from/into a file.

// Write processor memory to an SD card file.
boolean confirmWrite = false;
void checkUploadSwitch() {
#ifdef DESKTOP_MODULE
  if (pcfAux.readButton(pinAux2up) == 0) {
#else
  // Tablet:
  if (digitalRead(UPLOAD_SWITCH_PIN) == LOW) {
#endif
    if (!switchAux2up) {
      switchAux2up = true;
    }
  } else if (switchAux2up) {
    switchAux2up = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ AUX2 up, Upload Switched."));
#endif
    String senseSwitchValue = getSenseSwitchValue();
    String theFilename = senseSwitchValue + ".bin";
    if (theFilename == "11111111.bin") {
      Serial.println(F("- Warning, disabled, write to filename: 11111111.bin."));
      ledFlashError();
      return;
    }
    digitalWrite(HLDA_PIN, HIGH);
    saveClearLcdScreenData();
    lcdPrintln(0, "Confirm write> ");
    //             1234567890123456
    lcdPrintln(1, "File: " + senseSwitchValue);
    //
    // -------------------------------------------------------
    // Double flip to write.
    confirmWrite = false;
    switchAux2up = false; // Required to reset the switch state for confirmation.
    unsigned long timer = millis();
    while (!confirmWrite && (millis() - timer < 1200)) {
      // checkConfirmUploadSwitch();
#ifdef DESKTOP_MODULE
      if (pcfAux.readButton(pinAux2up) == 0) {
#else
      // Tablet:
      if (digitalRead(UPLOAD_SWITCH_PIN) == LOW) {
#endif
        if (!switchAux2up) {
          switchAux2up = true;
        }
      } else if (switchAux2up) {
        switchAux2up = false;
        confirmWrite = true;
      }
      delay(100);
    }
    if (!confirmWrite) {
      // Serial.print(F("+ Write cancelled."));
      lcdPrintln(1, "Write cancelled.");
      //             1234567890123456
      digitalWrite(HLDA_PIN, LOW);
      return;
    }
    // -------------------------------------------------------
    Serial.print(F("+ Write memory to filename: "));
    Serial.println(theFilename);
    writeProgramMemoryToFile(theFilename);
    // delay(2000); // Give to read the resulting message.
    restoreLcdScreenData();
  }
  digitalWrite(HLDA_PIN, LOW);
}

// -----------------------------------------------------
void checkDownloadSwitch() {
#ifdef DESKTOP_MODULE
  if (pcfAux.readButton(pinAux2down) == 0) {
#else
  // Tablet:
  if (digitalRead(DOWNLOAD_SWITCH_PIN) == LOW) {
#endif
    if (!switchAux2down) {
      switchAux2down = true;
      // Serial.print(F("+ AUX2 down switch pressed..."));
    }
  } else if (switchAux2down) {
    switchAux2down = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ AUX2 down, Download Switched."));
#endif
    String theFilename = getSenseSwitchValue() + ".bin";
    if (theFilename == "11111111.bin") {
      Serial.println(F("+ Set to download over the serial port."));
      programState = SERIAL_DOWNLOAD;
      return;
    }
    digitalWrite(HLDA_PIN, HIGH);
    // -------------------------------------------------------
    // Double flip confirmation.
    switchAux2down = false;      // Required to reset the switch state for confirmation.
    boolean confirmChoice = false;
    unsigned long timer = millis();
    while (!confirmChoice && (millis() - timer < 1200)) {
#ifdef DESKTOP_MODULE
      if (pcfAux.readButton(pinAux2down) == 0) {
#else
      // Tablet:
      if (digitalRead(DOWNLOAD_SWITCH_PIN) == LOW) {
#endif
        if (!switchAux2down) {
          switchAux2down = true;
        }
      } else if (switchAux2down) {
        switchAux2down = false;
        // Switch logic.
        confirmChoice = true;
      }
      delay(100);
    }
    if (!confirmChoice) {
      digitalWrite(HLDA_PIN, LOW);
      return;
    }
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ Choice confirmed."));
#endif
    // -------------------------------------------------------
    if (theFilename == "00000000.bin") {
#ifdef SWITCH_MESSAGES
      Serial.println(F("+ Zero out memory."));
#endif
      //             1234567890123456
      lcdPrintln(1, "Zero out memory");
      zeroOutMemory();
      controlResetLogic();
      digitalWrite(HLDA_PIN, LOW);
      return;
    }
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Read the filename into memory: "));
    Serial.println(theFilename);
#endif
    if (readProgramFileIntoMemory(theFilename)) {
      playerPlaySound(READ_FILE);
      ledFlashSuccess();
      delay(1500);                // Delay depends on the time length of the MP3 play time.
    }
    digitalWrite(HLDA_PIN, LOW);
  }
}

// --------------------------------------------------------
// --------------------------------------------------------
// Check Front Panel Control Switches, when in Player mode.

void checkProtectSetVolume() {
  // When not in player mode, used to change the volume.
  // -------------------
  if (pcfAux.readButton(pinProtect) == 0) {
    if (!switchProtect) {
      switchProtect = true;
    }
  } else if (switchProtect) {
    switchProtect = false;
    //
    // Switch logic, based on programState.
    if (playerVolume > 1) {
      playerVolume--;
    }
    mp3player.volume(playerVolume);
    // playerLights();
    // delay(300);
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player, decrease volume to "));
    Serial.println(playerVolume);
#endif
  }
  // -------------------
  if (pcfAux.readButton(pinUnProtect) == 0) {
    if (!switchUnProtect) {
      switchUnProtect = true;
    }
  } else if (switchUnProtect) {
    switchUnProtect = false;
    // Switch logic
    if (playerVolume < 30) {
      playerVolume++;
    }
    mp3player.volume(playerVolume);
    // playerLights();
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player, increase volume to "));
    Serial.println(playerVolume);
#endif
  }
}

void checkPlayerControls() {
  // -------------------
  if (pcfControl.readButton(pinStop) == 0) {
    if (!switchStop) {
      switchStop = true;
    }
  } else if (switchStop) {
    switchStop = false;
    // Switch logic
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ Player, STOP: stop playing."));
#endif
    mp3player.pause();
    playerStatus = playerStatus | HLTA_ON;
    playerLights();
  }
  // -------------------
  if (pcfControl.readButton(pinRun) == 0) {
    if (!switchRun) {
      switchRun = true;
    }
  } else if (switchRun) {
    switchRun = false;
    // Switch logic
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player, RUN: start playing, playerCounter="));
    Serial.println(playerCounter);
#endif
    // mp3player.play(playerCounter);
    mp3player.start();
    playerStatus = playerStatus & HLTA_OFF;
    playerLights();
  }
  // -------------------
  if (pcfControl.readButton(pinExamine) == 0) {
    if (!switchExamine) {
      switchExamine = true;
    }
  } else if (switchExamine) {
    switchExamine = false;
    // Switch logic
    if (playerCounter > 1) {
      playerCounter--;
    } else {
      playerCounter = playerCounterTop;
    }
    if (!(playerStatus & HLTA_ON)) {
      mp3player.play(playerCounter);
    }
    playerLights();
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player, Examine: play previous song, playerCounter="));
    Serial.println(playerCounter);
#endif
  }
  // -------------------
  if (pcfControl.readButton(pinExamineNext) == 0) {
    if (!switchExamineNext) {
      switchExamineNext = true;
    }
  } else if (switchExamineNext) {
    switchExamineNext = false;
    // Switch logic
    if (playerCounter < playerCounterTop) {
      playerCounter++;
    } else {
      playerCounter = 1;
    }
    if (!(playerStatus & HLTA_ON)) {
      mp3player.play(playerCounter);
    }
    playerLights();
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player, Examine Next: play next song, playerCounter="));
    Serial.println(playerCounter);
#endif
  }
  // -------------------
  if (pcfControl.readButton(pinStep) == 0) {
    if (!switchStep) {
      switchStep = true;
    }
  } else if (switchStep) {
    switchStep = false;
    // Switch logic
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player, Single Step, "));
#endif
    if (loopSingle) {
#ifdef SWITCH_MESSAGES
      Serial.print(F("Toggle loop a single song: off, song# "));
#endif
      loopSingle = false;
      // mp3player.disableLoop();
      playerStatus = playerStatus & M1_OFF;
    } else {
#ifdef SWITCH_MESSAGES
      Serial.print(F("Toggle loop a single song: on, song# "));
#endif
      loopSingle = true;
      // mp3player.loop(playerCounter);
      // playerStatus = playerStatus & HLTA_OFF;
      playerStatus = playerStatus | M1_ON;
    }
    playerLights();
#ifdef SWITCH_MESSAGES
    Serial.println(playerCounter);
#endif
  }
  // -------------------
  if (pcfAux.readButton(pinStepDown) == 0) {
    if (!switchStepDown) {
      switchStepDown = true;
    }
  } else if (switchStepDown) {
    switchStepDown = false;
    // Switch logic
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Toggle loop a single song using mp3player.loop("));
    Serial.print(playerCounter);
    Serial.println(F(")"));
#endif
    loopSingle = false;
    mp3player.loop(playerCounter);
    playerStatus = playerStatus & M1_OFF;
    playerLights();
  }
  // -------------------
  if (pcfControl.readButton(pinReset) == 0) {
    if (!switchReset) {
      switchReset = true;
    }
  } else if (switchReset) {
    switchReset = false;
    // Switch logic
    playerDirectory = 1;
    playerCounter = 1;
    if (!(playerStatus & HLTA_ON)) {
      mp3player.play(playerCounter);
    }
    playerLights();
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player, RESET: play first song, playerCounter="));
    Serial.println(playerCounter);
#endif
  }
  // -------------------
  if (pcfAux.readButton(pinProtect) == 0) {
    if (!switchProtect) {
      switchProtect = true;
    }
  } else if (switchProtect) {
    switchProtect = false;
    //
    // Switch logic, based on programState.
    if (playerVolume > 1) {
      playerVolume--;
    }
    mp3player.volume(playerVolume);
    playerLights();
    // delay(300);
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player, decrease volume to "));
    Serial.println(playerVolume);
#endif
  }
  // -------------------
  if (pcfAux.readButton(pinUnProtect) == 0) {
    if (!switchUnProtect) {
      switchUnProtect = true;
    }
  } else if (switchUnProtect) {
    switchUnProtect = false;
    // Switch logic
    if (playerVolume < 30) {
      playerVolume++;
    }
    mp3player.volume(playerVolume);
    playerLights();
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player, increase volume to "));
    Serial.println(playerVolume);
#endif
  }
  // -------------------
  if (pcfAux.readButton(pinClr) == 0) {
    if (!switchClr) {
      switchClr = true;
    }
  } else if (switchClr) {
    switchClr = false;
    // Switch logic
    unsigned int addressToggles = toggleAddress();
    if (addressToggles > 0 && addressToggles <= playerCounterTop) {
      playerCounter = addressToggles;
      if (!(playerStatus & HLTA_ON)) {
        mp3player.play(playerCounter);
      }
      playerLights();
#ifdef SWITCH_MESSAGES
      Serial.print(F("+ Player, CLR: play a specific song number, playerCounter="));
      Serial.println(playerCounter);
#endif
    } else {
      ledFlashError();
#ifdef SWITCH_MESSAGES
      Serial.print(F("+ Player, CLR: toggle address out of player file counts range: "));
      Serial.print(addressToggles);
      Serial.print(F(", player file counts: "));
      Serial.println(playerCounterTop);
#endif
    }
  }

  // -------------------
  if (pcfControl.readButton(pinDeposit) == 0) {
    if (!switchDeposit) {
      switchDeposit = true;
    }
  } else if (switchDeposit) {
    switchDeposit = false;
    // Switch logic
    playerStatus = playerStatus & HLTA_OFF;
    if (playerDirectory > 1) {
      playerDirectory --;
    } else if (playerDirectoryTop > 0) {
      // If playerDirectoryTop is set, then loop to the last directory.
      playerDirectory = playerDirectoryTop;
    }
    int currentPlayerCounter = playerCounter;
    mp3player.loopFolder(playerDirectory);
    delay(300);
    playerCounter = mp3player.readCurrentFileNumber();
    int i = 0;
    while ((i++ < 3) && ((playerCounter >= playerCounterTop) || playerCounter == currentPlayerCounter)) {
      delay(300);
      playerCounter = mp3player.readCurrentFileNumber();
    }
    playerLights();
    //
    // Disable the loop function so that playerCounter will be used for next MP3.
    // mp3player.disableLoop();
    //
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player, Deposit: play previous folder# "));
    Serial.print(playerDirectory);
    Serial.print(F(", playerCounter="));
    Serial.println(playerCounter);
#endif
  }
  // -------------------
  if (pcfControl.readButton(pinDepositNext) == 0) {
    if (!switchDepositNext) {
      switchDepositNext = true;
    }
  } else if (switchDepositNext) {
    switchDepositNext = false;
    // Switch logic
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player, Deposit Next"));
#endif
    playerStatus = playerStatus & HLTA_OFF;
    int currentPlayerCounter = playerCounter;
    //
    playerDirectory ++;
    mp3player.loopFolder(playerDirectory);
    delay(300);
    playerCounter = mp3player.readCurrentFileNumber();
    int i = 0;
    while ((i++ < 3) && ((playerCounter >= playerCounterTop) || playerCounter == currentPlayerCounter)) {
      delay(300);
      playerCounter = mp3player.readCurrentFileNumber();
      if ((currentPlayerCounter + 1) >= playerCounter) {
        // + Player, Deposit Next play next folder# 8, playerCounter,pre=59,post=84
        // + Player, Deposit Next play next folder# 9, playerCounter,pre=84,post=86
        playerDirectory--;
        playerDirectoryTop = playerDirectory;
        playerDirectory = 1;
        playerCounter = 1;
        mp3player.loopFolder(playerDirectory);
        // mp3player.play(playerCounter);
        delay(300);
      }
    }
    playerLights();
    //
    // Disable the loop function so that playerCounter will be used for next MP3.
    // mp3player.disableLoop();
    //
#ifdef SWITCH_MESSAGES
    Serial.print(F(" play next folder# "));
    Serial.print(playerDirectory);
    Serial.print(F(", playerCounter,pre="));
    Serial.print(currentPlayerCounter);
    Serial.print(F(",post="));
    Serial.println(playerCounter);
#endif
  }
  // ------------------------
#ifdef DESKTOP_MODULE
  if (pcfAux.readButton(pinAux2up) == 0) {
#else
  // Tablet:
  if (digitalRead(UPLOAD_SWITCH_PIN) == LOW) {
#endif
    if (!switchAux2up) {
      switchAux2up = true;
      // Serial.print(F("+ Player AUX2 up switch pressed..."));
    }
  } else if (switchAux2up) {
    switchAux2up = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player AUX2 up, switched."));
    Serial.println("");
#endif
  }
  // ------------------------
#ifdef DESKTOP_MODULE
  if (pcfAux.readButton(pinAux2down) == 0) {
#else
  // Tablet:
  if (digitalRead(DOWNLOAD_SWITCH_PIN) == LOW) {
#endif
    if (!switchAux2down) {
      switchAux2down = true;
      // Serial.print(F("+ Player, AUX2 down switch pressed..."));
    }
  } else if (switchAux2down) {
    switchAux2down = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player, AUX2 down, switched. Go into player file mode."));
    Serial.println("");
#endif
    playerState = PLAYER_FILE;
    digitalWrite(WAIT_PIN, HIGH);
    playerFileLights();
  }
  // -------------------
}

void checkPlayerFileControls() {
  // -------------------
  if (pcfControl.readButton(pinStop) == 0) {
    if (!switchStop) {
      switchStop = true;
    }
  } else if (switchStop) {
    switchStop = false;
    // Switch logic
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ Player, STOP: stop playing."));
#endif
    mp3player.pause();
    playerFileStatus = MEMR_ON | HLTA_ON;
    playerFileLights();
  }
  // -------------------
  if (pcfControl.readButton(pinRun) == 0) {
    if (!switchRun) {
      switchRun = true;
    }
  } else if (switchRun) {
    switchRun = false;
    // Switch logic
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player, RUN: start playing, playerFileData="));
    Serial.println(playerFileData);
#endif
    mp3player.play(playerFileData);
    playerFileStatus = MEMR_ON & HLTA_OFF;
    playerFileLights();
  }
  // -------------------
  if (pcfControl.readButton(pinExamine) == 0) {
    if (!switchExamine) {
      switchExamine = true;
    }
  } else if (switchExamine) {
    switchExamine = false;
    // Switch logic.
    playerFileAddress = toggleData();
    String sfbFilename = getSfbFilename(playerFileAddress);
    playerFileData = readFileByte(sfbFilename);
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player file, EXAMINE, switched. Data filename = "));
    Serial.print(sfbFilename);
    Serial.print(F(", file data byte="));
    printByte(playerFileData);
    Serial.print(F("="));
    Serial.print(playerFileData);
    Serial.println("");
#endif
    playerFileStatus = MEMR_ON | INP_ON | HLTA_ON;
    playerFileLights();
#ifdef SWITCH_MESSAGES
    // -------------------
    // For debugging player issues.
    /*
      Serial.print(F("+ "));
      Serial.print(F(" playerStatus="));
      printByte(playerStatus);
      Serial.print(F(" playerVolume="));
      Serial.print(playerVolume);
      Serial.print(F(" playerCounterTop="));
      Serial.print(playerCounterTop);
      Serial.print(F(" playerDirectory="));
      Serial.print(playerDirectory);
      Serial.print(F(" playerCounter="));
      Serial.print(playerCounter);
      Serial.print(F(", mp3player.read()="));
      Serial.print(mp3player.read());
      if (mp3player.available()) {
        Serial.print(F(", mp3player available"));
        int theType = mp3player.readType();
        if (theType == DFPlayerPlayFinished) {
          Serial.print(F(", Play Finished."));
        } else {
          // printDFPlayerMessage(theType, mp3player.read());
          Serial.print(F(", theType="));
          Serial.print(theType);
        }
      } else {
        // Song is playing or paused.
        Serial.print(F(", mp3player not available."));
      }
      Serial.println();
    */
#endif
  }
  // -------------------
  if (pcfControl.readButton(pinExamineNext) == 0) {
    if (!switchExamineNext) {
      switchExamineNext = true;
    }
  } else if (switchExamineNext) {
    switchExamineNext = false;
    // Switch logic
    playerFileAddress ++;
    String sfbFilename = getSfbFilename(playerFileAddress);
    playerFileData = readFileByte(sfbFilename);
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player file, EXAMINE, switched. Data filename = "));
    Serial.print(sfbFilename);
    Serial.print(F(", file data byte="));
    printByte(playerFileData);
    Serial.print(F("="));
    Serial.print(playerFileData);
    Serial.println("");
#endif
    playerFileStatus = MEMR_ON | INP_ON | HLTA_ON;
    playerFileLights();
  }
  // -------------------
  if (pcfControl.readButton(pinDeposit) == 0) {
    if (!switchDeposit) {
      switchDeposit = true;
    }
  } else if (switchDeposit) {
    switchDeposit = false;
    // Switch logic.
    playerFileData = toggleData();
    String sfbFilename = getSfbFilename(playerFileAddress);
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player file, DEPOSIT, switched. Data filename = "));
    Serial.print(sfbFilename);
    Serial.print(F(", file data byte="));
    printByte(playerFileData);
    Serial.print(F("="));
    Serial.print(playerFileData);
    Serial.println("");
#endif
    if (!writeFileByte(sfbFilename, playerFileData)) {
      Serial.println("- Failed to write sound file: ");
    }
    playerFileStatus = MEMR_ON | INP_ON | WO_ON | HLTA_ON;
    playerFileLights();
  }
  // -------------------
  if (pcfControl.readButton(pinDepositNext) == 0) {
    if (!switchDepositNext) {
      switchDepositNext = true;
    }
  } else if (switchDepositNext) {
    switchDepositNext = false;
    // Switch logic
    playerFileData = toggleData();
    playerFileAddress++;
    String sfbFilename = getSfbFilename(playerFileAddress);
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player file, DEPOSIT NEXT, switched. Data filename = "));
    Serial.print(sfbFilename);
    Serial.print(F(", file data byte="));
    printByte(playerFileData);
    Serial.print(F("="));
    Serial.print(playerFileData);
    Serial.println("");
#endif
    if (!writeFileByte(sfbFilename, playerFileData)) {
      Serial.println("- Failed to write sound file: ");
    }
    playerFileStatus = MEMR_ON | INP_ON | WO_ON | HLTA_ON;
    playerFileLights();
  }
  // -------------------
  if (pcfControl.readButton(pinReset) == 0) {
    if (!switchReset) {
      switchReset = true;
    }
  } else if (switchReset) {
    switchReset = false;
    // Switch logic
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ Player file, Reset. Load sound bite index array."));
#endif
    for (int i = 0; i < maxSoundEffects; i++) {
      soundEffects[i] = readFileByte(getSfbFilename(i));
    }
    ledFlashSuccess();
    playerFileStatus = MEMR_ON | WO_ON | HLTA_ON;
    playerFileLights();
  }
  // ------------------------
#ifdef DESKTOP_MODULE
  if (pcfAux.readButton(pinAux2down) == 0) {
#else
  // Tablet:
  if (digitalRead(DOWNLOAD_SWITCH_PIN) == LOW) {
#endif
    if (!switchAux2down) {
      switchAux2down = true;
      // Serial.print(F("+ Player file, UX2 down switch pressed..."));
    }
  } else if (switchAux2down) {
    switchAux2down = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Player file, AUX2 down, switched. Exit player file mode."));
    Serial.println("");
#endif
    playerState = PLAYER_MP3;
    digitalWrite(WAIT_PIN, LOW);
    playerLights();
  }
}

// -----------------------
void checkPlayerSwitch() {
#ifdef DESKTOP_MODULE
  if (pcfAux.readButton(pinAux1down) == 0) {
#else
  // Tablet:
  if (digitalRead(PLAYER_SWITCH_PIN) == LOW) {
#endif
    if (!switchAux1down) {
      switchAux1down = true;
      // Serial.print(F("+ AUX1 down switch pressed..."));
    }
  } else if (switchAux1down) {
    switchAux1down = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ AUX1 down, Upload Switched."));
#endif
    if (programState == PLAYER_RUN) {
      Serial.println(F("+ Return to the Altair 8800 emulator."));
      programState = PROGRAM_WAIT;
      digitalWrite(HLDA_PIN, LOW);
      digitalWrite(WAIT_PIN, HIGH);
      programLights();                // Restore the front panel lights.
    } else {
      Serial.println(F("+ MP3 player mode."));
      programState = PLAYER_RUN;
      digitalWrite(HLDA_PIN, HIGH);
      digitalWrite(WAIT_PIN, LOW);
      playerState = PLAYER_MP3;       // Default to MP3 player state.
    }
  }
}

// -----------------------------------------
// DFPlayer configuration and error messages.

void printDFPlayerMessage(uint8_t type, int value);
void printDFPlayerMessage(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      mp3player.play(playerCounter);
      playerStatus = playerStatus & HLTA_OFF;
      playerLights();
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError: "));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case FileIndexOut:
          if (loopSingle) {
            // For some reason, turning on single looping (mp3player.loop(playerCounter);)
            //    on the last song causes this error.
            //    This is ignore the error and get the song player.
            mp3player.start();
          } else {
            Serial.println(F("File Index Out of Bound"));
          }
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          Serial.print("+ Assume the directory number was incremented too high, play previous directory: ");
          if (playerDirectory > 1) {
            playerDirectory = 1;
          } else {
            playerDirectory = 1;
          }
          Serial.println(playerDirectory);
          mp3player.loopFolder(playerDirectory);
          break;
        default:
          Serial.print(F("Unknown DFPlayer error message value:"));
          Serial.println (value);
          break;
      }
      break;
    default:
      Serial.print(F("Unknown DFPlayer message type: "));
      Serial.print(type);
      Serial.print(F(", value:"));
      Serial.println(value);
      playerCounter = value;  // The song to play.
      playerLights();
      break;
  }
}

void playMp3() {
  if (mp3player.available()) {
    int theType = mp3player.readType();
    // ------------------------------
    if (theType == DFPlayerPlayFinished) {
      if (loopSingle) {
#ifdef SWITCH_MESSAGES
        Serial.print(F("+ Loop/play the same MP3: "));
#endif
      } else {
#ifdef SWITCH_MESSAGES
        Serial.print(F("+ Play the next MP3: "));
#endif
        if (playerCounter < playerCounterTop) {
          playerCounter++;
        } else {
          playerCounter = 1;
        }
      }
      mp3player.play(playerCounter);
      playerStatus = playerStatus & HLTA_OFF;
      playerLights();
#ifdef SWITCH_MESSAGES
      Serial.println(playerCounter);
#endif
      // ------------------------------
    } else if (theType == DFPlayerCardInserted ) {
      Serial.println(F("+ SD mini card inserted. Start playing"));
      mp3player.start();
    } else {
      // Print the detail message from DFPlayer to handle different errors and states,
      //   such as memory card not inserted.
      printDFPlayerMessage(theType, mp3player.read());
    }
  }
}

void playerRun() {
  saveClearLcdScreenData();
  lcd.noCursor();
  //             1234567890123456
  lcdPrintln(0, "MP3 Player mode,");
  lcdPrintln(1, "Not implemented.");
  //
  Serial.print(F("+ playerRun()"));
  playerPlaySound(PLAYER_ON);
  playerLights();
  Serial.println("");
  while (programState == PLAYER_RUN) {
    delay(60);
    switch (playerState) {
      // ----------------------------
      case PLAYER_MP3:
        // Play MP3 files
        if (!(playerStatus & HLTA_ON)) {
          playMp3();
        }
        checkPlayerControls();      // Player control functions from STOP to UNPROTECT.
        checkPlayerSwitch();        // Toggle player mode: AUX1 down.
        break;
      case PLAYER_FILE:
        checkPlayerFileControls();  // Player control functions from STOP to UNPROTECT.
        checkProtectSetVolume();
        break;
    }
    checkClockSwitch();             // If AUX1 is flipped up, change to clock mode.
  }
  playerPlaySound(PLAYER_OFF);
  restoreLcdScreenData();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Receive bytes through serial port.
// The bytes are loaded into processorr memory.

void DownloadProgram() {
  byte readByte = 0;
  int readByteCount = 0;
  // Set status lights:
  // HLDA on when in this mode. Later, HLDA off (LOW), then on (HIGH) when bytes downloading (Serial.available).
  digitalWrite(HLDA_PIN, HIGH);
  digitalWrite(WAIT_PIN, LOW);
  // INP on
  byte readStatusByte = INP_ON;
  readStatusByte = readStatusByte & M1_OFF;
  lightsStatusAddressData(readStatusByte, 0, 0);
  //
  readByteCount = 0;  // Counter where the downloaded bytes are entered into memory.
  while (programState == SERIAL_DOWNLOAD) {
    if (serial2.available() > 0) {
      // Input on the external serial port module.
      // Read and process an incoming byte.
      Serial.print("++ Byte array number: ");
      if (readByteCount < 10) {
        Serial.print(" ");
      }
      if (readByteCount < 100) {
        Serial.print(" ");
      }
      Serial.print(readByteCount);
      readByte = serial2.read();
      memoryData[readByteCount] = readByte;
      readByteCount++;
      Serial.print(", Byte: ");
      printByte(readByte);
      Serial.print(" ");
      printHex(readByte);
      Serial.print(" ");
      printOctal(readByte);
      Serial.print("   ");
      Serial.print(readByte, DEC);
      Serial.println("");
    }
    if (pcfControlinterrupted) {
      checkRunningButtons();
      pcfControlinterrupted = false; // Reset for next interrupt.
    }
  }
  Serial.print(F("+ Exit serial download state."));
  if (readByteCount > 0) {
    // Program bytes were loaded.
    // Reset the program. This takes care of the case that STOP was used to end the download.
    controlResetLogic();
  } else {
    // Reset to original panel light values.
    programLights();
  }
  digitalWrite(HLDA_PIN, LOW);  // Returning to the emulator.
  if (readByteCount > 0) {
    mp3player.play(117);        // Transfer of data is complete.
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void setup() {

  Serial.begin(115200); // 115200 or 9600
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println(F("+++ Setup."));

  // ----------------------------------------------------
#ifdef SETUP_LCD
  lcdSetup();
  Serial.println(F("+ LCD ready for output."));
  delay(1000);
#endif

  // ----------------------------------------------------
  // DFPlayer serial connection.
  //
  // ------------------------------------
  // Since Mega has its own hardware RX and TX pins,
  //    use pins 18 and 19, which has the label: Serial1.
  // Pin 18(TX) to resister to pin 2(RX).
  // Pin 19(RX) to pin 3(TX).
  Serial1.begin(9600);
  if (!mp3player.begin(Serial1)) {
    Serial.println(F("MP3 Player, unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    hwStatus = 4;
  }
  // ----------------------------------------------------
  // Initial player settings.
  //
  // If I add an SD card, I could save the state.
  //  When starting, set to the previous state.
  //
  // Set player front panel values.
  playerCounter = 1;                // For now, default to song/file 1.
  playerVolume = 16;
  playerStatus = OUT_ON | HLTA_ON;  // OUT_ON  LED status light to indicate the Player.
  playPause = true;                 // HLTA_ON implies that the player is Paused.
  //
  mp3player.volume(playerVolume);   // Set speaker volume from 0 to 30. Doesn't effect DAC output.
  delay(300);
  mp3player.setTimeOut(60);        // Set serial communications time out
  delay(300);
  //
  // DFPLAYER_DEVICE_SD DFPLAYER_DEVICE_U_DISK DFPLAYER_DEVICE_AUX DFPLAYER_DEVICE_FLASH DFPLAYER_DEVICE_SLEEP
  mp3player.outputDevice(DFPLAYER_DEVICE_SD);
  //
  // DFPLAYER_EQ_NORMAL DFPLAYER_EQ_POP DFPLAYER_EQ_ROCK DFPLAYER_EQ_JAZZ DFPLAYER_EQ_CLASSIC DFPLAYER_EQ_BASS
  mp3player.EQ(DFPLAYER_EQ_CLASSIC);
  //
  delay(300);
  playerCounterTop = mp3player.readFileCounts();
  Serial.print(F("+ DFPlayer is initialized. Number of MP3 files = "));
  Serial.println(playerCounterTop);

  // ----------------------------------------------------
  // Front panel toggle switches.

  // ------------------------------
  // System application status LED lights
  pinMode(WAIT_PIN, OUTPUT);    // Indicator: program wait state: LED on or LED off.
  pinMode(HLDA_PIN, OUTPUT);    // Indicator: clock process (LED on) or emulator (LED off).
  digitalWrite(WAIT_PIN, HIGH); // Default to wait state.
  digitalWrite(HLDA_PIN, LOW);  // Default to emulator.

  // AUX device switches.
#ifdef INCLUDE_AUX
  pinMode(CLOCK_SWITCH_PIN, INPUT_PULLUP);
  pinMode(PLAYER_SWITCH_PIN, INPUT_PULLUP);
  pinMode(UPLOAD_SWITCH_PIN, INPUT_PULLUP);
  pinMode(DOWNLOAD_SWITCH_PIN, INPUT_PULLUP);
  Serial.println(F("+ AUX device toggle switches are configured for input."));
#endif

  // ------------------------------
  // I2C Two Wire PCF module initialization
  pcfControl.begin();   // Control switches
  pcfData.begin();      // Tablet: Address/Sense switches
#ifdef DESKTOP_MODULE
  pcfSense.begin();
  pcfAux.begin();
#else
  pinMode(CLOCK_SWITCH_PIN, INPUT_PULLUP);
  pinMode(PLAYER_SWITCH_PIN, INPUT_PULLUP);
  pinMode(UPLOAD_SWITCH_PIN, INPUT_PULLUP);
  pinMode(DOWNLOAD_SWITCH_PIN, INPUT_PULLUP);
  Serial.println(F("+ Tablet AUX device toggle switches are configured for input."));
#endif
  // PCF8574 device interrupt initialization
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), pcfControlinterrupt, CHANGE);
  Serial.println(F("+ Front panel toggle switches are configured for input."));

  // ----------------------------------------------------
  // Status lights are off (statusByte=0) by default.
  // programCounter and curProgramCounter are 0 by default.
  statusByte = statusByte | MEMR_ON;
  statusByte = statusByte | M1_ON;
  statusByte = statusByte | WO_ON;  // WO: on, Inverse logic: off when writing out. On when not.
  dataByte = memoryData[curProgramCounter];
  Serial.println(F("+ Initialized: statusByte, programCounter & curProgramCounter, dataByte."));
  //
  pinMode(latchPinLed, OUTPUT);
  pinMode(clockPinLed, OUTPUT);
  pinMode(dataPinLed, OUTPUT);
  delay(300);
  programLights(); // Uses: statusByte, curProgramCounter, dataByte
  Serial.println(F("+ Front panel LED lights are initialized."));

  // ----------------------------------------------------
#ifdef SETUP_SDCARD
  // The csPin pin is connected to the SD card module select pin (CS).
  //            1234567890123456
#ifdef SETUP_LCD
  lcdPrintln(0, "Init SD card mod");
#endif
  if (!SD.begin(csPin)) {
    Serial.println("- Error initializing SD card module.");
    ledFlashError();
    hwStatus = 1;
  } else {
    Serial.println("+ SD card module is initialized.");
    ledFlashSuccess();
  }
  delay(300);
#endif

  // ----------------------------------------------------
  // Initialize the Real Time Clock (RTC).
#ifdef SETUP_CLOCK
  //            1234567890123456
#ifdef SETUP_LCD
  lcdPrintln(1, "Init Clock");
#endif
  if (!rtc.begin()) {
    Serial.println("- Error: Real time clock not found, not set.");
    ledFlashError();
    hwStatus = 2;
  } else {
    ledFlashSuccess();
    Serial.println("+ Real time clock is initialized.");
  }
  delay(300);
#endif
#ifdef SETUP_LCD
  delay(2000);
#endif

  // ----------------------------------------------------
  // Read sound byte information.
  Serial.println(F("+ Load sound bite index array."));
  for (int i = 0; i < maxSoundEffects; i++) {
    soundEffects[i] = readFileByte(getSfbFilename(i));
  }
  /*
    Serial.println(F("+ List sound bite index array."));
    for (int i = 0; i < 10; i++) {
    Serial.print(F("++ "));
    Serial.print(i);
    Serial.print(F(" "));
    Serial.println(soundEffects[i]);
    }
    Serial.println(F(""));
  */
  // ----------------------------------------------------
  // Read and Run an initialization program.
  // Can manually set 00000000.bin, to all zeros.
  // + The ability to set 00000000.bin, to all zeros.
  // ---------------------------
  // + If 00000000.bin exists, read it.
  // Serial.print(F("+ Program loaded from memory array."));
  readProgramFileIntoMemory("00000000.bin");
  int sumBytes = 0;
  for (int i = 0; i < 32; i++) {
    sumBytes = sumBytes + memoryData[i];
  }
  if (sumBytes > 0) {
    Serial.println(F("++ Since 00000000.bin, has non-zero bytes in the first 32 bytes, run it."));
    programState = PROGRAM_RUN;
    ledFlashSuccess();
  } else {
    // + Else, display the splash screen.
    // lcdSplash();
    controlResetLogic();
    Serial.println(F("++ Program system reset."));
  }
  // ----------------------------------------------------
  Serial.println(F("+ Starting the processor loop."));
}

// -----------------------------------------------------------------------------
// Device Loop for processing each byte of machine code.

void loop() {

  switch (programState) {
    // ----------------------------
    case PROGRAM_RUN:
      processData();
      // Program control: STOP or RESET.
      if (pcfControlinterrupted) {
        checkRunningButtons();
        pcfControlinterrupted = false; // Reset for next interrupt.
      }
      break;
    // ----------------------------
    case PROGRAM_WAIT:
      // Program control: RUN, SINGLE STEP, EXAMINE, EXAMINE NEXT, Examine previous, RESET.
      if (pcfControlinterrupted) {
        checkControlButtons();
        pcfControlinterrupted = false; // Reset for next interrupt.
      }
      checkClockSwitch();
      checkPlayerSwitch();
      checkUploadSwitch();
      checkDownloadSwitch();
      checkProtectSetVolume();
      delay(60);
      break;
    // ----------------------------
    case SERIAL_DOWNLOAD:
      Serial.println(F("+ State: SERIAL_DOWNLOAD"));
      serial2.begin(9600);
      if (serial2.isListening()) {
        Serial.println("+ serial2 is listening.");
        Serial.println("+ Ready to use the second serial port for receiving program bytes.");
        Serial.println("+                 Address  Data  Binary   Hex Octal Decimal");
        //              ++ Byte array number:   0, Byte: 00000110 06  006   6
        //              ++ Byte array number:   0, Byte: 00000110 Octal:006 Decimal6
        DownloadProgram();
      }
      break;
    // ----------------------------
    case CLOCK_RUN:
      Serial.println(F("+ State: CLOCK_RUN"));
      // HLDA on when in this mode.
      digitalWrite(HLDA_PIN, HIGH);
      clockRun();
      digitalWrite(HLDA_PIN, LOW);  // Returning to the emulator.
      break;
    // ----------------------------
    case PLAYER_RUN:
      Serial.println(F("+ State: PLAYER_RUN"));
      // HLDA on when in this mode.
      digitalWrite(HLDA_PIN, HIGH);
      playerRun();
      digitalWrite(HLDA_PIN, LOW);  // Returning to the emulator.
      break;
  }

}
// -----------------------------------------------------------------------------
