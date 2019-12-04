// -----------------------------------------------------------------------------
/*
  PCF8574 I2C Module

  Nice wiring sample:
    https://www.youtube.com/watch?v=mXMkgQf3fqU

 Sketch for 2 chips.
    https://github.com/GadgetReboot/Arduino/blob/master/Uno/PCF8574/PCF8574.ino

*/

// -----------------------------------------------------------------------------
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
