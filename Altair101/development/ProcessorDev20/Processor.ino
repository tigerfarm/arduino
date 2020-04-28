// -----------------------------------------------------------------------------
/*
  Altair 101 Processor program

  This is an Altair 8800 emulator program that runs on an Arduino Mega microcontroller.
  It emulates the basic Altair 8800 hardware processes--from 1975.
  It was built around the Intel 8080 CPU chip. The 8080's opcodes are the same for the 8085.
  This program implements many of the 8080 microprocessor machine instructions (opcodes).
  It has more than half of the opcodes implemented,
  which more than enough to run the classic program, Kill the Bit.

  ---------------------------------------------
  Current/Next Work

  User guide,
  + How to load a and run a program.
  + How to use the clock.

  Panel LED lights all display correctly. Toggle functions all work.
  I can show my steampunk tablet to the world.
  + Time to generate videos.

  Processor.ino works for both the tablet and desktop versions.
  + LED lights
  + Toggle switches
  + SD card adapter
  + Clock
  Next to do,
  + Serial port module
  Yet todo,
  + MP3 player

  When clicking clock or player switches,
  + Save front panel lights.
  + Set status lights off.
  When returning from clock or player mode by clicking clock or player switch,
  + Restore front panel lights.
  
  ---------------------------------------------
  ---------------------------------------------
  Tablet to Desktop module work: Output LED lights

  Done and Tested: The 3 Mega pins to 74HC595, updated and matched.

  Done and Tested: WAIT and HLDA need to be digital pin controlled.
  + HLDA_PIN = A10, HLDA is already controlled by a digital pin.
  + WAIT_PIN = A9,  Test code change of WAIT_ON/OFF to digital pin control.

  Done and Tested: Align status light shift pins or use compile options.
  + They are the same, just need to test.

  Done and Tested: Need compile options for shiftOut statements in functions:
  + processDataLights, and
  + lightsStatusAddressData.
  --- Tablet
  digitalWrite(latchPinLed, LOW);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, data8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, lowByte(address16bits));
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, highByte(address16bits));
  shiftOut(dataPinLed, clockPinLed, MSBFIRST, status8bits);
  digitalWrite(latchPinLed, HIGH);
  --- Desktop
  digitalWrite(latchPinLed, LOW);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, status8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, data8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, lowByte(address16bits));
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, highByte(address16bits));
  digitalWrite(latchPinLed, HIGH);
  ---

  ----------------------------------------------------------------------
  Tablet to Desktop module work: Input toggles

  -------------------------------------------
  Tablet module toggle inputs, 2 PCF modules,
  + pcfControl(0x020): controls: STOP, RUN, EXAMINE, EXAMINE NEXT, DEPOSIT, DEPOSIT NEXT
  ++ pcfControl has interupt enabled, Mega pin 2.
  ++ In Processor.ino, changed "pcf20" to "pcfControl".
  -------
  + pcfData(0x021): low address byte, and sense switches
  ++ In Processor.ino, changed "pcf21" to "pcfData". Check data and sense switch calls.
  ++ Toggle address = 0000000000000111, data=00000111, sense=00000111
  -------
  + Digital pin  8: AUX 1 up, clock
  + Digital pin  9: AUX 1 down, MP3 player
  + Digital pin 10: AUX 2 up, write/upload to the SD card
  + Digital pin 11: AUX 2 down, read/download from the SD card
  -------

  -------------------------------------------
  Desktop module toggle inputs: 4 PCF modules,
  -------
  ++ pcfControl(0x020): controls: STOP, RUN, EXAMINE, EXAMINE NEXT, DEPOSIT, DEPOSIT NEXT
  +++ pcfControl has interrupt enabled, Mega pin 2.
  -------
  ++ pcfData(0x021): low address byte
  -------
  ++ pcfSense(0x022): high address byte, and sense switches
  -------
  ++ pcfAux(0x023): AUX1 up and down, AUX2 up and down, PROTECT, UNPROTECT, CLR, and switch below STEP.
  +++ pcfAux has interrupt enabled, same Mega pin as pcfControl, pin 2.
  -------

  ---------------------------------------------
  Connect the Mega to the desktop front panel:

  Total = 12 Mega pin wire connects to the front panel, 4 of which are for power (+ and -).

  4 Mega pins to PCF8574 daisy chain, toggle inputs:
  + I2C SDA to Mega SDA
  + I2C SCL to Mega SCL
  + I2C +5V
  + I2C Ground
  --- Plus ---
  1 Mega pin 2 to the front panel. Then 2 wires to PCF8574 module interrupts for Control and AUX.

  3 Mega pins to 74HC595 daisy chain, LED output:
  + Mega pin A14 to 74HC595 14 Data pin.
  + Mega pin A12 to 74HC595 12 Latch pin.
  + Mega pin A11 to 74HC595 11 Clock pin.
  --- Plus ---
  2 wires for +5V and Ground.

  2 pins for the WAIT and HLDA LED lights.
  + const int WAIT_PIN = A9;
  + const int HLDA_PIN = A10;

  ---------------------------------------------
  ---------------------------------------------
  Other Work

  Now, can set the LCD backlight as on when prompting, and reset after.
  + LCD backlight on/off status is controlled: LcdBacklight.

  If 00000000.bin exists when starting up,
  + If 00000000.bin is not all zeros (NOPs), run it.
  + Else, don't run it, boot to wait state.

  Confirm before reading a file into memory,
  + Fast flash HLDA for 1 second.
  + If read or write switch repeated, then run, else return to program wait status.

  + Test write memory issue,
  ++ Seems to be a CALL opcode issue.
  ++ If byte count over 276, characters no longer are displayed.
  ++ Fails at the address: 00010100 (276)

  -----------------------------------------------------------------------------
  Processor program sections,
    Code compilation options.
    Arduino pin definitions.
    Output: 1602 LCD
    Output: Clock definitions.
    ----------------------------
    Definitions: machine memory, stack memory, and a sample machine code program in a memory array.
    Definitions: Front Panel Status LEDs.
    Output: Front Panel LED lights.
    ----------------------------
    Process a subset of the Intel 8080/8085 opcode instructions:
    + Control of the instruction set of opcodes.
    + Process opcode instruction machine cycle one (M1): fetch opcode and process it.
    + Process opcode instruction machine cycles greater than 1: address and immediate bytes.
    ----------------------------
    I/O: SD Card reader: save and load machine memory.
    I/O: Clock functions.
    ----------------------------
    Input: Front Panel toggle switch control and data entry events
    Input: Front Panel toggle switch AUX events for devices: clock and SD card module.
    Input: Infrared switch events
    ----------------------------
    setup() : Computer initialization.
    loop()  : Based on state, clock cycling through memory, show the time, or other state processing.
    ----------------------------
  ---------------------------------------------
  Altair 8800 Operator's Manual.pdf has a description of each opcode.
  Binary calculator:
    https://www.calculator.net/binary-calculator.html
  Bitwise operators:
    https://www.arduino.cc/reference/en/language/structure/bitwise-operators/bitwiseand/
*/
// -----------------------------------------------------------------------------
// Code compilation options.

