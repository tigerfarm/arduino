// -----------------------------------------------------------------------------
/*
  Altair 101 Processor program

  This is an Altair 8800 emulator program that is being developed on an Arduino Nano microprocessor.
  It emulates the basic Altair 8800 hardware which was built around the Intel 8080 CPU chip.
  This program includes a number of the Intel 8080 microprocessor machine instructions (opcodes).
  It has more than enough opcodes to run the classic programs, Kill the Bit and Pong.

  The Altair 101 is a hardware and software emulator of the basic Altair 8800 computer from 1975.
  The current development computer is functionally complete!

  The computer finally has the basic functionality.
  + It runs Kill the Bit, which is the standard de facto demonstration program of an Altair 8800 and its clones and replicas.
  + The only major difference, is that I don't have all the 8080 opcodes implemented.
  + The Altair 101 only has only 256 bytes of memory which is the same as the original basic Altair 8800.
  + Modern clones and replicas have 64K of memory.
  + Later, I can add memory and more opcodes to the 101.

  ---------------------------------------------
  Current work,
  + In the process of writing and testing the opcode CMP test program.

  As a quick test, re-connect the toggle keyboard to the Dev machine.
  + The toggle keyboard will replace the current breadboard control buttons.
  + Decide if I want to use the PCF8574 module for control switch input.

  ---------------------------------------------
  Build an new Dev machine.

  The new machine will be mounted on a $4 clipboard from Staples.
  This will allow me to use it like an Android tablet.

  Wire new breadboards:
  + 1 for shift registers.
  + 1 for LED lights: status, address, and data.
  + 1 for the Nano and clock module.
  + Mount the new breadboards onto the clipboard.
  + Cable to plug into a USB power supply that is plugged in a wall socket.
  + On/off toggle, or use a USB hub with on/off switches.
  + For portability, I should test using a 9V power supply.

  ---------------------------------------------
  Add modern I/O components and controls.

  Add SD card,
  + Save program memory to card, load program memory from card.
  ++ Use an on/off/on toggle: up to save (upload), down to load (download).

  Add a 1602 LCD,
  ++ Confirm messages on the LCD: "Confirm, save to file x." Or, "Confirm, load file x."
  +++ The file number, is the toggle value. For example, 003.MEM, is A8 and A9 toggles up.
  ++ View the result: "Saved.", "Loaded.", or "Error."

  Add DS3231 clock.
  + Add HLDA status light to signal when running in clock mode.
  + Use an on/off/on toggle to display the time on the LCD.
  + Time is shown on the LCD, when the LCD isn't used by a running program.
  + Program option to take over the LEDs to display the time.

  Add MP3 player (DFPlayer) and amp.
  ++ Controled using an infrared controller. At first, independent from programs running.

  ---------------------------------------------
  Build my first Altair 101 machine,

  + Complete the final design.
  + Order parts to build the machine.
  + Make enhancements to the case so that it's ready for the electronic parts.
  + Build a new breadboard computer to fit into the case.
  ++ Use an Audruino UNO.
  + Put it all together.

  ---------------------------------------------
  Program Development Phase

  + A basic assembler to convert assembly programs into machine code.
  + Implement the next major Altair 8800 sample program, Pong.
  + Update and enhance my set of test and development programs.
  ++ Samples: looping, branching, calling subroutines.
  ++ Continue adding opcodes with sample programs to confirm that the opcodes are working correctly.

  ---------------------------------------------
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
*/
// -----------------------------------------------------------------------------
// Code compilation options.

// #define INCLUDE_LCD 1
// #define INCLUDE_SDCARD 1
// #define RUN_DELAY 1
// #define RUN_NOW 1
// #define SWITCH_MESSAGES 1
// #define LOG_MESSAGES 1

// -----------------------------------------------------------------------------
// Infrared Receiver

#include <IRremote.h>

//        Nano pin
int IR_PIN = A1;

IRrecv irrecv(IR_PIN);
decode_results results;

