// -----------------------------------------------------------------------------
/*
  PCF8574 I2C Module

  I2C to 8-bit Parallel-Port Expander

  Module with adjustable pin address settings:
   --------------
  |  V G S S     | Above are female pins
  |  C N D C     |
  |  C D A L     |
  |              | 0 is up   (-)
  |     A2 A1 A0 | 1 is down (+), toward the male pins at the bottom.
  |     0  0  0  |  = 0x20
  |     0  0  1  |  = 0x21, A0 is connected down, the others up.
  |     0  1  0  |  = 0x22
  |     0  1  1  |  = 0x23
  |     1  0  0  |  = 0x24
  | P0  1  0  1  |  = 0x25
  | P1  1  1  0  |  = 0x26
  | P2  1  1  1  |  = 0x27
  | P3           |
  | P4           |
  | P5           |
  | P6           |
  | P7           |
  | INT          | For multiple, test using a diode to the Arduino interrupt pin.
  |              |
  |  V G S S     | Below are male pins
  |  C N D C     |
  |  C D A L     |
   --------------
     | | | |

  Wiring:
  + SDA to Nano A4 or Mega pin SDA 20.
  + SCL to Nano A5 or Mega pin SCL 21.
  + GND to Arduino GND
  + VCC to Arduino 5V
  + INT to interrupt pin, pin 2 on Nano, in this sample program.
  + P0 ... O7 to switches. Other side of the switch to ground.

*/
// -----------------------------------------------------------------------------
// Utility functions from Processor.ino

// --------------------------
// #define DESKTOP_MODULE 1

// --------------------------
#define PROGRAM_WAIT 0
#define PROGRAM_RUN 1
#define CLOCK_RUN 2
#define PLAYER_RUN 3
#define SERIAL_DOWNLOAD 4
int programState = PROGRAM_WAIT;  // Intial, default.

// --------------------------
// Instruction parameters:
byte dataByte = 0;           // db = Data byte (8 bit)

void printByte(byte b) {
  for (int i = 7; i >= 0; i--)
    Serial.print(bitRead(b, i));
}
void printWord(unsigned int theValue) {
  String sValue = String(theValue, BIN);
  for (int i = 1; i <= 16 - sValue.length(); i++) {
    Serial.print("0");
  }
  Serial.print(sValue);
}
void printOctal(byte b) {
  String sValue = String(b, OCT);
  for (int i = 1; i <= 3 - sValue.length(); i++) {
    Serial.print("0");
  }
  Serial.print(sValue);
}

char charBuffer[17];
void printData(byte theByte) {
  sprintf(charBuffer, "%3d = ", theByte);
  Serial.print(charBuffer);
  printOctal(theByte);
  Serial.print(F(" = "));
  printByte(theByte);
}

// -----------------------------------------------------------------------------
// Status LED lights

// Program wait status.
const int WAIT_PIN = A9;      // Program wait state: off/LOW or wait state on/HIGH.
// const int WAIT_PIN = 6;      // Program wait state: off/LOW or wait state on/HIGH.

// HLDA : 8080 processor goes into a hold state because of other hardware running.
const int HLDA_PIN = A10;     // Emulator processing (off/LOW) or clock processing (on/HIGH).
// const int HLDA_PIN = 7;     // Emulator processing (off/LOW) or clock processing (on/HIGH).

// -----------------------------------------------------------------------------
#include <PCF8574.h>
#include <Wire.h>

// -------------------------
// Interrupt handler routine.
//                   Mega pin for control toggle interrupt. Same pin for Nano.
const int INTERRUPT_PIN = 2;

boolean pcfControlinterrupted = false;
void pcfControlinterrupt() {
  pcfControlinterrupted = true;
}

// Test with a diode for having 2 modules use the same Arduino interrupt pin.
// pcfControl has interupt enabled.
// Implement: pcfAux interupt handling. Likely use the same pin, pin 2.

// -------------------------------------------
// Desktop version.
// Address for the PCF8574 module being tested.
PCF8574 pcfControl(0x020);  // Control: STOP, RUN, SINGLE STEP, EXAMINE, EXAMINE NEXT, DEPOSIT, DEPOSIT NEXT, REST
PCF8574 pcfData(0x021);     // Low bytes, data byte
PCF8574 pcfSense(0x022);    // High bytes, sense switch byte
PCF8574 pcfAux(0x023);      // AUX switches and others: Step down, CLR, Protect, Unprotect, AUX1 up, AUX1 down,  AUX2 up, AUX2 down

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Front Panel Switches

