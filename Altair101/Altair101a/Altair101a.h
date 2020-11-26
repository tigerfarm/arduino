// -----------------------------------------------------------------------------
// Altair 8800 Simulator
// Copyright (C) 2017 David Hansel
// -----------------------------------------------------------------------------

#include <Arduino.h>

// From cpucore_i8080.h
extern word status_wait;
extern word status_hlda;
extern word status_inte;
//
extern byte statusByteB;    // Status bits: MEMR INP M1 OUT HLTA STACK WO INT
extern byte statusByteA;
extern byte statusByteC;
extern byte statusByteG;
extern byte statusByteL;

// Used in cpucore_i8080.cpp
void singleStepWait();                      // Wait for "s" when single stepping.
void printData(byte theByte);               // To echo data bytes.
void altair_hlt();
void altair_set_outputs(uint16_t a, byte v);
byte altair_in(byte addr);
void altair_out(byte addr, byte val);
void altair_interrupt_enable();
void altair_interrupt_disable();

// -----------------------------------------------------------------------------
