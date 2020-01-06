// -----------------------------------------------------------------------------
/*
  PCF8574 I2C Module
  I2C to 8-bit Parallel-Port Expander

  Module adjustable pin address settings:
  A0 A1 A2
   0  0  0 = 0x20   Control toggle switches
   0  0  1 = 0x21   Data toggles
   0  1  0 = 0x22
    ...
   1  1  1 = 0x27

  PCF8574 Wiring:
  + SDA to Nano A4.
  + SCL to Nano A5.
  + GND to Nano GND
  + VCC to Nano 5V
  + INT to Nano interrupt pin, pin 2 in this sample program.
  + P0 ... O7 to switches.
  ++ Other side of the switches are connected together, and then to ground.
  ++ Connect(plug) the toggle grounds into the PCF8574 module ground.
  + Daisy chain 2 module: 1) Control toggle switches with interrupt, 2) Data toggles.

  Library:
    https://github.com/RobTillaart/Arduino/tree/master/libraries/PCF8574

  Reference for AUX switches:
   Digital Pins, Properties of Pins Configured as INPUT_PULLUP:
    https://www.arduino.cc/en/Tutorial/DigitalPins
  Sample curcuits:
    https://create.arduino.cc/projecthub/muhammad-aqib/arduino-button-tutorial-using-arduino-digitalread-function-08adb5#toc-using-internal-pull-up-resistor-9
  Note, INPUT_PULLUP didn't work for Mega pins 24 and 33.
    I read, to try using an external pull up resister.
    Also read, When connecting a sensor to a pin configured with INPUT_PULLUP, 
      the other end should be connected to the ground. 
      In case of a switch, the pin reads HIGH when the switch is open and LOW when the switch is pressed.
        void setup () { ... pinMode(button , INPUT_PULLUP); ... }
        void loop () { ... If (digitalRead(button ) == LOW) // if button pressed { ... }
  Mega pin notes,
    http://www.circuitstoday.com/arduino-mega-pinout-schematics
    Digital pin 2-13 can be used as PWM output with analogWrite().
    Digital pin 20 SDA and 21 for SCK, for I2C.
    Pin 22 - SS, Pin 23 - SCK, Pin 24 - MOSI, Pin 25 – MISO
  USART Pins :
    Pin 00 – RXD0, pin 01 – TXD0
    Pin 19 – RXD1, pin 18 – TXD1
    Pin 17 – RXD2, pin 16 – TXD2
    Pin 15 – RXD3, pin 14 – TXD3
  Pinchange Interrupt Pins:
    Digital Pin 0,10,11,12,13,15,14,22,23,24,25
    Analog  Pin 6,7,8,9,10,11,12,13,14,15
    Example :
      pinMode(0, OUTPUT);
      pinMode(1, INPUT_PULLUP);
      attachInterrupt(digitalPinToInterrupt(1), LOW, CHANGE);
    Digital pin 18 – 21,2,3 hardware interrupt

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
// -----------------------------------------------------------------------------
// Switches

const int CLOCK_SWITCH_PIN = A11;  // Tested pins, works: 4, A11. Doesn't work: 24, 33.
const int PLAYER_SWITCH_PIN = A12;
const int UPLOAD_SWITCH_PIN = A13;
const int DOWNLOAD_SWITCH_PIN = A14;

// Only do the action once, don't repeat if the switch is held down.
// Don't repeat action if the switch is not pressed.
boolean clockSwitchState = true;
boolean playerSwitchState = true;
boolean uploadSwitchState = true;
boolean downloadSwitchState = true;

void checkClockSwitch() {
  if (digitalRead(CLOCK_SWITCH_PIN) == HIGH) {
    if (!clockSwitchState) {
      Serial.println(F("+ Clock switch released."));
      clockSwitchState = false;
      // Switch logic ...
    }
    clockSwitchState = true;
  } else {
    if (clockSwitchState) {
      Serial.println(F("+ Clock switch pressed."));
      clockSwitchState = false;
      // Switch logic ...
    }
  }
}
void checkPlayerSwitch() {
  if (digitalRead(PLAYER_SWITCH_PIN) == HIGH) {
    if (!playerSwitchState) {
      Serial.println(F("+ Player switch released."));
      playerSwitchState = false;
      // Switch logic ...
    }
    playerSwitchState = true;
  } else {
    if (playerSwitchState) {
      Serial.println(F("+ Player switch pressed."));
      playerSwitchState = false;
      // Switch logic ...
    }
  }
}
void checkUploadSwitch() {
  if (digitalRead(UPLOAD_SWITCH_PIN) == HIGH) {
    if (!uploadSwitchState) {
      Serial.println(F("+ Upload switch released."));
      uploadSwitchState = false;
      // Switch logic ...
    }
    uploadSwitchState = true;
  } else {
    if (uploadSwitchState) {
      Serial.println(F("+ Upload switch pressed."));
      uploadSwitchState = false;
      // Switch logic ...
    }
  }
}
void checkDownloadSwitch() {
  if (digitalRead(DOWNLOAD_SWITCH_PIN) == HIGH) {
    if (!downloadSwitchState) {
      Serial.println(F("+ Download switch released."));
      downloadSwitchState = false;
      // Switch logic ...
    }
    downloadSwitchState = true;
  } else {
    if (downloadSwitchState) {
      Serial.println(F("+ Download switch pressed."));
      downloadSwitchState = false;
      // Switch logic ...
    }
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
  // I2C Two Wire + interrupt initialization
  pcf20.begin();
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), pcf20interrupt, CHANGE);
  Serial.println("+ PCF module initialized.");

  pinMode(CLOCK_SWITCH_PIN, INPUT_PULLUP);
  pinMode(PLAYER_SWITCH_PIN, INPUT_PULLUP);
  pinMode(UPLOAD_SWITCH_PIN, INPUT_PULLUP);
  pinMode(DOWNLOAD_SWITCH_PIN, INPUT_PULLUP);
  Serial.println(F("+ Toggle/button switches are configured for input."));

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
    checkClockSwitch();
    checkPlayerSwitch();
    checkUploadSwitch();
    checkDownloadSwitch();
    // ----------------------------
    delay(30);
  }

}
// -----------------------------------------------------------------------------
