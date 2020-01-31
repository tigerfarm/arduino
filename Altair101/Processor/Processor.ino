// -----------------------------------------------------------------------------
/*
  Altair 101 Processor program

  This is an Altair 8800 emulator program that is being developed on an Arduino Nano or Mega microprocessor.
  It emulates the basic Altair 8800 hardware--from 1975--which was built around the Intel 8080 CPU chip.
  This program includes many of the 8080 microprocessor machine instructions (opcodes).
  It has more than enough opcodes to run the classic programs, Kill the Bit and Pong.

  ---------------------------------------------
  Current/Next Work

  Don't write to SD card when toggles = 11111111.

  Get opLadSta.asm to work.
  + STA a: Store register A content to an address (a).
  + LDA a: Load register A from an address (a).
  + Use an immediate value for the address.
  + Use an address label for the address.

  ----------
  SD card module options,
  + When rebooting the Mega: if 00000000.bin exists, read it and run it.
  + Confirm saving or reading a file,
  ++ Fast flash HLDA for 1 second.
  ++ If read or write switch repeated, then run, else return to program wait status.

  ----------
  Add 1602 LED display,
  + Add OUT opcode to out characters to the LED display.
  + Add 1602 LED display clock time and to set the time.

  In checkRunningButtons(), replace for-loop with 2 if statements: reset and stop.

  -----------------------------------------------------------------------------
  Processor program sections,
    Sample machine code program in a memory array.
    Definitions: machine memory and stack memory.
    Memory Functions.
    Output: 1602 LCD
    Front Panel Status LEDs.
    Output: Front Panel LED lights.
    ----------------------------
    Process a subset of the Intel 8080 opcode instructions:
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
    loop()  : Clock cycling through memory.
    ----------------------------

  ---------------------------------------------
  Altair 8800 Operator's Manual.pdf has a description of each opcode.

  Binary calculator:
    https://www.calculator.net/binary-calculator.html

  Bitwise operators:
    https://www.arduino.cc/reference/en/language/structure/bitwise-operators/bitwiseand/

  ---------------------------------------------
    9 + -   - Nano connections
    | | |   - Infrared receiver pins
  ---------
  |S      |
  |       |
  |  ---  |
  |  | |  |
  |  ---  |
  |       |
  ---------
*/
// -----------------------------------------------------------------------------
// Code compilation options.

// #define FOR_MEGA 1
#define INCLUDE_AUX 1
#define INCLUDE_CLOCK 1
#define INCLUDE_SDCARD 1
// #define INCLUDE_LCD 1
// #define RUN_DELAY 1
// #define INFRARED_MESSAGES 1    // For a simple setup: Mega + infrared, with serial messages.
//
// #define LOG_MESSAGES 1         // Has large memory requirements.
#define SWITCH_MESSAGES 1
// #define RUN_NOW 1

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

//          Nano pin
int IR_PIN = A1;

IRrecv irrecv(IR_PIN);
decode_results results;

// -----------------------------------------------------------------------------
#include <PCF8574.h>
#include <Wire.h>

PCF8574 pcf20(0x020);
PCF8574 pcf21(0x021);

//                Nano pin for control toggle interrupt.
const int INTERRUPT_PIN = 2;

// Interrupt setup: interrupt pin to use, interrupt handler routine.
boolean pcf20interrupted = false;
void pcf20interrupt() {
  pcf20interrupted = true;
}

// -----------------------------------------------------------------------------
// Front Panel AUX Switches

#ifdef INCLUDE_AUX
//                              Mega pins
const int CLOCK_SWITCH_PIN =    A11;  // Tested pins, works: 4, A11. Doesn't work: 24, 33.
const int PLAYER_SWITCH_PIN =   A12;
const int UPLOAD_SWITCH_PIN =   A13;
const int DOWNLOAD_SWITCH_PIN = A14;
#endif

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Output LED light shift register(SN74HC595N) pins

//                Nano pin               74HC595 Pins
const int dataPinLed = 7;     // pin 14 Data pin.
const int latchPinLed = 8;    // pin 12 Latch pin.
const int clockPinLed = 9;    // pin 11 Clock pin.

#ifdef INCLUDE_AUX
// Status LED light,
// HLDA : 8080 processor go into a hold state because of other hardware.
const int HLDA_PIN = A10;     // Emulator processing (off/LOW) or clock processing (on/HIGH).
#endif

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
const byte theProgram[] = {
  //                //            ; --------------------------------------
  //                //            ; Test programs.
  B11000011, 6, 0,     //   0: jmp There
  B00000000,           //   3: nop
  B00000000,           //   4: nop
  B00000000,           //   5: nop
  B11000011, 0, 0,     //   6: jmp Start
  0                    //   9: End of program
};

// -----------------------------------------------------------------------------
// Kill the Bit program.

const byte theProgramKtb[] = {
  // ------------------------------------------------------------------
  // Kill the Bit program.
  // Before starting, make sure all the sense switches are in the down position.
  B00100001, 0, 0,     //   0: lxi h,0
  B00010110, 128,      //   3: mvi 00010110 (080h)
  B00000001, 0, 5,     //   5: lxi b,500h
  B00011010,           //   8: ldax d
  B00011010,           //   9: ldax d
  B00011010,           //  10: ldax d
  B00011010,           //  11: ldax d
  B00001001,           //  12: dad b
  B11010010, 8, 0,     //  13: jnc Begin
  B11011011, 255,      //  16: in 255 (0ffh)
  B10101010,           //  18: xra d
  B00001111,           //  19: rrc
  B01010111,           //  20: mov d,a
  B11000011, 8, 0,     //  21: jmp Begin
  0                    //  24: End of program
};

