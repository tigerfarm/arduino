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

/*
  Desktop module,
  +++ pcf20 has interupt enabled.
  ++ Implement: pcf23 with interupt enabled. Likely use the same Mega pin, pin 2.
 */
PCF8574 pcf20(0x020); // Controls: STOP, RUN, EXAMINE, EXAMINE NEXT, DEPOSIT, DEPOSIT NEXT
PCF8574 pcf21(0x021); // low address byte
PCF8574 pcf22(0x022); // high address byte, sense switches
PCF8574 pcf23(0x023); // AUX 1, AUX 2, PROTECT, UNPROTECT, CLR, and switch below STEP.

// -------------------------
// Interrupt setup: interrupt pin to use, interrupt handler routine.
//                   Mega pin for control toggle interrupt. Same pin for Nano.
const int INTERRUPT_PIN = 2;

boolean pcf20interrupted = false;
void pcf20interrupt() {
  pcf20interrupted = true;
}

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
  byte toggleByte = ~pcf21.read8();
  return toggleByte;
}
int toggleSenseByte() {
  // Invert byte bits using bitwise not operator: "~";
  // Bitwise "not" operator to invert bits:
  //  int a = 103;  // binary:  0000000001100111
  //  int b = ~a;   // binary:  1111111110011000 = -104
  byte toggleByte = ~pcf22.read8();
  return toggleByte;
}

// -------------------------
// Front Panel Control Switches, when a program is not running.

// Only do the action once, don't repeat if the button is held down.
// Don't repeat action if the button is not pressed.

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

// -------------------------------------------------------------------
// Front Panel Control Switches, when a program is not running (WAIT).

void checkExamineButton() {
  // Read PCF8574 input for this switch.
  if (pcf20.readButton(pinExamine) == 0) {
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

// --------------------------------------------------------
// Front Panel Control Switches, when a program is running.
// Switches: STOP and RESET.
// Switches: RUN, STEP, EXAMINE, EXAMINE NEXT, DEPOSIT, DEPOSIT NEXT,

void checkControlButtons() {
  // -------------------
  // Read PCF8574 input for this switch.
  if (pcf20.readButton(pinStop) == 0) {
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
  if (pcf20.readButton(pinReset) == 0) {
    if (!switchReset) {
      switchReset = true;
    }
  } else if (switchReset) {
    switchReset = false;
    // Switch logic.
    Serial.println(F("+ Running, Reset."));
  }
  // -------------------
  if (pcf20.readButton(pinRun) == 0) {
    if (!switchRun) {
      switchRun = true;
    }
  } else if (switchRun) {
    switchRun = false;
    // Switch logic.
    Serial.println(F("+ Control, pinRun."));
  }
  // -------------------
  if (pcf20.readButton(pinStep) == 0) {
    if (!switchStep) {
      switchStep = true;
    }
  } else if (switchStep) {
    switchStep = false;
    // Switch logic.
    Serial.println(F("+ Control, pinStep."));
  }
  // -------------------
  if (pcf20.readButton(pinExamine) == 0) {
    if (!switchExamine) {
      switchExamine = true;
    }
  } else if (switchExamine) {
    switchExamine = false;
    // Switch logic.
    Serial.print(F("+ Control, Examine: "));
    printByte(dataByte);
    Serial.print(" = ");
    printData(dataByte);
    Serial.println("");
  }
  // -------------------
  if (pcf20.readButton(pinExamineNext) == 0) {
    if (!switchExamineNext) {
      switchExamineNext = true;
    }
  } else if (switchExamineNext) {
    switchExamineNext = false;
    // Switch logic.
    Serial.println(F("+ Control, pinExamineNext."));
  }
  // -------------------
  if (pcf20.readButton(pinDeposit) == 0) {
    if (!switchDeposit) {
      switchDeposit = true;
    }
  } else if (switchDeposit) {
    switchDeposit = false;
    // Switch logic.
    Serial.println(F("+ Control, pinDeposit."));
  }
  // -------------------
  if (pcf20.readButton(pinDepositNext) == 0) {
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
  if (pcf23.readButton(pinAux1up) == 0) {
    if (!switchAux1up) {
      switchAux1up = true;
    }
  } else if (switchAux1up) {
    switchAux1up = false;
    // Switch logic.
    Serial.println(F("+ Control, pinAux1up."));
  }
  // -------------------
  if (pcf23.readButton(pinAux1down) == 0) {
    if (!switchAux1down) {
      switchAux1down = true;
    }
  } else if (switchAux1down) {
    switchAux1down = false;
    // Switch logic.
    Serial.println(F("+ Control, pinAux1down."));
  }
  // -------------------
  if (pcf23.readButton(pinAux2up) == 0) {
    if (!switchAux2up) {
      switchAux2up = true;
    }
  } else if (switchAux2up) {
    switchAux2up = false;
    // Switch logic.
    Serial.println(F("+ Control, pinAux2up."));
  }
  // -------------------
  if (pcf23.readButton(pinAux2down) == 0) {
    if (!switchAux1down) {
      switchAux1down = true;
    }
  } else if (switchAux1down) {
    switchAux1down = false;
    // Switch logic.
    Serial.println(F("+ Control, pinAux2down."));
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

  // ------------------------------
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
void loop() {

  if (pcf20interrupted) {
    // ----------------------
    Serial.println("+ Interrupt call, switchSetOn is true.");
    dataByte = pcf20.read8();                   // Read all PCF8574 inputs
    Serial.print("+ PCF8574 0x20 byte, read8      = ");
    printByte(dataByte);
    Serial.println("");
    // ----------------------
    Serial.print("+ PCF8574 0x20 byte, readButton = ");
    for (int pinGet = 7; pinGet >= 0; pinGet--) {
      int pinValue = pcf20.readButton(pinGet);  // Read each PCF8574 input
      Serial.print(pinValue);
    }
    // ----------------------
    Serial.println("");
    // ----------------------
    checkControlButtons();
    pcf20interrupted = false; // Reset for next interrupt.
  }

  delay (60);
}
// -----------------------------------------------------------------------------
