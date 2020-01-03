// -----------------------------------------------------------------------------
/*
  Altair 101 Processor program

  This is an Altair 8800 emulator program that is being developed on an Arduino Nano microprocessor.
  It emulates the basic Altair 8800 hardware--from 1975--which was built around the Intel 8080 CPU chip.
  This program includes a number of the 8080 microprocessor machine instructions (opcodes).
  It has more than enough opcodes to run the classic programs, Kill the Bit and Pong.

  ---------------------------------------------
  Current/Next Work

  Add more opcodes,
  ...
  
  -----------------------------
  +++ Fix:
  + Not working: B11001010, 265, 0,   // 259: jz okaym1
  + Should be:   B11001010, ?, ?,   // 259: jz okaym1

 > Register A =  79 = 117 = 01001111
 > Register A = 203 = 313 = 11001011⸮- Error
------------
+ regA: 114 = 162 = 01110010
+ regB:  70 = 106 = 01000110  regC:  78 = 116 = 01001110
+ regD:  70 = 106 = 01000110  regE:  70 = 106 = 01000110
+ regH:   0 = 000 = 00000000  regL:   0 = 000 = 00000000
+ Zero bit flag: 1, Carry bit flag: 0
  -----------------------------

  SD card program read and write,
  + Read(download) and write(uplaod) switches need to be connected to Mega pins.
  + Logic to monitor and react to the switches.
  + When saving or reading a file, get the filename from the sense switches.
  ++ If switches are set to: 00000101, then the filename is: 00000101.asm.
  ++ When rebooting or resetting the Mega, if 00000000.asm exists, load and run it.
  + Display LED lights to notify read/write success or failure.

  ---------------------------------------------
  Program Development Phase

  + Create an assembler to convert assembly programs into machine code.
  ++ Basic assembler works.
  ++ Create more opcode test programs.
  ++ Add more opcodes.
  ++ Need to handle directives: org and equ.
  + Implement the next major Altair 8800 sample program, Pong.
  + Create more samples: looping, branching, calling subroutines, sense switch interation.

  ---------------------------------------------
  Processor program sections,
    Sample machine code program in a memory array.
    Definitions: machine memory and stack memory.
    Memory Functions.
    Output: 1602 LCD
    Front Panel Status LEDs.
    Output: Front Panel LED lights.
    ----------------------------
    Process a subset of the Intel 8080 opcode instructions:
    + Instruction Set control.
    + Process opcode instruction machine cycle one (M1): fetch opcode and process it.
    + Process opcode instruction machine cycles greater than 1.
    ----------------------------
    I/O: SD Card reader: save and load machine memory.
    Input: Front Panel toggle switch control and data entry events
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

// #define INCLUDE_LCD 1
// #define INCLUDE_SDCARD 1
// #define RUN_DELAY 1
// #define RUN_NOW 1
#define SWITCH_MESSAGES 1
// #define LOG_MESSAGES 1

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

// ------------------------------------------------------------------------
// Output LED light shift register(SN74HC595N) pins

//                Nano pin               74HC595 Pins
const int dataPinLed = 7;     // pin 14 Data pin.
const int latchPinLed = 8;    // pin 12 Latch pin.
const int clockPinLed = 9;    // pin 11 Clock pin.

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
const int csPin = 10;  // SD Card module is connected to Nano pin 10.

File myFile;

#endif

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
const byte theProgram[] = {
  //                //            ; --------------------------------------
  //                //            ; Test opcode ANI and ORA.
  B11000011, 86, 0,    //   0: jmp Test
  B00111110, '\n',     //   3: mvi a,'\n'
  B11100011, 3,        //   5: out 3
  B00111110, '-',      //   7: mvi a,'-'
  B11100011, 3,        //   9: out 3
  B00111110, '-',      //  11: mvi a,'-'
  B11100011, 3,        //  13: out 3
  B00111110, ' ',      //  15: mvi a,' '
  B11100011, 3,        //  17: out 3
  B00111110, 'E',      //  19: mvi a,'E'
  B11100011, 3,        //  21: out 3
  B00111110, 'r',      //  23: mvi a,'r'
  B11100011, 3,        //  25: out 3
  B00111110, 'r',      //  27: mvi a,'r'
  B11100011, 3,        //  29: out 3
  B00111110, 'o',      //  31: mvi a,'o'
  B11100011, 3,        //  33: out 3
  B00111110, 'r',      //  35: mvi a,'r'
  B11100011, 3,        //  37: out 3
  B11100011, 39,       //  39: out 39
  B01110110,           //  41: hlt
  B11000011, 86, 0,    //  42: jmp Test
  B00111110, '\n',     //  45: mvi a,'\n'
  B11100011, 3,        //  47: out 3
  B00111110, ' ',      //  49: mvi a,' '
  B11100011, 3,        //  51: out 3
  B00111110, '+',      //  53: mvi a,'+'
  B11100011, 3,        //  55: out 3
  B00111110, 'S',      //  57: mvi a,'S'
  B11100011, 3,        //  59: out 3
  B00111110, 'u',      //  61: mvi a,'u'
  B11100011, 3,        //  63: out 3
  B00111110, 'c',      //  65: mvi a,'c'
  B11100011, 3,        //  67: out 3
  B00111110, 'c',      //  69: mvi a,'c'
  B11100011, 3,        //  71: out 3
  B00111110, 'e',      //  73: mvi a,'e'
  B11100011, 3,        //  75: out 3
  B00111110, 's',      //  77: mvi a,'s'
  B11100011, 3,        //  79: out 3
  B00111110, 's',      //  81: mvi a,'s'
  B11100011, 3,        //  83: out 3
  B01110110,           //  85: hlt
  B00111110, 176,      //  86: mvi a,176
  B11100110, 248,      //  88: ani 248
  B11100011, 37,       //  90: out 37
  B11111110, 176,      //  92: cpi 176
  B11001010, 100, 0,   //  94: jz okayani
  B11000011, 3, 0,     //  97: jmp Error
  B00111110, '\n',     // 100: mvi a,'\n'
  B11100011, 3,        // 102: out 3
  B00111110, '+',      // 104: mvi a,'+'
  B11100011, 3,        // 106: out 3
  B00111110, ' ',      // 108: mvi a,' '
  B11100011, 3,        // 110: out 3
  B00111110, 'S',      // 112: mvi a,'S'
  B11100011, 3,        // 114: out 3
  B00111110, 'u',      // 116: mvi a,'u'
  B11100011, 3,        // 118: out 3
  B00111110, 'c',      // 120: mvi a,'c'
  B11100011, 3,        // 122: out 3
  B00111110, 'c',      // 124: mvi a,'c'
  B11100011, 3,        // 126: out 3
  B00111110, 'e',      // 128: mvi a,'e'
  B11100011, 3,        // 130: out 3
  B00111110, 's',      // 132: mvi a,'s'
  B11100011, 3,        // 134: out 3
  B00111110, 's',      // 136: mvi a,'s'
  B11100011, 3,        // 138: out 3
  B00111110, '-',      // 140: mvi a,'-'
  B11100011, 3,        // 142: out 3
  B00111110, ' ',      // 144: mvi a,' '
  B11100011, 3,        // 146: out 3
  B00111110, 'A',      // 148: mvi a,'A'
  B11100011, 3,        // 150: out 3
  B00111110, 'N',      // 152: mvi a,'N'
  B11100011, 3,        // 154: out 3
  B00111110, 'I',      // 156: mvi a,'I'
  B11100011, 3,        // 158: out 3
  B00111110, 73,       // 160: mvi a,73
  B00000110, 70,       // 162: mvi b,70
  B10110000,           // 164: ora b
  B11100011, 37,       // 165: out 37
  B11111110, 79,       // 167: cpi 79
  B11001010, 175, 0,   // 169: jz okayb1
  B11000011, 3, 0,     // 172: jmp Error
  B00111110, 73,       // 175: mvi a,73
  B00001110, 70,       // 177: mvi c,70
  B10110001,           // 179: ora c
  B11100011, 37,       // 180: out 37
  B11111110, 79,       // 182: cpi 79
  B11001010, 190, 0,   // 184: jz okayc1
  B11000011, 3, 0,     // 187: jmp Error
  B00111110, 73,       // 190: mvi a,73
  B00010110, 70,       // 192: mvi d,70
  B10110010,           // 194: ora d
  B11111110, 79,       // 195: cpi 79
  B11001010, 203, 0,   // 197: jz okayd1
  B11000011, 3, 0,     // 200: jmp Error
  B00111110, 73,       // 203: mvi a,73
  B00011110, 70,       // 205: mvi e,70
  B10110011,           // 207: ora e
  B11100011, 37,       // 208: out 37
  B11111110, 79,       // 210: cpi 79
  B11001010, 218, 0,   // 212: jz okaye1
  B11000011, 3, 0,     // 215: jmp Error
  B00111110, 73,       // 218: mvi a,73
  B00100110, 70,       // 220: mvi h,70
  B10110100,           // 222: ora h
  B11100011, 37,       // 223: out 37
  B11111110, 79,       // 225: cpi 79
  B11001010, 233, 0,   // 227: jz okayh1
  B11000011, 3, 0,     // 230: jmp Error
  B00111110, 73,       // 233: mvi a,73
  B00101110, 70,       // 235: mvi l,70
  B10110101,           // 237: ora l
  B11100011, 37,       // 238: out 37
  B11111110, 79,       // 240: cpi 79
  B11001010, 248, 0,   // 242: jz okayl1
  B11000011, 3, 0,     // 245: jmp Error
  B00111110, 73,       // 248: mvi a,73
  B00100110, 0,        // 250: mvi h,0
  B00101110, 0,        // 252: mvi l,0
  B10110110,           // 254: ora m
  B11100011, 37,       // 255: out 37
  B11111110, 203,      // 257: cpi 203
  B11001010, 9, 1,     // 259: jz okaym1
  B11000011, 3, 0,     // 262: jmp Error
  B00111110, '\n',     // 265: mvi a,'\n'
  B11100011, 3,        // 267: out 3
  B00111110, '+',      // 269: mvi a,'+'
  B11100011, 3,        // 271: out 3
  B00111110, ' ',      // 273: mvi a,' '
  B11100011, 3,        // 275: out 3
  B00111110, 'S',      // 277: mvi a,'S'
  B11100011, 3,        // 279: out 3
  B00111110, 'u',      // 281: mvi a,'u'
  B11100011, 3,        // 283: out 3
  B00111110, 'c',      // 285: mvi a,'c'
  B11100011, 3,        // 287: out 3
  B00111110, 'c',      // 289: mvi a,'c'
  B11100011, 3,        // 291: out 3
  B00111110, 'e',      // 293: mvi a,'e'
  B11100011, 3,        // 295: out 3
  B00111110, 's',      // 297: mvi a,'s'
  B11100011, 3,        // 299: out 3
  B00111110, 's',      // 301: mvi a,'s'
  B11100011, 3,        // 303: out 3
  B00111110, '-',      // 305: mvi a,'-'
  B11100011, 3,        // 307: out 3
  B00111110, ' ',      // 309: mvi a,' '
  B11100011, 3,        // 311: out 3
  B00111110, 'O',      // 313: mvi a,'O'
  B11100011, 3,        // 315: out 3
  B00111110, 'R',      // 317: mvi a,'R'
  B11100011, 3,        // 319: out 3
  B00111110, 'A',      // 321: mvi a,'A'
  B11100011, 3,        // 323: out 3
  B00000000,           // 325: nop
  B11000011, 45, 0,    // 326: jmp Halt
  0                    // 329: End of program
};

// -----------------------------------------------------------------------------
// Kill the Bit program.

const byte theProgramKtb[] = {
  // ------------------------------------------------------------------
  // Kill the Bit program.
  // Before starting, make sure all the sense switches are in the down position.
  B00000000,           //   0: nop
  B00100001, 0, 0,     //   1: lxi h,0
  B00010110, 0x80,      //   4: mvi d,080h ... Change 080h to 0x80 to 128.
  B00000001, 0, 5,     //   6: lxi b,5
  B00011010,           //   9: ldax d
  B00011010,           //  10: ldax d
  B00011010,           //  11: ldax d
  B00011010,           //  12: ldax d
  B00001001,           //  13: dad b
  B11010010, 9, 0,     //  14: jnc Begin
  B11011011, 0xff,      //  17: in 0ffh ... Change 0ffh to 0xff to 255.
  B10101010,           //  19: xra d
  B00001111,           //  20: rrc
  B01010111,           //  21: mov d,a
  B11000011, 9, 0,     //  22: jmp Begin
  0                    //  25: End of program
};

// -----------------------------------------------------------------------------
// Memory definitions

const int memoryBytes = 1024;  // When using Mega: 1024
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
// Bit pattern for the status shift register (SN74HC595N):
// B10000000 : MEMR   The memory bus will be used for memory read data.
// B01000000 : INP    The address bus containing the address of an input device. The input data should be placed on the data bus when the data bus is in the input mode
// B00100000 : M1     Machine cycle 1, fetch opcode.
// B00010000 : OUT    The address contains the address of an output device and the data bus will contain the out- put data when the CPU is ready.
// B00001000 : HLTA   Machine opcode hlt, has halted the machine.
// B00000100 : STACK  Stack process
// B00000010 : WO     Write out (inverse logic)
// B00000001 : WAIT   For now, use this one for WAIT light status
// Not in use:
// INTE : On, interrupts enabled.
// PROT : Useful only if RAM has page protection impliemented. I'm not implementing PROT.
// HLDA : 8080 processor go into a hold state because of other hardware.
// INT : An interrupt request has been acknowledged.
//
byte statusByte = B00000000;        // By default, all are OFF.
const byte MEMR_ON =    B10000000;  // Use OR  to turn ON.
const byte INP_ON =     B01000000;
const byte M1_ON =      B00100000;
const byte OUT_ON =     B00010000;
const byte HLTA_ON =    B00001000;
const byte STACK_ON =   B00000100;
const byte WO_ON =      B00000010;
const byte WAIT_ON =    B00000001;

const byte MEMR_OFF =   B01111111;  // Use AND to turn OFF.
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

// For STA and LDa,
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

// ------------------------------------
// Processing opcodes and opcode cycles

// -----------------------------------------------------------------------------
//        Code   Octal       Inst Param  Encoding Flags  Description
const byte hlt    = 0166; // hlt         01110110        Halt processor
const byte nop    = 0000; // nop         00000000        No operation
const byte out    = 0343; // out pa      11010011        Write a to output port
const byte IN     = 0333;
// IN p      11011011 pa       -       Read input from port a, into A
// 0333, 0377,  // IN 0ffh    ; Check toggle sense switches for non-zero input. Used in the program, kill the bit.

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
#ifdef LOG_MESSAGES
  Serial.print(F("Addr: "));
  sprintf(charBuffer, "%4d:", programCounter);
  Serial.print(charBuffer);
  Serial.print(F(" Data: "));
  dataByte = memoryData[programCounter];
  printData(dataByte);
#endif
}

void processData() {
  if (opcode == 0) {
#ifdef LOG_MESSAGES
    Serial.print("> ");
#endif
    statusByte = statusByte | M1_ON; // Machine cycle 1, get an opcode.
    displayStatusAddressData();
#ifdef LOG_MESSAGES
    Serial.print(" ");
#endif
    processOpcode();
    programCounter++;
    instructionCycle = 0;
  } else {
#ifdef LOG_MESSAGES
    Serial.print("+ ");
#endif
    statusByte = statusByte & M1_OFF; // Machine cycle 1+x, getting opcode data.
    displayStatusAddressData();
#ifdef LOG_MESSAGES
    Serial.print(" ");
#endif
    instructionCycle++;
    processOpcodeData();      // Example: sta 42: 0. fetch the opcode, 1. fetch lb, 2. fetch hb, 3. store the data.
  }
#ifdef LOG_MESSAGES
  Serial.println("");
#endif
}

// -----------------------------------
// Process flags and values.
boolean runProgram = false;
boolean halted = false;       // Set true for an hlt opcode.

boolean flagCarryBit = false; // Set by dad. Used jnc.
boolean flagZeroBit = true;   // Set by cpi. Used by jz.

byte bit7;                    // For capturing a bit when doing bitwise calculations.
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

void displayCmp(String theRegister, byte theRegValue) {
  Serial.print(F("> cmp, compare register "));
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

// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
void processOpcode() {

  unsigned int anAddress = 0;
  byte dataByte = memoryData[programCounter];
  switch (dataByte) {

    // ---------------------------------------------------------------------
    // ++ ADI #     11000110 db       ZSCPA   Add immediate to A
    case B11000110:
      opcode = B11000110;
#ifdef LOG_MESSAGES
      Serial.print(F("> ani, Add immedite number to register A."));
#endif
      break;
    //-----------------------
    // ++ SUI #     11010110 db       ZSCPA   Subtract immediate from A
    case B11010110:
      opcode = B11010110;
#ifdef LOG_MESSAGES
      Serial.print(F("> sui, Subtract immedite number from register A."));
#endif
      break;
    // ---------------------------------------------------------------------
    //ANI #     11100110 db       ZSPCA   AND immediate with A
    case B11100110:
      opcode = B11100110;
#ifdef LOG_MESSAGES
      Serial.print(F("> ani, AND db with register A."));
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
      Serial.print(F("> cpi, compare next data byte to A, and set Zero bit and Carry bit flags."));
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
      Serial.print(F("> call, call a subroutine."));
#endif
      break;
    // -----------------
    // RET  11001001  Unconditional return from subroutine. 1 cycles.
    case B11001001:
#ifdef LOG_MESSAGES
      Serial.print(F("> ret, Return from a subroutine to the original CALL address: "));
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
      Serial.print(F("> push, Push register pair B:C onto the stack."));
#endif
      stackData[stackPointer--] = regC;
      stackData[stackPointer--] = regB;
      break;
    // -----------------
    //    11RP0001 Pop    register pair B from the stack. 1 cycles.
    case B11000001:
#ifdef LOG_MESSAGES
      Serial.print(F("> pop, Pop register pair B:C from the stack."));
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
      Serial.print(F("> push, Push register pair D:E onto the stack."));
#endif
      stackData[stackPointer--] = regE;
      stackData[stackPointer--] = regD;
      break;
    // -----------------
    //    11RP0001 Pop    register pair D:E from the stack. 1 cycles.
    case B11010001:
#ifdef LOG_MESSAGES
      Serial.print(F("> pop, Pop register pair D:E from the stack."));
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
      Serial.print(F("> push, Push register pair H:L onto the stack."));
#endif
      stackData[stackPointer--] = regL;
      stackData[stackPointer--] = regH;
      break;
    // -----------------
    //    11RP0001 Pop    register pair H:L from the stack. 1 cycles.
    case B11100001:
#ifdef LOG_MESSAGES
      Serial.print(F("> pop, Pop register pair H:L from the stack."));
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
      Serial.print(F("> dad, 16 bit add: B:C("));
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
      Serial.print(F("> dad, 16 bit add: D:E"));
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
      Serial.print(F("> dcr register A: "));
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
      Serial.print(F("> dcr register B: "));
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
      Serial.print(F("> dcr register C: "));
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
      Serial.print(F("> dcr register D: "));
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
      Serial.print(F("> dcr register E: "));
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
      Serial.print(F("> dcr register H: "));
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
      Serial.print(F("> dcr register L: "));
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
    // ---------------------------------------------------------------------
    case hlt:
      runProgram = false;
      halted = true;
#ifdef LOG_MESSAGES
      Serial.println(F("> hlt, halt the processor."));
#else
      Serial.println("");
#endif
      statusByte = statusByte | WAIT_ON;
      statusByte = statusByte & MEMR_OFF;
      statusByte = statusByte & M1_OFF;
      statusByte = statusByte | HLTA_ON;
      // lightsStatusAddressData(statusByte, programCounter, dataByte);
      // displayStatusAddressData();
#ifdef LOG_MESSAGES
      Serial.print(F("+ Process halted."));
#else
      Serial.println(F("+ Process halted."));
#endif
      break;
    case B11011011:
      opcode = B11011011;
      // INP status light is on when reading from an input port.
      statusByte = statusByte | INP_ON;
#ifdef LOG_MESSAGES
      Serial.print(F("> IN, If input value is available, get the input byte."));
#endif
      break;
    // ---------------------------------------------------------------------
    // inr d : increment a register. To do: update the flags.
    //    00DDD100    Flags:ZSPA
    case B00111100:
#ifdef LOG_MESSAGES
      Serial.print(F("> inr register A: "));
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
      Serial.print(F("> inr register B: "));
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
      Serial.print(F("> inr register C: "));
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
      Serial.print(F("> inr register D: "));
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
      Serial.print(F("> inr register E: "));
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
      Serial.print(F("> inr register H: "));
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
      Serial.print(F("> inr register L: "));
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
    // ---------------------------------------------------------------------
    // inx : increment a register pair.
    // ------------
    case B00000011:
      // inb b
      regC++;
      if (regC == 0) {
        regB++;
      }
#ifdef LOG_MESSAGES
      Serial.print(F("> inx, increment the 16 bit register pair B:C."));
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
      Serial.print(F("> inx, increment the 16 bit register pair D:E."));
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
      // inx m (h:l)
      regL++;
      if (regL == 0) {
        regH++;
      }
#ifdef LOG_MESSAGES
      Serial.print(F("> inx, increment the 16 bit register pair H:L."));
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
      Serial.print(F("> jnz, Jump if zero bit flag is not set(false)."));
#endif
      break;
    // ----------------
    //    110CC010
    case B11001010:
      opcode = B11001010;
#ifdef LOG_MESSAGES
      Serial.print(F("> jz, Jump if Zero bit flag is set(true)."));
#endif
      break;
    // ----------------
    //    110CC010
    case B11010010:
      opcode = B11010010;
#ifdef LOG_MESSAGES
      Serial.print(F("> jnc, Jump if carry bit is not set(false)."));
#endif
      break;
    // ----------------
    //    110CC010
    case B11011010:
      opcode = B11011010;
#ifdef LOG_MESSAGES
      Serial.print(F("> jc, Jump if carry bit is set(true)."));
#endif
      break;
    // ----------------
    case B11000011:
      opcode = B11000011;
#ifdef LOG_MESSAGES
      Serial.print(F("> jmp, get address low and high order bytes."));
#endif
      break;
    // ---------------------------------------------------------------------
    // ldax RP  00RP1010 - Load indirect through BC(RP=00) or DE(RP=01)
    // ---------------
    case B00001010:
      opcode = B00001010;
#ifdef LOG_MESSAGES
      Serial.print(F("> ldax, Into register A, load data from B:C address: "));
      printOctal(regB);
      Serial.print(F(" : "));
      printOctal(regC);
#endif
      break;
    // ---------------
    case B00011010:
      opcode = B00011010;
#ifdef LOG_MESSAGES
      Serial.print(F("> ldax, Into register A, load data from D:E address: "));
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
      Serial.print(F("> lxi, Move the lb hb data, into register pair B:C = hb:lb."));
#endif
      break;
    case B00010001:
      opcode = B00010001;
#ifdef LOG_MESSAGES
      Serial.print(F("> lxi, Move the lb hb data, into register pair D:E = hb:lb."));
#endif
      break;
    case B00100001:
      opcode = B00100001;
#ifdef LOG_MESSAGES
      Serial.print(F("> lxi, Move the lb hb data, into register pair H:L = hb:lb."));
#endif
      break;
    case B00110001:
      // stacy
      // opcode = B00110001;
#ifdef LOG_MESSAGES
      Serial.print(F("> lxi, Stacy, to do: move the lb hb data, to the stack pointer address."));
#endif
      Serial.print(F(" - Error, unhandled instruction."));
      runProgram = false;
      statusByte = statusByte | WAIT_ON;
      statusByte = statusByte | M1_ON;
      statusByte = statusByte | HLTA_ON;
      // lightsStatusAddressData(statusByte, programCounter, dataByte);
      displayStatusAddressData();
      break;
    // ------------------------------------------------------------------------------------------
    /*
      B01DDDSSS         // mov d,S  ; Move from one register to another.
    */
    // ---------------------------------------------------------------------
    case B01111110:
      anAddress = word(regH, regL);
      regA = memoryData[anAddress];
