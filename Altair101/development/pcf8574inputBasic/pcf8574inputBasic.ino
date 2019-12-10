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
#define SWITCH_MESSAGES 1
bool runProgram = false;
byte dataByte;
void printByte(byte b) {
  for (int i = 7; i >= 0; i--)
    Serial.print(bitRead(b, i));
}

// -----------------------------------------------------------------------------
#include <PCF8574.h>
#include <Wire.h>

PCF8574 pcf20(0x020);

// Set switch flag for on/off.
const int INTERRUPT_PIN = 2;
boolean switchSetOn = false;
// Interrupt setup: I2C address, interrupt pin to use, interrupt handler routine.
void pcf20interrupt() {
  switchSetOn = true;
}

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

void runningSwitches() {

  Serial.print("+ pinValue:");
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
        delay (10);
    }
  }
  Serial.println(":");
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
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
void loop() {
  /*
    dataByte = pcf20.read8();
    Serial.print("+ PCF8574 byte = ");
    printByte(dataByte);
    Serial.println("");

    Serial.print("+ PCF8574 byte = ");
    for (int pinGet = 7; pinGet >= 0; pinGet--) {
      int pinValue = pcf20.readButton(pinGet);  // Read each PCF8574 input
      Serial.print(pinValue);
    }
    Serial.println("");
  */
  if (switchSetOn) {
    // Serial.println("+ Interrupt call, switchSetOn is true.");
    runningSwitches();
    switchSetOn = false;
  }

  delay (60);
}
// -----------------------------------------------------------------------------