// Define for desktop module.
// Comment out for tablet model.
#define DESKTOP_MODULE 1

#define SETUP_SDCARD 1
// #define SETUP_CLOCK 1
// #define SETUP_LCD 1

#define INCLUDE_AUX 1
#define INCLUDE_CLOCK 1
#define INCLUDE_SDCARD 1
#define INCLUDE_LCD 1

// -----------------------------------
// #define LOG_MESSAGES 1         // Has large memory requirements.
#define SWITCH_MESSAGES 1
// #define INFRARED_MESSAGES 1    // For a simple setup: Mega + infrared, with serial messages.

// -----------------------------------------------------------------------------
// Program states

#define PROGRAM_WAIT 0
#define PROGRAM_RUN 1
#define CLOCK_RUN 2
#define PLAYER_RUN 3
#define SERIAL_DOWNLOAD 4
int programState = PROGRAM_WAIT;  // Intial, default.

// -----------------------------------------------------------------------------
// Infrared Receiver

#include <IRremote.h>

//          Mega pin
int IR_PIN = A1;

IRrecv irrecv(IR_PIN);
decode_results results;

// -----------------------------------------------------------------------------
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
#ifdef INCLUDE_AUX
//                              Mega pins
const int CLOCK_SWITCH_PIN =    8;  // Also works pins 4 and A11. Doesn't work: 24, 33.
const int PLAYER_SWITCH_PIN =   9;
const int UPLOAD_SWITCH_PIN =   10;
const int DOWNLOAD_SWITCH_PIN = 11;
#endif

// ----------------
// Status LED lights

// Program wait status.
const int WAIT_PIN = A9;      // Program wait state: off/LOW or wait state on/HIGH.

// HLDA : 8080 processor goes into a hold state because of other hardware running.
const int HLDA_PIN = A10;     // Emulator processing (off/LOW) or clock processing (on/HIGH).


// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// SD Card module is an SPI bus slave device.
#ifdef INCLUDE_SDCARD

#include <SPI.h>
#include <SD.h>

//     Mega Nano - SPI module pins
// Pin 53   10   - CS   : chip/slave select (SS pin). Can be any master(Nano) digital pin to enable/disable this device on the SPI bus.
// Pin 52   13   - SCK  : serial clock, SPI: accepts clock pulses which synchronize data transmission generated by Arduino.
// Pin 51   11   - 6 : master out slave in, SPI: input to the Micro SD Card Module.
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

