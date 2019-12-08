// -----------------------------------------------------------------------------
/*
  PCF8574 I2C Module

  I2C to 8-bit Parallel-Port Expander

  Module adjustable pin address settings:
  A0 A1 A2
   0  0  0 = 0x20
   0  0  1 = 0x21
   0  1  0 = 0x22
    ...
   1  1  1 = 0x27

  Wiring:
  + SDA to Nano A4.
  + SCL to Nano A5.
  + GND to Nano GND
  + VCC to Nano 5V
  + INT to Nano interrupt pin, pin 2 in this sample program.
  + P0 ... O7 to switches. Other side of the switch to ground.

  Library:
    https://github.com/xreef/PCF8574_library
  Information and code sample:
    https://protosupplies.com/product/pcf8574-i2c-i-o-expansion-module/
*/
// -----------------------------------------------------------------------------
#define SWITCH_MESSAGES 1

bool runProgram = true;

// -----------------------------------------------------------------------------
#include "Arduino.h"
#include "PCF8574.h"

// Button press flag.
boolean keyPress = false;
//
void PCF8574_Interrupt();
//
// Interrupt setup: I2C address, interrupt pin to use, interrupt handler routine.
int PCF_INTERRUPT_ADDRESS = 0x020;
const int INTERRUPT_PIN = 2;
void pcf01interrupt() {
  keyPress = true;
}
PCF8574 pcf8574(PCF_INTERRUPT_ADDRESS, INTERRUPT_PIN, pcf01interrupt);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -------------------------
// Front Panel Control Switches, when a program is running.

byte switchByte;
boolean switchStop = false;
boolean switchReset = false;
void checkRunningButtons() {
  // Check STOP button.
  if (pcf8574.digitalRead(0) == LOW) {
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
    // ...
  }
  // Check RESET button.
  if (pcf8574.digitalRead(1) == LOW) {
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
    // ...
  }
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  // ------------------------------
  // I2C Two Wire initialization

  pinMode(INTERRUPT_PIN, INPUT_PULLUP); // Enable pullup on interrupt pin of Uno

  pcf8574.pinMode(P0, INPUT);           // Set all pins as inputs on PCF8574
  pcf8574.pinMode(P1, INPUT);
  pcf8574.pinMode(P2, INPUT);
  pcf8574.pinMode(P3, INPUT);
  pcf8574.pinMode(P4, INPUT);
  pcf8574.pinMode(P5, INPUT);
  pcf8574.pinMode(P6, INPUT);
  pcf8574.pinMode(P7, INPUT);

  pcf8574.digitalWrite(P0, HIGH);     // Enable weak internal pullups of the PCF8574
  pcf8574.digitalWrite(P1, HIGH);     // To pull the inputs HIGH
  pcf8574.digitalWrite(P2, HIGH);
  pcf8574.digitalWrite(P3, HIGH);
  pcf8574.digitalWrite(P4, HIGH);
  pcf8574.digitalWrite(P5, HIGH);
  pcf8574.digitalWrite(P6, HIGH);
  pcf8574.digitalWrite(P7, HIGH);

  pcf8574.begin();

  Serial.println("+ PCF module initialized.");

  if (runProgram) {
    Serial.println("+ runProgram is true.");
  } else {
    Serial.println("+ runProgram is false.");
  }
  // ------------------------------
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

void loop() {

  if (runProgram) {
    // processData();
    if (keyPress) {
      // Serial.println("+ runProgram, keyPress is true.");
      checkRunningButtons();
      keyPress = false;
    }
    delay (60);
    // ----------------------------
  } else {
    if (keyPress) {
      // checkControlButtons();
      keyPress = false;
    }
    delay(60);
  }
}
// -----------------------------------------------------------------------------
