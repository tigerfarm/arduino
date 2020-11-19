// -----------------------------------------------------------------------------
// Altair 8800 Simulator
// Copyright (C) 2017 David Hansel
// -----------------------------------------------------------------------------

#include <Arduino.h>

extern word status_wait;
extern word status_inte;

void printData(byte theByte);
byte altair_in(byte addr);
void altair_out(byte addr, byte val);
void altair_hlt();
bool altair_isreset();
void altair_wait_step();
void altair_set_outputs(uint16_t a, byte v);
bool altair_read_intel_hex(uint16_t *start = NULL, uint16_t *end = NULL);
void altair_vi_register_ports();

// -----------------------------------------------------------------------------