int setClockValue = 0;
// rtc.adjust(DateTime(2019, 10, 9, 16, 22, 3));   // year, month, day, hour, minute, seconds
int theCounterYear = 0;
int theCounterMonth = 0;
int theCounterDay = 0;
int theCounterHours = 0;
int theCounterMinutes = 0;
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

void printClockInt(int theColumn, int theRow, int theInt) {
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
  printClockInt(++theCursor, printRowClockDate, theCounterMonth);
  // ---
  theCursor = theCursor + 2;
  lcd.print("/");
  printClockInt(++theCursor, printRowClockDate, theCounterDay);
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
      printClockInt(theSetCol, theSetRow, setValue);
      break;
    case 2:
      // Serial.print("minutes");
      // Stacy, need to clear hour.
      lcdPrintln(theSetRow, "Set:");
      theSetMax = 59;
      theSetMin = 0;
      theSetCol = thePrintColMin;
      setValue = theCounterMinutes;
      printClockInt(theSetCol, theSetRow, setValue);
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
      printClockInt(theSetCol, theSetRow, setValue);
      break;
    case 4:
      // Serial.print("day");
      lcdPrintln(theSetRow, "Set day:");
      theSetMax = 31;
      theSetMin = 1;
      theSetCol = thePrintColMin;
      setValue = theCounterDay;
      printClockInt(theSetCol, theSetRow, setValue);
      break;
    case 5:
      // Serial.print("month");
      lcdPrintln(theSetRow, "Set month:");
      theSetMax = 12;
      theSetMin = 1;
      theSetCol = thePrintColMin;
      setValue = theCounterMonth;
      printClockInt(theSetCol, theSetRow, setValue);
      break;
    case 6:
      // Serial.print("year");
      lcdPrintln(theSetRow, "Set year:");
      theSetMax = 2525; // In the year 2525, If man is still alive, If woman can survive...
      theSetMin = 1795; // Year John Keats the poet was born.
      theSetCol = thePrintColMin;
      setValue = theCounterYear;
      printClockInt(theSetCol, theSetRow, setValue);
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
// Front Panel Status LEDs

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
// const byte WAIT_ON =    B00000001;  // WAIT   Changed to a digital pin control.

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
// -----------------------------------------------------------------------------
// Memory definitions

const int memoryBytes = 1024;  // When using Mega: 1024, for Nano: 256
byte memoryData[memoryBytes];
unsigned int curProgramCounter = 0;     // Current program address value
unsigned int programCounter = 0;        // When processing opcodes, the next program address value.

const int stackBytes = 32;
int stackData[memoryBytes];
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

void processDataLights() {
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
#ifdef LOG_MESSAGES || INFRARED_MESSAGES
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
  processDataLights();  // Uses: statusByte, curProgramCounter, dataByte.
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
        regA = toggleSenseByte();
      } else {
        regA = 0;
#ifdef LOG_MESSAGES
        Serial.print(F(" IN not implemented on this port."));
#endif
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
#ifdef INFRARED_MESSAGES
        Serial.print(F("Serial terminal output of the contents of register A :"));
        Serial.print(regA);
        Serial.print(":");
#endif
#ifdef LOG_MESSAGES
        Serial.print(F(", Serial terminal output of the contents of register A :"));
        Serial.print(regA);
        Serial.print(":");
#endif
        asciiChar = regA;
        Serial.print(asciiChar);
#ifdef INFRARED_MESSAGES
        Serial.print(":");
        Serial.println("");
#endif
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
#ifdef INCLUDE_SDCARD

// Handle the case if the card is not inserted. Once inserted, the module will be re-initialized.
boolean sdcardFailed = false;
void initSdcard() {
  sdcardFailed = false;
  if (!SD.begin(csPin)) {
    sdcardFailed = true;
    Serial.println("- Error initializing SD card.");
    return; // When used in setup(), causes jump to loop().
  }
  Serial.println("+ SD card initialized.");
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
  processDataLights();
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
  processDataLights();
}

// -------------------------------------
// Write Program memory to a file.

void writeProgramMemoryToFile(String theFilename) {
  digitalWrite(HLDA_PIN, HIGH);
  if (sdcardFailed) {
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
    sdcardFailed = true;
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

void readProgramFileIntoMemory(String theFilename) {
  digitalWrite(HLDA_PIN, HIGH);
  if (sdcardFailed) {
    initSdcard();
  }
  // Serial.println("+ Read a file into program memory, file named: ");
  // Serial.print(theFilename);
  // Serial.println("+ Check if file exists. ");
  if (!SD.exists(theFilename)) {
    Serial.print(F("- Read ERROR, file doesn't exist: "));
    Serial.println(theFilename);
    ledFlashError();
    sdcardFailed = true;
    digitalWrite(HLDA_PIN, LOW);
    return;
  }
  myFile = SD.open(theFilename);
  if (!myFile) {
    Serial.print(F("- Read ERROR, cannot open file: "));
    Serial.println(theFilename);
    ledFlashError();
    sdcardFailed = true;
    digitalWrite(HLDA_PIN, LOW);
    return;
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
    }
  }
  myFile.close();
  Serial.println(F("+ Read completed, file closed."));
  ledFlashSuccess();
  digitalWrite(HLDA_PIN, LOW);
  controlResetLogic();
}

#endif

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

int toggleDataByte() {
  byte toggleByte = ~pcfData.read8();
  return toggleByte;
}
// --------------------------
int toggleSenseByte() {
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
  byte byteHigh = ~pcfSense.read8() * 256;
#else
  byte byteHigh = 0;                    // No high byte toggles on the tablet.
#endif
  return byteHigh + byteLow;
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

// -------------------------
void controlResetLogic() {
  programCounter = 0;
  curProgramCounter = 0;
  stackPointer = 0;
  opcode = 0;  // For the case when the processing cycle 2 or more.
  statusByte = 0;
  if (programState != PROGRAM_RUN) {
    programState = PROGRAM_WAIT;
    // statusByte = statusByte | WAIT_ON;
    digitalWrite(WAIT_PIN, HIGH);
  }
  // Fetch the first opcode, which display values to the panel.
  processData();
}

void controlStopLogic() {
  programState = PROGRAM_WAIT;
  // Create a STOP statusByte, to display on stopping.
  // When the program continues, the orginal statusByte should be used.
  int stopStatusByte = HLTA_ON;
  digitalWrite(WAIT_PIN, HIGH);
  // Display values to the panel lights.
  lightsStatusAddressData(stopStatusByte, programCounter, dataByte);
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
        processDataLights();
#ifdef SWITCH_MESSAGES
        Serial.print(F("+ Control, Examine: "));
        printByte(dataByte);
        Serial.print(" = ");
        printData(dataByte);
        Serial.println("");
#endif
        break;
      case CLOCK_RUN:
#ifdef SWITCH_MESSAGES
        Serial.println(F("+ Clock, Examine, set clock value."));
#endif
        // Serial.print("+ Key OK");
        if (setClockValue) {
          // Serial.print(" ");
          switch (setClockValue) {
            case 1:
              // Serial.print("seconds");
              theCounterSeconds = setValue;
              printClockInt(theSetCol, printRowClockPulse, setValue);
              break;
            case 2:
              // Serial.print("minutes");
              theCounterMinutes = setValue;
              printClockInt(theSetCol, printRowClockPulse, setValue);
              break;
            case 3:
              // Serial.print("hours");
              theCounterHours = setValue;
              printClockInt(theSetCol, printRowClockPulse, setValue);
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
        processDataLights();
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
        Serial.println(F("+ Clock, Examine Next."));
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
        dataByte = toggleDataByte();
        memoryData[programCounter] = dataByte;
        processDataLights();
        break;
      case CLOCK_RUN:
#ifdef SWITCH_MESSAGES
        Serial.println(F("+ Clock, Deposit, increment."));
#endif
        setValue++;
        if (setValue > theSetMax) {
          setValue = theSetMin;
        }
        printClockInt(theSetCol, theSetRow, setValue);
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
        dataByte = toggleDataByte();
        memoryData[programCounter] = dataByte;
        processDataLights();
        break;
      case CLOCK_RUN:
#ifdef SWITCH_MESSAGES
        Serial.println(F("+ Clock, Deposit Next, decrement."));
#endif
        setValue--;
        if (setValue < theSetMin) {
          setValue = theSetMax;
        }
        printClockInt(theSetCol, theSetRow, setValue);
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
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ Running, Stop."));
#endif
    controlStopLogic();
  }
  // -------------------
  // Read PCF8574 input for this switch.
  if (pcfControl.readButton(pinReset) == 0) {
    if (!switchReset) {
      switchReset = true;
    }
  } else if (switchReset) {
    switchReset = false;
    // Switch logic.
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
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ Control, Run."));
#endif
    // Switch logic...
    programState = PROGRAM_RUN;
    digitalWrite(WAIT_PIN, LOW);
    // statusByte = statusByte & WAIT_OFF;
    statusByte = statusByte & HLTA_OFF;
  }
  // -------------------
  // Read PCF8574 input for this switch.
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
  // Read PCF8574 input for this switch.
  if (pcfControl.readButton(pinStep) == 0) {
    if (!switchStep) {
      switchStep = true;
    }
  } else if (switchStep) {
    switchStep = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.println("+ Control, Step.");
#endif
    // Switch logic...
    statusByte = statusByte & HLTA_OFF;
    processData();
  }
  // -------------------
  checkExamineButton();
  checkExamineNextButton();
  checkDepositButton();
  checkDepositNextButton();
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
  printClockInt(theCursor, printRowClockPulse, theCounterHours);  // Column, Row
  theCursor = theCursor + 3;
  lcd.print(":");
  printClockInt(theCursor, printRowClockPulse, theCounterMinutes);
  theCursor = theCursor + 3;
  lcd.print(":");
  printClockInt(theCursor, printRowClockPulse, theCounterSeconds);
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
boolean HLDA_ON = false;
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
  /*
    if (now.second() != theCounterSeconds) {
    // When the clock second value changes, that's a second pulse.
    theCounterSeconds = now.second();
    // clockPulseSecond();
    if (HLDA_ON) {
      digitalWrite(HLDA_PIN, LOW);
      HLDA_ON = false;
    } else {
      digitalWrite(HLDA_PIN, HIGH);
      HLDA_ON = true;
    }
    if (theCounterSeconds == 0) {
      // When the clock second value changes to zero, that's a minute pulse.
      theCounterMinutes = now.minute();
      if (theCounterMinutes != 0) {
        // clockPulseMinute();
        Serial.print("+ clockPulseMinute(), theCounterMinutes= ");
        Serial.println(theCounterMinutes);
        // ----------------------------------------------
        displayTheTime( theCounterMinutes, theCounterHours );
      } else {
        // When the clock minute value changes to zero, that's an hour pulse.
        // clockPulseHour();
        theCounterHours = now.hour();
        Serial.print("++ clockPulseHour(), theCounterHours= ");
        Serial.println(theCounterHours);
        // ----------------------------------------------
        displayTheTime( theCounterMinutes, theCounterHours );
      }
    }
    }
  */
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
  displayTheTime( theCounterMinutes, theCounterHours );
  printClockInt(thePrintColHour, printRowClockPulse, theHour);
}
void clockPulseMinute() {
  Serial.print("+ clockPulseMinute(), theCounterMinutes= ");
  Serial.println(theCounterMinutes);
  displayTheTime( theCounterMinutes, theCounterHours );
  printClockInt(thePrintColMin, printRowClockPulse, theCounterMinutes);
}
void clockPulseSecond() {
  if (HLDA_ON) {
    digitalWrite(HLDA_PIN, LOW);
    HLDA_ON = false;
  } else {
    digitalWrite(HLDA_PIN, HIGH);
    HLDA_ON = true;
  }
  // Serial.print("+ theCounterSeconds = ");
  // Serial.println(theCounterSeconds);
  printClockInt(thePrintColSec, printRowClockPulse, theCounterSeconds);  // Column, Row
}

// ------------------------------------------------------------------------
// Display hours and minutes on LED lights.

void displayTheTime(byte theMinute, byte theHour) {
  byte theMinuteOnes = 0;
  byte theMinuteTens = 0;
  byte theBinaryMinute = 0;
  byte theBinaryHour1 = 0;
  byte theBinaryHour2 = 0;

  // ----------------------------------------------
  // Convert the minute into binary for display.
  if (theMinute < 10) {
    theBinaryMinute = theMinute;
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
    theBinaryMinute = 32 * theMinuteTens + theMinuteOnes;
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
      theBinaryHour1 = B00000001;
      theBinaryHour2 = 0;
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
  // Need enter: theBinaryMinute, theBinaryHour1, theBinaryHour2, into the following:
  // Data byte LED lights: theBinaryMinute.
  // Address word LED lights: theBinaryHour2 & theBinaryHour1.
  unsigned int hourWord = theBinaryHour2 * 256 + theBinaryHour1;
  lightsStatusAddressData(statusByte, hourWord, theBinaryMinute);
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

void clockRun() {
  Serial.println(F("+ clockRun()"));
  saveClearLcdScreenData();
  lcd.noCursor();
  syncCountWithClock();
  displayTheTime( theCounterMinutes, theCounterHours );
  while (programState == CLOCK_RUN) {
    // Clock process to display the time.
    processClockNow();
    // Switches to exit this mode.
    checkRunningButtons();
    checkClockSwitch();
    // Check control buttons for setting the time.
    checkExamineButton();
    checkExamineNextButton();
    checkDepositButton();
    checkDepositNextButton();
    //
    delay(100);
  }
  restoreLcdScreenData();
}
#endif

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
void playerRun() {
  Serial.println(F("+ playerRun()"));
  saveClearLcdScreenData();
  lcd.noCursor();
  //             1234567890123456
  lcdPrintln(0, "MP3 Player mode,");
  lcdPrintln(1, "Not implemented.");
  while (programState == PLAYER_RUN) {
    // processPlayer();
    // Switches to exit this mode.
    checkRunningButtons();
    checkPlayerSwitch();
    //
    delay(100);
  }
  restoreLcdScreenData();
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Front Panel AUX Switches

#ifdef INCLUDE_AUX
// Only do the action once, don't repeat if the switch is held down.
// Don't repeat action if the switch is not pressed.

boolean switchAux1up = false;
boolean switchAux1down = false;
boolean switchAux2up = false;
boolean switchAux2down = false;

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
      Serial.println(F("+ Stop clock mode, return to the Altair 8800 emulator."));
      // programState = PROGRAM_WAIT;    // controlStopLogic changes programState to wait.
      controlStopLogic();
      digitalWrite(HLDA_PIN, LOW);
    } else {
      Serial.println(F("+ Clock mode."));
      programState = CLOCK_RUN;
      digitalWrite(HLDA_PIN, HIGH);
      digitalWrite(WAIT_PIN, LOW);
    }
  }
}
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
      Serial.println(F("+ Stop MP3 player mode, return to the Altair 8800 emulator."));
      // programState = PROGRAM_WAIT;    // controlStopLogic changes programState to wait.
      controlStopLogic();
      digitalWrite(HLDA_PIN, LOW);
    } else {
      Serial.println(F("+ MP3 player mode."));
      programState = PLAYER_RUN;
      digitalWrite(HLDA_PIN, HIGH);
      digitalWrite(WAIT_PIN, LOW);
    }
  }
}

String getSenseSwitchValue() {
  byte bValue = toggleSenseByte();
  String sValue = String(bValue, BIN);
  int addZeros = 8 - sValue.length();
  for (int i = 0; i < addZeros; i++) {
    sValue = "0" + sValue;
  }
  return sValue;
}

// -----------------------------------------------------
boolean confirmWrite = false;
void checkConfirmUploadSwitch() {
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
    Serial.println(F("+ AUX1 up, Upload Switched for confirming write."));
#endif
    confirmWrite = true;
  }
}
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
#ifdef INCLUDE_SDCARD
    String senseSwitchValue = getSenseSwitchValue();
    String theFilename = senseSwitchValue + ".bin";
    if (theFilename != "11111111.bin") {
      saveClearLcdScreenData();
      lcdPrintln(0, "Confirm write> ");
      //             1234567890123456
      lcdPrintln(1, "File: " + senseSwitchValue);
      //
      confirmWrite = false;
      // -------------------------------------------------------
      // Must confirm within X seconds (milliseconds).
      unsigned long timer = millis();
      switchAux2up = false; // Required to reset the switch state for confirmation.
      while (!confirmWrite && (millis() - timer < 3000)) {
        checkConfirmUploadSwitch();
        delay(100);
      }
      // -------------------------------------------------------
      //
      if (confirmWrite) {
        Serial.print(F("+ Write memory to filename: "));
        Serial.println(theFilename);
        writeProgramMemoryToFile(theFilename);
      } else {
        // Serial.print(F("+ Write cancelled."));
        lcdPrintln(1, "Write cancelled.");
        //             1234567890123456
      }
      confirmWrite = 0;   // Reset for next time.
      delay(2000); // Give to read the resulting message.
      restoreLcdScreenData();
    } else {
      Serial.println(F("- Warning, disabled, write to filename: 11111111.bin."));
      ledFlashError();
    }
#endif
  }
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
#ifdef INCLUDE_SDCARD
    String theFilename = getSenseSwitchValue() + ".bin";
    if (theFilename == "11111111.bin") {
      Serial.println(F("+ Set to download over the serial port."));
      programState = SERIAL_DOWNLOAD;
    } else if (theFilename == "00000000.bin") {
      //             1234567890123456
      lcdPrintln(1, "Zero out memory");
      zeroOutMemory();
      controlResetLogic();
    } else {
      Serial.print(F("+ Read the filename into memory: "));
      Serial.println(theFilename);
      readProgramFileIntoMemory(theFilename);
    }
