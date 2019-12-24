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

  PCF8574 Wiring:
  + SDA to Nano A4.
  + SCL to Nano A5.
  + GND to Nano GND
  + VCC to Nano 5V
  + INT to Nano interrupt pin, pin 2 in this sample program.
  + P0 ... O7 to switches. Other side of the switch to ground.

  Library:
    https://github.com/RobTillaart/Arduino/tree/master/libraries/PCF8574
*/
// -----------------------------------------------------------------------------
#define SWITCH_MESSAGES 1

bool runProgram = false;
byte dataByte = 0;

void printByte(byte b) {
  for (int i = 7; i >= 0; i--)
    Serial.print(bitRead(b, i));
}

// -----------------------------------------------------------------------------
#include <PCF8574.h>
#include <Wire.h>

PCF8574 pcf20(0x020);
PCF8574 pcf21(0x021);

// Set switch flag for on/off.
const int INTERRUPT_PIN = 2;
boolean pcf20interrupted = false;
// Interrupt setup: I2C address, interrupt pin to use, interrupt handler routine.
void pcf20interrupt() {
  pcf20interrupted = true;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Front Panel Control Switches

// -------------------------
// Get Front Panel Toggles value, the sense switches.

int toggleSenseByte() {
  byte toggleByte = ~pcf21.read8();
  // Invert byte bits using bitwise not operator: "~";
  return toggleByte;
}

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
  for (int pinGet = 7; pinGet >= 0; pinGet--) {
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
          dataByte = toggleSenseByte();
#ifdef SWITCH_MESSAGES
          Serial.print("++ toggleSenseByte = ");
          printByte(dataByte);
          Serial.println("");
#endif
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
  for (int pinGet = 7; pinGet >= 0; pinGet--) {
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
  // I2C Two Wire + interrupt initialization
  pcf20.begin();
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), pcf20interrupt, CHANGE);
  Serial.println("+ PCF module initialized.");

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
    if (pcf20interrupted) {
      // Serial.println("+ runProgram = true, switchSetOn is true.");
      runningSwitches();
      delay(30);           // Handle switch debounce.
      pcf20interrupted = false;
    }
    // ----------------------------
  } else {
    if (pcf20interrupted) {
      // Serial.println("+ runProgram = false, switchSetOn is true.");
      controlSwitches();
      delay(30);           // Handle switch debounce.
      pcf20interrupted = false;
    }
    // ----------------------------
    delay(30);
  }

}
// -----------------------------------------------------------------------------