// -----------------------------------------------------------------------------
// Memory definitions

const int memoryBytes = 1024;  // When using Mega: 1024, for Nano: 256
byte memoryData[memoryBytes];
unsigned int programCounter = 0;     // Program address value

const int stackBytes = 32;
int stackData[memoryBytes];
unsigned int stackPointer = stackBytes;

// -----------------------------------------------------------------------------
// Memory Functions

char charBuffer[17];

void initMemoryToZero() {
  Serial.println(F("+ Initialize all memory bytes to zero."));
  for (int i = 0; i < memoryBytes; i++) {
    memoryData[i] = 0;
  }
}

void copyByteArrayToMemory(byte btyeArray[], int arraySize) {
  Serial.println(F("+ Copy the program into the computer's memory array."));
  for (int i = 0; i < arraySize; i++) {
    memoryData[i] = btyeArray[i];
  }
  Serial.println(F("+ Copied."));
}

void listByteArray(byte btyeArray[], int arraySize) {
  Serial.println(F("+ List the program."));
  Serial.println(F("++   Address:       Data value"));
  for (int i = 0; i < arraySize; i++) {
    Serial.print(F("+ Addr: "));
    sprintf(charBuffer, "%4d:", i);
    Serial.print(charBuffer);
    Serial.print(F(" Data: "));
    printData(btyeArray[i]);
    Serial.println("");
  }
  Serial.println(F("+ End of listing."));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// 1602 LCD
#ifdef INCLUDE_LCD

#include<Wire.h>
#include<LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

String theLine = "";
const int displayColumns = 16;
void displayPrintln(int theRow, String theString) {
  // To overwrite anything on the current line.
  String printString = theString;
  int theRest = displayColumns - theString.length();
  if (theRest < 0) {
    // Shorten to the display column length.
    printString = theString.substring(0, displayColumns);
  } else {
    // Buffer with spaces to the end of line.
    while (theRest < displayColumns) {
      printString = printString + " ";
      theRest++;
    }
  }
  lcd.setCursor(0, theRow);
  lcd.print(printString);
}

void readyLcd() {
  lcd.init();
  lcd.backlight();
  //                1234567890123456
  displayPrintln(0, "Altair 101");
  theLine = "LCD ready...";
  displayPrintln(1, theLine);
  // delay(3000);
  // lcd.clear();
}
#endif

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Front Panel Status LEDs

// ------------------------------
// Status LEDs
//
// Info: page 33 of the Altair 8800 oprator's manaul.
// Not in use:
// INTE : On, interrupts enabled.
// INT : An interrupt request has been acknowledged.
// PROT : Useful only if RAM has page protection impliemented. I'm not implementing PROT.

// Ready to wire and test the Standalone LED,
// HLDA : 8080 processor go into a hold state because of other hardware such as the clock.

// ------------
// Bit patterns for the status shift register (SN74HC595N):

byte statusByte = B00000000;        // By default, all are OFF.

// Use OR to turn ON. Example:
//  statusByte = statusByte | MEMR_ON;
const byte MEMR_ON =    B10000000;  // MEMR   The memory bus will be used for memory read data.
const byte INP_ON =     B01000000;  // INP    The address bus containing the address of an input device. The input data should be placed on the data bus when the data bus is in the input mode
const byte M1_ON =      B00100000;  // M1     Machine cycle 1, fetch opcode.
const byte OUT_ON =     B00010000;  // OUT    The address contains the address of an output device and the data bus will contain the out- put data when the CPU is ready.
const byte HLTA_ON =    B00001000;  // HLTA   Machine opcode hlt, has halted the machine.
const byte STACK_ON =   B00000100;  // STACK  Stack process
const byte WO_ON =      B00000010;  // WO     Write out (inverse logic)
const byte WAIT_ON =    B00000001;  // WAIT   For now, use this one for WAIT light status

// Use AND to turn OFF. Example:
//  statusByte = statusByte & M1_OFF;
const byte MEMR_OFF =   B01111111;
const byte INP_OFF =    B10111111;
const byte M1_OFF =     B11011111;
const byte OUT_OFF =    B11101111;
const byte HLTA_OFF =   B11110111;
const byte STACK_OFF =  B11111011;
const byte WO_OFF =     B11111101;
const byte WAIT_OFF =   B11111110;

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

// For STA and LDA,
// See the video: https://www.youtube.com/watch?v=3_73NwB6toY : 1:30 to 6:05 time.
// ---
// Process instruction: lda 40Q
// First:
// Opcode fetch: on: MEMR MI WO
// MEMR : Memory instruction
// MI : opcode fetch.
// WO : inverse logic for write output.
// Next
// Memory read: on: MEMR WO
// Fetch lb.
// Next
// Memory read: on: MEMR WO
// Fetch hb.
// Next, loading from memory.
// Memory read: on: MEMR WO
// Fetching the data, shows the address (40Q) and the data (at 40Q) on the LED lights.
// ---
// Next, to process next instruction: sta 41Q
// Opcode fetch: on: MEMR MI WO
// MI : opcode fetch.
// WO : inverse logic for write output.
// Next
// Memory read: on: MEMR WO
// Fetch lb.
// Next
// Memory read: on: MEMR WO
// Fetch hb.
// Next, writting to memory.
// Memory read: all status lights are off. Since it is a write output, and WO is reverse logic, WO is off.
// Address LED lights: 41Q.
// Data LED lights are all on because the data lights are tied to the data input bus, which is floating.

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Definitions: Instruction Set, Opcodes, Registers

// -----------------------------------------------------------------------------
//        Code   Octal       Inst Param  Encoding Flags  Description
const byte hlt    = 0166; // hlt         01110110        Halt processor
const byte nop    = 0000; // nop         00000000        No operation
const byte out    = 0343; // out pa      11010011        Write a to output port
const byte IN     = 0333;

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
void printWord(int theValue) {
  String sValue = String(theValue, BIN);
  for (int i = 1; i <= 16 - sValue.length(); i++) {
    Serial.print("0");
  }
  Serial.print(sValue);
}

void printOctal(byte b) {
  String sOctal = String(b, OCT);
  for (int i = 1; i <= 3 - sOctal.length(); i++) {
    Serial.print("0");
  }
  Serial.print(sOctal);
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

// ------------------------------------------------------------------------
// Data LED lights displayed using shift registers.

void lightsStatusAddressData( byte status8bits, unsigned int address16bits, byte data8bits) {
  digitalWrite(latchPinLed, LOW);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, data8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, lowByte(address16bits));
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, highByte(address16bits));
  shiftOut(dataPinLed, clockPinLed, MSBFIRST, status8bits); // MSBFIRST matches the bit to LED mapping.
  digitalWrite(latchPinLed, HIGH);
}

