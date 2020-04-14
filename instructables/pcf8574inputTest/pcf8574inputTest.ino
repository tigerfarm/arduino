// -----------------------------------------------------------------------------
/*
  PCF8574 I2C Module, I2C to 8-bit Parallel-Port Expander

  Program to test switch controls for PCF8574 module.

  
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
// Nicely formated print of a byte.

// Instruction parameters:
byte dataByte = 0;           // db = Data byte (8 bit)

void printByte(byte b) {
  for (int i = 7; i >= 0; i--)
    Serial.print(bitRead(b, i));
}

// -----------------------------------------------------------------------------
#include <PCF8574.h>
#include <Wire.h>

// -------------------------
// Interrupt handler routine.
//                   Mega pin for control toggle interrupt. Same pin for Nano.
const int INTERRUPT_PIN = 2;

boolean pcfSwitchesinterrupted = false;
void pcfSwitchesinterrupt() {
  pcfSwitchesinterrupted = true;
}

// Test with a diode for having 2 modules use the same Arduino interrupt pin.
// pcfSwitches has interupt enabled.
// Implement: pcfSwitches interupt handling. Likely use the same pin, pin 2.

// Address for the PCF8574 module being tested.

// -------------------------
PCF8574 pcfSwitches(0x020);

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// Front Panel Switches

// -------------------------
// Getting a byte of toggle values.

int toggleDataByte() {
  // Invert byte bits using bitwise not operator: "~";
  // Bitwise "not" operator to invert bits:
  //  int a = 103;  // binary:  0000000001100111
  //  int b = ~a;   // binary:  1111111110011000 = -104
  // byte toggleByte = ~pcfSwitches.read8();
  return ~pcfSwitches.read8();
}

// --------------------------------------------------------
// Front Panel Control Switches, when a program is not running.
// Switches: AUX 1, AUX 1, PROTECT, UNPROTECT, CLR, and down STEP.

const int pcfSwitch0 = 0;
const int pcfSwitch1 = 1;
const int pcfSwitch2 = 2;
const int pcfSwitch3 = 3;
const int pcfSwitch4 = 4;
const int pcfSwitch5 = 5;
const int pcfSwitch6 = 6;
const int pcfSwitch7 = 7;

boolean switchLogic0 = false;
boolean switchLogic1 = false;
boolean switchLogic3 = false;
boolean switchLogic4 = false;

void checkSwitches() {
  // -------------------
  if (pcfSwitches.readButton(pcfSwitch0) == 0) {
    if (!switchLogic0) {
      switchLogic0 = true;
    }
  } else if (switchLogic0) {
    switchLogic0 = false;
    // Switch logic.
    Serial.println(F("+ Control, pcfSwitch0."));
  }
  // -------------------
  if (pcfSwitches.readButton(pcfSwitch1) == 0) {
    if (!switchLogic1) {
      switchLogic1 = true;
    }
  } else if (switchLogic1) {
    switchLogic1 = false;
    // Switch logic.
    Serial.println(F("+ Control, pcfSwitch1."));
  }
  // -------------------
  if (pcfSwitches.readButton(pcfSwitch2) == 0) {
    if (!switchLogic3) {
      switchLogic3 = true;
    }
  } else if (switchLogic3) {
    switchLogic3 = false;
    // Switch logic.
    Serial.println(F("+ Control, pcfSwitch2."));
  }
  // -------------------
  if (pcfSwitches.readButton(pcfSwitch3) == 0) {
    if (!switchLogic1) {
      switchLogic1 = true;
    }
  } else if (switchLogic1) {
    switchLogic1 = false;
    // Switch logic.
    Serial.println(F("+ Control, pcfSwitch3."));
  }
  // -------------------
  if (pcfSwitches.readButton(pcfSwitch4) == 0) {
    Serial.println(F("+ Control, pcfSwitch4."));
  }
  if (pcfSwitches.readButton(pcfSwitch5) == 0) {
    Serial.println(F("+ Control, pcfSwitch5."));
  }
  if (pcfSwitches.readButton(pcfSwitch6) == 0) {
    Serial.println(F("+ Control, pcfSwitch6."));
  }
  if (pcfSwitches.readButton(pcfSwitch7) == 0) {
    Serial.println(F("+ Control, pcfSwitch7."));
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
  pcfSwitches.begin();
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), pcfSwitchesinterrupt, CHANGE);
  Serial.println("+ PCF module initialized.");

  // ------------------------------
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
void loop() {

  if (pcfSwitchesinterrupted) {
    // ----------------------
    Serial.println("+ Interrupt call, switchSetOn is true.");
    dataByte = pcfSwitches.read8();                   // Read all PCF8574 inputs
    Serial.print("+ PCF8574 byte, read8      = ");
    printByte(dataByte);
    Serial.println("");
    // ----------------------
    Serial.print("+ PCF8574 byte, readSwitvh = ");
    for (int pinGet = 7; pinGet >= 0; pinGet--) {
      int pinValue = pcfSwitches.readButton(pinGet);  // Read each PCF8574 input
      Serial.print(pinValue);
    }
    // ----------------------
    Serial.println("");
    // ----------------------
    checkSwitches();
    //
    pcfSwitchesinterrupted = false; // Reset for next interrupt.
  }

  delay (60);
}
// -----------------------------------------------------------------------------