#ifdef LOG_MESSAGES
      Serial.print(F("> mov"));
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
      Serial.print(F("> mov register a to b = "));
      printData(regB);
#endif
      break;
    case B01001111:
      regC = regA;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register a to c = "));
      printData(regC);
#endif
      break;
    case B01010111:
      regD = regA;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register a to d = "));
      printData(regD);
#endif
      break;
    case B01011111:
      regE = regA;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register a to e = "));
      printData(regE);
#endif
      break;
    case B01100111:
      regH = regA;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register a to h = "));
      printData(regH);
#endif
      break;
    case B01101111:
      regL = regA;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register a to l = "));
      printData(regL);
#endif
      break;
    // ---------------------------------------------------------------------
    case B01111000:
      regA = regB;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register b to a = "));
      printData(regL);
#endif
      break;
    case B01001000:
      regC = regB;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register b to c = "));
      printData(regL);
#endif
      break;
    case B01010000:
      regD = regB;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register b to d = "));
      printData(regD);
#endif
      break;
    case B01011000:
      regE = regB;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register b to e = "));
      printData(regE);
#endif
      break;
    case B01100000:
      regH = regB;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register b to h = "));
      printData(regH);
#endif
      break;
    case B01101000:
      regL = regB;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register b to l = "));
      printData(regL);