// -----------------------------------------------------------------------------
// Processor: Processing opcode instructions

byte opcode = 0;            // Opcode being processed
int instructionCycle = 0;   // Opcode process cycle

// Instruction parameters:
byte dataByte = 0;           // db = Data byte (8 bit)

byte lowOrder = 0;           // lb: Low order byte of 16 bit value.
byte highOrder = 0;          // hb: High order byte of 16 bit value.

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
#ifdef LOG_MESSAGES || INFRARED_MESSAGES
  Serial.print(F("Addr: "));
  sprintf(charBuffer, "%4d:", programCounter);
  Serial.print(charBuffer);
  Serial.print(F(" Data: "));
  printData(dataByte);
#endif
}

void processData() {
  if (opcode == 0) {
    // Instruction cycle 1 (M1 cycle), fetch the opcode.
    statusByte = statusByte | M1_ON;  // Machine cycle 1, get an opcode.
    instructionCycle = 1;
    // If no parameter bytes (immediate data byte or address bytes), process the opcode.
    // Else, the opcode variable is set to the opcode byte value.
    opcode = 0;
#ifdef LOG_MESSAGES
    Serial.print(" > ");
#endif
    displayStatusAddressData();       // Sets the dataByte.
    processOpcode();
    programCounter++;
    if (programState == PROGRAM_WAIT) {
      // Either HLT opcode was just processed, or using STEP to step through .
      if (dataByte == hlt) {
        // HLT opcode was just processed. Try the following, display the opcode after HLT.
        displayStatusAddressData();
      }
    }
  } else {
    // Machine cycles 2 and/or 3,
    //  Getting opcode data: an immediate data byte or an address of 2 bytes,
    //  Processing the opcode data.
    instructionCycle++;
    statusByte = statusByte & M1_OFF;
    //
#ifdef LOG_MESSAGES
    Serial.print("+ ");
#endif
    displayStatusAddressData();
#ifdef LOG_MESSAGES
    Serial.print(" ");
#endif
    //
    processOpcodeData(); // programCounter incremented if not a jump.
  }
#ifdef LOG_MESSAGES
  Serial.println("");
#endif
}

// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
void processOpcode() {

  unsigned int anAddress = 0;

  switch (dataByte) {

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
    // Compare register to A, then set Carry and Zero bit flags.
    // If #=A, set Zero bit to 1. If #>A, Carry bit = 1. If #<A, Carry bit = 0.
    //    10111SSS
    case B10111000:
      dataByte = regB;
      flagZeroBit = dataByte == regA;
      flagCarryBit = dataByte > regA;
#ifdef LOG_MESSAGES
      displayCmp("B", regB);
#endif
      break;
    // ------------------------
    //    10111SSS
    case B10111001:
      dataByte = regC;
      flagZeroBit = dataByte == regA;
      flagCarryBit = dataByte > regA;
#ifdef LOG_MESSAGES
      displayCmp("C", regC);
#endif
      break;
    // ------------------------
    //    10111SSS
    case B10111010:
      dataByte = regD;
      flagZeroBit = dataByte == regA;
      flagCarryBit = dataByte > regA;
#ifdef LOG_MESSAGES
      displayCmp("D", regD);
#endif
      break;
    // ------------------------
    //    10111SSS
    case B10111011:
      dataByte = regE;
      flagZeroBit = dataByte == regA;
      flagCarryBit = dataByte > regA;
#ifdef LOG_MESSAGES
      displayCmp("E", regE);
#endif
      break;
    // ------------------------
    //    10111SSS
    case B10111100:
      dataByte = regH;
      flagZeroBit = dataByte == regA;
      flagCarryBit = dataByte > regA;
#ifdef LOG_MESSAGES
      displayCmp("H", regH);
#endif
      break;
    // ------------------------
    //    10111SSS
    case B10111101:
      dataByte = regL;
      flagZeroBit = dataByte == regA;
      flagCarryBit = dataByte > regA;
#ifdef LOG_MESSAGES
      displayCmp("L", regH);
#endif
      break;
    // ------------------------
    //    10111SSS  Stacy, need to get memory data to compare.
    case B10111110:
      flagZeroBit = regB == regA;
      flagCarryBit = regB > regA;
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
    // ----------------------------------
    //    11RP0101 Push    register pair B onto the stack. 1 cycles.
    case B11000101:
#ifdef LOG_MESSAGES
      Serial.print(F(" > push, Push register pair B:C onto the stack."));
#endif
      stackData[stackPointer--] = regC;
      stackData[stackPointer--] = regB;
      break;
    // -----------------
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
    //    11RP0101 Push    register pair D:E onto the stack. 1 cycles.
    case B11010101:
#ifdef LOG_MESSAGES
      Serial.print(F(" > push, Push register pair D:E onto the stack."));
#endif
      stackData[stackPointer--] = regE;
      stackData[stackPointer--] = regD;
      break;
    // -----------------
    //    11RP0001 Pop    register pair D:E from the stack. 1 cycles.
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
      break;
    // ----------------------------------
    //    11RP0101 Push    register pair H:L onto the stack. 1 cycles.
    case B11100101:
#ifdef LOG_MESSAGES
      Serial.print(F(" > push, Push register pair H:L onto the stack."));
#endif
      stackData[stackPointer--] = regL;
      stackData[stackPointer--] = regH;
      break;
    // -----------------
    //    11RP0001 Pop    register pair H:L from the stack. 1 cycles.
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
    case hlt:
#ifdef LOG_MESSAGES
      Serial.print(F("+ HLT opcode, program halted."));
#else
      Serial.println(F("\n+ HLT opcode, program halted."));
#endif
      controlStopLogic();
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
    // inr d : increment a register. To do: update the flags.
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
      controlStopLogic();
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
    case nop:
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
      dataByte = memoryData[hlValue];
#ifdef LOG_MESSAGES
      Serial.print(F(" > ora, OR data from address register M(H:L): "));
      printByte(dataByte);
      Serial.print(F(", with register A: "));
      printByte(regA);
      Serial.print(F(" = "));
#endif
      regA = regA | dataByte;
#ifdef LOG_MESSAGES
      printByte(regA);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case out:
      opcode = out;
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
      printData(dataByte);
      Serial.println(F(""));
      Serial.print(F("- Error, at programCounter: "));
      printData(programCounter);
      Serial.println(F(""));
#endif
      controlStopLogic();
  }
}

// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
void processOpcodeData() {

  // Process data types:
  // 1) Immediate data byte:
  // + Instruction cycle 1 (M1), get the opcode in processOpcode().
  // + Instruction cycle 2, get the immediate data byte and process it.
  // 2) Address bytes:
  // + Instruction cycle 1 (M1), get the opcode in processOpcode().
  // + Instruction cycle 2, get the lower data byte (lb: lowByte).
  // + Instruction cycle 3, get the higher data byte (hb: lowByte), and process the address, hb:lb.

  // Note,
  //    if not jumping, increment programCounter.
  //    if jumping, don't increment programCounter.

  dataByte = memoryData[programCounter];

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
    case out:
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
        case 1:
          Serial.print(F(", terminal output to be implemented on LCD."));
          break;
        case 3:
          // Handled before this switch statement.
          break;
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
        default:
          regA = 0;
#ifdef LOG_MESSAGES
          Serial.print(F(". OUT not implemented on this port."));
#endif
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
        return;
      }
      // instructionCycle == 4
      hlValue = highOrder * 256 + lowOrder;
      memoryData[hlValue] = regA;
      statusByte = 0;
      // Turn all the status lights off, during this step.
      lightsStatusAddressData(statusByte, hlValue, regA);
      // Stacy, to do:
      //  Set address lights to hb:lb. Set data lights to regA.
      //  Turn status light MEMR and WO are no, MI is off, during this step.
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
        return;
      }
      // instructionCycle == 4
      //  Status lights, MEMR and WO are on, MI is off, during this step.
      statusByte = statusByte & M1_OFF;
      //
      hlValue = highOrder * 256 + lowOrder;
      regA = memoryData[hlValue];
      lightsStatusAddressData(statusByte, hlValue, regA);
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
      controlStopLogic();
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
  int delayTime = 60;
  lightsStatusAddressData(0, 0, B00000000);
  delay(delayTime);
  for (int i = 0; i < 2; i++) {
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
  // Reset to original.
  lightsStatusAddressData(statusByte, programCounter, dataByte);
}
void ledFlashError() {
  int delayTime = 300;
  for (int i = 0; i < 3; i++) {
    lightsStatusAddressData(0, 0, B11111111);
    delay(delayTime);
    lightsStatusAddressData(0, 0, B00000000);
    delay(delayTime);
  }
  // Reset to original.
  lightsStatusAddressData(statusByte, programCounter, dataByte);
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
  if (SD.exists(theFilename)) {
    // Serial.println("++ Exists, so it can be read.");
  } else {
    Serial.print(F("- Read ERROR, file doesn't exist: "));
    Serial.println(theFilename);
    ledFlashError();
    sdcardFailed = true;
    digitalWrite(HLDA_PIN, LOW);
    return;
  }
  statusByte = statusByte | INP_ON;
  statusByte = statusByte & M1_OFF;
  lightsStatusAddressData(statusByte, programCounter, dataByte);
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
  statusByte = statusByte & INP_OFF;
  controlResetLogic();
}

