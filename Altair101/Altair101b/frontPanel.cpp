// -------------------------------------------------------------------------------
/*
  Switch logic for front panel toggles and switches.

  Functionality:
  + ...

  -----------------------------------------------------------------------------
  +++ Next


*/
// -------------------------------------------------------------------------------
#include "Altair101b.h"

// Switch definitions
#include <PCF8574.h>
#include <Wire.h>

// -----------------------------------------------------------------------------
// Desktop version.
// Address for the PCF8574 module being tested.
PCF8574 pcfControl(0x020);  // Control: STOP, RUN, SINGLE STEP, EXAMINE, EXAMINE NEXT, DEPOSIT, DEPOSIT NEXT, REST
PCF8574 pcfData(0x021);     // Low bytes, data byte
PCF8574 pcfSense(0x022);    // High bytes, sense switch byte
PCF8574 pcfAux(0x023);      // AUX switches and others: Step down, CLR, Protect, Unprotect, AUX1 up, AUX1 down,  AUX2 up, AUX2 down

//                   Mega pin for control toggle interrupt. Same pin for Nano.
const int INTERRUPT_PIN = 2;

// Interrupt setup: interrupt pin to use, interrupt handler routine.
boolean pcfControlinterrupted = false;

void setPcfControlinterrupted(boolean theTruth) {
  pcfControlinterrupted = theTruth;
}
boolean getPcfControlinterrupted() {
  return pcfControlinterrupted;
}

void pcfControlinterrupt() {
  pcfControlinterrupted = true;
}

const int pinStop = 7;
const int pinRun = 6;
const int pinStep = 5;
const int pinExamine = 4;
const int pinExamineNext = 3;
const int pinDeposit = 2;
const int pinDepositNext = 1;
const int pinReset = 0;

const int pinAux1up = 3;
const int pinAux1down = 2;
const int pinAux2up = 1;
const int pinAux2down = 0;
const int pinProtect = 5;
const int pinUnProtect = 4;
const int pinClr = 6;
const int pinStepDown = 7;

boolean switchStop = false;
boolean switchRun = false;
boolean switchStep = false;
boolean switchExamine = false;
boolean switchExamineNext = false;
boolean switchDeposit = false;;
boolean switchDepositNext = false;;
boolean switchReset = false;
boolean switchProtect = false;
boolean switchUnProtect = false;
boolean switchClr = false;
boolean switchStepDown = false;

boolean switchAux1up = false;
boolean switchAux1down = false;
boolean switchAux2up = false;
boolean switchAux2down = false;

// -----------------------------------------------------------------------------
// Return hardware sense, data, or address toggle values.

byte fpToggleSense() {
  byte toggleByte = ~pcfSense.read8();
  return toggleByte;
}
byte fpToggleData() {
  byte toggleByte = ~pcfData.read8();
  return toggleByte;
}
uint16_t fpToggleAddress() {
  byte byteLow = ~pcfData.read8();
  byte byteHigh = ~pcfSense.read8();
  return byteHigh * 256 + byteLow;
}
// Invert byte bits using bitwise not operator: "~";

// -----------------------------------------------------------------------------
// Front Panel Control Switches, when a program is running.
// Switches: STOP and RESET.

void checkRunningButtons() {
  // -------------------
  // Read PCF8574 input for this switch.
  if (pcfControl.readButton(pinStop) == 0) {
    if (!switchStop) {
      switchStop = true;
    }
  } else if (switchStop) {
    switchStop = false;
    // Switch logic
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ Running, Stop."));
#endif
    processRunSwitch(stopByte);
  }
  // -------------------
  if (pcfControl.readButton(pinReset) == 0) {
    if (!switchReset) {
      switchReset = true;
    }
  } else if (switchReset) {
    switchReset = false;
    // Switch logic
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ Running, Reset."));
#endif
    processRunSwitch(resetByte);
  }
  // -------------------
}

// -----------------------------------------------------------------------------
// Front Panel Control Switches, when a program is not running (WAIT).
// Switches: RUN, RESET, STEP, EXAMINE, EXAMINE NEXT, DEPOSIT, DEPOSIT NEXT,