#endif
      break;
    // ---------------------------------------------------------------------
    case B01111001:
      regA = regC;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register c to a = "));
      printData(regA);
#endif
      break;
    case B01000001:
      regB = regC;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register c to b = "));
      printData(regB);
#endif
      break;
    case B01010001:
      regD = regC;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register c to d = "));
      printData(regD);
#endif
      break;
    case B01011001:
      regE = regC;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register c to e = "));
      printData(regE);
#endif
      break;
    case B01100001:
      regH = regC;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register c to h = "));
      printData(regH);
#endif
      break;
    case B01101001:
      regL = regC;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register c to l = "));
      printData(regL);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B01111010:
      regA = regD;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register d to a = "));
      printData(regA);
#endif
      break;
    case B01000010:
      regB = regD;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register d to b = "));
      printData(regB);
#endif
      break;
    case B01001010:
      regC = regD;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register d to c = "));
      printData(regC);
#endif
      break;
    case B01011010:
      regE = regD;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register d to e = "));
      printData(regE);
#endif
      break;
    case B01100010:
      regH = regD;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register d to h = "));
      printData(regH);
#endif
      break;
    case B01101010:
      regL = regD;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register d to l = "));
      printData(regL);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B01111011:
      regA = regE;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register e to a = "));
      printData(regA);