// -----------------------------------------------------------------------------
// Input toggle shift register(SN74HC595N) pins

//        Nano pin               74HC595 Pins
const int dataPinIn = 4;      // pin 14 Data pin.
const int latchPinIn = 5;     // pin 12 Latch pin.
const int clockPinIn = 6;     // pin 11 Clock pin.

const int dataInputPin = A0;  // Data input pin to check switches. Digital pins and some analog pins, work.

// ------------------------------------------------------------------------
// Output LED light shift register(SN74HC595N) pins

//        Nano pin               74HC595 Pins
const int dataPinLed = 7;     // pin 14 Data pin.
const int latchPinLed = 8;    // pin 12 Latch pin.
const int clockPinLed = 9;    // pin 11 Clock pin.

// -----------------------------------------------------------------------------
// SD Card module is an SPI bus slave device.
#ifdef INCLUDE_SDCARD

#include <SPI.h>
#include <SD.h>

// Nano pin - SPI module pins
// Pin 10   - CS   : chip/slave select (SS pin). Can be any master(Nano) digital pin to enable/disable this device on the SPI bus.
// Pin 13   - SCK  : serial clock, SPI: accepts clock pulses which synchronize data transmission generated by Arduino.
// Pin 11   - MOSI : master out slave in, SPI: input to the Micro SD Card Module.
// Pin 12   - MISO : master in slave Out, SPI: output from the Micro SD Card Module.
// Pin 5V+  - VCC  : can use 3.3V or 5V
// Pin GND  - GND  : ground
// Note, Nano pins are declared in the SPI library for SCK, MOSI, and MISO.

// The CS pin is the only one that is not really fixed as any of the Arduino digital pin.
const int csPin = 10;  // SD Card module is connected to Nano pin 10.

File myFile;

#endif

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
byte theProgram[] = {
  //                //            ; --------------------------------------
  //                //            ; Test CMP and conditional jumps.
  //                              ; Compare a register to A, and then set Carry and Zero bit flags.
  //                              ; If #=A, set Zero bit to 1, Carry bit to 0. If #>A, Carry bit = 1. If #<A, Carry bit = 0.
  //                              ; Note, register A remain the same after the compare.
  //
  B11000011, 6, 0,  // jmp Test   ; Jump to bypass the halt.
  //
  //                // Error:
  B11100011, 39,    // out 39     ; Print the registers other system values.
  //
  //                // Halt:
  B01110110,        // hlt        ; The program will halt at each iteration, after the first.
  //
  //                //            ; --------------------------------------
  //                // Test:
  B00111110, 73,    // mvi a,73   ; Move # to register A.
  //
  //                //            ; --------------------------------------
  //0RRR110
  B00000110, 73,    // mvi b,73   ; Move # to register B.
  //0111SSS
  B10111000,        // cmp b      ; B = A. Zero bit flag is true. Carry bit is false.
  B11000010, 3, 0,  // jnz Error  ; Zero bit flag is set, don't jump.
  B11011010, 3, 0,  // jc Error   ; Carry bit flag is not set, don't jump.
  B11001010, 23, 0, // jz okay1a  ; Zero bit flag is set, jump.
  B11000011, 3, 0,  // jmp Error  ; The above should have jumped passed this.
  //                // okay1a:
  B11010010, 29, 0, // jnc okay1b ; Carry bit flag is not set, jump to the end of this test.
  B11000011, 3, 0,  // jmp Error  ; The above should have jumped passed this.
  //                // okay1b:
  //
  //                //            ; --------------------------------------
  0,                // NOP
  B11000011, 5, 0,  // jmp Halt   ; Jump back to the early halt command.
  0000              //            ; End.
};

// -----------------------------------------------------------------------------
// Kill the Bit program.

