// -----------------------------------------------------------------------------
/*
  Load Altair 101a Sample programs in processor memory.

*/
// -----------------------------------------------------------------------------
#include "cpucore_i8080.h"

int cnt;

// -----------------------------------------------------------------------------
//  From prog_games.cpp, program hex bytes:
/*
  Before added the array.
Sketch uses 70290 bytes (27%) of program storage space. Maximum is 253952 bytes.
Global variables use 7608 bytes (92%) of dynamic memory, leaving 584 bytes for local variables. Maximum is 8192 bytes.
  After
Sketch uses 70290 bytes (27%) of program storage space. Maximum is 253952 bytes.
Global variables use 7608 bytes (92%) of dynamic memory, leaving 584 bytes for local variables. Maximum is 8192 bytes.
 */

static const byte PROGMEM killbits[] = {
  0x21, 0x00, 0x00, 0x16, 0x80, 0x01, 0x0E, 0x00, 0x1A, 0x1A, 0x1A, 0x1A, 0x09, 0xD2, 0x08, 0x00,
  0xDB, 0xFF, 0xAA, 0x0F, 0x57, 0xC3, 0x08, 0x00
};

void loadKillTheBitArray() {
  cnt = 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Load sample programs.

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
  MWRITE( cnt++, B00000001 & 0xff);  // ++ immediate:1:1
  MWRITE( cnt++, B10101010 & 0xff);  // ++ opcode:xra:10101010:d
  MWRITE( cnt++, B00001111 & 0xff);  // ++ opcode:rrc:00001111
  MWRITE( cnt++, B01010111 & 0xff);  // ++ opcode:mov:01010111:d:a
  MWRITE( cnt++, B11000011 & 0xff);  // ++ opcode:jmp:11000011:Begin
  MWRITE( cnt++, B00000101 & 0xff);  // ++ lb:Begin:5
  MWRITE( cnt++, B00000000 & 0xff);  // ++ hb:0
}

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
