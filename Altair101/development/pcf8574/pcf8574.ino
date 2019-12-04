// -----------------------------------------------------------------------------
/*
  PCF8574 I2C Module

  I2C to 8-bit Parallel-Port Expander

  Pin addressing:
  A0 A1 A2
   0  0  0 = 0x20
   0  0  1 = 0x21
   0  1  0 = 0x22
    ...
   1  1  1 = 0x27


  Nice wiring sample:
    https://www.youtube.com/watch?v=mXMkgQf3fqU

 Sketch for 2 chips.
    https://github.com/GadgetReboot/Arduino/blob/master/Uno/PCF8574/PCF8574.ino

 Input sample with a keypad:
    https://www.instructables.com/id/Arduino-Nano-I2C-Matrix-Keypad-With-PCF8574PCF8574/
    https://www.bastelgarage.ch/index.php?route=extension/d_blog_module/post&post_id=8
*/

// -----------------------------------------------------------------------------
#include<Wire.h>

void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  // ------------------------------
  // I2C Two Wire initialization

  Wire.begin();
  Serial.println("+ Turn OFF all pins by sending a high byte (1 bit per byte).");
  Wire.beginTransmission(0x027);
  Wire.write(B11111111);
  Wire.endTransmission();

  Wire.begin();
  Serial.println("+ Turn ON all pins by sending a high byte (1 bit per byte).");
  Wire.beginTransmission(0x027);
  Wire.write(B00000000);
  Wire.endTransmission();

  Serial.println("+ PCF module initialized.");

  // ------------------------------
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
void loop() {
  Serial.println("+ Loop.");

  // ON: setting bit LOW (to zero)
  // OFF: setting bit HIGH (to one)

  Wire.beginTransmission(0x027);
  Wire.write(B01010101);
  Wire.endTransmission();
  delay (500);

  Wire.beginTransmission(0x027);
  Wire.write(B01010101);
  Wire.endTransmission();
  delay (500);
}
// -----------------------------------------------------------------------------