#endif
      break;
    case B01000011:
      regB = regE;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register e to b = "));
      printData(regB);
#endif
      break;
    case B01001011:
      regC = regE;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register e to c = "));
      printData(regC);
#endif
      break;
    case B01010011:
      regD = regE;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register e to d = "));
      printData(regD);
#endif
      break;
    case B01100011:
      regH = regE;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register e to h = "));
      printData(regH);
#endif
      break;
    case B01101011:
      regL = regE;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register e to l = "));
      printData(regL);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B01111100:
      regA = regH;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register h to a = "));
      printData(regA);
#endif
      break;
    case B01000100:
      regB = regH;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register h to b = "));
      printData(regB);
#endif
      break;
    case B01001100:
      regC = regH;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register h to c = "));
      printData(regC);
#endif
      break;
    case B01010100:
      regD = regH;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register h to d = "));
      printData(regD);
#endif
      break;
    case B01011100:
      regE = regH;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register h to e = "));
      printData(regE);
#endif
      break;
    case B01101100:
      regL = regH;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register h to l = "));
      printData(regL);
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B01111101:
      regA = regL;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register l to a = "));
      printData(regA);
#endif
      break;
    case B01000101:
      regB = regL;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register l to b = "));
      printData(regB);
#endif
      break;
    case B01001101:
      regC = regL;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register l to c = "));
      printData(regC);
