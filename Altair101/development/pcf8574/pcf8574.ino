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
  + P0 ... O7 to (+) side of LED. (-) side of LED to resister, to ground.

  Nice wiring sample:
    https://www.youtube.com/watch?v=mXMkgQf3fqU
  Nice information page:
    https://www.instructables.com/id/PCF8574-GPIO-Extender-With-Arduino-and-NodeMCU/

 Sketch for 2 chips.
    https://github.com/GadgetReboot/Arduino/blob/master/Uno/PCF8574/PCF8574.ino

 Input sample with a keypad:
    https://www.instructables.com/id/Arduino-Nano-I2C-Matrix-Keypad-With-PCF8574PCF8574/
    https://www.bastelgarage.ch/index.php?route=extension/d_blog_module/post&post_id=8
*/

// -----------------------------------------------------------------------------
#include<Wire.h>

int pcf01 = 0x020;

void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  // ------------------------------
  // I2C Two Wire initialization

  Wire.begin();
  
  Serial.println("+ Turn all pins ON by sending a HIGH byte (1 bit per byte).");
  Wire.beginTransmission(pcf01);
  Wire.write(B11111111);
  Wire.endTransmission();
  delay (10000);
  
  Serial.println("+ Turn all pins OFF by sending a LOW byte (0 bit per byte).");
  Wire.beginTransmission(pcf01);
  Wire.write(B00000000);
  Wire.endTransmission();
  delay (10000);

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

  Wire.beginTransmission(pcf01);
  Wire.write(B01010101);
  Wire.endTransmission();
  delay (1000);

  Wire.beginTransmission(pcf01);
  Wire.write(B10101010);
  Wire.endTransmission();
  delay (1000);
}
// -----------------------------------------------------------------------------