#ifdef DESKTOP_MODULE
const int pinStop = 7;
const int pinReset = 0;
const int pinRun = 6;
const int pinStep = 5;
const int pinExamine = 4;
const int pinExamineNext = 3;
const int pinDeposit = 2;
const int pinDepositNext = 1;
const int pinReset = 0;
#else
// TABLET_MODULE
const int pinStop = 0;
const int pinRun = 1;
const int pinStep = 2;
const int pinExamine = 3;
const int pinExamineNext = 4;
const int pinDeposit = 5;
const int pinDepositNext = 6;
const int pinReset = 7;
#endif

const int pinAux1up = 3;
const int pinAux1down = 2;
const int pinAux2up = 1;
const int pinAux2down = 0;
const int pinProtect = 5;
const int pinUnProtect = 4;
const int pinClr = 6;
const int pinStepDown = 7;

// -------------------------
// Get Front Panel address/data/sense toggle values.

int toggleDataByte() {
  // Invert byte bits using bitwise not operator: "~";
  // Bitwise "not" operator to invert bits:
  //  int a = 103;  // binary:  0000000001100111
  //  int b = ~a;   // binary:  1111111110011000 = -104
  byte toggleByte = ~pcfData.read8();
  return toggleByte;
}
int toggleSenseByte() {
  byte toggleByte = ~pcfSense.read8();
  return toggleByte;
}
unsigned int toggleAddress() {
  byte byteLow = ~pcfData.read8();
  byte byteHigh = ~pcfSense.read8();
  return byteHigh * 256 + byteLow;
}

// -------------------------------------------------------------------
// Front Panel Control Switches, when a program is not running.
// Switches: STOP and RESET.

boolean switchStop = false;
boolean switchReset = false;

void checkRunningButtons() {
  if (pcfControl.readButton(pinStop) == 0) {
    if (!switchStop) {
      switchStop = true;
    }
  } else if (switchStop) {
    switchStop = false;
    // Switch logic.
    Serial.println(F("+ Running, Stop."));
    programState = PROGRAM_WAIT;
    digitalWrite(WAIT_PIN, HIGH);
    digitalWrite(HLDA_PIN, LOW);
  }
  // -------------------
  // Read PCF8574 input for this switch.
  if (pcfControl.readButton(pinReset) == 0) {
    if (!switchReset) {
      switchReset = true;
    }
  } else if (switchReset) {
    switchReset = false;
    // Switch logic.
    Serial.println(F("+ Running, Reset."));
  }
}

// -------------------------------------------------------------------
// Front Panel Control Switches, when a program is not running (WAIT).

// Switches: RESET.
// Switches: RUN, STEP, EXAMINE, EXAMINE NEXT, DEPOSIT, DEPOSIT NEXT,

// Only do the action once, don't repeat if the button is held down.
// Don't repeat action if the button is not pressed.

boolean switchRun = false;
boolean switchStep = false;
boolean switchExamine = false;
boolean switchExamineNext = false;
boolean switchDeposit = false;;
boolean switchDepositNext = false;;

void checkControlButtons() {
  // -------------------
  // Read PCF8574 input for this switch.
  if (pcfControl.readButton(pinStop) == 0) {
    if (!switchStop) {
      switchStop = true;
    }
  } else if (switchStop) {
    switchStop = false;
    // Switch logic.
    Serial.println(F("+ Control, Stop."));
  }
  // -------------------
  // Read PCF8574 input for this switch.
  if (pcfControl.readButton(pinReset) == 0) {
    if (!switchReset) {
      switchReset = true;
    }
  } else if (switchReset) {
    switchReset = false;
    // Switch logic.
    Serial.println(F("+ Control, Reset."));
  }
  // -------------------
  if (pcfControl.readButton(pinRun) == 0) {
    if (!switchRun) {
      switchRun = true;
    }
  } else if (switchRun) {
    switchRun = false;
    // Switch logic.
    Serial.println(F("+ Control, pinRun."));
    programState = PROGRAM_RUN;
    digitalWrite(WAIT_PIN, LOW);
    digitalWrite(HLDA_PIN, LOW);
  }
  // -------------------
  if (pcfControl.readButton(pinStep) == 0) {
    if (!switchStep) {
      switchStep = true;
    }
  } else if (switchStep) {
    switchStep = false;
    // Switch logic.
    Serial.println(F("+ Control, pinStep."));
  }
  // -------------------
  if (pcfControl.readButton(pinExamine) == 0) {
    if (!switchExamine) {
      switchExamine = true;
    }
  } else if (switchExamine) {
    switchExamine = false;
    // Switch logic.
    Serial.println(F("+ Control, Examine: "));
    Serial.print(" Togle address = ");
    printWord(toggleAddress());
    Serial.print(", data=");
    printByte(toggleDataByte());
    Serial.print(", sense=");
    printByte(toggleSenseByte());
    Serial.println("");
  }
  // -------------------
  if (pcfControl.readButton(pinExamineNext) == 0) {
    if (!switchExamineNext) {
      switchExamineNext = true;
    }
  } else if (switchExamineNext) {
    switchExamineNext = false;
    // Switch logic.
    Serial.println(F("+ Control, pinExamineNext."));
  }
  // -------------------
  if (pcfControl.readButton(pinDeposit) == 0) {
    if (!switchDeposit) {
      switchDeposit = true;
    }
  } else if (switchDeposit) {
    switchDeposit = false;
    // Switch logic.
    unsigned int theToggleAddress = toggleAddress();
    Serial.println(F("+ Control, pinDeposit."));
    Serial.print(" Togle address = ");
    printWord(toggleAddress());
    Serial.print(", data=");
    printByte(toggleDataByte());
    Serial.print(", sense=");
    printByte(toggleSenseByte());
    Serial.println("");
    digitalWrite(WAIT_PIN, HIGH);
    digitalWrite(HLDA_PIN, HIGH);
  }
  // -------------------
  if (pcfControl.readButton(pinDepositNext) == 0) {
    if (!switchDepositNext) {
      switchDepositNext = true;
    }
  } else if (switchDepositNext) {
    switchDepositNext = false;
    // Switch logic.
    Serial.println(F("+ Control, pinDepositNext."));
    digitalWrite(WAIT_PIN, LOW);
    digitalWrite(HLDA_PIN, LOW);
  }
}