#endif
  }
}
#endif

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------
// Infrared options when a program is NOT running.

void displayStatusAddressData() {
  //
  dataByte = memoryData[programCounter];
  lightsStatusAddressData(statusByte, programCounter, dataByte);
  //
#ifdef INFRARED_MESSAGES
  Serial.print(F("Addr: "));
  sprintf(charBuffer, "%4d:", programCounter);
  Serial.print(charBuffer);
  Serial.print(F(" Data: "));
  printData(dataByte);
  Serial.println("");
#endif
}

void infraredControl() {
  // Serial.print(F("+ infraredControl: "));
  // Serial.println(results.value);
  switch (results.value) {
    case 0xFFFFFFFF:
      // Ignore. This is from holding the key down.
      break;
    // -----------------------------------
    case 0xFF10EF:
    case 0xE0E0A659:
      Serial.println(F("+ Key <"));
      break;
    case 16734885:
    // case 0xFF5AA5:
    case 0xE0E046B9:
      Serial.println(F("+ Key > - next: SINGLE STEP toggle/button switch."));
      statusByte = statusByte & HLTA_OFF;
      processData();
      break;
    case 0xFF18E7:
    case 0xE0E006F9:
      // Serial.println(F("+ Key up"));
      Serial.println(F("+ Key ^ - Run process."));
      programState = PROGRAM_RUN;
      digitalWrite(WAIT_PIN, LOW);
      // statusByte = statusByte & WAIT_OFF;
      statusByte = statusByte & HLTA_OFF;
      break;
    case 0xFF4AB5:
    case 0xE0E08679:
      Serial.println(F("+ Key down"));
      break;
    case 0xFF38C7:
    case 0xE0E016E9:
      // Serial.println(F("+ Key OK - Toggle RUN and STOP."));
      Serial.println(F("+ Run process."));
      programState = PROGRAM_RUN;
      digitalWrite(WAIT_PIN, LOW);
      // statusByte = statusByte & WAIT_OFF;
      statusByte = statusByte & HLTA_OFF;
      break;
    // -----------------------------------
    case 0xFF9867:
    case 0xE0E08877:
      Serial.print(F("+ Key 0:"));
      Serial.println("");
      break;
    case 16753245:
      // Serial.println(F("+ Key 1: "));
      Serial.println("+ Examine Previous.");
      programCounter--;
      displayStatusAddressData();
      break;
    case 16736925:
      // case 0xFF629D:
      // Serial.println(F("+ Key 2: "));
      Serial.println(F("+ Examine."));
      displayStatusAddressData();
      break;
    case 0xFFE21D:
      // Serial.println(F("+ Key 3: "));
      Serial.println("+ Examine Next.");
      programCounter++;
      displayStatusAddressData();
      break;
    case 0xFF22DD:
      Serial.print(F("+ Key 4: "));
      Serial.println("");
      break;
    case 0xFF02FD:
      Serial.print(F("+ Key 5: "));
      Serial.println("");
      break;
    case 0xFFC23D:
      Serial.print(F("+ Key 6: "));
      Serial.println("");
      break;
    case 0xFFE01F:
      Serial.print(F("+ Key 7: "));
      Serial.println("");
      break;
    case 0xFFA857:
      Serial.print(F("+ Key 8: "));
      Serial.println("");
      break;
    case 0xFF906F:
      Serial.print(F("+ Key 9: "));
      Serial.println("");
      break;
    // -----------------------------------
    case 0xFF6897:
    case 0xE0E01AE5:
      Serial.println(F("+ Key * (Return) - Reset"));
      // Use as Reset when running.
      controlResetLogic();
      displayStatusAddressData();
      break;
    case 0xFFB04F:
    case 0xE0E0B44B:
      Serial.println(F("+ Key # (Exit)"));
      break;
    // -----------------------------------
    default:
      /*
        Serial.print("+ Result value: ");
        Serial.print(results.value);
        Serial.print(" HEX:");
        Serial.println(results.value, HEX);
      */
      break;
      // -----------------------------------
  } // end switch

  irrecv.resume();

}

