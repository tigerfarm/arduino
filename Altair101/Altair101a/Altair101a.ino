// -----------------------------------------------------------------------------
// Altair 101a Processor program, which is an Altair 8800 simulator.
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
  Altair 101a Processor program

  + Serial interactivity simulator.
  + Uses the default Arduino USB serial port, and optionally, Serial2 port.
  ++ Tested using the Arduino IDE serial monitor and Mac terminal which has VT100 features.
  ++ Can run programs from command line, or with a simulated front panel (printed to the serial port).
  + This simulator uses David Hansel's Altair 8800 Simulator code to process machine instructions.
  ++ cpucore_i8080.cpp and cpucore_i8080.h.

  Differences to the original Altair 8800:
  + HLT goes into STOP state which allows RUN to restart the process from where it was halted.
  + When SINGLE STEP read and write, I've chosen to show the actual data value,
    rather than having all the data lights on, which is what the original Altair 8800 does.

  ---------------------------------------------------------
  Next to work on

  + Consider serial I/O to handle 88-2SIO CHANNEL SERIAL INTERFACE functionality.
  ++ Then, could use the original serial routines in Galaxy80.asm.

  Implement interupt handling.
  + opcodes: ei and di. And handling interupts once ei is run.
  + This is required when I try to run complex programs such as CPM, and likely Basic.

  Option to have HLT work the same as Altair 8800.

  Starting integrating hardware functions using the Altair tablet.
  + Get the Altair tablet to work.
  + Test with Altair101a.
  + Then update Altair101a with the ability to flash the front panel lights.
  + Then update Altair101a implementing interactivity with the front panel switches.

  ---------------------------------------------------------
  Hex programs:

  Try loading Kill the Bit from: prog_games.cpp.
  If that works, can load Basic from: prog_basic.cpp.

  From prog_games.cpp, program hex bytes:
    static const byte PROGMEM killbits[] = {
    0x21, 0x00, 0x00, 0x16, 0x80, 0x01, 0x0E, 0x00, 0x1A, 0x1A, 0x1A, 0x1A, 0x09, 0xD2, 0x08, 0x00,
    0xDB, 0xFF, 0xAA, 0x0F, 0x57, 0xC3, 0x08, 0x00 };

  prog_basic.cpp, as hex bytes for 4K Basic.
    const byte PROGMEM basic4k[] = { 0xae, 0xae, ... 0x00 };
  + Will need to search Hex codes to see if interupt is required.
  ++ Interupt is required.

  +++ Integration steps to merge this code with Processor.ino.
  + Continue use testing Altair101a.
  + Continue adding Processor features into Altair101a.
  ++ Upload Altair101a to the Altair 101 machine and test with lights.
  ++ Altair101a code updates to handle hardware toggles and switches.

  After Altair101a works and processes programs,
  + Add the other features: player, clock, timer, and counter.

  - Consider preventing lockup when using PUSH A, i.e. PUSH called before setting SP.
  + In the PUSH process, if SP < 4, error.

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
  Wire the serial module to the Mega Serial2 pins.
  Serial module RX pin to Mega TX pin 16.
  Serial module TX pin to Mega RX pin 17.

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
    runDownloadProgram()
  + SD card R/W functions
    readProgramFileIntoMemory(String theFilename)
    writeProgramMemoryToFile(String theFilename)
  --------------------------
  + Setup()
  + Looop()

*/
// -----------------------------------------------------------------------------
#include "Altair101a.h"
#include "cpucore_i8080.h"

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
byte hwStatus = B11111111;            // Initial state.
// hwStatus = 1;  // 0001 SD card
// hwStatus = 2;  // 0010 Clock module
// hwStatus = 4;  // 0100 MP3 Player
const byte SD_ON  =    B00000001;
const byte CL_ON  =    B00000010;
const byte PL_ON  =    B00000100;
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
const int csPin = 53;  // SD Card module is connected to Mega pin 53.

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

