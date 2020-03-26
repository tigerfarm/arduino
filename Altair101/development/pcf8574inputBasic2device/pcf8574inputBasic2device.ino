// -----------------------------------------------------------------------------
/*
  PCF8574 I2C Module

  I2C to 8-bit Parallel-Port Expander.

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
  + SCL to Nano A5.
  + SDA to Nano A4.
  + GND to Nano GND
  + VCC to Nano 5V
  + P0 ... O7 to switches. Other side of the switch to ground.
  + INT to Nano interrupt pin, pin 2 in this sample program.
  ++ For multiple devices using INT, I used a diode from the PCF to one Nano interrupt pin.

  Library:
    https://github.com/RobTillaart/Arduino/tree/master/libraries/PCF8574
  Sample switch/button program:
    https://github.com/RobTillaart/Arduino/blob/master/libraries/PCF8574/examples/buttonRead/buttonRead.ino
  Reference:
    https://forum.arduino.cc/index.php?topic=204596.msg1506639#msg1506639

  Example statement:
    uint8_t value = pcf20.read8();
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

PCF8574 pcf20(0x020);
PCF8574 pcf21(0x021);

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
  // PCF8574 device initialization
  pcf20.begin();
  pcf21.begin();
  // PCF8574 device Interrupt initialization
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), pcfinterrupt, CHANGE);
  Serial.println("+ PCF8574 modules initialized.");

  // ------------------------------
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
void loop() {

  if (switchSetOn) {
    Serial.println("+ Interrupt call, switchSetOn is true.");
    // ------------------------------
    dataByte = pcf20.read8();                   // Read all PCF8574 #20 inputs
    Serial.print("+ PCF8574 0x20 byte, read8      = ");
    printByte(dataByte);
    Serial.println("");
    Serial.print("+ PCF8574 0x20 byte, readButton = ");
    for (int pinGet = 7; pinGet >= 0; pinGet--) {
      int pinValue = pcf20.readButton(pinGet);  // Read each PCF8574 #20 input pin
      Serial.print(pinValue);
    }
    Serial.println("");
    // ------------------------------
    dataByte = pcf21.read8();                   // Read all PCF8574 #21 inputs
    Serial.print("+ PCF8574 0x21 byte, read8      = ");
    printByte(dataByte);
    Serial.println("");
    Serial.print("+ PCF8574 0x21 byte, readButton = ");
    for (int pinGet = 7; pinGet >= 0; pinGet--) {
      int pinValue = pcf21.readButton(pinGet);  // Read each PCF8574 #21 input pin
      Serial.print(pinValue);
    }
    // ------------------------------
    Serial.println("");
    //
    switchSetOn = false;                        // Reset for the next switch event.
  }

  delay (60);
}
// -----------------------------------------------------------------------------