// -----------------------------------------------------------------------------
// Infrared options while a program is running.

void infraredRunning() {
  // Serial.println(F("+ infraredSwitch"));
  switch (results.value) {
    case 0xFFFFFFFF:
      // Ignore. This is from holding the key down.
      break;
    // -----------------------------------
    case 0xFF10EF:
    case 0xE0E0A659:
      // Serial.println(F("+ Key < - previous"));
      break;
    case 0xFF5AA5:
    case 0xE0E046B9:
      // Serial.println(F("+ Key > - next."));
      break;
    case 0xFF18E7:
    case 0xE0E006F9:
      // Serial.println(F("+ Key up"));
      break;
    case 0xFF4AB5:
    case 0xE0E08679:
      // Serial.println(F("+ Key down"));
      Serial.println(F("+ Stop process."));
      programState = PROGRAM_WAIT;
      digitalWrite(WAIT_PIN, HIGH);
      // statusByte = statusByte | WAIT_ON;
      statusByte = statusByte & HLTA_OFF;
      displayStatusAddressData();
      break;
    case 0xFF38C7:
    case 0xE0E016E9:
      // Serial.println(F("+ Key OK - Toggle RUN and STOP."));
      Serial.println(F("+ Stop process."));
      programState = PROGRAM_WAIT;
      digitalWrite(WAIT_PIN, HIGH);
      // statusByte = statusByte | WAIT_ON;
      statusByte = statusByte & HLTA_OFF;
      displayStatusAddressData();
      break;
    // -----------------------------------
    case 0xFF9867:
    case 0xE0E08877:
      // Serial.println(F("+ Key 0:"));
      break;
    case 0xFFA25D:
      // Serial.println(F("+ Key 1: "));
      break;
    case 0xFF629D:
      // Serial.println(F("+ Key 2: "));
      break;
    case 0xFFE21D:
      // Serial.println(F("+ Key 3: "));
      break;
    case 0xFF22DD:
      // Serial.println(F("+ Key 4: "));
      break;
    case 0xFF02FD:
      // Serial.println(F("+ Key 5: "));
      break;
    case 0xFFC23D:
      // Serial.println(F("+ Key 6: "));
      break;
    case 0xFFE01F:
      // Serial.println(F("+ Key 7: "));
      break;
    case 0xFFA857:
      // Serial.println(F("+ Key 8: "));
      break;
    case 0xFF906F:
      // Serial.println(F("+ Key 9: "));
      break;
    // -----------------------------------
    case 0xFF6897:
    case 0xE0E01AE5:
      // Serial.println(F("+ Key * (Return)"));
      // Use as Reset when running.
      controlResetLogic();
      break;
    case 0xFFB04F:
    case 0xE0E0B44B:
      // Serial.println(F("+ Key # (Exit)"));
      break;
    // -----------------------------------
    default:
      // Serial.print("+ Result value: ");
      // Serial.println(results.value, HEX);
      break;
      // -----------------------------------
  } // end switch

  irrecv.resume();

}

