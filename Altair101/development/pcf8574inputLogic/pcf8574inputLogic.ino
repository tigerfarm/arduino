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
    https://github.com/RobTillaart/Arduino/tree/master/libraries/PCF8574
  Sample switch/button program:
    https://github.com/RobTillaart/Arduino/blob/master/libraries/PCF8574/examples/buttonRead/buttonRead.ino
  Reference:
    https://forum.arduino.cc/index.php?topic=204596.msg1506639#msg1506639

  Example statements:
    uint8_t value = PCF_38.read8();
    Serial.println(pcf20.read8(), BIN);
*/
// -----------------------------------------------------------------------------
#define SWITCH_MESSAGES 1

bool runProgram = false;

// -----------------------------------------------------------------------------
#include "PCF8574.h"
#include <Wire.h>

const int INTERRUPT_PIN = 2;

int PCF_INTERRUPT_ADDRESS = 0x020;
PCF8574 pcf20(PCF_INTERRUPT_ADDRESS);                                                        

// Set switch flag for on/off.
boolean switchSetOn = false;
// Interrupt setup: I2C address, interrupt pin to use, interrupt handler routine.
void pcf01interrupt() {
  switchSetOn = true;
}

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

// void controlSwitches(int pinGet, int pinValue) {
void controlSwitches() {
  for (int pinGet = 0; pinGet < 8; pinGet++) {
    int pinValue = pcf20.readButton(pinGet);  // Read each PCF8574 input
    switch (pinGet) {
      // -------------------
      case pinRun:
        if (pinValue == 0) {    // 0 : switch is on.
          if (!switchRun) {
            switchRun = true;
          }
        } else if (switchRun) {
          switchRun = false;
#ifdef SWITCH_MESSAGES
          Serial.println("+ Control, Run > run the program.");
#endif
          // ...
          runProgram = true;
        }
        break;
      // -------------------
      case pinStep:
        if (pinValue == 0) {
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
        break;
      // -------------------
      case pinExamine:
        if (pinValue == 0) {
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
        break;
      // -------------------
      case pinExamineNext:
        if (pinValue == 0) {
          if (!switchExamineNext) {
            switchExamineNext = true;
          }
        } else if (switchExamineNext) {
          switchExamineNext = false;
#ifdef SWITCH_MESSAGES
          Serial.println("+ Control, Examine Next.");
#endif
          // ...
        }
        break;
      // -------------------
      case pinDeposit:
        if (pinValue == 0) {
          if (!switchDeposit) {
            switchDeposit = true;
          }
        } else if (switchDeposit) {
          switchDeposit = false;
#ifdef SWITCH_MESSAGES
          Serial.println("+ Control, Deposit.");
#endif
          // ...
        }
        break;
      // -------------------
      case pinDepositNext:
        if (pinValue == 0) {
          if (!switchDepositNext) {
            switchDepositNext = true;
          }
        } else if (switchDepositNext) {
          switchDepositNext = false;
#ifdef SWITCH_MESSAGES
          Serial.println("+ Control, Deposit Next.");
#endif
          // ...
        }
        break;
      // -------------------
      case pinReset:
        if (pinValue == 0) {
          if (!switchReset) {
            switchReset = true;
          }
        } else if (switchReset) {
          switchReset = false;
#ifdef SWITCH_MESSAGES
          Serial.println("+ Control, Reset.");
#endif
          // ...
        }
        break;
    }
    // -------------------
  }
}

// ----------------------------------------------
// When a program is running.

void runningSwitches() {
  for (int pinGet = 0; pinGet < 8; pinGet++) {
    int pinValue = pcf20.readButton(pinGet);  // Read each PCF8574 input
    switch (pinGet) {
      case pinStop:
        if (pinValue == 0) {    // 0 : switch is on.
          if (!switchStop) {
            switchStop = true;
          }
        } else if (switchStop) {
          switchStop = false;
#ifdef SWITCH_MESSAGES
          Serial.println("+ Running, Stop > hlt, halt the processor.");
#endif
          // ...
          runProgram = false;
        }
        break;
      // -------------------
      case pinReset:
        if (pinValue == 0) {
          if (!switchReset) {
            switchReset = true;
          }
        } else if (switchReset) {
          switchReset = false;
#ifdef SWITCH_MESSAGES
          Serial.println("+ Running, Reset.");
#endif
          // ...
        }
        break;
      // -------------------
      default:
        delay(3);
    }
  }
  // Serial.println(":");
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
  // pinMode(INTERRUPT_PIN, INPUT_PULLUP); // Enable pullup on interrupt pin of Uno
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), pcf01interrupt, CHANGE);
  pcf20.begin();
  // delay(300); // required to give startup time for the PCF8574 module.
  Serial.println("+ I2C PCF input module initialized.");

  // ------------------------------
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
    if (switchSetOn) {
      Serial.println("+ runProgram = true, switchSetOn is true.");
      runningSwitches();
      delay(30);           // Handle switch debounce.
      switchSetOn = false;
    } else {
      delay(30);
    }
    // ----------------------------
  } else {
    if (switchSetOn) {
      // Serial.println("+ runProgram = false, switchSetOn is true.");
      controlSwitches();
      delay(30);           // Handle switch debounce.
      switchSetOn = false;
    } else {
      delay(30);
    }
    // ----------------------------
  }

}
// -----------------------------------------------------------------------------