#endif

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Clock

#ifdef INCLUDE_CLOCK

// For the clock board.
#include "RTClib.h"
RTC_DS3231 rtc;
DateTime now;

int theCounterHours = 0;
int theCounterMinutes = 0;
int theCounterSeconds = 0;

void syncCountWithClock() {
  now = rtc.now();
  theCounterHours = now.hour();
  theCounterMinutes = now.minute();
  theCounterSeconds = now.second();
  //
  Serial.print("+ syncCountWithClock,");
  Serial.print(" theCounterHours=");
  Serial.print(theCounterHours);
  Serial.print(" theCounterMinutes=");
  Serial.print(theCounterMinutes);
  Serial.print(" theCounterSeconds=");
  Serial.println(theCounterSeconds);
}

// -----------------------------------------------------------------------------
boolean HLDA_ON = false;
void processClockNow() {
  //
  now = rtc.now();
  //
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
      theBinaryHour1 = B00000001; // Note, on the shift register, B00000001 is not wired, not used.
      theBinaryHour2 = 0;
      break;
    case 2:
      theBinaryHour1 = B00000010;
      theBinaryHour2 = 0;
      break;
    case 3:
      theBinaryHour1 = B00000100;
      theBinaryHour2 = 0;
      break;
    case 4:
      theBinaryHour1 = B00001000;
      theBinaryHour2 = 0;
      break;
    case 5:
      theBinaryHour1 = B00010000;
      theBinaryHour2 = 0;
      break;
    case 6:
      theBinaryHour1 = B00100000;
      theBinaryHour2 = 0;
      break;
    case 7:
      theBinaryHour1 = B01000000;
      theBinaryHour2 = 0;
      break;
    case 8:
      theBinaryHour1 = B10000000;
      theBinaryHour2 = 0;
      break;
    case 9:
      theBinaryHour1 = 0;
      theBinaryHour2 = B00000001;
      break;
    case 10:
      theBinaryHour1 = 0;
      theBinaryHour2 = B00000010;
      break;
    case 11:
      theBinaryHour1 = 0;
      theBinaryHour2 = B00000100;
      break;
    case 12:
      theBinaryHour1 = 0;
      theBinaryHour2 = B00001000;
      break;
  }
  // ----------------------------------------------
  // Need enter: theBinaryMinute, theBinaryHour1, theBinaryHour2, into the following:
  // Data byte LED lights: theBinaryMinute.
  // Address word LED lights: theBinaryHour2 & theBinaryHour1.
  unsigned int hourWord = theBinaryHour2 * 256 + theBinaryHour1;
  lightsStatusAddressData(statusByte, hourWord, theBinaryMinute);
}

// ------------------------
void clockRun() {
  // theCounterSeconds = 99;  // 99 will cause the immediate display of time.
  syncCountWithClock();
  displayTheTime( theCounterMinutes, theCounterHours );
  while (programState == CLOCK_RUN) {
    processClockNow();
    checkRunningButtons();
    checkClockSwitch();
    delay(100);
  }
}
#endif

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Front Panel Switches

// -------------------------
// Get Front Panel Toggles value, the sense switches.

int toggleSenseByte() {
  // Invert byte bits using bitwise not operator: "~";
  // Bitwise "not" operator to invert bits:
  //  int a = 103;  // binary:  0000000001100111
  //  int b = ~a;   // binary:  1111111110011000 = -104
  byte toggleByte = ~pcf21.read8();
  return toggleByte;
}

// -------------------------
// Front Panel Control Switches, when a program is not running.

// Only do the action once, don't repeat if the button is held down.
// Don't repeat action if the button is not pressed.

const int pinStop = 0;
const int pinRun = 1;
const int pinStep = 2;
const int pinExamine = 3;
const int pinExamineNext = 4;
const int pinDeposit = 5;
const int pinDepositNext = 6;
const int pinReset = 7;

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
  stackPointer = 0;
  opcode = 0;  // For the case when the processing cycle 2 or more.
  dataByte = memoryData[programCounter];
  lightsStatusAddressData(statusByte, programCounter, dataByte);
  if (programState == CLOCK_RUN || programState == SERIAL_DOWNLOAD) {
    programState = PROGRAM_WAIT;
    statusByte = statusByte | WAIT_ON;
  }
}

void controlStopLogic() {
  programState = PROGRAM_WAIT;
  statusByte = 0;
  statusByte = statusByte | WAIT_ON;
  statusByte = statusByte | HLTA_ON;
  dataByte = memoryData[programCounter];
  lightsStatusAddressData(statusByte, programCounter, dataByte);
}

