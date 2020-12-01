// -----------------------------------------------------------------------------
// Altair 8800 Simulator
// Copyright (C) 2017 David Hansel
// -----------------------------------------------------------------------------

#include <Arduino.h>

// -----------------------------------------------------------------------------
// From cpucore_i8080.h
extern word status_wait;
extern word status_hlda;
extern word status_inte;
//
extern byte fpStatusByte;     // Status byte bits: MEMR INP M1 OUT HLTA STACK WO INT
extern uint16_t fpAddressWord;  // Status byte bits: MEMR INP M1 OUT HLTA STACK WO INT
extern byte fpAddressLb;      // Front panel address lb.
extern byte fpAddressHb;      // Front panel address hb.
extern byte fpDataByte;       // Front panel data byte.

// -----------------------------------------------------------------------------
// Altair101a.ino program functions used in cpucore_i8080.cpp
// Program states
#define LIGHTS_OFF 0
#define PROGRAM_WAIT 1
#define PROGRAM_RUN 2
#define PROGRAM_LOAD 6
#define CLOCK_RUN 3
#define PLAYER_RUN 4
#define SERIAL_DOWNLOAD 5
extern int programState;

void singleStepWait();                      // Wait for "s" when single stepping.
void printFrontPanel();
void printData(byte theByte);               // To echo data bytes.
void printByte(byte theByte);               // To echo bytes.
byte altair_in(byte addr);
void altair_out(byte addr, byte val);

// Moved from Altair101a.ino, to cpucore_i8080.cpp
// void altair_hlt();
// void altair_interrupt_enable();
// void altair_interrupt_disable();

// -----------------------------------------------------------------------------
