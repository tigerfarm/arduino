// -----------------------------------------------------------------------------
// Altair101a Processor program, which is an Altair 8800 simulator.
// Copyright (C) 2021 Stacy David Thurston
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
/*
  Altair101a Processor program

  This program is an Altair 8800 simulator.
  Interactivity is through the default Arduino USB serial port, and optionally, the Serial2 port.
  It was tested using the Arduino IDE serial monitor and Mac terminal which has VT100 features.
  It runs programs from command line, or with a simulated front panel (printed to the serial port).
  This simulator uses David Hansel's Altair 8800 Simulator code to process machine instructions.
  Altair101a program has a basic OS for command line interation.

  Control program files: Altair101a.ino and Altair101a.h.
  Machine instruction program files: cpucore_i8080.cpp and cpucore_i8080.h.

  Differences to the original Altair 8800:
  + HLT goes into STOP state which allows RUN to restart the process from where it was halted.
  + When SINGLE STEP read and write, I've chosen to show the actual data value,
    rather than having all the data lights on, which happens on the original Altair 8800.

  My Arduino Mega 2560 Altair 8800 simulator runs 4K Basic.
  Yes, the Basic language running in 4K of memory.
  It’s the assembler program Bill Gates and Paul Allen wrote, the first Microsoft software product.

  ---------------------------------------------------------
  Next to work on

  Add hardware to display values:
  + Player song.
  + Clock date and time.

  Consider:
  + Rename cpucore_i8080, intel8080cpu.
  + Add MP3 player control call in the RUN loop so that I can be play music while running programs such as Galaxy 101.
  + Adding my own sample assembler programs.
  + SD card in it's own CPP file.

  Update README.md files:
  + Altair101a
  + Galaxy101a
  + 4K Basic
  + programsAltair: programs people can load and run on the Altair101a.

  A Serial2 buffer for reliably uploading basic programs.

  Consider, when using Arduino IDE Serial Monitor, remove duplicate messages.
  + Maybe ignore "CR".

  ---------------------------------------------------------
  Get porgrams to work on an Altari-Duino or actual Altair 8800.
  + Need to get in contact with the online group to have help testing.

  + Get 88-2SIO to work based on Gaxaly101.asm.
  ++ Should test using port 0 and 1 which work with 4K Basic.
  ++ Or convert Gaxaly101.asm to use 4K Basic I/O routines.
  + Consider enhancing how Altair101a uses
    serial I/O to handle 88-2SIO CHANNEL SERIAL INTERFACE functionality
    based on how I use it for 4K Basic.

  ---------------------------------------------------------
  Starting integrating hardware functions using the Altair tablet or desktop.
  + If Altair101 tablet module, get the tablet to work.
  + Test running Altair101b.
  + Then update Altair101b with the ability to flash the front panel lights.
  + Then update Altair101b implementing interactivity with the front panel switches.

  Option to have HLT work the same as Altair 8800.

  Implement interrupt handling.
  + opcodes: ei and di. And handling interupts once ei is run.
  + This is likely required for CPM, but don't know.

  ---------------------------------------------------------
  Integration: continue adding Processor.ino features into Altair101a.

  Front panel: add functions, upload to tablet or desktop machine, and test.
  + Lights
  + Toggles and switches:
  + Add the others: player, clock, timer, and counter.

  ---------------------------------------------------------
  - Consider preventing lockup when using PUSH A, i.e. PUSH called before setting SP.
  + In the PUSH process, if SP < 4, error.

  ---------------------------------------------------------
  Software downloads:
  https://altairclone.com/downloads/
  Download CPM:
  http://cpmarchives.classiccmp.org/cpm/mirrors/www.schorn.ch/cpm/zip/cpm2.zip
  Simulator and method to run CPM:
  http://cpmarchives.classiccmp.org/cpm/mirrors/www.schorn.ch/cpm/intro.php
  Download the simulator with documentation and CP/M 2.2.
  Put all files into one folder.
  Start the simulator and type the command "do cpm2" at the sim> command prompt and CP/M is booted.
  $ ./altairz80 cpm2
  A> DIR
  A> B:

  ---------------------------------------------------------
  VT100 reference:
       http://ascii-table.com/ansi-escape-sequences-vt-100.php
    Esc[H     Move cursor to upper left corner, example: Serial.print(F("\033[H"));
    Esc[J     Clear screen from cursor down, example: Serial.print(F("\033[J"));
    Esc[2J    Clear entire screen, example: Serial.print(F("\033[H"));
    Example:  Serial.print(F("\033[H\033[2J"));         // Move home and clear entire screen.
    Esc[K     Clear line from cursor right
    Esc[nA    Move cursor up n lines.
    Example:  Serial.print(F("\033[3A"));  Cursor Up 3 lines.
    Esc[nB    Move cursor down n lines.
    Example:  Serial.print(F("\033[6B"));  Cursor down 6 lines.
    Esc[nC    Move cursor right n positions.
    Example:  Serial.print(F("\033[H\033[4B\033[2C"));  // Print on: row 4, column 2.
    Esc[r;cH  Move cursor to a specific row(r) and column(c).
    Example:  Serial.print(F("\033[4;2H*"));            // Print on: row 4, column 2 and print "*".

  Reference: printf/sprintf formats:
  http://www.cplusplus.com/reference/cstdio/printf/

  ---------------------------------------------------------
  ---------------------------------------------------------
  Program sections

  + Declarations
    printByte(byte b)
  --------------------------
  + Process functions
    singleStepWait()
    controlResetLogic()
  --------------------------
  + Front panel functions
    printFrontPanel()
  + Input
    altair_in(byte portDataByte)
  + Output
    altair_out(byte portDataByte, byte regAdata)
  --------------------------
  + RUN process mode
    processDataOpcode()
    processRunSwitch(byte readByte)
    runProcessor()
    + Loop the above 2 processes.
  + WAIT processor mode: process switch byte options.
    void processWaitSwitch(byte readByte)
    runProcessorWait()
    + Loop input keys and call processWaitSwitch(inputByte)
  --------------------------
  + Load hardcoded programs
    Early test programs
  + Copy paste into Serial port window.
    Only works for really short programs.
  + Download program bytes through Serial2 port from the laptop.
    modeDownloadProgram()
  + SD card R/W functions
    readProgramFileIntoMemory(String theFilename)
    writeProgramMemoryToFile(String theFilename)
  --------------------------
  + Setup()
  + Looop()

  -----------------------------------------------------------------------------
  Wire the serial module:

  Module  Due/Mega pins:
  TX      RX:17
  RX      TX:16
  VCC     VCC
  GND     GND

  Due pins                TX2:16 RX2:17
   ---------------------------------------
  |                            x x        | GND
  |                                       |
  |                                       | ...
  |                                       |
  |                                       | VCC
   ---------------------------------------

  -----------------------------------------------------------------------------
  Wire the SD Card reader/writer module:

  From the back of the SD Card module, the pins:
  1    2    3    4    5    6
  CS   SCK  MOSI MISO VCC  GND
  53   Blck Brwn Whte red

  From the Front of the SD Card module, the pins:
  GND  VCC  MISO MOSI SCK  CS
       Red  Whte Brwn Blck To pin 53.

  Due SPI center pins. Should work for Mega and Uno as well (not tested).
   ---------------------------------------
  |                                      -| GND
  |                 MISO x x + VCC        |
  |                  SCK x x MOSI         |
  |                RESET x x - GND        |
  |                                      x| pin 53
  |                                      +| VCC
   ---------------------------------------

  -----------------------------------------------------------------------------
  DFPlayer Mini pins

  DFPlayer pin 3 (TX) to a 1K resister. 1K resister to Arduino RX19.
  DFPlayer pin 2 (RX) to a 5K resister. 5K resister to Arduino TX18.
  DFPlayer pin GND    to Arduino GND. Arduino GND to USB hub GND.
  DFPlayer pin VCC    to USB hub VCC.

  RX/TX pin voltage,
  + The Arduino Due has three 3.3V TTL serial ports.
  + Found the suggestion: connect a 1k resistor between the module and MP3 on TX-RX and RX-TX.
    Because DFPlayer Mini Module operating voltage is 3.3V.

         ----------
    VCC |   |  |   | BUSY, low:playing, high:not playing
     RX |    __    | USB port - (DM, clock)
     TX | DFPlayer | USB port + (DP, data)
  DAC_R |          | ADKEY_2 Play fifth segment.
  DAC_L |  ------  | ADKEY_1 Play first segment.
  SPK - | |      | | IO_2 short press, play next. Long press, increase volume.
    GND | |      | | GND
  SPK + | Micro SD | IO_1 short press, play previous. Long press, decrease volume.
         ----------

  Due pins                    TX1:18 RX1:19
   ---------------------------------------
  |                                x x   -| GND
  |                                       |
  |                                       | ...
  |                                       |
  |                                       |
   ---------------------------------------

  USB cable connector to power the DFPlayer.
  From top:
     ---------------
    | VCC D- D+ GND |
    |   -       _   |
    |  | |     | |  |
    |   _       _   |
    |               |
     ---------------
          |  |
          |  |
          |  |
  My USB cable has:
  + A ground wire wrap.
  + Purple VCC, +5 VCC.s

  -----------------------------------------------------------------------------
  Clock module pins

  -----------------------
 |     Curcuit           |
 |     side              |
  -----------------------
   |   |   |   |   |   |
  32K SQW SCL SDA VCC GND

  -----------------------
 |     Battery           |
 |     side              |
  -----------------------
   |   |   |   |   |   |
  GND VCC SDA SCL SQW 32K
  
*/
// -----------------------------------------------------------------------------
#include "Altair101b.h"
#include "cpucore_i8080.h"
#include "Mp3Player.h"
#include "rtClock.h"

// #define LOG_MESSAGES 1    // For debugging.
// #define LOG_OPCODES  1    // Print each called opcode.

// I didn't add to option to remove Serial2 options using a "#define",
//    basically, because it doesn't cause issues.

boolean logMessages = false;  // Input log messages.

byte opcode = 0xff;

// ------------------------------
// Added this to identify hardware status.
// if hardware has an error, or hardware is not initialized, hwStatus > 0.
// Else hwStatus = 0.
byte hwStatus = B11111111;         // Initial state.
const byte SD_ON  =    B00000001;  // 0001 SD card
const byte CL_ON  =    B00000010;  // 0010 Clock module
const byte PL_ON  =    B00000100;  // 0100 MP3 Player
const byte SD_OFF =    B11111110;
const byte CL_OFF =    B11111101;
const byte PL_OFF =    B11111011;

