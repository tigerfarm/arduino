// -----------------------------------------------------------------------------
#include <Arduino.h>

// -----------------------------------------------------------------------------
// Altair101a.ino program functions used in cpucore_i8080.cpp
// Program states
#define PROGRAM_WAIT 1
#define PROGRAM_RUN 2
#define CLOCK_RUN 3
#define PLAYER_RUN 4

extern int programState;
extern void ledFlashError();
extern void ledFlashSuccess();
extern byte hwStatus;

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Infrared definitions that can be used in multiple programs.
// Motherboard Specific setup for Infrared and DFPlayer communications

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