#endif
      break;
    case B01010101:
      regD = regL;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register l to d = "));
      printData(regD);
#endif
      break;
    case B01011101:
      regE = regL;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register l to e = "));
      printData(regE);
#endif
      break;
    case B01100101:
      regH = regL;
#ifdef LOG_MESSAGES
      Serial.print(F("> mov register l to h = "));
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
      Serial.print(F("> mvi, move db address into register A."));
#endif
      break;
    case B00000110:
      opcode = B00000110;
#ifdef LOG_MESSAGES
      Serial.print(F("> mvi, move db address into register B."));
#endif
      break;
    case B00001110:
      opcode = B00001110;
#ifdef LOG_MESSAGES
      Serial.print(F("> mvi, move db address into register C."));
#endif
      break;
    case B00010110:
      opcode = B00010110;
#ifdef LOG_MESSAGES
      Serial.print(F("> mvi, move db address into register D."));
#endif
      break;
    case B00011110:
      opcode = B00011110;
#ifdef LOG_MESSAGES
      Serial.print(F("> mvi, move db address into register E."));
#endif
      break;
    case B00100110:
      opcode = B00100110;
#ifdef LOG_MESSAGES
      Serial.print(F("> mvi, move db address into register H."));
#endif
      break;
    case B00101110:
      opcode = B00101110;
#ifdef LOG_MESSAGES
      Serial.print(F("> mvi, move db address into register L."));
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case nop:
#ifdef LOG_MESSAGES
      Serial.print(F("> nop ---------------------------"));
#endif
      delay(100);
      break;
    // ------------------------------------------------------------------------------------------
    // ORA10110SSS
    case B10110000:
#ifdef LOG_MESSAGES
      Serial.print(F("> ora, OR register B: "));
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
      Serial.print(F("> ora, OR register C: "));
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
      Serial.print(F("> ora, OR register D: "));
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
      Serial.print(F("> ora, OR register E: "));
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
      Serial.print(F("> ora, OR register H: "));
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
      Serial.print(F("> ora, OR register L: "));
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
      Serial.print(F("> ora, OR data from address register M(H:L): "));
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
      Serial.print(F("> OUT, Write output to the port address(following db)."));
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B00001111:
#ifdef LOG_MESSAGES
      Serial.print(F("> rrc"));
      Serial.print(F(", Shift register A: "));
      printByte(regA);
      Serial.print(F(", right 1 bit: "));
