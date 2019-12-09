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

bool runProgram = false;

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
// Front Panel Control Switches

const int pinStop = 0;
const int pinRun = 1;
const int pinStep = 2;
const int pinExamine = 3;
const int pinExamineNext = 4;
const int pinDeposit = 5;
const int pinDepositNext = 6;
const int pinReset = 7;

int switchValue = 1;

boolean switchStop = false;
boolean switchRun = false;
boolean switchStep = false;
boolean switchExamine = false;
boolean switchExamineNext = false;
boolean switchDeposit = false;;
boolean switchDepositNext = false;;
boolean switchReset = false;

// ----------------------------------------------
// When a program is not running.

void controlSwitches() {
  // -------------------
  // Check Run switch.
  switchValue = pcf8574.digitalRead(pinRun);
  if (switchValue == 0) {
    if (!switchRun) {
      switchRun = true;
    }
  } else if (switchRun) {
    switchRun = false;
#ifdef SWITCH_MESSAGES
    Serial.println("+ Control, Run.");
#endif
    // ...
    runProgram = true;
  }
  // -------------------
  // Check Step switch.
  if (pcf8574.digitalRead(pinStep) == LOW) {
    if (!switchStep) {
      switchStep = true;
    }
  } else if (switchStep) {
    switchStep = false;
#ifdef SWITCH_MESSAGES
    Serial.println("+ Control, Step.");
#endif
    // ...
  }
  // -------------------
  // Check Examine switch.
  if (pcf8574.digitalRead(pinExamine) == LOW) {
    if (!switchExamine) {
      switchExamine = true;
    }
  } else if (switchExamine) {
    switchExamine = false;
#ifdef SWITCH_MESSAGES
    Serial.println("+ Control, Examine.");
#endif
    // ...
  }
  // -------------------
  // Check ExamineNext switch.
  if (pcf8574.digitalRead(pinExamineNext) == LOW) {
    if (!switchExamineNext) {
      switchExamineNext = true;
    }
  } else if (switchExamineNext) {
    switchExamineNext = false;
#ifdef SWITCH_MESSAGES
    Serial.println("+ Control, ExamineNext.");
#endif
    // ...
  }
  // -------------------
  // Check Deposit switch.
  if (pcf8574.digitalRead(pinDeposit) == LOW) {
    if (!switchDeposit) {
      switchReset = true;
    }
  } else if (switchDeposit) {
    switchDeposit = false;
#ifdef SWITCH_MESSAGES
    Serial.println("+ Control, Deposit.");
#endif
    // ...
  }
  // -------------------
  // Check DepositNext switch.
  if (pcf8574.digitalRead(pinDepositNext) == LOW) {
    if (!switchDepositNext) {
      switchReset = true;
    }
  } else if (switchDepositNext) {
    switchDepositNext = false;
#ifdef SWITCH_MESSAGES
    Serial.println("+ Control, DepositNext.");
#endif
    // ...
  }
  // -------------------
  // Check RESET switch.
  if (pcf8574.digitalRead(pinReset) == LOW) {
    if (!switchReset) {
      switchReset = true;
    }
  } else if (switchReset) {
    switchReset = false;
#ifdef SWITCH_MESSAGES
    Serial.println("+ Control, RESET.");
#endif
    // ...
  }
  // -------------------
}

// ----------------------------------------------
// When a program is running.

void runningSwitches(int thePin) {
  switch (thePin) {
    case pinStop:
      // -------------------
      // Set STOP switch.
      if (switchValue == 0) {
        if (!switchStop) {
          switchStop = true;
        }
      } else if (switchStop) {
        switchStop = false;
#ifdef SWITCH_MESSAGES
        Serial.println(F("+ Running, STOP > hlt, halt the processor."));
#endif
        runProgram = false;
        // ...
      }
      break;
    case pinReset:
      // -------------------
      // Check RESET switch.
      if (pcf8574.digitalRead(pinReset) == LOW) {
        if (!switchReset) {
          switchReset = true;
        }
      } else if (switchReset) {
        switchReset = false;
#ifdef SWITCH_MESSAGES
        Serial.println("+ Running, RESET.");
#endif
        // ...
      }
      // -------------------
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
  // I2C based switch initialization

  // Can I use attachInterrupt?
  // I need change or PULLDOWN?
  pinMode(INTERRUPT_PIN, INPUT_PULLUP); // Enable pullup on interrupt pin of Uno
  // attachInterrupt (digitalPinToInterrupt(PinCLK), rotarydetect, CHANGE);

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
      // Serial.println("+ runProgram = true, keyPress is true.");
      // runningSwitches();
      for (int i = 0; i < 8; i++) {
        int val = pcf8574.digitalRead(i);  // Read each PCF8574 input
        if (val == 0) {                    // If LOW, button was pushed
          Serial.print("+ Button pressed, pin: ");
          Serial.println(i);
          runningSwitches(i);
        }
      }
      delay(30);           // Handle switch debounce.
      keyPress = false;
    }
    delay (30);
    // ----------------------------
  } else {
    if (keyPress) {
      // Serial.println("+ runProgram = false, keyPress is true.");
      // controlSwitches();
      for (int i = 0; i < 8; i++) {
        int val = pcf8574.digitalRead(i);  // Read each PCF8574 input
        if (val == 0) {                    // If LOW, button was pushed
          Serial.print("+ Button pressed, pin: ");
          Serial.println(i);
        }
      }
      delay(30);           // Handle switch debounce.
      keyPress = false;
    }
    delay(30);
  }

}
// -----------------------------------------------------------------------------