unsigned long downloadBaudRate = 115200;   // 57600 Needs to match the upload program.

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
const int WAIT_PIN = 13;        // Change to onboard pin for the Altair 101a machine.

// HLDA : 8080 processor goes into a hold state because of other hardware running.
// const int HLDA_PIN = A10;     // Emulator processing (off/LOW) or clock/player processing (on/HIGH).

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

byte inputBytePort2 = 0;
byte altair_in(byte portDataByte) {
  byte inputDataByte;
  //
  // Set status lights and get an input byte from the input port.
  host_clr_status_led_MEMR();
  host_set_status_led_INP();
  //
  switch (portDataByte) {
    case 1:
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
    case 2:
    case 16:
    case 17:
    // pGalaxy80.asm input port.
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
    default:
      {
        inputDataByte = 0;
        Serial.print(F("- Error, input port not available: "));
        Serial.println(portDataByte);
      }
  }
  //
  //#ifdef LOG_MESSAGES
  if (inputDataByte > 0 && logMessages) {
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
    Serial.println(F("+ R, RESET"));
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
    // Terminal mode: case 3: (Crtl+c) instead of case 's'.
    stopByte = 3;
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
      Serial.println(F("+ T, Terminal output VT100 escape codes is enabled. Use Crtl+c to STOP, Crtl+Z to RESET."));
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
    //
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
      Serial.print(Serial2);
      Serial.print(F(", baud rate: "));
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
    case 'N':
      loadProgramBytes();
      break;
    // -------------------------------------
    case 'h':
      Serial.print(F("+ h, Print help information."));
      Serial.println();
      Serial.println(F("----------------------------------------------------"));
      Serial.println(F("+++ Commands"));
      Serial.println(F("-------------"));
      Serial.println(F("+ r, RUN mode     When in WAIT mode, change to RUN mode."));
      Serial.println(F("+ s, STOP         When in RUN mode, change to WAIT mode."));
      Serial.println(F("+ s, SINGLE STEP  When in WAIT mode, SINGLE STEP."));
      Serial.println(F("+ x, EXAMINE      current switch address."));
      Serial.println(F("+ X, EXAMINE NEXT address, current address + 1."));
      Serial.println(F("+ p, DEPOSIT      into current address."));
      Serial.println(F("+ P, DEPOSIT NEXT address, deposit into current address + 1."));
      Serial.println(F("+ R, RESET        Set program counter address to zero."));
      Serial.println(F("+ C, CLR          Clear memory, set registers and program counter address to zero."));
      Serial.println(F("+ D, Download     Download mode, from serial port (Serial2)."));
      Serial.println(F("-------------"));
      Serial.println(F("+ 0...9, a...f    Toggle address switches:  A0...A9, A10...A15."));
      Serial.println(F("-------------"));
      Serial.println(F("+ m, Read         Memory: Read an SD card file into program memory."));
      Serial.println(F("+ M, Write        Memory: Write program memory to an SD card file."));
      Serial.println(F("+ n, Directory    Directory file listing of the SD card."));
      Serial.println(F("+ l, Load sample  Load a sample program."));
      Serial.println(F("+ L, Load hex     Load hex code from the serial port."));
      Serial.println(F("-------------"));
      Serial.println(F("+ i, info         Information print of registers."));
      Serial.println(F("+ j, settings     Settings information."));
      Serial.println(F("-------------"));
      Serial.println(F("+ t/T Terminal    Disable/enable VT100 terminal commandline (cli) escape codes."));
      Serial.println(F("+ v/V Front panel Disable/enable VT100 virtual front panel."));
      Serial.println(F("+ w/W USB serial  Disable/enable USB serial output."));
      Serial.println(F("+ y/Y Serial2     Disable/enable Serial2 for I/O."));
      Serial.println(F("+ o/O LEDs        Disable/enable LED light output."));
      Serial.println(F("-------------"));
      Serial.println(F("+ Enter key       Refresh USB serial output front panel display."));
      Serial.println(F("+ u/U Log msg     Log messages off/on."));
      Serial.println(F("+ z/Z cursor      VT100 block cursor off/on."));
      Serial.println(F("----------------------------------------------------"));
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
      processWaitSwitch(readByte);
      if (singleStepWaitByte) {
        // This handles inputs during a SINGLE STEP cycle that hasn't finished.
        processWaitSwitch(readByte);
      }
      /*
        if (SERIAL_CLI && !SERIAL_FRONT_PANEL) {
        processWaitSwitch(10);    // Since the terminal doesn't send a CR or LF, send one.
        }
      */
      if (!SERIAL_FRONT_PANEL) {
        Serial.print(F("?- "));
      }
    }
    delay(60);
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Processor WAIT mode: Load sample programs.

void loadProgram() {
  int cnt;
  cnt = 0;
#ifdef LOG_MESSAGES
  Serial.println(F("+ loadProgram()"));
#endif
  if (loadProgramName != "") {
    Serial.print(F("+ Program previously loaded: "));
    Serial.println(loadProgramName);
  }
  programState = PROGRAM_LOAD;
  while (programState == PROGRAM_LOAD) {
    if (Serial.available() > 0) {
      readByte = Serial.read();    // Read and process an incoming byte.
      switch (readByte) {
        case 'k':
          loadProgramName = "Serial Kill the Bit";
          Serial.println(F("+ k, load: A version of Kill the Bit for serial I/O."));
          programState = PROGRAM_WAIT;
          if (SERIAL_FRONT_PANEL) {
            Serial.print(F("\033[J"));     // From cursor down, clear the screen, .
          }
          MWRITE( cnt++, B00100001 & 0xff);  // ++ opcode:lxi:00100001:h:0
          MWRITE( cnt++, B00000000 & 0xff);  // ++ lb:0:0
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
          MWRITE( cnt++, B00010110 & 0xff);  // ++ opcode:mvi:00010110:d:080h
          MWRITE( cnt++, B10000000 & 0xff);  // ++ immediate:080h:128
          MWRITE( cnt++, B00011010 & 0xff);  // ++ opcode:ldax:00011010:d
          MWRITE( cnt++, B00011010 & 0xff);  // ++ opcode:ldax:00011010:d
          MWRITE( cnt++, B00011010 & 0xff);  // ++ opcode:ldax:00011010:d
          MWRITE( cnt++, B00011010 & 0xff);  // ++ opcode:ldax:00011010:d
          MWRITE( cnt++, B00011010 & 0xff);  // ++ opcode:ldax:00011010:d
          MWRITE( cnt++, B00011010 & 0xff);  // ++ opcode:ldax:00011010:d
          MWRITE( cnt++, B00011010 & 0xff);  // ++ opcode:ldax:00011010:d
          MWRITE( cnt++, B00011010 & 0xff);  // ++ opcode:ldax:00011010:d
          MWRITE( cnt++, B00011010 & 0xff);  // ++ opcode:ldax:00011010:d
          MWRITE( cnt++, B00011010 & 0xff);  // ++ opcode:ldax:00011010:d
          MWRITE( cnt++, B00011010 & 0xff);  // ++ opcode:ldax:00011010:d
          MWRITE( cnt++, B00011010 & 0xff);  // ++ opcode:ldax:00011010:d
          MWRITE( cnt++, B11011011 & 0xff);  // ++ opcode:in:11011011:2
          MWRITE( cnt++, B00000001 & 0xff);  // ++ immediate:1:1
          MWRITE( cnt++, B10101010 & 0xff);  // ++ opcode:xra:10101010:d
          MWRITE( cnt++, B00001111 & 0xff);  // ++ opcode:rrc:00001111
          MWRITE( cnt++, B01010111 & 0xff);  // ++ opcode:mov:01010111:d:a
          MWRITE( cnt++, B11000011 & 0xff);  // ++ opcode:jmp:11000011:Begin
          MWRITE( cnt++, B00000101 & 0xff);  // ++ lb:Begin:5
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
          break;
        case 'm':
          loadProgramName = "Register set test";
          Serial.println(F("+ m, load: a program that uses MVI to set the registers with values from 1 to 7."));
          Serial.println(F("     Use option 'i' to list the register values."));
          programState = PROGRAM_WAIT;
          if (SERIAL_FRONT_PANEL) {
            Serial.print(F("\033[J"));     // From cursor down, clear the screen, .
          }
          MWRITE( cnt++, B00111110 & 0xff);  // ++ opcode:mvi:00111110:a:1
          MWRITE( cnt++, B00000001 & 0xff);  // ++ immediate:1:1
          MWRITE( cnt++, B00000110 & 0xff);  // ++ opcode:mvi:00000110:b:2
          MWRITE( cnt++, B00000010 & 0xff);  // ++ immediate:2:2
          MWRITE( cnt++, B00001110 & 0xff);  // ++ opcode:mvi:00001110:c:3
          MWRITE( cnt++, B00000011 & 0xff);  // ++ immediate:3:3
          MWRITE( cnt++, B00010110 & 0xff);  // ++ opcode:mvi:00010110:d:4
          MWRITE( cnt++, B00000100 & 0xff);  // ++ immediate:4:4
          MWRITE( cnt++, B00011110 & 0xff);  // ++ opcode:mvi:00011110:e:5
          MWRITE( cnt++, B00000101 & 0xff);  // ++ immediate:5:5
          MWRITE( cnt++, B00100110 & 0xff);  // ++ opcode:mvi:00100110:h:6
          MWRITE( cnt++, B00000110 & 0xff);  // ++ immediate:6:6
          MWRITE( cnt++, B00101110 & 0xff);  // ++ opcode:mvi:00101110:l:7
          MWRITE( cnt++, B00000111 & 0xff);  // ++ immediate:7:7
          MWRITE( cnt++, B01110110 & 0xff);  // ++ opcode:hlt:01110110
          MWRITE( cnt++, B11000011 & 0xff);  // ++ opcode:jmp:11000011:Start
          MWRITE( cnt++, B00000000 & 0xff);  // ++ lb:Start:0
          MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
          break;
        // -------------------------------------
        case 'x':
          Serial.println(F("< x, Exit load program."));
          programState = PROGRAM_WAIT;
          if (SERIAL_FRONT_PANEL) {
            Serial.print(F("\033[J"));     // From cursor down, clear the screen, .
          }
          break;
        default:
          if (SERIAL_FRONT_PANEL) {
            Serial.print(F("\033[9;1H"));  // Move cursor to below the prompt: line 9, column 1.
            Serial.print(F("\033[J"));     // From cursor down, clear the screen, .
          }
          Serial.println(F("+ Sample programs."));
          Serial.println(F("++ k, Kill the Bit version for serial VT100 input."));
          Serial.println(F("++ m, MVI testing to the setting of registers."));
          Serial.println(F("----------"));
          Serial.println(F("+ x, Exit: don't load a program."));
      }
    }
  }
  if (readByte != 'x') {
    // Do EXAMINE 0 after the load;
    regPC = 0;
    setAddressData(regPC, MREAD(regPC));
    if (SERIAL_FRONT_PANEL) {
      serialPrintFrontPanel();
    }
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
void runDownloadProgram() {
  Serial.println(F("+ Download mode: ready to receive a program. Enter, x, to exit."));
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
      /* Commented out to simplify output during the receiving of bytes.
      if (readByteCount < 10) {
        Serial.print(F(" "));
      }
      if (readByteCount < 100) {
        Serial.print(F(" "));
      }
      if (readByteCount < 1000) {
        Serial.print(F(" "));
      }
      if (readByteCount < 10000) {
        Serial.print(F(" "));
      }
      if (readByteCount < 100000) {
        Serial.print(F(" "));
      }
      */
      Serial.print(readByteCount);
      //
      // Input on the external serial port module.
      // Process the incoming byte.
      MWRITE(readByteCount, readByte)
      readByteCount++;
      /* Commented out to simplify output during the receiving of bytes.
      Serial.print(F(", Byte: "));
      printByte(readByte);
      Serial.print(F(" "));
      printHex(readByte);
      Serial.print(F(" "));
      printOctal(readByte);
      Serial.print(F("   "));
      if (readByte < 10) {
        Serial.print(F(" "));
      }
      if (readByte < 100) {
        Serial.print(F(" "));
      }
      Serial.print(readByte);
      */
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
const byte programBytes[] = {
  // Kill the Bit program.
  0x21, 0x00, 0x00, 0x16, 0x80, 0x01, 0x0E, 0x00, 0x1A, 0x1A, 0x1A, 0x1A, 0x09, 0xD2, 0x08, 0x00,
  0xDB, 0xFF, 0xAA, 0x0F, 0x57, 0xC3, 0x08, 0x00
};
void loadProgramBytes() {
  int theSize = sizeof(programBytes);
  for (int i = 0; i < theSize; i++ ) {
    byte theValue = programBytes[i];
    Serial.print(F("+ programBytes["));
    if (i < 10) {
      Serial.print(F("0"));
    }
    MWRITE(i, programBytes[i]);
    Serial.print(i);
    Serial.print(F("] = D:"));
    if (theValue < 100) {
      Serial.print(F("0"));
    }
    if (theValue < 10) {
      Serial.print(F("0"));
    }
    Serial.print(theValue);
    Serial.print(F(" H:"));
    printHex(theValue);
    Serial.print(F(":B:"));
    printByte(theValue);
    Serial.print(F(":O:"));
    printOctal(theValue);
    Serial.print(F(":"));
    ;
    theValue = MREAD(i);
    if (theValue < 100) {
      Serial.print(F("0"));
    }
    if (theValue < 10) {
      Serial.print(F("0"));
    }
    Serial.print(theValue, DEC);
    Serial.println();
  }
}

// -----------------------------------------------------------------------------
void setup() {
  // Speed for serial read, which matches the sending program.
  Serial.begin(115200);         // Baud rates: 9600 19200 57600 115200
  delay(2000);
  Serial.println(); // Newline after garbage characters.
  Serial.print(F("+++ "));
  Serial.println(__func__); // prints "setup"
  
  //
  // ----------------------------------------------------
  // ----------------------------------------------------
  // Front panel LED lights.

  // System application status LED lights
  pinMode(WAIT_PIN, OUTPUT);        // Indicator: program WAIT state: LED on or LED off.
  // pinMode(HLDA_PIN, OUTPUT);     // Indicator: clock or player process (LED on) or simulator (LED off).
  digitalWrite(WAIT_PIN, HIGH);     // Default to wait state.
  // digitalWrite(HLDA_PIN, HIGH);  // Default to simulator.
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
    Serial.println(F("+ SD card module is initialized."));
    ledFlashSuccess();
  }
  delay(300);
#else
  Serial.println(F("+ SD card module is not included in this compiled version."));
#endif
  //
  // ----------------------------------------------------
  // ----------------------------------------------------
  programState = PROGRAM_WAIT;
  host_set_status_leds_READMEM_M1();
  regPC = 0;
  opcode = MREAD(regPC);
  host_set_addr_leds(regPC);
  host_set_data_leds(opcode);
  printFrontPanel();
  Serial.println(F("+++ Altair 101a initialized."));
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
      runDownloadProgram();
      host_clr_status_led_HLDA();
      host_set_status_led_WAIT();
      printFrontPanel();
      if (SERIAL2_OUTPUT) {
        // runDownloadProgram() opens and closes (begin and end) the connection.
        // The following will re-open it for output.
        Serial2.begin(downloadBaudRate);
      }
      break;
  }
  delay(30);
}
// -----------------------------------------------------------------------------