// -----------------------------------------------------------------------------
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
  // readStatusByte = readStatusByte & WAIT_OFF;
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
    processDataLights();
  }
  digitalWrite(HLDA_PIN, LOW);  // Returning to the emulator.
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
  irrecv.enableIRIn();
  Serial.println(F("+ Infrared receiver ready for input."));

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
  // statusByte = statusByte | WAIT_ON;
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
  processDataLights(); // Uses: statusByte, curProgramCounter, dataByte
  Serial.println(F("+ Front panel LED lights are initialized."));

  // ----------------------------------------------------
#ifdef SETUP_SDCARD
  // The csPin pin is connected to the SD card module select pin (CS).
  //            1234567890123456
  lcdPrintln(0, "Init SD card mod");
  if (!SD.begin(csPin)) {
    Serial.println("- Error initializing SD card module.");
    ledFlashError();
  } else {
    Serial.println("+ SD card module is initialized.");
    ledFlashSuccess();
  }
  delay(2000);
#endif

  // ----------------------------------------------------
  // Initialize the Real Time Clock (RTC).
#ifdef SETUP_CLOCK
  //            1234567890123456
  lcdPrintln(0, "Init Clock");
  if (!rtc.begin()) {
    Serial.println("- Error: Real time clock not found, not set.");
    ledFlashError();
  } else {
    ledFlashSuccess();
    Serial.println("+ Real time clock is initialized.");
  }
  delay(2000);
