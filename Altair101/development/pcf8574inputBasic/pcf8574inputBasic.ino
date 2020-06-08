// -----------------------------------------------------------------------------
/*
  PCF8574 I2C Module

  I2C to 8-bit Parallel-Port Expander

  Module with adjustable pin address settings:
   --------------
  |              | 0 is up   (-)
  |     A2 A1 A0 | 1 is down (+), toward the pins at the bottom.
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
  | INT          |
  |  V G S S     |
  |  C N D C     | For multiple, I used a diode to each.
  |  C D A L     |
   --------------
     | | | |
     
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
  Information and code sample:
    https://protosupplies.com/product/pcf8574-i2c-i-o-expansion-module/

  Example statements:
    uint8_t value = pcfModule.read8();
    Serial.println(pcfModule.read8(), BIN);
*/
// -----------------------------------------------------------------------------
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

PCF8574 pcfControl(0x020);  // Control: STOP, RUN, SINGLE STEP, EXAMINE, EXAMINE NEXT, DEPOSIT, DEPOSIT NEXT, REST
PCF8574 pcfData(0x021);     // Low bytes, data byte
PCF8574 pcfSense(0x022);    // High bytes, sense switch byte
PCF8574 pcfAux(0x023);      // AUX switches and others: Step down, CLR, Protect, Unprotect, AUX1 up, AUX1 down,  AUX2 up, AUX2 down

PCF8574 pcfModule(0x023);

// Interrupt setup: interrupt pin to use, interrupt handler routine.
const int INTERRUPT_PIN = 2;
boolean switchSetOn = false;
void pcfinterrupt() {
  switchSetOn = true;
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
  pcfModule.begin();
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), pcfinterrupt, CHANGE);
  Serial.println("+ PCF module initialized.");

  // ------------------------------
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
void loop() {

  if (switchSetOn) {
    // ----------------------
    Serial.println("+ Interrupt call, switchSetOn is true.");
    dataByte = pcfModule.read8();                   // Read all PCF8574 inputs
    Serial.print("+ PCF8574 0x20 byte, read8      = ");
    printByte(dataByte);
    Serial.println("");
    // ----------------------
    Serial.print("+ PCF8574 0x20 byte, readButton = ");
    for (int pinGet = 7; pinGet >= 0; pinGet--) {
      int pinValue = pcfModule.readButton(pinGet);  // Read each PCF8574 input
      Serial.print(pinValue);
    }
    // ----------------------
    Serial.println("");
    switchSetOn = false;                        // Reset for the next switch event.
  }

  delay (60);
}
// -----------------------------------------------------------------------------