#endif
      // # 0x0f RRC 1 CY  A = A >> 1; bit 7 = prev bit 0; CY = prev bit 0
      // Get bit 7, and use it to set bit 0, after the shift.
      bit7 = regA & B00000001;
      regA = regA >> 1;
      if (bit7 == 1) {
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
      Serial.print(F("> shld, Store register L to memory address hb:lb. Store register H to hb:lb + 1."));
#endif
      break;
    // ------------------------------------------------------------------------------------------
    case B00110010:
      opcode = B00110010;
#ifdef LOG_MESSAGES
      Serial.print(F("> sta, Store register A to the hb:lb address."));
#endif
      break;
    case B00111010:
      opcode = B00111010;
#ifdef LOG_MESSAGES
      Serial.print(F("> lda, Load register A with data from the hb:lb address."));
#endif
      break;
    // ------------------------------------------------------------------------------------------
    // XRA S     10101SSS          ZSPCA   ExclusiveOR register with A.
    //                             ZSPCA   Stacy, need to set flags.
    // ---------------
    case B10101000:
      // xra b
#ifdef LOG_MESSAGES
      Serial.print(F("> xra"));
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
      Serial.print(F("> xra"));
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
      Serial.print(F("> xra"));
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
      Serial.print(F("> xra"));
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
      Serial.print(F("> xra"));
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
      Serial.print(F("> xra"));
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
      Serial.print(F(" - Error, unknown instruction."));
#ifdef LOG_MESSAGES
#else
      Serial.println(F(""));
#endif
      runProgram = false;
      statusByte = statusByte | WAIT_ON;
      statusByte = statusByte | M1_ON;
      statusByte = statusByte | HLTA_ON;
      // lightsStatusAddressData(statusByte, programCounter, dataByte);
      displayStatusAddressData();
  }
}

