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
#include "Arduino.h"
// -----------------------------------------------------------------------------
#define SWITCH_MESSAGES 1

bool runProgram = true;
byte statusByte = 0;
void processData() {
  Serial.println("++ processData();");
}

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
// Front Panel Control Switches, when a program is not running.

const int pinStop = 0;
const int pinRun = 1;
const int pinStep = 2;
const int pinExamine = 3;
const int pinExamineNext = 4;
const int pinDeposit = 5;
const int pinDepositNext = 6;
const int pinReset = 7;

// Only do the action once, don't repeat if the button is held down.
// Don't repeat action if the button is not pressed.

const int numberOfSwitches = 7;
boolean switchState[numberOfSwitches] = {
  false, false, false, false, false, false, false
};
void checkControlButtons() {
  // Start with the run button.
  for (int i = 1; i < numberOfSwitches; i++) {
    if (pcf8574.digitalRead(i) == LOW) {
      if (!switchState[i]) {
        switchState[i] = true;
        // Serial.print("+ Button pressed: ");
        // Serial.println(i);
      }
    } else if (switchState[i]) {
      switchState[i] = false;
      //
      // -------------------
      if (i == pinRun) {
        // -------------------
#ifdef SWITCH_MESSAGES
        Serial.println(F("+ Run process."));
#endif
        runProgram = true;
      } else if (i == pinStep) {
        // -------------------
#ifdef SWITCH_MESSAGES
        Serial.println(F("+ Single Step."));
#endif
      } else if (i == pinExamine) {
        // -------------------
#ifdef SWITCH_MESSAGES
        Serial.println(F("+ Examine."));
#endif
      } else if (i == pinExamineNext) {
        // -------------------
#ifdef SWITCH_MESSAGES
        Serial.println(F("+ Examine Next address."));
#endif
      } else if (i == pinDeposit) {
        // -------------------
#ifdef SWITCH_MESSAGES
        Serial.print(F("+ Deposit."));
        Serial.println("");
#endif
      } else if (i == pinDepositNext) {
        // -------------------
#ifdef SWITCH_MESSAGES
        Serial.println(F("+ Deposit next."));
#endif
      } else if (i == pinReset) {
        // -------------------
#ifdef SWITCH_MESSAGES
        Serial.println("+ RESET.");
#endif
      } else {
        // Serial.print("+ Button released: ");
        // Serial.println(i);
      }
    }
    //
    // dataByte = dataByte >> 1;
  }
}

// -------------------------
// Front Panel Control Switches, when a program is running.

byte switchByte;
boolean switchStop = false;
boolean switchReset = false;
void checkRunningButtons() {
  // -------------------
  // Check STOP button.
  if (pcf8574.digitalRead(pinStop) == LOW) {
    if (!switchStop) {
      switchStop = true;
#ifdef SWITCH_MESSAGES
      // Serial.println("+ Running, STOP Button pressed.");
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
  // -------------------
  // Check RESET button.
  if (pcf8574.digitalRead(pinReset) == LOW) {
    if (!switchReset) {
      switchReset = true;
#ifdef SWITCH_MESSAGES
      // Serial.println("+ Running, RESET Button pressed.");
#endif
    }
  } else if (switchReset) {
    switchReset = false;
#ifdef SWITCH_MESSAGES
    Serial.println("+ Running, RESET Button released.");
#endif
    // ...
  }
  // -------------------
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
    if (keyPress) {
      // Serial.println("+ runProgram, keyPress is true.");
      delay (30);
      checkRunningButtons();
      keyPress = false;
    }
    delay (30);
    // ----------------------------
  } else {
    if (keyPress) {
      delay (30);
      checkControlButtons();
      keyPress = false;
    }
    delay(30);
  }
}
// -----------------------------------------------------------------------------
