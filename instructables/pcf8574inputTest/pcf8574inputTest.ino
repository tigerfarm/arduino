// -----------------------------------------------------------------------------
/*
  PCF8574 I2C Module

  I2C to 8-bit Parallel-Port Expander

  Module adjustable pin address settings:
   --------------
  |              |
  |              | 0 is up   (-)
  | A2 A1 A0     | 1 is down (+)
  | 0  0  0      |  = 0x20
  | 1  0  0      |  = 0x21, A2 is connected down, the others up.
  | 0  1  0      |  = 0x22
  | 1  1  0      |  = 0x23
  | 0  0  1      |  = 0x24
  | 1  0  1   P0 |  = 0x25
  | 0  1  1   P1 |  = 0x26
  | 1  1  1   P2 |  = 0x27
  |           P3 |
  |           P4 |
  |           P5 |
  |           P6 |
  |  V G S S  P7 |
  |  C N D C INT | For multiple, I used a diode to each.
  |  C D A L     |
   --------------
     | | | |

  Wiring:
  + SDA to Nano A4.
  + SCL to Nano A5.
  + GND to Nano GND
  + VCC to Nano 5V
  + INT to interrupt pin, pin 2 on Nano, in this sample program.
  + P0 ... O7 to switches. Other side of the switch to ground.

*/
// -----------------------------------------------------------------------------
#define SWITCH_MESSAGES 1

char charBuffer[17];

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
void printData(byte theByte) {
  sprintf(charBuffer, "%3d = ", theByte);
  Serial.print(charBuffer);
  printOctal(theByte);
  Serial.print(F(" = "));
  printByte(theByte);
}

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

/*
  Desktop version.
 */
PCF8574 pcfControl(0x020);  // Controls: STOP, RUN, EXAMINE, EXAMINE NEXT, DEPOSIT, DEPOSIT NEXT
PCF8574 pcfAux(0x023);      // AUX 1, AUX 2, PROTECT, UNPROTECT, CLR, and switch below STEP
PCF8574 pcfData(0x021);     // low address byte, data byte
PCF8574 pcfSense(0x022);    // high address byte, sense switch byte

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Front Panel Switches

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
  return byteHigh*256 + byteLow;
}

// -------------------------------------------------------------------
// Front Panel Control Switches, when a program is not running.
// Switches: STOP and RESET.

const int pinStop = 0;
const int pinReset = 7;

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

const int pinRun = 1;
const int pinStep = 2;
const int pinExamine = 3;
const int pinExamineNext = 4;
const int pinDeposit = 5;
const int pinDepositNext = 6;

boolean switchRun = false;
boolean switchStep = false;
boolean switchExamine = false;
boolean switchExamineNext = false;
boolean switchDeposit = false;;
boolean switchDepositNext = false;;

void checkExamineButton() {
  // Read PCF8574 input for this switch.
  if (pcfControl.readButton(pinExamine) == 0) {
    if (!switchExamine) {
      switchExamine = true;
    }
  } else if (switchExamine) {
    switchExamine = false;
    //
    // Switch logic, based on programState.
    //
    // --------------------------
    dataByte = toggleSenseByte();
    // programCounter = toggleSenseByte();
    // curProgramCounter = programCounter;     // Synch for control switches.
    // dataByte = memoryData[programCounter];
    // processDataLights();
    // --------------------------
    //
#ifdef SWITCH_MESSAGES
    Serial.print(F("+ Control, Examine: "));
    printByte(dataByte);
    Serial.print(" = ");
    printData(dataByte);
    Serial.println("");
#endif
  }

}

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
    Serial.println(F("+ Running, Stop."));
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
  // -------------------
  if (pcfControl.readButton(pinRun) == 0) {
    if (!switchRun) {
      switchRun = true;
    }
  } else if (switchRun) {
    switchRun = false;
    // Switch logic.
    Serial.println(F("+ Control, pinRun."));
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
    Serial.print(F("+ Control, pinDeposit."));
    Serial.print(F(", address: "));
    Serial.println(theToggleAddress);
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
  }
}

// --------------------------------------------------------
// Front Panel Control Switches, when a program is not running.
// Switches: AUX 1, AUX 1, PROTECT, UNPROTECT, CLR, and down STEP.

const int pinAux1up = 0;
const int pinAux1down = 1;
const int pinAux2up = 2;
const int pinAux2down = 3;
const int pinProtect = 4;
const int pinUnProtect = 5;
const int pinClr = 6;
const int pinStepDown = 7;

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
  }
  // -------------------
  if (pcfAux.readButton(pinProtect) == 0) {
    Serial.println(F("+ Control, pinProtect."));
  }
  if (pcfAux.readButton(pinUnProtect) == 0) {
    Serial.println(F("+ Control, pinUnProtect."));
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
  pcfControl.begin();
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), pcfControlinterrupt, CHANGE);
  Serial.println("+ PCF module initialized.");

  // ------------------------------
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
void loop() {

  if (pcfControlinterrupted) {
    // ----------------------
    Serial.println("+ Interrupt call, switchSetOn is true.");
    dataByte = pcfControl.read8();                   // Read all PCF8574 inputs
    Serial.print("+ PCF8574 0x20 byte, read8      = ");
    printByte(dataByte);
    Serial.println("");
    // ----------------------
    Serial.print("+ PCF8574 0x20 byte, readButton = ");
    for (int pinGet = 7; pinGet >= 0; pinGet--) {
      int pinValue = pcfControl.readButton(pinGet);  // Read each PCF8574 input
      Serial.print(pinValue);
    }
    // ----------------------
    Serial.println("");
    // ----------------------
    checkRunningButtons();
    checkControlButtons();
    //
    pcfControlinterrupted = false; // Reset for next interrupt.
  }

  delay (60);
}
// -----------------------------------------------------------------------------