// ------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------
void processOpcodeData() {
  // Note,
  //    if not jumping, increment programCounter.
  //    if jumping, don't increment programCounter.

  switch (opcode) {

    // ---------------------------------------------------------------------
    //ADI #     11000110 db   ZSCPA Add immediate number to register A.
    case B11000110:
      // instructionCycle == 1
      dataByte = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("> adi, Add immediate number:"));
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
      // instructionCycle == 1
      dataByte = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("> adi, Subtract immediate number:"));
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
      // instructionCycle == 1
      dataByte = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("> ani, AND db:"));
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
      if (instructionCycle == 1) {
        lowOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< call, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 2
      highOrder = memoryData[programCounter];
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
      // instructionCycle == 1
      dataByte = memoryData[programCounter];
      // Compare #(dataByte) to A, then set Carry and Zero bit flags.
      // If #=A, set Zero bit to 1. If #>A, Carry bit = 1. If #<A, Carry bit = 0.
      flagZeroBit = dataByte == regA;
      flagCarryBit = dataByte > regA;
#ifdef LOG_MESSAGES
      Serial.print(F("> cpi, compare the result db: "));
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
      // instructionCycle == 1
      // INP & WO are on when reading from an input port.
      // IN p      11011011 pa       -       Read input for port a, into A
      //                    pa = 0ffh        Check toggle sense switches for non-zero input.
      //                         0ffh = B11111111 = 255
      //
      dataByte = memoryData[programCounter];
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
      if (instructionCycle == 1) {
        lowOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("> jmp, lb:"));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 2
#ifdef LOG_MESSAGES
      highOrder = memoryData[programCounter];
#endif
      programCounter = word(highOrder, lowOrder);
#ifdef LOG_MESSAGES
      Serial.print(F("> jmp, hb:"));
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
      if (instructionCycle == 1) {
        lowOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< jnz, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 2
      highOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< jnz, hb: "));
      sprintf(charBuffer, "%4d:", highOrder);
      Serial.print(charBuffer);
#endif
      if (!flagZeroBit) {
        programCounter = word(highOrder, lowOrder);
#ifdef LOG_MESSAGES
        Serial.print(F("> jnz, Zero bit flag is false, jump to:"));
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
      if (instructionCycle == 1) {
        lowOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< jz, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 2
      highOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< jz, hb: "));
      sprintf(charBuffer, "%4d:", highOrder);
      Serial.print(charBuffer);
#endif
      if (flagZeroBit) {
        programCounter = word(highOrder, lowOrder);
#ifdef LOG_MESSAGES
        Serial.print(F("> jz, jump to:"));
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
      if (instructionCycle == 1) {
        lowOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< jnc, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 2
      highOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< jnc, hb: "));
      sprintf(charBuffer, "%4d:", highOrder);
      Serial.print(charBuffer);
#endif
      if (!flagCarryBit) {
        programCounter = word(highOrder, lowOrder);
#ifdef LOG_MESSAGES
        Serial.print(F("> jnc, Carry bit flag is false, jump to:"));
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
      if (instructionCycle == 1) {
        lowOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< jc, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 2
      highOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< jc, hb: "));
      sprintf(charBuffer, "%4d:", highOrder);
      Serial.print(charBuffer);
#endif
      if (flagCarryBit) {
        programCounter = word(highOrder, lowOrder);
#ifdef LOG_MESSAGES
        Serial.print(F("> jc, jump to:"));
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
      if (instructionCycle == 1) {
        regC = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< lxi, lb data: "));
        sprintf(charBuffer, "%4d:", regC);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 2
      regB = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< lxi, hb data: "));
      sprintf(charBuffer, "%4d:", regB);
      Serial.print(charBuffer);
      Serial.print(F("> B:C = "));
      printOctal(regB);
      Serial.print(F(":"));
      printOctal(regC);
#endif
      programCounter++;
      break;
    // ---------------------------------------------------------------------
    case B00010001:
      // lxi d,16-bit-address
      if (instructionCycle == 1) {
        regE = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< lxi, lb data: "));
        sprintf(charBuffer, "%4d:", regE);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 2
      regD = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< lxi, hb data: "));
      sprintf(charBuffer, "%4d:", regD);
      Serial.print(charBuffer);
      Serial.print(F("> D:E = "));
      printOctal(regD);
      Serial.print(F(":"));
      printOctal(regE);
#endif
      programCounter++;
      break;
    // -------------------
    case B00100001:
      // lxi h,16-bit-address
      if (instructionCycle == 1) {
        regL = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< lxi, lb data: "));
        sprintf(charBuffer, "%4d:", regL);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 2
      regH = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< lxi, hb data: "));
      sprintf(charBuffer, "%4d:", regH);
      Serial.print(charBuffer);
      Serial.print(F("> H:L = "));
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
      regA = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register A: "));
      printData(regA);
#endif
      programCounter++;
      break;
    case B00000110:
      regB = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register B: "));
      printData(regB);
#endif
      programCounter++;
      break;
    case B00001110:
      regC = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register C: "));
      printData(regC);
#endif
      programCounter++;
      break;
    case B00010110:
      regD = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register D: "));
      printData(regD);
#endif
      programCounter++;
      break;
    case B00011110:
      regE = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register E: "));
      printData(regE);
#endif
      programCounter++;
      break;
    case B00100110:
      regH = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register H: "));
      printData(regH);
#endif
      programCounter++;
      break;
    case B00101110:
      regL = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< mvi, move db > register L: "));
      printData(regL);
#endif
      programCounter++;
      break;
    // ---------------------------------------------------------------------
    case out:
      // instructionCycle == 1
      dataByte = memoryData[programCounter];
#ifdef LOG_MESSAGES
      Serial.print(F("< OUT, input port: "));
      Serial.print(dataByte);
#else
      // -----------------------------------------
      // Special case of output to serial monitor.
      if (dataByte == 3) {
        asciiChar = regA;
        Serial.print(asciiChar);
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
#ifdef LOG_MESSAGES
          Serial.print(F(", Serial terminal output of register A."));
          Serial.print(regA);
          Serial.print(":");
#endif
          asciiChar = regA;
          Serial.print(asciiChar);
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
        case 37:
          Serial.print(F(" > Register A = "));
          printData(regA);
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
      if (instructionCycle == 1) {
        lowOrder = memoryData[programCounter];
#ifdef LOG_MESSAGE
        Serial.print(F("< sta, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      if (instructionCycle == 2) {
        highOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< sta, hb: "));
        sprintf(charBuffer, "%4d:", highOrder);
        Serial.print(charBuffer);
#endif
        return;
      }
      // instructionCycle == 3
      hlValue = highOrder * 256 + lowOrder;
      memoryData[hlValue] = regA;
      // Stacy, to do:
      //  Set address lights to hb:lb. Set data lights to regA.
      //  Turn status light MEMR, MI, and WO off during this step.
#ifdef LOG_MESSAGES
      Serial.print(F("> sta, register A: "));
      Serial.print(regA);
      Serial.print(F(", stored to the hb:lb address."));
#endif
      programCounter++;
      break;
    // -----------------------------------------
    case B00111010:
      if (instructionCycle == 1) {
        lowOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< lda, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      if (instructionCycle == 2) {
        highOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< lda, hb: "));
        sprintf(charBuffer, "%4d:", highOrder);
        Serial.print(charBuffer);
#endif
        return;
      }
      hlValue = highOrder * 256 + lowOrder;
      regA = memoryData[hlValue];
#ifdef LOG_MESSAGES
      Serial.print(F("> lda, hb:lb address data is moved to register A: "));
      Serial.print(regA);
#endif
      programCounter++;
      break;
    // ------------------------------------------------------------------------------------------
    case B00100010:
      // shld a
      if (instructionCycle == 1) {
        lowOrder = memoryData[programCounter];
#ifdef LOG_MESSAGES
        Serial.print(F("< shld, lb: "));
        sprintf(charBuffer, "%4d:", lowOrder);
        Serial.print(charBuffer);
#endif
        programCounter++;
        return;
      }
      if (instructionCycle == 2) {
        highOrder = memoryData[programCounter];
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
      Serial.print(F("> shld, Store register L:"));
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
      runProgram = false;
      statusByte = statusByte | WAIT_ON;
      statusByte = statusByte | M1_ON;
      statusByte = statusByte | HLTA_ON;
      // lightsStatusAddressData(statusByte, programCounter, dataByte);
      displayStatusAddressData();
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

// -------------------------------------
// Write Program memory to a file.

void writeProgramMemoryToFile(String theFilename) {
  Serial.println("+ Write program memory to a new file named: ");
  Serial.print(theFilename);
  Serial.println("+ Check if file exists. ");
  if (SD.exists(theFilename)) {
    SD.remove(theFilename);
    Serial.println("++ Exists, so it was deleted.");
  } else {
    Serial.println("++ Doesn't exist.");
  }
  myFile = SD.open(theFilename, FILE_WRITE);
  if (!myFile) {
    Serial.print("- Error opening file: ");
    Serial.println(theFilename);
    return; // When used in setup(), causes jump to loop().
  }
  Serial.println("++ New file opened.");
  Serial.println("++ Write binary memory to the file.");
  for (int i = 0; i < memoryBytes; i++) {
    myFile.write(memoryData[i]);
  }
  myFile.close();
  Serial.println("+ Completed, file closed.");
}

// -------------------------------------
// Read program memory from a file.

void readProgramFileIntoMemory(String theFilename) {
  Serial.println("+ Read a file into program memory, file named: ");
  Serial.print(theFilename);
  Serial.println("+ Check if file exists. ");
  if (SD.exists(theFilename)) {
    Serial.println("++ Exists, so it can be read.");
  } else {
    Serial.println("++ Doesn't exist, cannot read.");
    return;
  }
  myFile = SD.open(theFilename);
  if (!myFile) {
    Serial.print("- Error opening file: ");
    Serial.println(theFilename);
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
    printOctal (memoryData[i]);
    Serial.print(":");
    Serial.println(memoryData[i], DEC);
#endif
    i++;
    if (i > memoryBytes) {
      Serial.println("-+ Warning, file contains more data bytes than available memory.");
    }
  }
  myFile.close();
  Serial.println("+ File closed.");
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

void controlResetLogic() {
  programCounter = 0;
  stackPointer = 0;
  dataByte = memoryData[programCounter];
  opcode = 0;  // For the case when the processing cycle 2 or more.
  lightsStatusAddressData(statusByte, programCounter, dataByte);
}

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
          runProgram = true;
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
          Serial.println(F("+ Control, Examine."));
          printByte(dataByte);
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
          Serial.println(F("+ Control, Examine Next."));
          printByte(dataByte);
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
#ifdef SWITCH_MESSAGES
          Serial.println(F("+ Control, Stop > stop running the program."));
          Serial.println(F("> hlt, halt the processor."));
#endif
          // Switch logic...
          runProgram = false;
          statusByte = 0;
          statusByte = statusByte | WAIT_ON;
          statusByte = statusByte | HLTA_ON;
          // Only here, causes a compile error: displayStatusAddressData();
          dataByte = memoryData[programCounter];
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
        }
        break;
    }
    // -------------------
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------
// Infrared options when a program is NOT running.

void infraredControl() {
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
      // Serial.println(F("+ Key > - next: SINGLE STEP toggle/button switch."));
      statusByte = statusByte & HLTA_OFF;
      processData();
      break;
    case 0xFF18E7:
    case 0xE0E006F9:
      // Serial.println(F("+ Key up"));
      Serial.println(F("+ Run process."));
      runProgram = true;
      statusByte = statusByte & WAIT_OFF;
      statusByte = statusByte & HLTA_OFF;
      break;
    case 0xFF4AB5:
    case 0xE0E08679:
      // Already stopped.
      // Serial.println(F("+ Key down"));
      // Serial.println(F("+ Stop process."));
      // runProgram = false;
      // digitalWrite(WAIT_PIN, HIGH);
      // digitalWrite(HLTA_PIN, LOW);
      break;
    case 0xFF38C7:
    case 0xE0E016E9:
      // Serial.println(F("+ Key OK - Toggle RUN and STOP."));
      Serial.println(F("+ Run process."));
      runProgram = true;
      statusByte = statusByte & WAIT_OFF;
      statusByte = statusByte & HLTA_OFF;
      break;
    // -----------------------------------
    case 0xFF9867:
    case 0xE0E08877:
      // Serial.print(F("+ Key 0:"));
      // Serial.println("");
      break;
    case 0xFFA25D:
      // Serial.println(F("+ Key 1: "));
      // Serial.println("+ Examine.");
#ifdef LOG_MESSAGES
      Serial.print("? ");
      displayStatusAddressData();
      Serial.println("");
#else
      displayStatusAddressData();
#endif
      break;
    case 0xFF629D:
      // Serial.println(F("+ Key 2: "));
      // Serial.println("+ Examine Next.");
      programCounter++;
#ifdef LOG_MESSAGES
      Serial.print("? ");
      displayStatusAddressData();
      Serial.println("");
#else
      displayStatusAddressData();
#endif
      break;
    case 0xFFE21D:
      // Serial.println(F("+ Key 3: "));
      // Serial.println("+ Examine Previous.");
      programCounter--;
#ifdef LOG_MESSAGES
      Serial.print("? ");
      displayStatusAddressData();
      Serial.println("");
#else
      displayStatusAddressData();
#endif
      break;
    case 0xFF22DD:
      // Serial.print(F("+ Key 4: "));
      // Serial.println("");
      break;
    case 0xFF02FD:
      // Serial.print(F("+ Key 5: "));
      // Serial.println("");
      break;
    case 0xFFC23D:
      // Serial.print(F("+ Key 6: "));
      // Serial.println("");
      break;
    case 0xFFE01F:
      // Serial.print(F("+ Key 7: "));
      // Serial.println("");
      break;
    case 0xFFA857:
      // Serial.print(F("+ Key 8: "));
      // Serial.println("");
      break;
    case 0xFF906F:
      // Serial.print(F("+ Key 9: "));
      // Serial.println("");
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
      runProgram = false;
      statusByte = statusByte | WAIT_ON;
      statusByte = statusByte & HLTA_OFF;
      displayStatusAddressData();
      break;
    case 0xFF38C7:
    case 0xE0E016E9:
      // Serial.println(F("+ Key OK - Toggle RUN and STOP."));
      Serial.println(F("+ Stop process."));
      runProgram = false;
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
  Serial.begin(115200);
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println(F("+++ Setup."));

  // ----------------------------------------------------
  int programSize = sizeof(theProgram);
  // List a program.
  listByteArray(theProgram, programSize);
  // Load a program.
  copyByteArrayToMemory(theProgram, programSize);
#ifdef RUN_NOW
  runProgram = true;
#endif
  Serial.print(F("+ Program loaded."));
  if (runProgram) {
    Serial.println(F(" It will start automatically."));
  }

  // ----------------------------------------------------
  irrecv.enableIRIn();
  Serial.println(F("+ infrared receiver ready for input."));

#ifdef INCLUDE_LCD
  readyLcd();
  Serial.println(F("+ LCD ready for output."));
#endif

  // ------------------------------
  // PCF8574 device initialization
  pcf20.begin();
  pcf21.begin();
  // PCF8574 device Interrupt initialization
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), pcf20interrupt, CHANGE);
  Serial.println("+ PCF8574 modules initialized.");

  // ----------------------------------------------------
  pinMode(latchPinLed, OUTPUT);
  pinMode(clockPinLed, OUTPUT);
  pinMode(dataPinLed, OUTPUT);
  delay(300);
  // Serial.println(F("+ Front panel LED shift registers ready."));
  //
  // Status lights are off by default.
  statusByte = statusByte | WAIT_ON;
  statusByte = statusByte | MEMR_ON;
  statusByte = statusByte | M1_ON;
  statusByte = statusByte | WO_ON;  // WO: on, Inverse logic: off when writing out. On when not.
  // lightsStatusAddressData(statusByte, programCounter, dataByte);
  int testLights = B10011001 * 256 + B01100110;
  lightsStatusAddressData(statusByte, testLights, dataByte);
  Serial.println(F("+ Front panel LED lights initialized."));
  // ----------------------------------------------------

  Serial.println(F("+++ Start the processor loop."));
}

// -----------------------------------------------------------------------------
// Device Loop for processing each byte of machine code.

#ifdef RUN_DELAY
static unsigned long timer = millis();
#endif
void loop() {

  if (runProgram) {
    // ----------------------------
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
    // ----------------------------
  } else {
    // Program control: RUN, SINGLE STEP, EXAMINE, EXAMINE NEXT, Examine previous, RESET.
    if (irrecv.decode(&results)) {
      infraredControl();
    }
    if (pcf20interrupted) {
      checkControlButtons();
      pcf20interrupted = false; // Reset for next interrupt.
    }
    delay(60);
  }

}
// -----------------------------------------------------------------------------