// -------------------------
void checkControlButtons() {
  for (int pinGet = 7; pinGet >= 0; pinGet--) {
    int pinValue = pcf20.readButton(pinGet);  // Read each PCF8574 input
    switch (pinGet) {
      // -------------------
      case pinRun:
        if (pinValue == 0) {    // 0 : switch is on.
          if (!switchRun) {
            switchRun = true;
          }
        } else if (switchRun) {
          switchRun = false;
#ifdef SWITCH_MESSAGES
          Serial.println(F("+ Control, Run > run the program."));
#endif
          // Switch logic...
          programState = PROGRAM_RUN;
          statusByte = statusByte & WAIT_OFF;
          statusByte = statusByte & HLTA_OFF;
          processData();
        }
        break;
      // -------------------
      case pinStep:
        if (pinValue == 0) {
          if (!switchStep) {
            switchStep = true;
          }
        } else if (switchStep) {
          switchStep = false;
#ifdef SWITCH_MESSAGES
          Serial.println("+ Control, Step.");
#endif
          // Switch logic...
          statusByte = statusByte & HLTA_OFF;
          processData();
          dataByte = memoryData[programCounter];
          lightsStatusAddressData(statusByte, programCounter, dataByte);
        }
        break;
      // -------------------
      case pinExamine:
        if (pinValue == 0) {
          if (!switchExamine) {
            switchExamine = true;
          }
        } else if (switchExamine) {
          switchExamine = false;
          // Switch logic...
          programCounter = toggleSenseByte();
          dataByte = memoryData[programCounter];
          lightsStatusAddressData(statusByte, programCounter, dataByte);
#ifdef SWITCH_MESSAGES
          Serial.print(F("+ Control, Examine: "));
          printByte(dataByte);
          Serial.print(" = ");
          Serial.print(dataByte);
          Serial.println("");
#endif
        }
        break;
      // -------------------
      case pinExamineNext:
        if (pinValue == 0) {
          if (!switchExamineNext) {
            switchExamineNext = true;
          }
        } else if (switchExamineNext) {
          switchExamineNext = false;
          // Switch logic...
          programCounter++;
          dataByte = memoryData[programCounter];
          lightsStatusAddressData(statusByte, programCounter, dataByte);
#ifdef SWITCH_MESSAGES
          Serial.print(F("+ Control, Examine Next: "));
          printByte(dataByte);
          Serial.print(" = ");
          Serial.print(dataByte);
          Serial.println("");
#endif
        }
        break;
      // -------------------
      case pinDeposit:
        if (pinValue == 0) {
          if (!switchDeposit) {
            switchDeposit = true;
          }
        } else if (switchDeposit) {
          switchDeposit = false;
#ifdef SWITCH_MESSAGES
          Serial.println("+ Control, Deposit.");
#endif
          // Switch logic...
          dataByte = toggleSenseByte();
          memoryData[programCounter] = dataByte;
          lightsStatusAddressData(statusByte, programCounter, dataByte);
        }
        break;
      // -------------------
      case pinDepositNext:
        if (pinValue == 0) {
          if (!switchDepositNext) {
            switchDepositNext = true;
          }
        } else if (switchDepositNext) {
          switchDepositNext = false;
#ifdef SWITCH_MESSAGES
          Serial.println(F("+ Control, Deposit Next."));
#endif
          // Switch logic...
          programCounter++;
          dataByte = toggleSenseByte();
          memoryData[programCounter] = dataByte;
          lightsStatusAddressData(statusByte, programCounter, dataByte);
        }
        break;
      // -------------------
      case pinReset:
        if (pinValue == 0) {
          if (!switchReset) {
            switchReset = true;
          }
        } else if (switchReset) {
          switchReset = false;
#ifdef SWITCH_MESSAGES
          Serial.println(F("+ Control, Reset."));
#endif
          // Switch logic...
          controlResetLogic();
          break;
        }
        // -------------------
    }
  }
}

// -------------------------
// Front Panel Control Switches, when a program is running.