byte theProgramKtb[] = {
  // ------------------------------------------------------------------
  // Kill the Bit program.
  // Before starting, make sure all the sense switches are in the down position.
  //
  //                    //            ; Start program.
  0,                    // org 0      ; An assembler directive, I guess.
  0041, 0, 0,           // LXI H,0    ; Move the lb hb data values into the register pair H(hb):L(lb). Initialize counter
  0026, 128,            // mvi D,80h  ; Move db to register D. Set initial display bit.  080h = 128 = regD = 10 000 000
  0001, 0, 5,           // LXI B,?  0  ; Load a(lb:hb) into register B:C. Higher value = faster.
  //            ;    Default: 0014 = B:C  = 00 010 000
  //            ;    Slow:    0020 = B:C  = 00 010 000
  //            ;    Nice:    0040 = B:C  = 00 100 000
  //            ;    Fast:    0100 = B:C  = 01 000 000
  //            ;Too fast:    0100 = B:C  = 01 001 000
  //
  //  ; Display bit pattern on upper 8 address lights.
  //                    // BEG:
  // 0343, 38,
  // 0166,                 // HLT
  0032,                 // ldax d     ; Move data from address D:E, to register A.
  0032,                 // ldax d     ; Move data from address D:E, to register A.
  0032,                 // ldax d     ; Move data from address D:E, to register A.
  0032,                 // ldax d     ; Move data from address D:E, to register A.
  //
  0011,                 // DAD B      ; Add B:C to H:L. Set carry bit. Increments the display counter
  0322, 9, 0,           // JNC BEG    ; If carry bit false, jump to BEG, LDAX instruction start.
  //
  // 0343, 32,             // out d      ; Show register D, which contains the value that is matched to input toggles.
  0333, 0377,           // IN 0ffh    ; Input into A. Check for toggled input, at port 377 (toggle sense switches), that can kill the bit.
  0252,                 // XRA D      ; Exclusive OR register with A
  //
  0017,                 // RRC        ; Rotate A right (shift byte right 1 bit). Set carry bit. Rotate display right one bit
  0127,                 // MOV D,A    ; Move register A to register D. Move data to display reg
  //
  0303, 9, 0,           // JMP BEG    ; Jump to BEG, halt before LDAX instructions restart.
  // ------------------------------------------------------------------
  0000, 0000, 0000      //             ; end
};

// -----------------------------------------------------------------------------
// Memory definitions

const int memoryBytes = 256;
byte memoryData[memoryBytes];
unsigned int programCounter = 0;     // Program address value

const int stackBytes = 32;
int stackData[memoryBytes];
unsigned int stackPointer = stackBytes;

// -----------------------------------------------------------------------------
// Memory Functions

char charBuffer[17];
byte zeroByte = B00000000;

