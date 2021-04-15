// -----------------------------------------------------------------------------
#include <Arduino.h>

// -----------------------------------------------------------------------------
// Altair101a.ino program functions used in cpucore_i8080.cpp
// Program states
#define PROGRAM_WAIT 1
#define CLOCK_RUN 3
//
extern const byte MEMR_ON;
extern const byte INP_ON;
extern const byte M1_ON;
extern const byte OUT_ON;
extern const byte HLTA_ON;
extern const byte STACK_ON;
extern const byte WO_ON;
extern const byte INT_ON1;

extern int programState;
extern void clockLights(byte theMinute, byte theHour);
extern boolean VIRTUAL_FRONT_PANEL;
extern void initVirtualFrontPanel();
//
extern void lightsStatusAddressData( byte status8bits, unsigned int address16bits, byte data8bits);
//
extern int KNIGHT_RIDER_SCANNER;
extern void playerSoundEffect(byte theFileNumber);
extern void playerPlaySoundWait(int theFileNumber);
extern void mp3playerPlay(int theCounter);
extern void playerContinuous();
extern uint8_t playerStatus;
extern uint16_t playerCounter;
extern int TIMER_COMPLETE;
extern int TIMER_MINUTE;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Infrared pin definitions that can be used in multiple programs.

// -----------------------------------------------
#if defined(__AVR_ATmega2560__)
// ------------------
#include <IRremote.h>
// ------------------
// Mega uses a hardware serial port (RX/TX) for communications with the DFPlayer.
// For Arduino Mega, I use pin 24 because it's closer to where I'm doing my wiring.
//  Feel free to use another digital or analog pin.
#define IR_PIN 24
//
// -----------------------------------------------
#elif defined(__SAM3X8E__)
// ------------------
#include <IRremote2.h>    // Special infrared library for the Due.
// For Arduino Due, I use pin 24 because it's closer to where I'm doing my wiring.
//  Feel free to use another digital or analog pin.
#define IR_PIN 24
// ------------------
// Due uses a hardware serial port (RX/TX) for communications with the DFPlayer.
//
// -----------------------------------------------
#else
// ------------------
#include <IRremote.h>
// Digital and analog pins work. Also tested with other analog pins.
#define IR_PIN A1
#endif

// -----------------------------------------------------------------------------
// eof
