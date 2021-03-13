// -----------------------------------------------------------------------------
#include <Arduino.h>

#define READ_FILE 1
#define WRITE_FILE 2

// -----------------------------------------------------------------------------
// Altair101a.ino program functions used in cpucore_i8080.cpp
// Program states
#define PROGRAM_WAIT 1
#define SERIAL_DOWNLOAD 5
#define SDCARD_RUN 7

extern int programState;
// extern void ledFlashError();
// extern void ledFlashSuccess();
extern byte hwStatus;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// From cpuIntel8080.h

#define MEMSIZE 256
extern byte Mem[MEMSIZE];
#define MWRITE(a,v) { Mem[a]=v; }
#define MREAD(a)    (Mem[a])

// -----------------------------------------------------------------------------
// eof