int READ_FILE         = 1;
int TIMER_COMPLETE    = 2;
int RESET_COMPLETE    = 2;
int CLOCK_ON          = 3;
int CLOCK_OFF         = 4;
int PLAYER_ON         = 3;
int PLAYER_OFF        = 4;
int KR5               = 5;
int CLOCK_CUCKOO      = 6;
int TIMER_MINUTE      = 7;
int DOWNLOAD_COMPLETE = 8;
int WRITE_FILE        = 9;
void playerPlaySoundWait(int theFileNumber) {};

void ledFlashSuccess() {};
void ledFlashError() {};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// SD Card setup and functions.

#define SETUP_SDCARD 1
String theFilename;

#ifdef SETUP_SDCARD

// SD Card module is an SPI bus slave device.
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
const int csPin = 53;     // SD Card module is connected to Mega pin 53.

File myFile;

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

// Write Program memory to a file.

void writeProgramMemoryToFile(String theFilename) {
  if (!sdcardInitiated) {
    initSdcard();
  }
  // Serial.print(F("+ Write program memory to a new file named: "));
  // Serial.println(theFilename);
  // Serial.println(F("+ Check if file exists. "));
  if (SD.exists(theFilename)) {
    SD.remove(theFilename);
    Serial.println(F("++ Exists, so it was deleted."));
  }
  myFile = SD.open(theFilename, FILE_WRITE);
  if (!myFile) {
    Serial.print(F("- Error opening file: "));
    Serial.println(theFilename);
    ledFlashError();
    sdcardInitiated = false;
    return; // When used in setup(), causes jump to loop().
  }
  // Serial.println(F("++ New file opened."));
  // Serial.println(F("++ Write binary memory to the file."));
  host_set_status_led_HLDA();
  for (int i = 0; i < MEMSIZE; i++) {
    // myFile.write(memoryData[i]);
    byte memoryData = MREAD(i);
    myFile.write(memoryData);
  }
  myFile.close();
  Serial.println(F("+ Write completed, file closed."));
  ledFlashSuccess();
  playerPlaySoundWait(WRITE_FILE);
  host_clr_status_led_HLDA();
}

// -------------------------------------
// Read program memory from a file.

boolean readProgramFileIntoMemory(String theFilename) {
  if (!sdcardInitiated) {
    initSdcard();
  }
  // Serial.println(F("+ Read a file into program memory, file named: "));
  // Serial.print(theFilename);
  // Serial.println(F("+ Check if file exists. "));
  if (!SD.exists(theFilename)) {
    Serial.print(F("- Read ERROR, file doesn't exist: "));
    Serial.println(theFilename);
    ledFlashError();
    sdcardInitiated = false;
    return (false);
  }
  myFile = SD.open(theFilename);
  if (!myFile) {
    Serial.print(F("- Read ERROR, cannot open file: "));
    Serial.println(theFilename);
    ledFlashError();
    sdcardInitiated = false;
    return (false);
  }
  host_set_status_led_HLDA();
  int i = 0;
  while (myFile.available()) {
    // Reads one character at a time.
    byte memoryData = myFile.read();
    MWRITE(i, memoryData);
#ifdef LOG_MESSAGES
    // Print Binary:Octal:Decimal values.
    Serial.print(F("B"));
    printByte(MREAD(i));
    Serial.print(F(":"));
    printOctal(MREAD(i));
    Serial.print(F(":"));
    Serial.println(MREAD(i), DEC);
#endif
    i++;
    if (i > MEMSIZE) {
      Serial.println(F("-+ Warning, file contains more data bytes than available memory."));
      ledFlashError();
      break;
    }
  }
  myFile.close();
  Serial.println(F("+ Program file loaded and ready to use."));
  controlResetLogic();
  playerPlaySoundWait(READ_FILE);
  host_clr_status_led_HLDA();
  return (true);
}

void printSpacing(String theString) {
  for (int i = theString.length(); i < 14; i++) {
    Serial.print(F(" "));
  }
}

void sdListDirectory() {
  if (!sdcardInitiated) {
    initSdcard();
  }
  // listDirectory(root);
  // List files for a single directory.
  File dir = SD.open("/");
  String tabString = "   ";
  File entry = dir.openNextFile();
  while (entry) {
    if (entry.isDirectory()) {
      Serial.print(F("++ Directory: "));
      Serial.print(entry.name());
    } else {
      Serial.print(F("++ File:      "));
      Serial.print(entry.name());
      printSpacing(entry.name());
      Serial.print(entry.size(), DEC);
    }
    Serial.println();
    entry.close();
    entry =  dir.openNextFile();
  }
  dir.close();
}
#endif  // SETUP_SDCARD

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Types of interactions

// Needs to match the upload program:
//  9600, 19200, 38400, 57600, 115200
unsigned long downloadBaudRate = 115200;

// Virtual serial front panel using VT100 escape codes.
// For example, Macbook terminal is VT100 enabled using the UNIX "screen" command.
boolean SERIAL_FRONT_PANEL = false;

// This option is for VT100 terminal command line interactivity.
// Each character is immediately sent.
// Uses CR instead of LF.
//    For example, Ctrl+l to clear the terminal screen.
boolean SERIAL_CLI = false;

// This option is for using the Arduino IDE monitor.
// It prevents virtual front panel printing, unless requested.
// The monitor requires an enter key to send a string of characters which is then terminated with LF.
boolean SERIAL_IO_IDE = false;

// Hardware LED lights
boolean LED_IO = false;

// -----------------------------------
// Using Serial2 for output.
boolean SERIAL2_OUTPUT = false;

