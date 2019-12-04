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

  Basic code sample:
    https://protosupplies.com/product/pcf8574-i2c-i-o-expansion-module/
  Input sample with a keypad:
    https://www.instructables.com/id/Arduino-Nano-I2C-Matrix-Keypad-With-PCF8574PCF8574/
    https://www.bastelgarage.ch/index.php?route=extension/d_blog_module/post&post_id=8
*/

// -----------------------------------------------------------------------------
#include "Arduino.h"
#include "PCF8574.h"


// Interrupt setup: pin to use, I2C address, interrupt handler routine)
const int INTERRUPT_PIN = 2;
int PCF_INTERRUPT_ADDRESS = 0x020;
void PCF8574_Interrupt();
PCF8574 pcf8574(PCF_INTERRUPT_ADDRESS, INTERRUPT_PIN, pcf01interrupt);
bool keyPress = false;  // Button press flag.
void pcf01interrupt() {
  keyPress = true;
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  // ------------------------------
  // I2C Two Wire initialization

  pinMode(INTERRUPT_PIN, INPUT_PULLUP); // Enable pullup on interrupt pin of Uno

  pcf8574.pinMode(P0, INPUT);           // Set all pins as inputs on PCF8574
  pcf8574.pinMode(P1, INPUT);
  pcf8574.pinMode(P2, INPUT);
  pcf8574.pinMode(P3, INPUT);
  pcf8574.pinMode(P4, INPUT);
  pcf8574.pinMode(P5, INPUT);
  pcf8574.pinMode(P6, INPUT);
  pcf8574.pinMode(P7, INPUT);

  pcf8574.digitalWrite(P0, HIGH);     // Enable weak internal pullups of the PCF8574
  pcf8574.digitalWrite(P1, HIGH);     // To pull the inputs HIGH
  pcf8574.digitalWrite(P2, HIGH);
  pcf8574.digitalWrite(P3, HIGH);
  pcf8574.digitalWrite(P4, HIGH);
  pcf8574.digitalWrite(P5, HIGH);
  pcf8574.digitalWrite(P6, HIGH);
  pcf8574.digitalWrite(P7, HIGH);

  pcf8574.begin();

  Serial.println("+ PCF module initialized.");

  // ------------------------------
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
void loop() {
  Serial.println("+ Loop.");
  if (keyPress) {
    for (int i = 0; i < 8; i++) {
      int val = pcf8574.digitalRead(i);  // Read each PCF8574 input
      if (val == 0) {                    // If LOW, button was pushed
        Serial.print("+ Button pressed, pin: ");
        Serial.print(i);
      }
    }
    delay(250);           // Handle switch debounce.
    keyPress = false;
  }
  delay (60);
}
// -----------------------------------------------------------------------------
