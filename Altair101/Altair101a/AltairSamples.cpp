// -----------------------------------------------------------------------------
/*
  Load Altair 101a Sample programs into processor memory.

  Programs are loading as an arrary:
    MWRITE( cnt++, B00010110 & 0xff);  // ++ opcode:mvi:00010110:d:080h
  Or, programs are loading from a PROGMEM array.
    static const byte PROGMEM pong[] = { ... }
*/
// -----------------------------------------------------------------------------
#include "cpucore_i8080.h"

int cnt;

void loadProgramList() {
  Serial.println(F("+ Sample programs."));
  Serial.println(F("----------"));
  Serial.println(F("++ k, Kill the Bit version for serial VT100 input."));
  Serial.println(F("++ m, MVI testing to the setting of registers."));
  Serial.println(F("----------"));
  Serial.println(F("++ K, Kill the Bit front panel fast version."));
  Serial.println(F("----------"));
  Serial.println(F("+ x, Exit: don't load a program."));
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Processor WAIT mode: Load sample programs.

void loadProgram() {
#ifdef LOG_MESSAGES
  Serial.println(F("+ loadProgram()"));
#endif
  if (loadProgramName != "") {
    Serial.print(F("+ Program previously loaded: "));
    Serial.println(loadProgramName);
  }
  programState = PROGRAM_LOAD;
  loadProgramList();
  while (programState == PROGRAM_LOAD) {
    if (Serial.available() > 0) {
      readByte = Serial.read();    // Read and process an incoming byte.
      switch (readByte) {
        case 'K':
          loadProgramName = "Serial Kill the Bit array";
          Serial.println(F("+ B, load array: A version of Kill the Bit for serial I/O."));
          programState = PROGRAM_WAIT;
          if (SERIAL_FRONT_PANEL) {
            Serial.print(F("\033[J"));     // From cursor down, clear the screen, .
          }
          loadKillTheBitArray();
          break;
        case 'k':
          loadProgramName = "Serial Kill the Bit";
          Serial.println(F("+ k, load: A version of Kill the Bit for serial I/O."));
          programState = PROGRAM_WAIT;
          if (SERIAL_FRONT_PANEL) {
            Serial.print(F("\033[J"));     // From cursor down, clear the screen, .
          }
          loadKillTheBit();
          break;
        case 'm':
          loadProgramName = "Register set test";
          Serial.println(F("+ m, load: a program that uses MVI to set the registers with values from 1 to 7."));
          Serial.println(F("     Use option 'i' to list the register values."));
          programState = PROGRAM_WAIT;
          if (SERIAL_FRONT_PANEL) {
            Serial.print(F("\033[J"));     // From cursor down, clear the screen, .
          }
          loadMviRegisters();
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
          loadProgramList();
          Serial.print("?- ");
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
// Load sample programs.

// -----------------------------------------------------------------------------
void loadKillTheBit() {
  cnt = 0;
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
  MWRITE( cnt++, B00000100 & 0xff);  // ++ immediate:4:4
  MWRITE( cnt++, B10101010 & 0xff);  // ++ opcode:xra:10101010:d
  MWRITE( cnt++, B00001111 & 0xff);  // ++ opcode:rrc:00001111
  MWRITE( cnt++, B01010111 & 0xff);  // ++ opcode:mov:01010111:d:a
  MWRITE( cnt++, B11000011 & 0xff);  // ++ opcode:jmp:11000011:Begin
  MWRITE( cnt++, B00000101 & 0xff);  // ++ lb:Begin:5
  MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
}

// -----------------------------------------------------------------------------
void loadMviRegisters() {
  cnt = 0;
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
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//  From prog_games.cpp, program hex bytes:
/*
  Before
  Sketch uses 70236 bytes (27%) of program storage space. Maximum is 253952 bytes.
  Global variables use 7634 bytes (93%) of dynamic memory, leaving 558 bytes for local variables. Maximum is 8192 bytes.
  L  After
  Sketch uses 70290 bytes (27%) of program storage space. Maximum is 253952 bytes.
  Global variables use 7608 bytes (92%) of dynamic memory, leaving 584 bytes for local variables. Maximum is 8192 bytes.
*/

/*
    Need to update the speed.
*/
// Reference: https://forum.arduino.cc/?topic=723495#msg4868404
static const byte PROGMEM KillTheBitArray[] = {
  // const byte programBytes[] = {
  // Kill the Bit program.
  0x21, 0x00, 0x00, 0x16, 0x80, 0x01, 0x0E, 0x00, 0x1A, 0x1A, 0x1A, 0x1A, 0x09, 0xD2, 0x08, 0x00,
  0xDB, 0xFF, 0xAA, 0x0F, 0x57, 0xC3, 0x08, 0x00
};
void loadKillTheBitArray() {
  int theSize = sizeof(KillTheBitArray);
  // int theSize = sizeof(programBytes);      // If reading from a non-PROGMEM array.
  for (int i = 0; i < theSize; i++ ) {
    byte theValue = pgm_read_byte(&KillTheBitArray[i]);
    Serial.print(F("+ programBytes["));
    if (i < 10) {
      Serial.print(F("0"));
    }
    MWRITE(i, theValue);
    Serial.print(i);
    Serial.print(F("] = MWRITE-D:"));
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
    //
    Serial.print(F(" MREAD-D:"));
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

static const byte PROGMEM pong[] = {
0001, 0016, 0000, 0061, 0175, 0000, 0041, 0000, 0000, 0042, 0200, 0000, 0021, 0000, 0200, 0303,
0036, 0000, 0041, 0000, 0000, 0032, 0032, 0032, 0032, 0011, 0322, 0025, 0000, 0311, 0315, 0022,
0000, 0333, 0377, 0346, 0001, 0312, 0055, 0000, 0172, 0263, 0346, 0037, 0137, 0172, 0346, 0001,
0312, 0105, 0000, 0173, 0346, 0002, 0302, 0077, 0000, 0173, 0346, 0001, 0302, 0163, 0000, 0041,
0201, 0000, 0064, 0036, 0000, 0172, 0017, 0127, 0303, 0036, 0000, 0315, 0022, 0000, 0333, 0377,
0346, 0200, 0312, 0135, 0000, 0172, 0017, 0017, 0017, 0263, 0346, 0037, 0137, 0172, 0346, 0200,
0312, 0165, 0000, 0173, 0346, 0010, 0302, 0157, 0000, 0173, 0346, 0020, 0302, 0103, 0000, 0041,
0200, 0000, 0064, 0036, 0000, 0172, 0007, 0127, 0303, 0113, 0000};

// -----------------------------------------------------------------------------
// eof