void Serial_println() {
  if (SERIAL2_OUTPUT) {
    Serial2.write('\r');
    Serial2.write('\n');
  } else {
    Serial.println();
  }
}
void Serial_print(byte regAdata) {
  if (SERIAL2_OUTPUT) {
    Serial2.write(regAdata);
  } else {
    Serial.write(regAdata);
  }
}
void Serial_print(String regAdata) {
  if (SERIAL2_OUTPUT) {
    Serial2.print(regAdata);
  } else {
    Serial.print(regAdata);
  }
}
void Serial_write(byte regAdata) {
  if (SERIAL2_OUTPUT) {
    Serial2.print(regAdata);
  } else {
    Serial.print(regAdata);
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
char charBuffer[17];

void printByte(byte b) {
  for (int i = 7; i >= 0; i--) {
    //  Serial_print(bitRead(b, i));
    if (bitRead(b, i)) {
      Serial_print(F("1"));
    } else {
      Serial_print(F("0"));
    }
  }
}
void printOctal(byte b) {
  String sValue = String(b, OCT);
  for (int i = 1; i <= 3 - sValue.length(); i++) {
    Serial_print(F("0"));
  }
  Serial_print(sValue);
}
void printHex(byte b) {
  String sValue = String(b, HEX);
  for (int i = 1; i <= 3 - sValue.length(); i++) {
    Serial_print(F("0"));
  }
  Serial.print(sValue);
}
void printData(byte theByte) {
  sprintf(charBuffer, "%3d = ", theByte);
  Serial_print(charBuffer);
  printOctal(theByte);
  Serial_print(F(" = "));
  printByte(theByte);
}

// Convert a hex string to decimal. Example call:
// int v  = x2i(s);
int x2i(char *s) {
  int x = 0;
  for (;;) {
    char c = *s;
    if (c >= '0' && c <= '9') {
      x *= 16;
      x += c - '0';
    }
    else if (c >= 'A' && c <= 'F') {
      x *= 16;
      x += (c - 'A') + 10;
    }
    else break;
    s++;
  }
  return x;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Front Panel declarations

int programState = LIGHTS_OFF;  // Intial, default.

// ------------------------------
// Status Indicator LED lights

byte readByte = 0;

// Program wait status.
// const int WAIT_PIN = A9;     // Processor program wait state: off/LOW or wait state on/HIGH.
const int WAIT_PIN = 13;        // Change to onboard pin for the Altair101a/b machine.

// HLDA : 8080 processor goes into a hold state because of other hardware running.
// const int HLDA_PIN = A10;     // Emulator processing: off/LOW. Clock or player processing: on/HIGH.

// Use OR to turn ON. Example:
const byte MEMR_ON =    B10000000;  // MEMR   The memory bus will be used for memory read data.
const byte INP_ON =     B01000000;  // INP    The address bus containing the address of an input device. The input data should be placed on the data bus when the data bus is in the input mode
const byte M1_ON =      B00100000;  // M1     Machine cycle 1, fetch opcode.
const byte OUT_ON =     B00010000;  // OUT    The address contains the address of an output device and the data bus will contain the out- put data when the CPU is ready.
const byte HLTA_ON =    B00001000;  // HLTA   Machine opcode hlt, has halted the machine.
const byte STACK_ON =   B00000100;  // STACK  Stack process
const byte WO_ON =      B00000010;  // WO     Write out (inverse logic)
const byte INT_ON =     B00000001;  // INT    Interrupt

void setWaitStatus(boolean waitStatus) {
  if (waitStatus) {
    host_set_status_led_WAIT();
    if (SERIAL_FRONT_PANEL) {
      Serial.print(F("\033[4;2H*"));  // Print on: row 4, column 2.
    }
  } else {
    host_clr_status_led_WAIT();
    if (SERIAL_FRONT_PANEL) {
      Serial.print(F("\033[4;2H."));  // Print off: row 4, column 2.
    }
  }
}

// -----------------------------------------------------------------------------
// Front Panel Status LEDs

// Output LED lights shift register(SN74HC595N) pins
//          Mega/Nano pins       74HC595 Pins
const int dataPinLed  = 5;    // pin ? Data pin.
const int latchPinLed = 6;    // pin ? Latch pin.
const int clockPinLed = 7;    // pin ? Clock pin.

// -------------------------------------------------
void lightsStatusAddressData( byte status8bits, unsigned int address16bits, byte data8bits) {
  digitalWrite(latchPinLed, LOW);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, status8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, data8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, lowByte(address16bits));
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, highByte(address16bits));
  digitalWrite(latchPinLed, HIGH);
  //
#ifdef LOG_MESSAGES
  Serial.print(F("+ lightsStatusAddressData, status:"));
  printByte(status8bits);
  Serial.print(F(" address:"));
  sprintf(charBuffer, "%5d", address16bits);
  Serial.print(charBuffer);
  Serial.print(F(" dataByte:"));
  printByte(data8bits);
  Serial.println();
#endif
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Output Front Panel Information.

void printFrontPanel() {
  uint16_t theAddressWord = 0;
  if (LED_IO) {
    lightsStatusAddressData(fpStatusByte, host_read_addr_leds(), host_read_data_leds());
  }
  if (SERIAL_FRONT_PANEL) {
    serialPrintFrontPanel();
#ifdef LOG_MESSAGES
    // Serial.print(F("\033[2K")); // Clear line from cursor right
    Serial.print(F("+ printFrontPanel SERIAL_FRONT_PANEL, status:"));
    printByte(fpStatusByte);
    //
    uint16_t theAddress = host_read_addr_leds();
    Serial.print(F(" address:"));
    sprintf(charBuffer, "%5d", theAddress);
    Serial.print(charBuffer);
    Serial.print(F("="));
    printByte(highByte(theAddress));
    Serial.print(F(":"));
    printByte(lowByte(theAddress));
    //
    Serial.print(F(" dataByte:"));
    printByte(host_read_data_leds());
    Serial.println();
    // Serial.print(F("\033[2K")); // Clear line
#endif
    /* SERIAL_IO_IDE will do the same:
      } else if (SERIAL_CLI) {
        Serial.print(F("+ printFrontPanel SERIAL_CLI, status:"));
        printByte(fpStatusByte);
        Serial.print(F(" dataByte:"));
        printByte(host_read_data_leds());
        theAddressWord = host_read_addr_leds();
        Serial.print(F(" address:"));
        sprintf(charBuffer, "%5d ", theAddressWord);
        Serial.print(charBuffer);
        printByte(highByte(theAddressWord));
        Serial.print(F(":"));
        printByte(lowByte(theAddressWord));
        Serial.println();
    */
  } else if (SERIAL_IO_IDE) {
    if (host_read_status_led_WAIT()) {
      serialPrintFrontPanel();
    } else {
      Serial.print(F("+ printFrontPanel SERIAL_IO_IDE, status:"));
      printByte(fpStatusByte);
      Serial.print(F(" dataByte:"));
      printByte(host_read_data_leds());
      theAddressWord = host_read_addr_leds();
      Serial.print(F(" address:"));
      sprintf(charBuffer, "%5d ", theAddressWord);
      Serial.print(charBuffer);
      printByte(highByte(theAddressWord));
      Serial.print(F(":"));
      printByte(lowByte(theAddressWord));
      Serial.println();
    }
  }
}

// -----------------------------------------------------------------------------
// Print the front panel to either the Arduino IDE monitor, or to the VT00 terminal.
// In a VT00 terminal, the front panel stays in the top left of the terminal screen.
// In the Arduino IDE monitor, the front panel is reprinted and scrolls.

// byte prev_fpStatusByte = 250;     // This will take more time to figure out.
byte prev_fpDataByte = 1;
uint16_t prev_fpAddressWord = 0;
uint16_t fpAddressToggleWord = 0;
uint16_t prev_fpAddressToggleWord = 1;

void serialPrintFrontPanel() {
  //
  // --------------------------
  // Status
  if (!SERIAL_FRONT_PANEL) {
    Serial.print(F("INTE MEMR INP M1 OUT HLTA STACK WO INT        D7  D6   D5  D4  D3   D2  D1  D0\r\n"));
    // Note, PROT is not implemented.
  } else {
    // No need to rewrite the title.
    Serial.print(F("\033[H"));   // Move cursor home
    Serial.print(F("\033[1B"));  // Cursor down
  }
  String thePrintLine = "";
  if ( host_read_status_led_INTE() ) thePrintLine += ((" *  "));   else thePrintLine += ((" .  "));
  if ( fpStatusByte & MEMR_ON  )     thePrintLine += (("  * "));   else thePrintLine += (( "  . "));
  if ( fpStatusByte & INP_ON   )     thePrintLine += (( "   * ")); else thePrintLine += (( "   . "));
  if ( fpStatusByte & M1_ON    )     thePrintLine += (( " * "));   else thePrintLine += (( " . "));
  if ( fpStatusByte & OUT_ON   )     thePrintLine += (( "  * "));  else thePrintLine += (( "  . "));
  if ( fpStatusByte & HLTA_ON  )     thePrintLine += (( "  * "));  else thePrintLine += (( "  . "));
  if ( fpStatusByte & STACK_ON )     thePrintLine += (( "   * ")); else thePrintLine += (( "   . "));
  if ( fpStatusByte & WO_ON    )     thePrintLine += (( "   * ")); else thePrintLine += (( "   . "));
  if ( fpStatusByte & INT_ON   )     thePrintLine += (( "  *"));   else thePrintLine += (( "  ."));
  thePrintLine += (("       "));
  Serial.print(thePrintLine);
  thePrintLine = "";
  //
  // --------------------------
  // Data
  if ((prev_fpDataByte != fpDataByte) || (!SERIAL_FRONT_PANEL)) {
    // If VT100 and no change, don't reprint.
    prev_fpDataByte = fpDataByte;
    if ( fpDataByte & 0x80 )   thePrintLine += (("  *" ));  else thePrintLine += (( "  ." ));
    if ( fpDataByte & 0x40 )   thePrintLine += (( "   *")); else thePrintLine += (( "   ."));
    thePrintLine += (( " "));
    if ( fpDataByte & 0x20 )   thePrintLine += (( "   *")); else thePrintLine += (( "   ."));
    if ( fpDataByte & 0x10 )   thePrintLine += (( "   *")); else thePrintLine += (( "   ."));
    if ( fpDataByte & 0x08 )   thePrintLine += (( "   *")); else thePrintLine += (( "   ."));
    thePrintLine += (( " "));
    if ( fpDataByte & 0x04 )   thePrintLine += (( "   *")); else thePrintLine += (( "   ."));
    if ( fpDataByte & 0x02 )   thePrintLine += (( "   *")); else thePrintLine += (( "   ."));
    if ( fpDataByte & 0x01 )   thePrintLine += (( "   *")); else thePrintLine += (( "   ."));
    Serial.print(thePrintLine);
    thePrintLine = "";
  }
  //
  // --------------------------
  // WAIT, HLDA, and Address
  //
  if (SERIAL_FRONT_PANEL) {
    // No need to rewrite the title.
    Serial.println();
    Serial.print(F("\033[1B"));  // Cursor down
  } else {
    Serial.print(F("\r\nWAIT HLDA   A15 A14 A13 A12 A11 A10  A9  A8   A7  A6   A5  A4  A3   A2  A1  A0"));
    Serial.print(F("\r\n"));
  }
  // WAIT and HLDA
  if ( host_read_status_led_WAIT() ) thePrintLine += ((" *  "));   else thePrintLine += ((" .  "));
  if ( host_read_status_led_HLDA() ) thePrintLine += (("  *   ")); else thePrintLine += (("  .   "));
  Serial.print(thePrintLine);
  thePrintLine = "";
  //
  // Address
  if ((prev_fpAddressWord != fpAddressWord) || (!SERIAL_FRONT_PANEL)) {
    // If VT100 and no change, don't reprint.
    prev_fpAddressWord = fpAddressWord;
    if ( fpAddressWord & 0x8000 ) thePrintLine += (("   *")); else thePrintLine += (("   ."));
    if ( fpAddressWord & 0x4000 ) thePrintLine += (("   *")); else thePrintLine += (("   ."));
    if ( fpAddressWord & 0x2000 ) thePrintLine += (("   *")); else thePrintLine += (("   ."));
    if ( fpAddressWord & 0x1000 ) thePrintLine += (("   *")); else thePrintLine += (("   ."));
    if ( fpAddressWord & 0x0800 ) thePrintLine += (("   *")); else thePrintLine += (("   ."));
    if ( fpAddressWord & 0x0400 ) thePrintLine += (("   *")); else thePrintLine += (("   ."));
    if ( fpAddressWord & 0x0200 ) thePrintLine += (("   *")); else thePrintLine += (("   ."));
    if ( fpAddressWord & 0x0100 ) thePrintLine += (("   *")); else thePrintLine += (("   ."));
    thePrintLine += ((" "));
    if ( fpAddressWord & 0x0080 ) thePrintLine += (("   *")); else thePrintLine += (("   ."));
    if ( fpAddressWord & 0x0040 ) thePrintLine += (("   *")); else thePrintLine += (("   ."));
    thePrintLine += ((" "));
    if ( fpAddressWord & 0x0020 ) thePrintLine += (("   *")); else thePrintLine += (("   ."));
    if ( fpAddressWord & 0x0010 ) thePrintLine += (("   *")); else thePrintLine += (("   ."));
    if ( fpAddressWord & 0x0008 ) thePrintLine += (("   *")); else thePrintLine += (("   ."));
    thePrintLine += ((" "));
    if ( fpAddressWord & 0x0004 ) thePrintLine += (("   *")); else thePrintLine += (("   ."));
    if ( fpAddressWord & 0x0002 ) thePrintLine += (("   *")); else thePrintLine += (("   ."));
    if ( fpAddressWord & 0x0001 ) thePrintLine += (("   *")); else thePrintLine += (("   ."));
    Serial.print(thePrintLine);
    thePrintLine = "";
  }
  // --------------------------
  // Address/Data switches
  if (SERIAL_FRONT_PANEL) {
    // No need to rewrite the title.
    Serial.println();
    Serial.print(F("\033[1B"));  // Cursor down
  } else {
    Serial.print(F("\r\n            S15 S14 S13 S12 S11 S10  S9  S8   S7  S6   S5  S4  S3   S2  S1  S0\r\n"));
  }
  if ((prev_fpAddressToggleWord != fpAddressToggleWord) || (!SERIAL_FRONT_PANEL)) {
    // If VT100 and no change, don't reprint.
    prev_fpAddressToggleWord = fpAddressToggleWord;
    thePrintLine += (("          "));
    if ( fpAddressToggleWord & 0x8000 ) thePrintLine += (("   ^")); else thePrintLine += (("   v"));
    if ( fpAddressToggleWord & 0x4000 ) thePrintLine += (("   ^")); else thePrintLine += (("   v"));
    if ( fpAddressToggleWord & 0x2000 ) thePrintLine += (("   ^")); else thePrintLine += (("   v"));
    if ( fpAddressToggleWord & 0x1000 ) thePrintLine += (("   ^")); else thePrintLine += (("   v"));
    if ( fpAddressToggleWord & 0x0800 ) thePrintLine += (("   ^")); else thePrintLine += (("   v"));
    if ( fpAddressToggleWord & 0x0400 ) thePrintLine += (("   ^")); else thePrintLine += (("   v"));
    if ( fpAddressToggleWord & 0x0200 ) thePrintLine += (("   ^")); else thePrintLine += (("   v"));
    if ( fpAddressToggleWord & 0x0100 ) thePrintLine += (("   ^")); else thePrintLine += (("   v"));
    thePrintLine += ((" "));
    if ( fpAddressToggleWord & 0x0080 ) thePrintLine += (("   ^")); else thePrintLine += (("   v"));
    if ( fpAddressToggleWord & 0x0040 ) thePrintLine += (("   ^")); else thePrintLine += (("   v"));
    thePrintLine += ((" "));
    if ( fpAddressToggleWord & 0x0020 ) thePrintLine += (("   ^")); else thePrintLine += (("   v"));
    if ( fpAddressToggleWord & 0x0010 ) thePrintLine += (("   ^")); else thePrintLine += (("   v"));
    if ( fpAddressToggleWord & 0x0008 ) thePrintLine += (("   ^")); else thePrintLine += (("   v"));
    thePrintLine += ((" "));
    if ( fpAddressToggleWord & 0x0004 ) thePrintLine += (("   ^")); else thePrintLine += (("   v"));
    if ( fpAddressToggleWord & 0x0002 ) thePrintLine += (("   ^")); else thePrintLine += (("   v"));
    if ( fpAddressToggleWord & 0x0001 ) thePrintLine += (("   ^")); else thePrintLine += (("   v"));
    Serial.print(thePrintLine);
    thePrintLine = "";
  }
  if (SERIAL_FRONT_PANEL) {
    // No need to rewrite the prompt.
    Serial.print(F("\033[2B"));  // Cursor down 2 lines.
    Serial.println();
  } else {
    Serial.print(F("\r\n ------ \r\n"));
    Serial.println(F("+ Ready to receive command."));
  }

}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Process functions

// -----------------------------------------------------------------------------
// Pause during a SINGLE STEP process.
byte singleStepWaitByte = 0;
void singleStepWait() {
  Serial.println(F("+ singleStepWait()"));
  if (singleStepWaitByte != 0) {         // Complete the instruction cycles and process the byte, example r, to RUN the program.
    return;
  }
  printFrontPanel();                // Status, data/address lights already set.
  bool singleStepWaitLoop = true;
  while (singleStepWaitLoop) {
    if (Serial.available() > 0) {
      readByte = Serial.read();    // Read and process an incoming byte.
      switch (readByte) {
        case 's':
          singleStepWaitLoop = false;
          break;
        case 'i':
        case 'p':
          Serial.print(F("++ readByte: "));
          Serial.write(readByte);
          processWaitSwitch(readByte);
          break;
        case 10:
          Serial.println(F("++ Ignore serial input line feed character."));
          break;
        default:
          singleStepWaitLoop = false;
          singleStepWaitByte = readByte;
          Serial.print(F("+ Complete the machine instruction and exit SINGLE STEP mode. Input byte: "));
          Serial.write(singleStepWaitByte);
          Serial.println();
      }
    }
  }
}

void controlResetLogic() {
  regA = 0;
  regB = 0;
  regC = 0;
  regD = 0;
  regE = 0;
  regH = 0;
  regL = 0;
  regSP = 0;
  regPC = 0;
  setAddressData(regPC, MREAD(regPC));
  host_clr_status_led_HLTA();
  host_set_status_leds_READMEM_M1();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// IN opcode, input processing to get a byte from an input port.

// Check sense switch toggles and return the switch byte value as a string.
String getSenseSwitchValue() {
  // From serial toggle switches.
  byte bValue = highByte(fpAddressToggleWord);
  // From hardware is not implemented.
  // byte bValue = toggleSense();
  String sValue = String(bValue, BIN);
  int addZeros = 8 - sValue.length();
  for (int i = 0; i < addZeros; i++) {
    sValue = "0" + sValue;
  }
  return sValue;
}

// ----------------------------------------
// Serial INPUT

byte inputBytePort2 = 0;
byte inputBytePort0 = 0;

// Called from: cpu_IN();
byte altair_in(byte portDataByte) {
  byte inputDataByte;
  //
  // Set status lights and get an input byte from the input port.
  host_clr_status_led_MEMR();
  host_set_status_led_INP();
  //
  switch (portDataByte) {
    // ---------------------------------------------------------------------------------------
    // 4K Basic
    //
    // 4K Basic using 88-2SIO same as GALAXY80.asm, though, GALAXY80.asm uses different ports.
    // Has double input ports, and returns the following:
    //    Port 00 indicates input: 0 for input, 1 for no input.
    //    Port 01 is the actual input character.
    /*
        InputChar:
              IN 00
              ANI 01
              JNZ InputChar
              IN 01
              ANI 7Fh
              RET

      00001010 LF 10 000A LF is from the terminal.
      00001101 CR 13 000D Basic 4K only works with CR.

      ; --------------------------------------
      ; Characters with decimal number.
      ;
      : :!:":#:$:%:&:':(:):*:+:,:-:.:/:0:1:2:3:4:5:6:7:8:9:
      32 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7
                40                  50
      :::;:<:=:>:?:@:
      8 9 0 1 2 3 4
      58          64

      :A:B:C:D:E:F:G:H:I:J:K:L:M:N:O:P:Q:R:S:T:U:V:W:X:Y:Z:
      65 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0
          70                  80                  90
      :[:\:]:^:_:`:
      91 2 3 4 5 5

      :a:b:c:d:e:f:g:h:i:j:k:l:m:n:o:p:q:r:s:t:u:v:w:x:y:z:
      97 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2
      100                 110       115       120

      :{:|:}:~:
      3 4 5 6
      126

    */
    case 0:
      // Convert LF to CR, because 4K Basic only recognizes CR.
      if (SERIAL2_OUTPUT) {
        if (Serial2.available() > 0) {
          // Input from Serial2 port, port 2.
          inputBytePort0 = Serial2.read();          // Save the character input value.
          inputDataByte = 0;                        // Reply with the character input indicator.
        } else {
          inputDataByte = 1;                        // Reply with the NO character input indicator.
          inputBytePort0 = 0;
        }
      } else {
        // Input from default Serial port, port 0.
        if (inputBytePort2 > 0) {
          inputBytePort0 = inputBytePort2;          // Save the character input value.
          inputBytePort2 = 0;
          inputDataByte = 0;                        // Reply with the character input indicator.
        } else {
          inputDataByte = 1;                        // Reply with the NO character input indicator.
          inputBytePort0 = 0;
        }
      }
      break;
    case 1:
      if (inputBytePort0 > 0) {
        if (inputBytePort0 >= 'a' && inputBytePort0 <= 'z') {
          inputDataByte = inputBytePort0 - 32;      // Convert from lowercase to uppercase.
        } else if (inputBytePort0 == 10) {
          inputDataByte = 13;                       // Convert LF to CR, which Basic 4K uses.
        } else {
          inputDataByte = inputBytePort0;           // Original character input value.
        }
        inputBytePort0 = 0;
      } else {
        inputDataByte = 0;
      }
      break;
    // ---------------------------------------------------------------------------------------
    case 2:
    case 16:
    case 17:
      // 16 and 17 are pGalaxy80.asm input ports.
      //  SIOCTL  EQU 10H   ;88-2SIO CONTROL PORT
      //  IN   SIOCTL
      {
        if (SERIAL2_OUTPUT) {
          // Input from the external USB component Serial2 port.
          inputDataByte = 0;
          if (Serial2.available() > 0) {
            inputDataByte = Serial2.read();    // Read and process an incoming byte.
          }
        } else {
          // Input from default serial port. This is good for testing.
          inputDataByte = inputBytePort2;
          inputBytePort2 = 0;
        }
        break;
      }
    case 3:
      {
        // Input(inputBytePort2) comes from the RUN mode loop, USB default serial port.
        // Only keep the most recent input, not queueing the inputs at this time.
        inputDataByte = inputBytePort2;
        inputBytePort2 = 0;
        break;
      }
    case 4:
    case B11111111:
      {
        // case 1, for running Kill the Bit in serial mode.
        // case B11111111, is the common sense switch input port#.
        //
        // For serial Sense switches, get the input and add it to the Address Toggle Word high byte.
        //  Check for new sense switch toggle event from the serial input (0..9 or a..f).
        //  If there was a toggle event, set the address toggle word to include the new event.
        int senseByte = 0;
        if (SERIAL2_OUTPUT) {
          // Input from the external USB component Serial2 port.
          if (Serial2.available() > 0) {
            senseByte = Serial2.read();    // Read and process an incoming byte.
          }
        } else {
          // Input from default serial port. This is good for testing.
          senseByte = inputBytePort2;
          inputBytePort2 = 0;
        }
        if ( senseByte >= '0' && senseByte <= '9' ) {
          fpAddressToggleWord = fpAddressToggleWord ^ (1 << (senseByte - '0'));
        }
        if ( senseByte >= 'a' && senseByte <= 'f' ) {
          fpAddressToggleWord = fpAddressToggleWord ^ (1 << (senseByte - 'a' + 10));
        }
        // Reply with the high byte of the address toggles, which are the sense switch toggles.
        inputDataByte = highByte(fpAddressToggleWord);
        break;
      }
    default:
      {
        inputDataByte = 0;
        Serial.print(F("- Error, input port not available: "));
        Serial.println(portDataByte);
      }
  }
  //
  //#ifdef LOG_MESSAGES
  if (inputDataByte > 1 && logMessages) {
    // Changed to "> 1" from "> 0" to handle Basic 4K.
    // No input at this time.
    // > Input port# 3 inputDataByte value =  3, printByte=00000011, writeInByte=''
    // Samples, Ctrl+c=3
    Serial.print(F("> Input port# "));
    Serial.print(portDataByte);
    Serial.print(F(" inputDataByte value = "));
    sprintf(charBuffer, "%3d %3d", inputDataByte, inputBytePort2);
    Serial.print(charBuffer);
    Serial.print(F(", printByte="));
    printByte(inputDataByte);
    Serial.print(F(", writeInByte="));
    // Special characters.
    if (inputDataByte == 10) {
      Serial.print(F("<LF>"));
    } else if (inputDataByte == 13) {
      Serial.print(F("<CR>"));
    } else {
      Serial.print(F("'"));
      Serial.write(inputDataByte);
      Serial.print(F("'"));
    }
    Serial.print(F("> inputBytePort2="));
    Serial.print(inputBytePort2);
    Serial.println();
  }
  //#endif
  host_set_data_leds(inputDataByte);
  host_set_addr_leds(portDataByte + portDataByte * 256); // As does the origanal Altair 8800: lb and hb set to the port#.
  if (host_read_status_led_WAIT()) {
    // Dave regA = inputDataByte;
    singleStepWait();
  } else {
    printFrontPanel();
  }
  host_clr_status_led_INP();
  return inputDataByte;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Output

uint16_t hlValue;

void altair_out(byte portDataByte, byte regAdata) {
  // Opcode: out <port>
  // Called from: cpu_OUT() { altair_out(MEM_READ(regPC), regA); ... }
#ifdef LOG_MESSAGES
  Serial.print(F("< OUT, port# "));
  Serial.print(portDataByte);
  Serial.print(F(" regA="));
  Serial.print(regAdata);
  Serial.println(F("."));
#endif
  host_set_data_leds(regAdata);
  host_set_addr_leds(portDataByte + portDataByte * 256); // The low and high bytes are each set to the portDataByte.
  host_clr_status_led_MEMR();
  host_set_status_led_OUT();
  host_set_status_led_WO();
  //
  // Write output byte to the output port.
  switch (portDataByte) {
    case 0:
    case 1:
      // 4K Basic
      // Need to set the 8th bit to 0.
      regAdata = regAdata & B01111111;
    case 2:
    case 16:
    case 17:
      // pGalaxy80.asm output port.
      //  SIOCTL  EQU 10H   ;88-2SIO CONTROL PORT
      //  SIODAT  EQU 11H   ;88-2SIO DATA PORT
      //  OUT  SIOCTL
      //  OUT  SIODAT
      if (SERIAL2_OUTPUT) {
        Serial2.write(regAdata);
      } else {
        Serial.write(regAdata);
      }
      break;
    case 3:
      // Out to the USB serial port.
      Serial.write(regAdata);
      break;
    //case 16:
    // Actual output of bytes. Example output a byte to the serial port (IDE monitor).
    // Test port: 20Q (octal: 020).
    // Serial_print(F("++ Test output port, byte output to USB serial port:"));
    // Serial.print(regAdata);         // Write regAdata to serial port.
    // Serial.println(F(":"));
    // break;
    // ---------------------------------------
    // Echo processor values.
    case 30:
      Serial_println();
      Serial_print(F(" > Register B = "));
      printData(regB);
      break;
    case 31:
      Serial_println();
      Serial_print(F(" > Register C = "));
      printData(regC);
      break;
    case 32:
      Serial_println();
      Serial_print(F(" > Register D = "));
      printData(regD);
      break;
    case 33:
      Serial_println();
      Serial_print(F(" > Register E = "));
      printData(regE);
      break;
    case 34:
      Serial_println();
      Serial_print(F(" > Register H = "));
      printData(regH);
      break;
    case 35:
      Serial_println();
      Serial_print(F(" > Register L = "));
      printData(regL);
      break;
    case 36:
      Serial_println();
      Serial_print(F(" > Register H:L = "));
      sprintf(charBuffer, "%3d", regH);
      Serial_print(charBuffer);
      Serial_print(F(":"));
      sprintf(charBuffer, "%3d = ", regL);
      Serial_print(charBuffer);
      printByte(regH);
      Serial_print(':');
      printByte(regL);
      Serial_print(F(", Data: "));
      hlValue = regH * 256 + regL;
      printData(MREAD(hlValue));
      break;
    case 37:
      Serial_println();
      Serial_print(F(" > Register A = "));
      printData(regA);
      break;
    case 38:
#ifdef LOG_MESSAGES
      Serial_println();
#endif
      printRegisters();
      break;
    case 39:
#ifdef LOG_MESSAGES
      Serial_println();
#endif
      Serial_print(F("------------"));
      cpucore_i8080_print_registers();
      // printRegisters();
      // printOther();
      Serial_print(F("------------"));
      break;
    case 40:
      Serial_println();
      Serial_print(F(" > Register B:C = "));
      sprintf(charBuffer, "%3d", regB);
      Serial_print(charBuffer);
      Serial_print(F(":"));
      sprintf(charBuffer, "%3d", regC);
      Serial_print(charBuffer);
      Serial_print(F(", Data: "));
      hlValue = regB * 256 + regC;
      printData(MREAD(hlValue));
      // printOctal(theByte);
      // printByte(theByte);
      break;
    case 41:
      Serial_println();
      Serial_print(F(" > Register D:E = "));
      sprintf(charBuffer, "%3d", regD);
      Serial_print(charBuffer);
      Serial_print(F(":"));
      sprintf(charBuffer, "%3d", regE);
      Serial_print(charBuffer);
      Serial_print(F(", Data: "));
      hlValue = regD * 256 + regE;
      printData(MREAD(hlValue));
      break;
    case 43:
      Serial_println();
      Serial_print(F(" > Register SP = "));
      sprintf(charBuffer, "%5d = ", regSP);
      Serial_print(charBuffer);
      Serial_print(F(" = "));
      printByte(highByte(regSP));
      Serial_print((":"));
      printByte(lowByte(regSP));
      Serial_print(F(" Status flag byte, regS"));
      cpu_print_regS();
      break;
    case 44:
      Serial_println();
      cpucore_i8080_print_registers();
      // printOther();
      break;
    case 45:
      Serial_println();
      Serial_print(F(" > Register SP = "));
      sprintf(charBuffer, "%5d = ", regSP);
      Serial_print(charBuffer);
      printByte(highByte(regSP));
      Serial_print(F(":"));
      printByte(lowByte(regSP));
      break;
    // ---------------------------------------
    default:
      Serial_println();
      Serial_print(F("- Error, output port is not available: "));
      Serial_write(portDataByte);
      Serial_println();
  }
  if (host_read_status_led_WAIT()) {
    singleStepWait();
  } else {
    printFrontPanel();
  }
  host_clr_status_led_OUT();
  host_clr_status_led_WO();
}

void setAddressData(uint16_t addressWord, byte dataByte) {
#ifdef LOG_MESSAGES
  Serial.print(F("+ altair_set_outputs, address:"));
  Serial.print(addressWord);
  Serial.print(F(" dataByte:"));
  Serial.println(dataByte);
#endif
  host_set_addr_leds(addressWord);
  host_set_data_leds(dataByte);
}

void printRegisters() {
  Serial_print(F("------------\r\n"));
  Serial_print(F("+ regA: "));
  printData(regA);
  Serial_println();
  // ---
  Serial_print(F("+ regB: "));
  printData(regB);
  Serial_print(F("  regC: "));
  printData(regC);
  Serial_println();
  // ---
  Serial_print(F("+ regD: "));
  printData(regD);
  Serial_print(F("  regE: "));
  printData(regE);
  Serial_println();
  // ---
  Serial_print(F("+ regH: "));
  printData(regH);
  Serial_print(F("  regL: "));
  printData(regL);
  // ---
  Serial_println();
  Serial_print(F("------------\r\n"));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Processor RUN mode: Running machine instructions

void processDataOpcode() {
#ifdef LOG_OPCODES
  Serial.print(F("++ processDataOpcode(), regPC:"));
  Serial.print(regPC);
  Serial.print(F(": data, opcode:"));
  printData(MREAD(regPC));
  Serial.println();
#endif
  host_set_status_leds_READMEM_M1();
  opcode = MREAD(regPC);
  host_clr_status_led_M1();
  host_set_addr_leds(regPC);
  host_set_data_leds(opcode);
  if (!SERIAL_IO_IDE || (programState == PROGRAM_RUN)) {
    printFrontPanel();
  }
  regPC++;
  CPU_EXEC(opcode);
  //
  // Set for next opcode read:
  if (!host_read_status_led_HLTA()) {
    host_set_status_leds_READMEM_M1();
  }
}

// Serial IDE mode: case 's'.
// Terminal mode: case  3 (Crtl+c) instead of case 's'.
byte stopByte = 's';
//
// Terminal mode: case 26 (Crtl+z) instead of case 'R'.
// Serial IDE mode: case 'R'.
byte resetByte = 'R';
//
void processRunSwitch(byte readByte) {
  if (SERIAL_FRONT_PANEL) {
    Serial.print(F("\033[J"));     // From cursor down, clear the screen, .
  }
  if (readByte == stopByte) {
    Serial.println(F("+ s, STOP"));
    programState = PROGRAM_WAIT;
    host_set_status_led_WAIT();
    host_set_status_leds_READMEM_M1();
    if (!SERIAL_IO_IDE) {
      printFrontPanel();  // <LF> will refresh the display.
    }
  } else if (readByte == resetByte) {
    Serial.println(F("+ R, RESET (run)"));
    // Set to the first memory address.
    regPC = 0;
    setAddressData(regPC, MREAD(regPC));
    // Then continue running, if in RUN mode.
    // -------------------------------------
  } else {
    // Load the byte for use by cpu_IN();
    inputBytePort2 = readByte;
  }
}

void runProcessor() {
  Serial.println(F("+ runProcessor()"));
  if (SERIAL_CLI && !SERIAL_FRONT_PANEL) {
    // Control character reference: https://en.wikipedia.org/wiki/ASCII
    // Terminal mode: case 3: (Crtl+c) instead of case 's'.
    // Terminal mode: case 4: (Crtl+d) instead of case 's'.
    stopByte = 4; // Use 4 because Basic 4K using 3.
    // Terminal mode: case 26 (Crtl+z) instead of case 'R'.
    resetByte = 26;
  } else {
    stopByte = 's';
    resetByte = 'R';
  }
  host_set_addr_leds(regPC);
  programState = PROGRAM_RUN;
  while (programState == PROGRAM_RUN) {
    processDataOpcode();
    if (Serial.available() > 0) {
      readByte = Serial.read();    // Read and process an incoming byte.
      processRunSwitch(readByte);
    }
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Processor WAIT mode: process switch byte options.

String loadProgramName = "";

void processWaitSwitch(byte readByte) {
  // -------------------------------
  // Process an address/data toggle.
  //
  int data = readByte;  // Use int.
  if ( data >= '0' && data <= '9' ) {
    // Serial input, not hardware input.
    fpAddressToggleWord = fpAddressToggleWord ^ (1 << (data - '0'));
    if (!SERIAL_IO_IDE) {
      printFrontPanel();
    }
    return;
  }
  if ( data >= 'a' && data <= 'f' ) {
    // Serial input, not hardware input.
    fpAddressToggleWord = fpAddressToggleWord ^ (1 << (data - 'a' + 10));
    if (!SERIAL_IO_IDE) {
      printFrontPanel();
    }
    return;
  }
  // -------------------------------
  // The following can either initiate from a serial connection or from a hardware switch.
  // Process command switches: RUN, SINGLE STEP, EXAMINE, EXAMINE NEXT, DEPOSIT, DEPOSIT NEXT, RESET.
  //
  uint16_t cnt;
  byte readConfirmByte;
  //
  if (SERIAL_FRONT_PANEL) {
    Serial.print(F("\033[9;1H"));  // Move cursor to below the prompt: line 9, column 1.
    Serial.print(F("\033[J"));     // From cursor down, clear the screen.
  }
  //
  switch (readByte) {
    case 'r':
      Serial.println(F("+ r, RUN."));
      if (fpStatusByte & HLTA_ON) {
        // If previous instruction was HLT, step to next instruction after HLT, then continue.
        regPC++;
        host_clr_status_led_HLTA();
      }
      host_clr_status_led_WAIT();
      programState = PROGRAM_RUN;
      break;
    case 's':
      singleStepWaitByte = 0;   // Used to identify if there was command input during a SINGLE STEP cycle.
      if (fpStatusByte & HLTA_ON) {
        Serial.println(F("+ s, SINGLE STEP, from HLT."));
        regPC++;
        setAddressData(regPC, MREAD(regPC));
        host_clr_status_led_HLTA();
      } else {
        Serial.println(F("+ s, SINGLE STEP, processDataOpcode()"));
        processDataOpcode();
      }
      host_set_status_leds_READMEM_M1();
      setAddressData(regPC, MREAD(regPC));
      if (!SERIAL_IO_IDE) {
        printFrontPanel();  // <LF> will refresh the display.
      }
      break;
    case 'x':
      Serial.print(F("+ x, EXAMINE: "));
      regPC = fpAddressToggleWord;
      Serial.println(regPC);
      setAddressData(regPC, MREAD(regPC));
      if (!SERIAL_IO_IDE) {
        printFrontPanel();  // <LF> will refresh the display.
      }
      break;
    case 'X':
      Serial.print(F("+ X, EXAMINE NEXT: "));
      regPC = regPC + 1;
      Serial.println(regPC);
      setAddressData(regPC, MREAD(regPC));
      if (!SERIAL_IO_IDE) {
        printFrontPanel();  // <LF> will refresh the display.
      }
      break;
    case 'p':
      Serial.print(F("+ p, DEPOSIT to: "));
      Serial.println(regPC);
      MWRITE(regPC, fpAddressToggleWord & 0xff);
      setAddressData(regPC, MREAD(regPC));
      if (!SERIAL_IO_IDE) {
        printFrontPanel();  // <LF> will refresh the display.
      }
      break;
    case 'P':
      Serial.print(F("+ P, DEPOSIT NEXT to: "));
      regPC++;
      Serial.println(regPC);
      MWRITE(regPC, fpAddressToggleWord & 0xff);
      setAddressData(regPC, MREAD(regPC));
      if (!SERIAL_IO_IDE) {
        printFrontPanel();  // <LF> will refresh the display.
      }
      break;
    case 'R':
      Serial.println(F("+ R, RESET."));
      controlResetLogic();
      fpAddressToggleWord = 0;                // Reset all toggles to off.
      if (!SERIAL_IO_IDE) {
        printFrontPanel();  // <LF> will refresh the display.
      }
      break;
    case 'C':
      readConfirmByte;
      Serial.println(F("+ C, CLR: Clear memory, set registers to zero, set data and address to zero."));
      Serial.print(F("+ Confirm CLR, y/n: "));
      readConfirmByte = 's';
      while (!(readConfirmByte == 'y' || readConfirmByte == 'n')) {
        if (Serial.available() > 0) {
          readConfirmByte = Serial.read();    // Read and process an incoming byte.
        }
        delay(60);
      }
      if (readConfirmByte != 'y') {
        Serial.println(F("+ CLR cancelled."));
        break;
      }
      Serial.println(F("+ CLR confirmed."));
      Serial.print(F("+ Clear memory: "));
      Serial.println(MEMSIZE);
      for (int i = 0; i < MEMSIZE; i++) {
        MWRITE(i, 0);
      }
      // ---------------------------------
      Serial.println(F("+ CLR registers."));
      regA = 0;
      regB = 0;
      regC = 0;
      regD = 0;
      regE = 0;
      regH = 0;
      regL = 0;
      regPC = 0;
      regSP = 0;
      //
      Serial.println(F("+ CLR data, address, switches, and reset status lights."));
      fpStatusByte = MEMR_ON | M1_ON | WO_ON; // Status: Get next opcode.
      fpDataByte = 0;                         // Data
      fpAddressWord = 0;                      // Address word
      fpAddressToggleWord = 0;                // Reset all toggles to off.
      loadProgramName = "";                   // As there may have been a sample loaded program.
      if (SERIAL_FRONT_PANEL) {
        printFrontPanel();      // For Arduino IDE monitor, <LF> will refresh the display.
      }
      break;
    // -------------------------------------------------------------------
    //
    case 'v':
      Serial.println(F("+ v, VT100 escapes are disabled and block cursor on."));
      if (SERIAL_FRONT_PANEL) {
        SERIAL_FRONT_PANEL = false;
        Serial.print(F("\033[0m\033[?25h"));       // Insure block cursor display: on.
      }
      // Note, VT100 escape sequences don't work on the Ardino IDE monitor.
      break;
    case 'V':
      // The following requires a VT100 terminal such as a Macbook terminal.
      // Insure the previous value is different to current, to insure an intial printing.
      prev_fpDataByte = host_read_data_leds() + 1;
      prev_fpAddressWord = host_read_addr_leds() + 1;
      prev_fpAddressToggleWord = 1;           // Set to different value.
      fpAddressToggleWord = 0;                // Set all toggles off.
      Serial.print(F("\033[0m\033[?25l"));       // Block cursor display: off.
      Serial.print(F("\033[H\033[2J"));          // Cursor home and clear the screen.
      SERIAL_FRONT_PANEL = false;                // Insure labels are printed.
      serialPrintFrontPanel();                // Print the complete front panel: labels and indicators.
      SERIAL_FRONT_PANEL = true;                 // Must be after serialPrintFrontPanel(), to have the labels printed.
      SERIAL_IO_IDE = false;                      // Insure it's disabled.
      Serial.println(F("+ V, VT100 escapes are enabled and block cursor off."));
      break;
    // -------------------------------------------------------------------
    case 't':
      SERIAL_CLI = false;
      Serial.println(F("+ t, terminal output VT100 escape codes is disabled."));
      break;
    case 'T':
      SERIAL_CLI = true;
      Serial.println(F("+ T, Terminal output VT100 escape codes is enabled. Use Crtl+d(or Crtl+c) to STOP, Crtl+Z to RESET."));
      break;
    // -------------------------------------------------------------------
    case 'w':
      SERIAL_IO_IDE = false;
      Serial.println(F("+ w, USB serial output is disabled."));
      break;
    case 'W':
      SERIAL_IO_IDE = true;
      Serial.println(F("+ W, USB serial output is enabled."));
      break;
    // -------------------------------------
    case 'o':
      Serial.println(F("+ o, disable output to LED lights."));
      LED_IO = false;
      break;
    case 'O':
      Serial.println(F("+ O, enable output to LED lights."));
      LED_IO = true;
      break;
    //
    // -------------------------------------------------------------------
    case 'z':
      Serial.print(F("++ Cursor off."));
      Serial.print(F("\033[0m\033[?25l"));
      Serial.println();
      break;
    case 'Z':
      Serial.print(F("++ Cursor on."));
      Serial.print(F("\033[0m\033[?25h"));
      Serial.println();
      break;
    //
    // -------------------------------------
    case 'l':
      Serial.println(F("+ l, load a sample program."));
      loadProgram();
      if (!SERIAL_IO_IDE) {
        printFrontPanel();  // <LF> will refresh the display.
      }
      break;
    case 'L':
      Serial.println(F("+ L, Load hex code from the serial port. Enter space(' ') to exit."));
      loadProgramSerial();
      if (!SERIAL_IO_IDE) {
        printFrontPanel();  // <LF> will refresh the display.
      }
      break;
    // -------------------------------------
    case 'S':
      Serial.println(F("+ S, Refresh front panel."));
      printFrontPanel();
      break;
    // -------------------------------------
    case 'D':
      programState = SERIAL_DOWNLOAD;
      Serial.println(F("+ D, Download mode."));
      break;
    // -------------------------------------------------------------------
    case 'y':
      Serial.print(F("+ y, Serial2 off (end)."));
      Serial.println();
      Serial2.end();
      SERIAL2_OUTPUT = false;
      break;
    case 'Y':
      Serial.print(F("+ Y, Serial2 on (begin), baud rate: "));
      Serial.print(downloadBaudRate);
      Serial.println(F("."));
      Serial2.begin(downloadBaudRate);
      SERIAL2_OUTPUT = true;
      break;
    case 'B':
      Serial.println(F("+ B, Select Serial2 baud rate: "));
      Serial.println(F("++ 1. 9600, 2. 19200, 3. 38400, 4. 57600, 5. 115200; x. Exit, no change"));
      Serial.print(F("++ ?- "));
      readConfirmByte = 's';
      while (!(readConfirmByte > '0' && readConfirmByte < '6') && readConfirmByte != 'x' && readConfirmByte != 'X') {
        if (Serial.available() > 0) {
          readConfirmByte = Serial.read();    // Read and process an incoming byte.
        }
        delay(60);
      }
      if (readConfirmByte == '1') {
        downloadBaudRate = 9600;
      } else if (readConfirmByte == '2') {
        downloadBaudRate = 19200;
      } else if (readConfirmByte == '3') {
        downloadBaudRate = 38400;
      } else if (readConfirmByte == '4') {
        downloadBaudRate = 57600;
      } else if (readConfirmByte == '5') {
        downloadBaudRate = 115200;
      }
      if (SERIAL2_OUTPUT) {
        Serial2.begin(downloadBaudRate);
        Serial.print(F("Serial2 baud rate set to: "));
        Serial.print(downloadBaudRate);
        Serial.println(F("."));
      } else {
        Serial.print(F("Serial2 baud rate will be set to: "));
        Serial.print(downloadBaudRate);
        Serial.println(F(" when next used."));
      }
      break;
    // -------------------------------------
    case 'j':
      Serial.println(F("+ j: Setting Information."));
      Serial.println(F("------------"));
      Serial.print(F("++ programState: "));
      Serial.println(programState);
      Serial.print(F("++ LED_IO="));
      Serial.print(LED_IO);
      Serial.print(F(" SERIAL_FRONT_PANEL="));
      Serial.print(SERIAL_FRONT_PANEL);
      Serial.print(F(" SERIAL_IO_IDE="));
      Serial.print(SERIAL_IO_IDE);
      Serial.print(F(" SERIAL_CLI="));
      Serial.println(SERIAL_CLI);
      Serial.print(F("++ Serial: "));
      if (Serial) {
        Serial.print(F("on, "));
      } else {
        Serial.print(F("off, "));
      }
      Serial.println(Serial);
      Serial.print(F("++ Serial2: "));
      if (SERIAL2_OUTPUT) {
        // Note, the following doesn't say if it's on or off: "if (Serial2)".
        Serial.print(F("on, "));
      } else {
        Serial.print(F("off, "));
      }
      Serial.print(F("baud rate: "));
      Serial.print(downloadBaudRate );
      Serial.print(F(", data bits, stop bits, and parity: 8, 1, 0"));
      Serial.println();
      Serial.print(F("++ logMessages: "));
      if (logMessages) {
        Serial.print(F("on, "));
      } else {
        Serial.print(F("off, "));
      }
      Serial.println();
      break;
    // -------------------------------------
    case 'u':
      logMessages = false;
      Serial.println(F("+ u, Log messages off."));
      break;
    case 'U':
      logMessages = true;
      Serial.println(F("+ U, Log messages on."));
      break;
    case 'i':
      Serial.println(F("+ i: Information."));
      cpucore_i8080_print_registers();
      break;
    // -------------------------------------
    case 'h':
      Serial.print(F("+ h, Print help information."));
      Serial.println();
      Serial.println(F("----------------------------------------------------"));
      Serial.println(F("+++ Altair 8800 Virtual Front Panel Switch Controls"));
      Serial.println(F("-------------"));
      Serial.println(F("+ s, STOP         When in RUN mode, change to WAIT mode."));
      Serial.println(F("+ r, RUN mode     When in WAIT mode, change to RUN mode."));
      Serial.println(F("+ s, SINGLE STEP  When in WAIT mode, SINGLE STEP."));
      Serial.println(F("+ x, EXAMINE      sense switch address."));
      Serial.println(F("+ X, EXAMINE NEXT address, current address + 1."));
      Serial.println(F("+ p, DEPOSIT      into current address."));
      Serial.println(F("+ P, DEPOSIT NEXT address, deposit into current address + 1."));
      Serial.println(F("+ R, RESET        Set program counter address to zero."));
      Serial.println(F("+ C, CLR          Clear memory, set registers and program counter address to zero."));
      Serial.println(F("-------------"));
      Serial.println(F("+ 0...9, a...f    Toggle sense/address/data switches:  A0...A9, A10...A15."));
      Serial.println(F("----------------------------------------------------"));
      Serial.println(F("+++ Command Operations and Module Controls"));
      Serial.println(F("-------------"));
      Serial.println(F("+ l, Load sample  Load a sample program."));
      Serial.println(F("+ i, info         Information print of registers."));
      Serial.println(F("+ j, settings     Settings information."));
      Serial.println(F("+ v/V VT100 panel Disable/enable VT100 virtual front panel."));
      Serial.println(F("+ t/T Terminal    Disable/enable VT100 terminal commandline (cli) escape codes."));
      Serial.println(F("-------------"));
      Serial.println(F("+ D, Download     DOWNLOAD mode for receiving bytes from the Serial2 serial port."));
      Serial.println(F("+ y/Y Serial2     Disable/enable Serial2 for program I/O."));
      Serial.println(F("+ B Serial2 baud  Set Serial2 baud rate."));
      Serial.println(F("-------------"));
      Serial.println(F("+ Q, Clock        CLOCK mode, interact with the clock."));
      Serial.println(F("+ q, Time         Show the clock's data and time."));
      Serial.println(F("-------------"));
      Serial.println(F("+ H, MP3 Player   PLAYER mode, run the MP3 player."));
      Serial.println(F("+ I, Player Info  MP3 player software and hardware settings."));
      Serial.println(F("+ g/G Play        Pause/Play MP3 song."));
      Serial.println(F("+ k/K Volume      Down/Up player volume."));
      Serial.println(F("-------------"));
      Serial.println(F("+ m, Read         Read an SD card file into program memory."));
      Serial.println(F("+ M, Write        Write program memory to an SD card file."));
      Serial.println(F("+ n, Directory    Directory file listing of the SD card."));
      Serial.println(F("-------------"));
      Serial.println(F("+ Enter key       Refresh USB serial output front panel display."));
      Serial.println(F("+ u/U Log msg     Log messages off/on."));
      Serial.println(F("+ z/Z cursor      VT100 block cursor off/on."));
      Serial.println(F("+ L, Load hex     Load hex code from the serial port."));
      Serial.println(F("+ o/O LEDs        Disable/enable LED light output."));
      Serial.println(F("+ w/W USB serial  Disable/enable USB serial output."));
      Serial.println(F("----------------------------------------------------"));
      break;
    // -------------------------------------
    // For command line. Note playerContinuous() has infrared controls in place.
    case 'Q':
      Serial.println(F("+ Q, MP3 Player   PLAYER mode, run the MP3 player."));
      programState = CLOCK_RUN;
      break;
    case 'q':
      clockSwitch('r');
      break;
    // -------------------------------------
    // For command line. Note playerContinuous() has infrared controls in place.
    case 'H':
      Serial.println(F("+ H, MP3 Player   PLAYER mode, run the MP3 player."));
      programState = PLAYER_RUN;
      break;
    case 'I':
      playerSwitch('i');
      break;
    case 'g':
      playerSwitch('s');
      break;
    case 'G':
      playerSwitch('r');
      break;
    case 'k':
      playerSwitch('v');
      Serial.print(F("> "));
      break;
    case 'K':
      playerSwitch('V');
      Serial.print(F("> "));
      break;
    // -------------------------------------
    case 13:
      // USB serial, VT100 terminal.
      Serial.println();
      if (!SERIAL_FRONT_PANEL) {
        printFrontPanel();  // <LF> will refresh the display.
        serialPrintFrontPanel();
      }
      break;
    case 10:
      // USB serial, non VT100.
      Serial.println();
      serialPrintFrontPanel();
      break;
    case 12:
      // Ctrl+l is ASCII 7, which is form feed (FF).
      if (SERIAL_FRONT_PANEL || SERIAL_CLI) {
        Serial.print(F("\033[H\033[2J"));          // Cursor home and clear the screen.
      }
      if (SERIAL_FRONT_PANEL) {
        // Refresh the front panel
        SERIAL_FRONT_PANEL = false;                // Insure labels are printed.
        serialPrintFrontPanel();                // Print the complete front panel: labels and indicators.
        SERIAL_FRONT_PANEL = true;                 // Must be after serialPrintFrontPanel(), to have the labels printed.
      }
      break;
    case 'n':
      Serial.println(F("+ n, SD card directory lising."));
#ifdef SETUP_SDCARD
      sdListDirectory();
#else
      Serial.println(F("- SD card not enabled."));
#endif  // SETUP_SDCARD
      break;
    case 'm':
      Serial.println(F("+ m, Read file into program memory."));
#ifdef SETUP_SDCARD
      theFilename = getSenseSwitchValue() + ".bin";
      if (theFilename == "00000000.bin") {
        Serial.println(F("+ Set to download over the serial port."));
        programState = SERIAL_DOWNLOAD;
        return;
      }
      Serial.print(F("++ Program filename: "));
      Serial.println(theFilename);
      Serial.println(F("++ Confirm, y/n: "));
      readConfirmByte = 's';
      while (!(readConfirmByte == 'y' || readConfirmByte == 'n')) {
        if (Serial.available() > 0) {
          readConfirmByte = Serial.read();    // Read and process an incoming byte.
        }
        delay(60);
      }
      if (readConfirmByte != 'y') {
        Serial.println(F("+ Cancelled."));
        break;
      }
      Serial.println(F("+ Confirmed."));
      //
      host_set_status_led_HLDA();
      if (readProgramFileIntoMemory(theFilename)) {
        ledFlashSuccess();
        controlResetLogic();
        fpAddressToggleWord = 0;                // Reset all toggles to off.
        playerPlaySoundWait(READ_FILE);
        // } else {
        // Redisplay the front panel lights.
        // printFrontPanel();
      }
      printFrontPanel();
      host_clr_status_led_HLDA();
#else
      Serial.println(F("- SD card not enabled."));
#endif  // SETUP_SDCARD
      break;
    case 'M':
      Serial.println(F("+ M, Write program Memory into a file."));
#ifdef SETUP_SDCARD
      String senseSwitchValue = getSenseSwitchValue();
      theFilename = senseSwitchValue + ".bin";
      if (theFilename == "11111111.bin") {
        Serial.println(F("- Warning, disabled, write to filename: 11111111.bin."));
        ledFlashError();
        return;
      }
      Serial.print(F("++ Write filename: "));
      Serial.println(theFilename);
      Serial.println(F("++ Confirm, y/n: "));
      readConfirmByte = 's';
      while (!(readConfirmByte == 'y' || readConfirmByte == 'n')) {
        if (Serial.available() > 0) {
          readConfirmByte = Serial.read();    // Read and process an incoming byte.
        }
        delay(60);
      }
      if (readConfirmByte != 'y') {
        Serial.println(F("+ Cancelled."));
        break;
      }
      Serial.println(F("+ Confirmed."));
      //
      host_set_status_led_HLDA();
#ifdef LOG_MESSAGES
      Serial.print(F("+ Write memory to filename: "));
      Serial.println(theFilename);
#endif
      // -------------------------------------------------------
      writeProgramMemoryToFile(theFilename);
      printFrontPanel();
#else
      Serial.println(F("- SD card not enabled."));
#endif  // SETUP_SDCARD
      break;
      // -------------------------------------
      /*
        default:
        {
        #ifdef LOG_MESSAGES
        Serial.print(F("- Ignored: "));
        printByte(readByte);
        Serial.println();
        #endif
        break;
        }
      */
  }
}

void runProcessorWait() {
#ifdef LOG_MESSAGES
  Serial.println(F("+ runProcessorWait()"));
#endif
  while (programState == PROGRAM_WAIT) {
    // Program control: RUN, SINGLE STEP, EXAMINE, EXAMINE NEXT, Examine previous, RESET.
    // And other options such as enable VT100 output enabled or load a sample program.
    if (Serial.available() > 0) {
      readByte = Serial.read();    // Read and process an incoming byte.
      if (!(readByte == 27 || readByte == 91 || readByte == 65 || readByte == 66)) {
        // Ignore Mouse wheel
        processWaitSwitch(readByte);
        if (singleStepWaitByte) {
          // This handles inputs during a SINGLE STEP cycle that hasn't finished.
          processWaitSwitch(readByte);
        }
        if (!SERIAL_FRONT_PANEL) {
          Serial.print(F("?- "));
        }
      }
    }
    // Allow for the music to keep playing, and infrared player controls to work.
    playerContinuous();
    delay(60);
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Receive bytes through serial port.
// The bytes are loaded into processor memory.

// -----------------------------------------------------------------------------
// Receive a string of hex numbers and load them into simulator memory as bytes.

void loadProgramSerial() {
  int cnt = 0;
  int x = 0;
  uint16_t programCounter = 0;
  char hexNumber[3] = "";
  Serial.println(F("+ loadProgramSerial()"));
  programState = PROGRAM_LOAD;
  while (programState == PROGRAM_LOAD) {
    if (Serial.available() > 0) {
      readByte = Serial.read();    // Read and process an incoming byte.
      if (readByte == ' ') {
        programState = PROGRAM_WAIT;
      } else if (readByte >= '0' && readByte <= '9') {
        x *= 16;
        x += readByte - '0';
        hexNumber[cnt] = readByte;
        cnt ++;
      } else if (readByte >= 'A' && readByte <= 'F') {
        x *= 16;
        x += 10 + (readByte - 'A');
        hexNumber[cnt] = readByte;
        cnt ++;
      }
      if (cnt == 2) {
        MWRITE(programCounter++, x)
        Serial.print(F("+ "));
        if (programCounter < 10) {
          Serial.print(F("0"));
        }
        Serial.print(programCounter);
        Serial.print(F(": "));
        Serial.write(hexNumber[0]);
        Serial.write(hexNumber[1]);
        Serial.print(F(" = "));
        if (x < 100) {
          Serial.print(F("0"));
        }
        if (x < 10) {
          Serial.print(F("0"));
        }
        Serial.print(x);
        Serial.print(F(" = "));
        printByte(x);
        Serial.println();
        cnt = 0;
        x = 0;
      }
    }
  }
  if (programCounter > 0) {
    Serial.print(F("++ Number of program bytes = "));
    Serial.print(programCounter);
    Serial.println();
    regPC = 0;
    host_set_addr_leds(0);
    host_set_data_leds(MREAD(0));
  }
}

// -----------------------------------------------------------------------------
void modeDownloadProgram() {
  Serial.println(F("+ Download mode: ready to receive a program bytes on Serial2. Enter, x, to exit."));
  Serial.print(F("++ Serial 2 baud rate: "));
  Serial.println(downloadBaudRate);
  // Status: ready for input and not yet writing to memory.
  host_set_status_led_INP();
  host_clr_status_led_M1();
  host_set_addr_leds(0);
  host_set_data_leds(0);
  printFrontPanel();
  if (SERIAL_FRONT_PANEL) {
    Serial.print(F("\033[11;1H"));  // Move cursor to below the prompt: line 9, column 1.
    Serial.print(F("\033[J"));     // From cursor down, clear the screen.
  }
  byte readByte = 0;
  int readByteCount = 0;      // Count the downloaded bytes that are entered into processor memory.
  unsigned long timer;        // Indicator used to identify when download has ended.
  boolean flashWaitOn = false;
  boolean downloadStarted = false;
  boolean check_SERIAL2_OUTPUT = false;
  if (SERIAL2_OUTPUT) {
    SERIAL2_OUTPUT = false;               // Don't print to Serial2 during this operation.
    check_SERIAL2_OUTPUT = true;          // Re-open before exiting this function.
  }
  Serial2.begin(downloadBaudRate);
  while (programState == SERIAL_DOWNLOAD) {
    if (Serial2.available() > 0) {
      readByte = Serial2.read();      // Read the incoming byte.
      if (flashWaitOn) {
        setWaitStatus(false);
        flashWaitOn = false;
      } else {
        host_clr_status_led_WAIT();
        setWaitStatus(true);
        flashWaitOn = true;
      }
      if (!downloadStarted) {
        downloadStarted = true;
        host_set_status_leds_WRITEMEM();   // Now writing to processor memory.
        if (SERIAL_FRONT_PANEL) {
          Serial.print(F("\033[11;1H"));    // Move cursor to below the prompt.
        }
        // Serial.println(F("+       Address  Data  Binary   Hex Octal Decimal"));
        Serial.println(F("+       Address"));
        //              ++ Byte#     17, Byte: 00000000 000 000     0
      }
      if (SERIAL_FRONT_PANEL) {
        Serial.print(F("\033[12;1H"));    // Move cursor to below the prompt: line 10, column 1.
      }
      //
      // Buffer space up to 64K.
      Serial.print(F("++ Byte# "));
      Serial.print(readByteCount);
      //
      // Input on the external serial port module.
      // Process the incoming byte.
      MWRITE(readByteCount, readByte)
      readByteCount++;
      Serial.println();
      //
      timer = millis();
    }
    if (downloadStarted && ((millis() - timer) > 500)) {
      // Exit download state, if the bytes were downloaded and then stopped for 1/2 second.
      //  This indicates that the download is complete.
      Serial.println(F("+ Download complete."));
      programState = PROGRAM_WAIT;
    }
    // -----------------------------------------------
    // Flip or send serial character RESET(R) or STOP(s), to exit download mode.
    //
    // Check serial input for RESET or STOP.
    if (Serial.available() > 0) {
      readByte = Serial.read();    // Read and process an incoming byte.
      if (readByte == 'R' || readByte == 's' || readByte == 'x') {
        Serial.println(F("+ Exit download mode."));
        programState = PROGRAM_WAIT;
        delay(100); // Required to wait for the LF when in IDE serial monitor.
        while (Serial.available() > 0) {
          // Serial.println(F("+ Exit, Clear other characters in the buffer, example: LF."));
          readByte = Serial.read();
          delay(100); // Required to wait for the LF when in IDE serial monitor.
        }
      }
    }
    // Check hardware for RESET or STOP.
    /*
      if (pcfControlinterrupted) {
      // -------------------
      if (pcfControl.readButton(pinReset) == 0) {
        if (!switchReset) {
          switchReset = true;
        }
      } else if (switchReset) {
        switchReset = false;
        // Switch logic
        programState = PROGRAM_WAIT;
      }
      // -------------------
      pcfControlinterrupted = false; // Reset for next interrupt.
      }
    */
    // -----------------------------------------------
  }
  Serial2.end();        // Close and allow this port for output use.
  if (check_SERIAL2_OUTPUT) {
    Serial.println(F("+ Note, Serial2 was open. It's now closed after download."));
  }
  if (readByteCount > 0) {
    // Program bytes were downloaded into memory.
    controlResetLogic();                // Reset the program and front panel, including status lights.
    playerPlaySoundWait(READ_FILE);
  } else {
    // No bytes downloaded, reset the panel light values.
    host_set_addr_leds(regPC);
    host_read_addr_leds(MREAD(regPC));
  }
  host_clr_status_led_INP();
#ifdef SWITCH_MESSAGES
  Serial.println(F("+ Exit serial download mode."));
#endif
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void setup() {
  // Speed for serial read, which matches the sending program.
  Serial.begin(115200);         // Baud rates: 9600 19200 57600 115200
  delay(2000);
  Serial.println(); // Newline after garbage characters.
  Serial.print(F("+++ "));
  // Serial.println(__func__); // prints "setup"
  Serial.println(F("setup()"));
  //
  // ----------------------------------------------------
  // ----------------------------------------------------
  // Front panel LED lights.

  // System application status LED lights
  pinMode(WAIT_PIN, OUTPUT);        // Indicator: Altair 8800 emulator program WAIT state: LED on or LED off.
  // pinMode(HLDA_PIN, OUTPUT);     // Indicator: clock or player process: LED on. Emulator: LED off.
  digitalWrite(WAIT_PIN, HIGH);     // Default to WAIT state.
  // digitalWrite(HLDA_PIN, HIGH);  // Default to emulator.
  // ------------------------------
  // Set status lights.
#ifdef LOG_MESSAGES
  Serial.println(F("+ Initialized: statusByte, programCounter & curProgramCounter, dataByte."));
#endif
  pinMode(latchPinLed, OUTPUT);
  pinMode(clockPinLed, OUTPUT);
  pinMode(dataPinLed, OUTPUT);
  delay(300);
  ledFlashSuccess();
#ifdef LOG_MESSAGES
  Serial.println(F("+ Front panel LED lights are initialized."));
#endif
  //
  // ----------------------------------------------------
  // ----------------------------------------------------
#ifdef SETUP_SDCARD
  // The csPin pin is connected to the SD card module select pin (CS).
  if (!SD.begin(csPin)) {
    Serial.println(F("- Error initializing SD card module."));
    ledFlashError();
    hwStatus = 1;
  } else {
    Serial.println(F("+ Initialized: SD card module."));
    ledFlashSuccess();
  }
  delay(300);
#else
  Serial.println(F("+ SD card module is not included in this compiled version."));
#endif
  //
  // ----------------------------------------------------
  setupMp3Player();
  setupClock();
  // ----------------------------------------------------
  programState = PROGRAM_WAIT;
  host_set_status_leds_READMEM_M1();
  regPC = 0;
  opcode = MREAD(regPC);
  host_set_addr_leds(regPC);
  host_set_data_leds(opcode);
  printFrontPanel();
  Serial.println(F("+++ Altair101b initialized, version 0.92.b."));
  // ----------------------------------------------------
}

// -----------------------------------------------------------------------------
// Device Loop
void loop() {
  switch (programState) {
    // ----------------------------
    case PROGRAM_RUN:
      host_clr_status_led_WAIT();
      runProcessor();
      break;
    // ----------------------------
    case PROGRAM_WAIT:
      if (!SERIAL_FRONT_PANEL) {
        Serial.print(F("?- "));
      }
      host_set_status_led_WAIT();
      runProcessorWait();
      break;
    // ----------------------------
    case SERIAL_DOWNLOAD:
      host_clr_status_led_WAIT();
      host_set_status_led_HLDA();
      modeDownloadProgram();
      host_clr_status_led_HLDA();
      host_set_status_led_WAIT();
      printFrontPanel();
      if (SERIAL2_OUTPUT) {
        // modeDownloadProgram() opens and closes (begin and end) the connection.
        // The following will re-open it for output.
        Serial2.begin(downloadBaudRate);
      }
      break;
    // ----------------------------
    case PLAYER_RUN:
      host_clr_status_led_WAIT();
      host_set_status_led_HLDA();
      mp3PlayerRun();
      host_clr_status_led_HLDA();
      break;
    // ----------------------------
    case CLOCK_RUN:
      host_clr_status_led_WAIT()
      host_set_status_led_HLDA();
      rtClockRun();
       host_clr_status_led_HLDA();
      break;
  }
  delay(30);
}
// -----------------------------------------------------------------------------