// --------------------------------------------------------
// Front Panel Control Switches, when a program is not running.
// Switches: AUX 1, AUX 1, PROTECT, UNPROTECT, CLR, and down STEP.

boolean switchAux1up = false;
boolean switchAux1down = false;
boolean switchAux2up = false;
boolean switchAux2down = false;

void checkAuxButtons() {
  // -------------------
  if (pcfAux.readButton(pinAux1up) == 0) {
    if (!switchAux1up) {
      switchAux1up = true;
    }
  } else if (switchAux1up) {
    switchAux1up = false;
    // Switch logic.
    Serial.println(F("+ Control, pinAux1up."));
    digitalWrite(WAIT_PIN, HIGH);
    digitalWrite(HLDA_PIN, HIGH);
  }
  // -------------------
  if (pcfAux.readButton(pinAux1down) == 0) {
    if (!switchAux1down) {
      switchAux1down = true;
    }
  } else if (switchAux1down) {
    switchAux1down = false;
    // Switch logic.
    Serial.println(F("+ Control, pinAux1down."));
    digitalWrite(WAIT_PIN, LOW);
    digitalWrite(HLDA_PIN, LOW);
  }
  // -------------------
  if (pcfAux.readButton(pinAux2up) == 0) {
    if (!switchAux2up) {
      switchAux2up = true;
    }
  } else if (switchAux2up) {
    switchAux2up = false;
    // Switch logic.
    Serial.println(F("+ Control, pinAux2up."));
    digitalWrite(WAIT_PIN, HIGH);
    digitalWrite(HLDA_PIN, HIGH);
  }
  // -------------------
  if (pcfAux.readButton(pinAux2down) == 0) {
    if (!switchAux1down) {
      switchAux1down = true;
    }
  } else if (switchAux1down) {
    switchAux1down = false;
    // Switch logic.
    Serial.println(F("+ Control, pinAux2down."));
    digitalWrite(WAIT_PIN, LOW);
    digitalWrite(HLDA_PIN, LOW);
  }
  // -------------------
  if (pcfAux.readButton(pinProtect) == 0) {
    Serial.println(F("+ Control, pinProtect."));
  }
  if (pcfAux.readButton(pinUnProtect) == 0) {
    Serial.println(F("+ Control, pinUnProtect."));
  }
  if (pcfAux.readButton(pinClr) == 0) {
    Serial.println(F("+ Control, pinClr."));
  }
  if (pcfAux.readButton(pinStepDown) == 0) {
    Serial.println(F("+ Control, pinStepDown."));
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
  // System application status LED lights
  
  digitalWrite(WAIT_PIN, HIGH);
  digitalWrite(HLDA_PIN, LOW);  // Default to emulator.

  // ------------------------------
  // I2C Two Wire PCF module initialization
  pcfControl.begin();
  pcfAux.begin();
  pcfData.begin();
  pcfSense.begin();

  // PCF module interrupt initialization
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), pcfControlinterrupt, CHANGE);

  Serial.println("+ Toggle PCF8574 input modules initialized.");

  // ------------------------------
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
void loop() {

  if (pcfControlinterrupted) {
    if (programState == PROGRAM_RUN) {
      checkRunningButtons();
    } else {
      checkControlButtons();
#ifdef DESKTOP_MODULE
      checkAuxButtons();
#endif
    }
    //
    pcfControlinterrupted = false; // Reset for next interrupt.
  }

  delay (60);
}
// -----------------------------------------------------------------------------