void waitControlSwitches() {
  // Read PCF8574 input for each switch.
  // ----------------------------------------------
  if (pcfControl.readButton(pinRun) == 0) {
    if (!switchRun) {
      switchRun = true;
    }
  } else if (switchRun) {
    switchRun = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ WAIT, RUN."));
#endif
    programState = PROGRAM_RUN;
  }
  // ----------------------------------------------
  if (pcfControl.readButton(pinExamine) == 0) {
    if (!switchExamine) {
      switchExamine = true;
    }
  } else if (switchExamine) {
    switchExamine = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ WAIT, EXAMINE."));
#endif
    fpAddressToggleWord = fpToggleAddress();
    processWaitSwitch('x');
  }
  // ----------------------------------------------
  if (pcfControl.readButton(pinExamineNext) == 0) {
    if (!switchExamineNext) {
      switchExamineNext = true;
    }
  } else if (switchExamineNext) {
    switchExamineNext = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ WAIT, EXAMINE NEXT."));
#endif
    processWaitSwitch('X');
  }
  // ----------------------------------------------
  if (pcfControl.readButton(pinDeposit) == 0) {
    if (!switchDeposit) {
      switchDeposit = true;
    }
  } else if (switchDeposit) {
    switchDeposit = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ WAIT, DEPOSIT."));
#endif
    fpAddressToggleWord = fpToggleAddress();
    processWaitSwitch('p');
  }
  // ----------------------------------------------
  if (pcfControl.readButton(pinDepositNext) == 0) {
    if (!switchDepositNext) {
      switchDepositNext = true;
    }
  } else if (switchDepositNext) {
    switchDepositNext = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ WAIT, DEPOSIT NEXT."));
#endif
    fpAddressToggleWord = fpToggleAddress();
    processWaitSwitch('P');
  }
  // ----------------------------------------------
  if (pcfControl.readButton(pinReset) == 0) {
    if (!switchReset) {
      switchReset = true;
    }
  } else if (switchReset) {
    switchReset = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ WAIT, RESET."));
#endif
    processWaitSwitch('R');
  }
  // ----------------------------------------------
  if (pcfControl.readButton(pinStep) == 0) {
    if (!switchStep) {
      switchStep = true;
    }
  } else if (switchStep) {
    switchStep = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ WAIT, SINGLE STEP."));
#endif
    processWaitSwitch('s');
  }
  // ----------------------------------------------
  if (pcfAux.readButton(pinStepDown) == 0) {
    if (!switchStepDown) {
      switchStepDown = true;
    }
  } else if (switchStepDown) {
    switchStepDown = false;
    // Switch logic
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ WAIT, SINGLE STEP down, programCounter: "));
#endif
  }
  // ----------------------------------------------
  if (pcfAux.readButton(pinClr) == 0) {
    if (!switchClr) {
      switchClr = true;
    }
  } else if (switchClr) {
    switchClr = false;
    // Switch logic
    // -------------
    // Double flip confirmation.
    switchClr = false;      // Required to reset the switch state for confirmation.
    boolean confirmChoice = false;
    unsigned long timer = millis();
    while (!confirmChoice && (millis() - timer < 1000)) {
      if (pcfAux.readButton(pinClr) == 0) {
        if (!switchClr) {
          switchClr = true;
        }
      } else if (switchClr) {
        switchClr = false;
        // Switch logic.
        confirmChoice = true;
      }
      delay(100);
    }
    if (!confirmChoice) {
#ifdef SWITCH_MESSAGES
      Serial.println(F("+ WAIT, CLR: cancelled."));
#endif
      return;
    }
    // -------------
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ WAIT, CLR: confirmed."));
#endif
    doClear();
  }
  // -------------------
}

// -----------------------------------------------------------------------------
void checkAux1() {
  // ----------------------------------------------
  if (pcfAux.readButton(pinAux1up) == 0) {
    if (!switchAux1up) {
      switchAux1up = true;
    }
  } else if (switchAux1up) {
    switchAux1up = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ AUX1, up."));
#endif
    if (programState == CLOCK_RUN) {
      Serial.println(F(" WAIT mode: exit CLOCK mode."));
      programState = PROGRAM_WAIT;
    } else {
      Serial.println(F(" CLOCK mode."));
      programState = CLOCK_RUN;
    }
  }
  // ----------------------------------------------
  if (pcfAux.readButton(pinAux1down) == 0) {
    if (!switchAux1down) {
      switchAux1down = true;
    }
  } else if (switchAux1down) {
    switchAux1down = false;
    // Switch logic.
#ifdef SWITCH_MESSAGES
    Serial.println(F("+ AUX1, down."));
#endif
    if (programState == PLAYER_RUN) {
      Serial.println(F(" WAIT mode: exit PLAYER mode."));
      programState = PROGRAM_WAIT;
    } else {
      Serial.println(F(" MP3 PLAYER mode."));
      programState = PLAYER_RUN;
    }
  }
}

void checkProtectSetVolume() {
  // When not in player mode, used to change the volume.
  // ----------------------------------------------
  if (pcfAux.readButton(pinProtect) == 0) {
    if (!switchProtect) {
      switchProtect = true;
    }
  } else if (switchProtect) {
    switchProtect = false;
    // Switch logic.
    processWaitSwitch('V');
  }
  // ----------------------------------------------
  if (pcfAux.readButton(pinUnProtect) == 0) {
    if (!switchUnProtect) {
      switchUnProtect = true;
    }
  } else if (switchUnProtect) {
    switchUnProtect = false;
    // Switch logic
    processWaitSwitch('v');
  }
}

// -----------------------------------------------------------------------------
// SD card module functions

boolean setupFrontPanel() {
  // ----------------------------------------------------
  // Front Panel Switches.
  // I2C Two Wire PCF module initialization
  pcfControl.begin();   // Control switches
  pcfData.begin();      // Tablet: Address/Sense switches
  pcfSense.begin();
  pcfAux.begin();
  // PCF8574 device interrupt initialization
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), pcfControlinterrupt, CHANGE);
  Serial.println(F("+ Front panel toggle switches are configured for input."));
  //
  return true;
}

// -----------------------------------------------------------------------------
