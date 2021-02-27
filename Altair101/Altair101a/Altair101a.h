// -----------------------------------------------------------------------------
#include <Arduino.h>

// -----------------------------------------------------------------------------
// From cpucore_i8080.h
extern word status_wait;
extern word status_hlda;
extern word status_inte;
//
extern byte fpStatusByte;         // Status byte bits: MEMR INP M1 OUT HLTA STACK WO INT
extern uint16_t fpAddressWord;    // Status byte bits: MEMR INP M1 OUT HLTA STACK WO INT
// extern byte fpAddressLb;       // Front panel address lb.
// extern byte fpAddressHb;       // Front panel address hb.
extern byte fpDataByte;           // Front panel data byte.

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
void printFrontPanel();                     // Print the front panel display.
void printData(byte theByte);               // To echo data bytes.
void printByte(byte theByte);               // To echo bytes.
void Serial_print(String regAdata);         // Print a string to the Serial or Serial2.
byte altair_in(byte addr);                  // Called from cpu_in();
void altair_out(byte addr, byte val);       // Called from cpu_out();

// -----------------------------------------------------------------------------
// From AltairSample.cpp program.

void loadProgram();
void loadProgramList();

void loadBasic4kArray();
void loadKillTheBitArray();
void loadKillTheBit();
void loadMviRegisters();

// From Altair101a.ino program, for AltairSample.cpp.
void printByte(byte b);
void printOctal(byte b);
void printHex(byte b);

// From here, for the AltairSample.cpp program.
extern boolean SERIAL_FRONT_PANEL;
extern void setAddressData(uint16_t addressWord, byte dataByte);
extern void serialPrintFrontPanel();
extern String loadProgramName;
extern byte readByte;

// From here, for the Mp3Player.cpp program.
extern void ledFlashError();
extern void ledFlashSuccess();
extern byte hwStatus;

// -----------------------------------------------------------------------------
// eof
