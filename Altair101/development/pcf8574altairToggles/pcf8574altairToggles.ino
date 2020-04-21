// -----------------------------------------------------------------------------
/*
  Testing toggle implementations for both the Tablet and the Desktop modules.

*/
// -----------------------------------------------------------------------------
// If defined, Desktop module. Else Tablet module.
// #define DESKTOP_MODULE 1
//
// #ifdef DESKTOP_MODULE
//    ... Desktop module code ...
// #else
//    ... Tablet module code ...
// #endif

// -----------------------------------------------------------------------------
// Utility functions from Processor.ino

// --------------------------
#define PROGRAM_WAIT 0
#define PROGRAM_RUN 1
#define CLOCK_RUN 2
#define PLAYER_RUN 3
#define SERIAL_DOWNLOAD 4
int programState = PROGRAM_WAIT;  // Intial, default.

// -----------------------------------------------------------------------------
// Tablet module
//                              Mega pins
const int CLOCK_SWITCH_PIN =    8;  // Tested pins: 8..11. Doesn't work: 24, 33.
const int PLAYER_SWITCH_PIN =   9;
const int UPLOAD_SWITCH_PIN =   10;
const int DOWNLOAD_SWITCH_PIN = 11;

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
      Serial.print(F("+ pinStep switch pressed..."));
    }
  } else if (switchStep) {
    switchStep = false;
    // Switch logic.
    Serial.println(F(" Released."));
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
      Serial.print(F("+ pinExamineNext switch pressed..."));
    }
  } else if (switchExamineNext) {
    switchExamineNext = false;
    // Switch logic.
    Serial.println(F(" Released."));
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
      Serial.print(F("+ pinDepositNext switch pressed..."));
    }
  } else if (switchDepositNext) {
    switchDepositNext = false;
    // Switch logic.
    Serial.println(F(" Released."));
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
#ifdef DESKTOP_MODULE
  if (pcfAux.readButton(pinAux1up) == 0) {
#else
  // Tablet:
  if (digitalRead(CLOCK_SWITCH_PIN) == LOW) {
#endif
    if (!switchAux1up) {
      switchAux1up = true;
      Serial.print(F("+ pinAux1up switch pressed..."));
    }
  } else if (switchAux1up) {
    switchAux1up = false;
    // Switch logic.
    Serial.println(F(" Released."));
    if (programState == CLOCK_RUN) {
      programState = PROGRAM_WAIT;
      digitalWrite(HLDA_PIN, LOW);
      digitalWrite(WAIT_PIN, HIGH);
    } else {
      programState = CLOCK_RUN;
      digitalWrite(HLDA_PIN, HIGH);
      digitalWrite(WAIT_PIN, LOW);
    }
  }

  // -------------------
#ifdef DESKTOP_MODULE
  if (pcfAux.readButton(pinAux1down) == 0) {
#else
  // Tablet:
  if (digitalRead(PLAYER_SWITCH_PIN) == LOW) {
#endif
    if (!switchAux1down) {
      switchAux1down = true;
      Serial.print(F("+ AUX1 down switch pressed..."));
    }
  } else if (switchAux1down) {
    switchAux1down = false;
    // Switch logic.
    Serial.println(F(" Released."));
    digitalWrite(WAIT_PIN, LOW);
    digitalWrite(HLDA_PIN, LOW);
  }


  // -------------------
#ifdef DESKTOP_MODULE
  if (pcfAux.readButton(pinAux2up) == 0) {
#else
  // Tablet:
  if (digitalRead(UPLOAD_SWITCH_PIN) == LOW) {
#endif
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
#ifdef DESKTOP_MODULE
  if (pcfAux.readButton(pinAux2down) == 0) {
#else
  // Tablet:
  if (digitalRead(DOWNLOAD_SWITCH_PIN) == LOW) {
#endif
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

#ifdef DESKTOP_MODULE
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
#endif
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
  pinMode(WAIT_PIN, OUTPUT);    // Indicator: program wait state: LED on or LED off.
  pinMode(HLDA_PIN, OUTPUT);    // Indicator: emulator (LED off)v or other devices (LED on) such as clock or MP3 player.
  digitalWrite(WAIT_PIN, HIGH); // Default to wait state.
  digitalWrite(HLDA_PIN, LOW);  // Default to emulator.

  // ------------------------------
  // I2C Two Wire PCF module initialization
  pcfControl.begin();
  pcfData.begin();
#ifdef DESKTOP_MODULE
  pcfSense.begin();
  pcfAux.begin();
#else
  pinMode(CLOCK_SWITCH_PIN, INPUT_PULLUP);
  pinMode(PLAYER_SWITCH_PIN, INPUT_PULLUP);
  pinMode(UPLOAD_SWITCH_PIN, INPUT_PULLUP);
  pinMode(DOWNLOAD_SWITCH_PIN, INPUT_PULLUP);
  Serial.println(F("+ Tablet AUX device toggle switches are configured for input."));
#endif
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
      //
      // AUX controls are interrupt controlled on the Tablet and Desktop configurations.
#ifdef DESKTOP_MODULE
      checkAuxButtons();
#endif
    }
    //
    pcfControlinterrupted = false; // Reset for next interrupt.
  }

  // AUX controls are not interrupt controlled on the Tablet and Desktop configurations.
#ifdef DESKTOP_MODULE
#else
  checkAuxButtons();     // Not an interrupt switch on the tablet.
#endif

  delay (60);
}
// -----------------------------------------------------------------------------