#endif

  // ----------------------------------------------------
  // Future option to Read and Run an initialization program.
  // Requires a new function:
  // + The ability to delete 00000000.bin, which is the option not to read and run.
  // ---------------------------
  // + If 00000000.bin exists, read it and run it.
  // Serial.print(F("+ Program loaded from memory array."));
  // Serial.println(F(" It will run automatically."));
  // + Else, display the splash screen.
  lcdSplash();
  // ----------------------------------------------------
  controlResetLogic();
  Serial.println(F("+++ Program system reset."));
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
      if (irrecv.decode(&results)) {
        // Future: use the keypress value(1-8) as input into the running program via IN opcode.
        infraredRunning();
      }
      if (pcfControlinterrupted) {
        checkRunningButtons();
        pcfControlinterrupted = false; // Reset for next interrupt.
      }
      break;
    // ----------------------------
    case PROGRAM_WAIT:
      // Program control: RUN, SINGLE STEP, EXAMINE, EXAMINE NEXT, Examine previous, RESET.
      if (irrecv.decode(&results)) {
        infraredControl();
      }
      if (pcfControlinterrupted) {
        checkControlButtons();
        pcfControlinterrupted = false; // Reset for next interrupt.
      }
      checkClockSwitch();
      checkPlayerSwitch();
      checkUploadSwitch();
      checkDownloadSwitch();
      delay(60);
      break;
    // ----------------------------
    case SERIAL_DOWNLOAD:
      Serial.println(F("+ State: SERIAL_DOWNLOAD"));
      // ----------------------------------------------------
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