void initMemoryToZero() {
  Serial.println(F("+ Initialize all memory bytes to zero."));
  for (int i = 0; i < memoryBytes; i++) {
    memoryData[i] = zeroByte;
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
int displayColumns = 16;
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
#else
  /*
    Serial.print(" > ");
    printByte(highByte(programCounter));
    Serial.print(":");
    printByte(lowByte(programCounter));
    Serial.print(":");
    printByte(lowByte(regA));
  */
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
      statusByte = statusByte | WAIT_ON;
      statusByte = statusByte & MEMR_OFF;
      statusByte = statusByte & M1_OFF;
      statusByte = statusByte | HLTA_ON;
      // lightsStatusAddressData(statusByte, programCounter, dataByte);
      // displayStatusAddressData();
#else
      Serial.println("");
#endif
      Serial.print(F("+ Process halted."));
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
    // stacy
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
        lowOrder = programCounter;
#ifdef LOG_MESSAGES
        Serial.print(F("> jmp, lb: "));
        Serial.print(lowOrder);
#endif
        programCounter++;
        return;
      }
      // instructionCycle == 2
      programCounter = word(memoryData[programCounter], memoryData[lowOrder]);
#ifdef LOG_MESSAGES
      Serial.print(F("> jmp, jump to:"));
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
      Serial.println("");
#endif
      switch (dataByte) {
        case 1:
          Serial.print(F(", terminal output to be implemented on LCD."));
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

byte fileMemoryData[memoryBytes];

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
    fileMemoryData[i] = myFile.read();
    // Print Binary:Octal:Decimal values.
    Serial.print("B");
    printByte(fileMemoryData[i]);
    Serial.print(":");
    printOctal (fileMemoryData[i]);
    Serial.print(":");
    Serial.println(fileMemoryData[i], DEC);
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
// Get Front Panel Toggles value

const int numberOfToogles = 8;
int toggleSenseByte() {
  byte toggleByte = B00000000;
  byte toggleAddressBit = B10000000;
  for (int i = 0; i < numberOfToogles; i++) {
    digitalWrite(latchPinIn, LOW);
    shiftOut(dataPinIn, clockPinIn, LSBFIRST, toggleAddressBit);
    shiftOut(dataPinIn, clockPinIn, LSBFIRST, 0);
    shiftOut(dataPinIn, clockPinIn, LSBFIRST, 0);
    digitalWrite(latchPinIn, HIGH);
    if (digitalRead(dataInputPin) == HIGH) {
      toggleByte = toggleByte | toggleAddressBit;
    }
    toggleAddressBit = toggleAddressBit >> 1;
  }
  return toggleByte;
}

// -------------------------
// Front Panel Control Switches, when a program is not running.

// Only do the action once, don't repeat if the button is held down.
// Don't repeat action if the button is not pressed.

const int numberOfSwitches = 7;
boolean switchState[numberOfSwitches] = {
  false, false, false, false, false, false, false
};
void checkControlButtons() {
  // Start with the run button.
  byte dataByte = B01000000;
  for (int i = 0; i < numberOfSwitches; i++) {
    digitalWrite(latchPinIn, LOW);
    shiftOut(dataPinIn, clockPinIn, LSBFIRST, 0);
    shiftOut(dataPinIn, clockPinIn, LSBFIRST, 0);
    shiftOut(dataPinIn, clockPinIn, LSBFIRST, dataByte);
    digitalWrite(latchPinIn, HIGH);
    //
    if (digitalRead(dataInputPin) == HIGH) {
      if (!switchState[i]) {
        switchState[i] = true;
        // Serial.print("+ Button pressed: ");
        // Serial.println(i);
      }
    } else if (switchState[i]) {
      switchState[i] = false;
      //
      if (i == 0) {
#ifdef SWITCH_MESSAGES
        Serial.println(F("+ Run process."));
#endif
        runProgram = true;
        statusByte = statusByte & WAIT_OFF;
        statusByte = statusByte & HLTA_OFF;
      } else if (i == 1) {
        // Single Step
        statusByte = statusByte & HLTA_OFF;
        processData();
      } else if (i == 2) {
#ifdef SWITCH_MESSAGES
        Serial.println(F("+ Examine toggle address data. Address bits: A0...A7."));
#endif
        programCounter = toggleSenseByte();
        dataByte = memoryData[programCounter];
        lightsStatusAddressData(statusByte, programCounter, dataByte);
      } else if (i == 3) {
#ifdef SWITCH_MESSAGES
        Serial.println(F("+ Examine Next address."));
#endif
        programCounter++;
        dataByte = memoryData[programCounter];
        lightsStatusAddressData(statusByte, programCounter, dataByte);
      } else if (i == 4) {
        dataByte = toggleSenseByte();
        memoryData[programCounter] = dataByte;
        lightsStatusAddressData(statusByte, programCounter, dataByte);
#ifdef SWITCH_MESSAGES
        Serial.print(F("+ Deposited toggle address byte: "));
        printByte(dataByte);
        Serial.println("");
#endif
      } else if (i == 5) {
#ifdef SWITCH_MESSAGES
        Serial.println(F("+ Deposit toggle byte into the next address."));
#endif
        dataByte = toggleSenseByte();
        programCounter++;
        memoryData[programCounter] = dataByte;
        lightsStatusAddressData(statusByte, programCounter, dataByte);
      } else if (i == 6) {
#ifdef SWITCH_MESSAGES
        Serial.println("+ Running RESET Button pressed.");
        Serial.println(F("+ Reset program counter, program address, to 0."));
#endif
        programCounter = 0;
        stackPointer = 0;
        dataByte = memoryData[programCounter];
        lightsStatusAddressData(statusByte, programCounter, dataByte);
      } else {
        // Serial.print("+ Button released: ");
        // Serial.println(i);
      }
    }
    //
    dataByte = dataByte >> 1;
  }
}

// -------------------------
// Front Panel Control Switches, when a program is running.

byte switchByte;
boolean switchStop = false;
boolean switchReset = false;
void checkRunningButtons() {
  // Check STOP button.
  switchByte = B10000000;
  digitalWrite(latchPinIn, LOW);
  shiftOut(dataPinIn, clockPinIn, LSBFIRST, 0);
  shiftOut(dataPinIn, clockPinIn, LSBFIRST, 0);
  shiftOut(dataPinIn, clockPinIn, LSBFIRST, switchByte);
  digitalWrite(latchPinIn, HIGH);
  if (digitalRead(dataInputPin) == HIGH) {
    if (!switchStop) {
      switchStop = true;
#ifdef SWITCH_MESSAGES
      Serial.println("+ Running, STOP Button pressed.");
#endif
    }
  } else if (switchStop) {
    switchStop = false;
#ifdef SWITCH_MESSAGES
    Serial.println("+ Running, STOP Button released.");
    Serial.println(F("> hlt, halt the processor."));
#endif
    runProgram = false;
    statusByte = 0;
    statusByte = statusByte | WAIT_ON;
    statusByte = statusByte | HLTA_ON;
    displayStatusAddressData();
  }
  // Check RESET button.
  switchByte = B00000001;
  digitalWrite(latchPinIn, LOW);
  shiftOut(dataPinIn, clockPinIn, LSBFIRST, 0);
  shiftOut(dataPinIn, clockPinIn, LSBFIRST, 0);
  shiftOut(dataPinIn, clockPinIn, LSBFIRST, switchByte);
  digitalWrite(latchPinIn, HIGH);
  if (digitalRead(dataInputPin) == HIGH) {
    if (!switchReset) {
      switchReset = true;
#ifdef SWITCH_MESSAGES
      Serial.println("+ Running, RESET Button pressed.");
#endif
    }
  } else if (switchReset) {
    switchReset = false;
#ifdef SWITCH_MESSAGES
    Serial.println("+ Running, RESET Button released.");
    Serial.println(F("+ Reset program counter, program address, to 0."));
#endif
    programCounter = 0;
    stackPointer = 0;
    dataByte = memoryData[programCounter];
    lightsStatusAddressData(statusByte, programCounter, dataByte);
  }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------
// Infrared options when a program is NOT running.

void infraredSwitchControl() {
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

void infraredSwitchInput() {
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

  // ----------------------------------------------------
  irrecv.enableIRIn();
  Serial.println(F("+ infrared receiver ready for input."));

#ifdef INCLUDE_LCD
  readyLcd();
  Serial.println(F("+ LCD ready for output."));
#endif

  // ----------------------------------------------------
  pinMode(latchPinIn, OUTPUT);
  pinMode(clockPinIn, OUTPUT);
  pinMode(dataPinIn, OUTPUT);
  pinMode(dataInputPin, INPUT);
  delay(300);
  Serial.println("+ Front panel switches ready to be used.");

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
      infraredSwitchInput();
    }
    checkRunningButtons();
    // ----------------------------
  } else {
    // Program control: RUN, SINGLE STEP, EXAMINE, EXAMINE NEXT, Examine previous, RESET.
    if (irrecv.decode(&results)) {
      infraredSwitchControl();
    }
    checkControlButtons();
    delay(60);
  }

}
// -----------------------------------------------------------------------------