void checkRunningButtons() {
  for (int pinGet = 7; pinGet >= 0; pinGet--) {
    int pinValue = pcf20.readButton(pinGet);  // Read each PCF8574 input
    switch (pinGet) {
      // -------------------
      case pinStop:
        if (pinValue == 0) {    // 0 : switch is on.
          if (!switchStop) {
            switchStop = true;
          }
        } else if (switchStop) {
          switchStop = false;
          // Switch logic...
#ifdef SWITCH_MESSAGES
          Serial.println(F("+ Control, Stop > stop running the program."));
          Serial.println(F(" > Program halted."));
#endif
          controlStopLogic();
        }
        break;
      // -------------------
      case pinReset:
        if (pinValue == 0) {
          if (!switchReset) {
            switchReset = true;
          }
        } else if (switchReset) {
          switchReset = false;
#ifdef SWITCH_MESSAGES
          Serial.println(F("+ Control, Reset."));
#endif
          // Switch logic...
          controlResetLogic();
        }
        break;
    }
    // -------------------
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Front Panel AUX Switches

#ifdef INCLUDE_AUX
// Only do the action once, don't repeat if the switch is held down.
// Don't repeat action if the switch is not pressed.
boolean clockSwitchState = true;
boolean playerSwitchState = true;
boolean uploadSwitchState = true;
boolean downloadSwitchState = true;

void checkClockSwitch() {
  if (digitalRead(CLOCK_SWITCH_PIN) == HIGH) {
    if (!clockSwitchState) {
      // Serial.println(F("+ Clock switch released."));
      clockSwitchState = false;
      // Switch logic ...
      if (programState == CLOCK_RUN) {
#ifdef SWITCH_MESSAGES
        Serial.println(F("+ Stop running the clock, return to the 8080 emulator."));
#endif
        controlStopLogic();   // Changes programState to wait.
        digitalWrite(HLDA_PIN, LOW);
      } else {
        programState = CLOCK_RUN;
        digitalWrite(HLDA_PIN, HIGH);
        statusByte = statusByte & WAIT_OFF;
      }
    }
    clockSwitchState = true;
  } else {
    if (clockSwitchState) {
      // Serial.println(F("+ Clock switch pressed."));
      clockSwitchState = false;
      // Switch logic ...
    }
  }
}
void checkPlayerSwitch() {
  if (digitalRead(PLAYER_SWITCH_PIN) == HIGH) {
    if (!playerSwitchState) {
      Serial.println(F("+ Player switch released."));
      playerSwitchState = false;
      // Switch logic ...
    }
    playerSwitchState = true;
  } else {
    if (playerSwitchState) {
      Serial.println(F("+ Player switch pressed."));
      playerSwitchState = false;
      // Switch logic ...
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
void checkUploadSwitch() {
  if (digitalRead(UPLOAD_SWITCH_PIN) == HIGH) {
    if (!uploadSwitchState) {
      // Serial.println(F("+ Upload switch released."));
      uploadSwitchState = false;
      // Switch logic ...
#ifdef INCLUDE_SDCARD
      String theFilename = getSenseSwitchValue() + ".bin";
      Serial.print(F("+ Write memory to filename: "));
      Serial.println(theFilename);
      writeProgramMemoryToFile(theFilename);
#endif
    }
    uploadSwitchState = true;
  } else {
    if (uploadSwitchState) {
      // Serial.println(F("+ Upload switch pressed."));
      uploadSwitchState = false;
      // Switch logic ...
    }
  }
}
void checkDownloadSwitch() {
  if (digitalRead(DOWNLOAD_SWITCH_PIN) == HIGH) {
    if (!downloadSwitchState) {
      // Serial.println(F("+ Download switch released."));
      downloadSwitchState = false;
      // Switch logic ...
#ifdef INCLUDE_SDCARD
      String theFilename = getSenseSwitchValue() + ".bin";
      if (theFilename == "11111111.bin") {
        Serial.println(F("+ Set to download over the serial port."));
        programState = SERIAL_DOWNLOAD;
      } else {
        Serial.print(F("+ Read the filename into memory: "));
        Serial.println(theFilename);
        readProgramFileIntoMemory(theFilename);
      }
#endif
    }
    downloadSwitchState = true;
  } else {
    if (downloadSwitchState) {
      // Serial.println(F("+ Download switch pressed."));
      downloadSwitchState = false;
      // Switch logic ...
    }
  }
}
#endif

void downloadSerialBytes() {
  Serial.println(F("+ Download Bytes from the serial port into memory."));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------
// Infrared options when a program is NOT running.

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
      statusByte = statusByte & WAIT_OFF;
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
      statusByte = statusByte & WAIT_OFF;
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
      statusByte = statusByte | WAIT_ON;
      statusByte = statusByte & HLTA_OFF;
      displayStatusAddressData();
      break;
    case 0xFF38C7:
    case 0xE0E016E9:
      // Serial.println(F("+ Key OK - Toggle RUN and STOP."));
      Serial.println(F("+ Stop process."));
      programState = PROGRAM_WAIT;
      statusByte = statusByte | WAIT_ON;
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
// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200); // 115200 or 9600
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println(F("+++ Setup."));

  // ----------------------------------------------------
#ifdef INCLUDE_LCD
  readyLcd();
  Serial.println(F("+ LCD ready for output."));
#endif

  // ----------------------------------------------------
  irrecv.enableIRIn();
  Serial.println(F("+ Infrared receiver ready for input."));

  // ----------------------------------------------------
  // Front panel toggle switches.

  // AUX device switches.
#ifdef INCLUDE_AUX
  pinMode(HLDA_PIN, OUTPUT);    // Indicator: clock process (LED on) or emulator (LED off).
  digitalWrite(HLDA_PIN, LOW);  // Default to emulator.
  pinMode(CLOCK_SWITCH_PIN, INPUT_PULLUP);
  pinMode(PLAYER_SWITCH_PIN, INPUT_PULLUP);
  pinMode(UPLOAD_SWITCH_PIN, INPUT_PULLUP);
  pinMode(DOWNLOAD_SWITCH_PIN, INPUT_PULLUP);
  Serial.println(F("+ AUX device toggle switches are configured for input."));
#endif

  // PCF8574 device initialization
  // Control switches
  pcf20.begin();
  // Address/Sense switches
  pcf21.begin();
  // PCF8574 device Interrupt initialization
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), pcf20interrupt, CHANGE);
  Serial.println(F("+ All front panel toggle switches are configured for input."));

  // ----------------------------------------------------
  // Status lights are off by default.
  statusByte = statusByte | WAIT_ON;
  statusByte = statusByte | MEMR_ON;
  statusByte = statusByte | M1_ON;
  statusByte = statusByte | WO_ON;  // WO: on, Inverse logic: off when writing out. On when not.
  programCounter = 0;
  dataByte = memoryData[programCounter];
  Serial.println(F("+ Initialized: statusByte, programCounter, dataByte."));
  //
  pinMode(latchPinLed, OUTPUT);
  pinMode(clockPinLed, OUTPUT);
  pinMode(dataPinLed, OUTPUT);
  delay(300);
  lightsStatusAddressData(statusByte, programCounter, dataByte);
  Serial.println(F("+ Front panel LED lights are initialized."));

  // ----------------------------------------------------
#ifdef INCLUDE_SDCARD
  // The csPin pin is connected to the SD card module select pin (CS).
  if (!SD.begin(csPin)) {
    Serial.println("- Error initializing SD card module.");
    ledFlashError();
  } else {
    Serial.println("+ SD card module is initialized.");
    ledFlashSuccess();
  }
#endif

  // ----------------------------------------------------
  // Initialize the Real Time Clock (RTC).
#ifdef INCLUDE_CLOCK
  if (!rtc.begin()) {
    Serial.println("- Error: RTC not found, not set.");
    ledFlashError();
  }
  Serial.println("+ Read time clock is initialized.");
#endif

  // ----------------------------------------------------
  serial2.begin(9600);
  if (serial2.isListening()) {
    Serial.println("+ serial2 is listening.");
    Serial.println("+ Ready to use the second serial port for receiving program bytes.");
  }

  // ----------------------------------------------------
  // Options to Load/Read/Run a initialization program
  //
  // ---------------------------
  // + If 00000000.bin exists, read it in, and run it.
  // ---------------------------
  // + If 00000000.bin not exists, load the memory array program.
  int programSize = sizeof(theProgram);
  if (programSize > 0) {
    // List the program array.
    listByteArray(theProgram, programSize);
    copyByteArrayToMemory(theProgram, programSize);
    Serial.print(F("+ Program loaded from memory array."));
#ifdef RUN_NOW
    programState = PROGRAM_RUN;
    Serial.println(F(" It will start automatically."));
#endif
  }
  controlResetLogic();

  // ----------------------------------------------------
  Serial.println(F("\n+++ Start the processor loop."));
}

// -----------------------------------------------------------------------------
// Device Loop for processing each byte of machine code.

#ifdef RUN_DELAY
static unsigned long timer = millis();
#endif
byte readByte = 0;
int readByteCount = 0;
void loop() {

  switch (programState) {
    // ----------------------------
    case PROGRAM_RUN:
#ifdef RUN_DELAY
      // For testing, clock process timing is controlled by the timer.
      // Example, 500 : once every 1/2 second.
      if (millis() - timer >= 500) {
#endif
        processData();
#ifdef RUN_DELAY
        timer = millis();
      }
#endif
      // Program control: STOP or RESET.
      if (irrecv.decode(&results)) {
        // Future: use the keypress value(1-8) as input into the running program via IN opcode.
        infraredRunning();
      }
      if (pcf20interrupted) {
        checkRunningButtons();
        pcf20interrupted = false; // Reset for next interrupt.
      }
      break;
    // ----------------------------
    case PROGRAM_WAIT:
      // Program control: RUN, SINGLE STEP, EXAMINE, EXAMINE NEXT, Examine previous, RESET.
      if (irrecv.decode(&results)) {
        infraredControl();
      }
      if (pcf20interrupted) {
        checkControlButtons();
        pcf20interrupted = false; // Reset for next interrupt.
      }
#ifdef INCLUDE_AUX
      checkClockSwitch();
      checkPlayerSwitch();
      checkUploadSwitch();
      checkDownloadSwitch();
#endif
      delay(60);
      break;
      // ----------------------------
#ifdef INCLUDE_AUX
    case SERIAL_DOWNLOAD:
      Serial.println(F("+ State: SERIAL_DOWNLOAD"));
      //
      // Set status lights:
      // HLDA on when in this mode. Later, HLDA off (LOW), then on (HIGH) when bytes downloading (Serial.available).
      digitalWrite(HLDA_PIN, HIGH);
      // INP on
      statusByte = statusByte | INP_ON;
      statusByte = statusByte & M1_OFF;
      statusByte = statusByte & WAIT_OFF;
      lightsStatusAddressData(statusByte, programCounter, dataByte);
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
          Serial.print(" Octal:");
          printOctal(readByte);
          Serial.print(" Decimal");
          Serial.print(readByte, DEC);
          Serial.println("");
        }
        if (pcf20interrupted) {
          checkRunningButtons();
          pcf20interrupted = false; // Reset for next interrupt.
        }
      }
      Serial.print(F("+ Exit serial download state."));
      statusByte = statusByte & INP_OFF;
      statusByte = statusByte | WAIT_ON;
      if (readByteCount > 0) {
        // Program bytes were loaded. Reset and display the statusByte.
        programCounter = 0;
        dataByte = memoryData[programCounter];
        lightsStatusAddressData(statusByte, programCounter, dataByte);
      }
      digitalWrite(HLDA_PIN, LOW);  // Returning to the emulator.
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
      Serial.println(F("+ State: PLAYER_RUN. Not implemented, yet."));
      break;
#endif
  }

}
// -----------------------------------------------------------------------------
