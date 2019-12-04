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
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  // ------------------------------
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
void loop() {
  Serial.println("+ Loop.");

    byte error;
    byte address;
    int nDevices;
     
    Serial.println("Scanning...");
    nDevices = 0;
    for (address = 1; address < 127; address++ ) {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0) {
          Serial.print("+ I2C device found at address 0x");
          if (address<16)
            Serial.print("0");
          Serial.print(address,HEX);
          Serial.println("  !");
          nDevices++;
        } else if (error==4) {
          Serial.print("Unknown error at address 0x");
          if (address<16)
            Serial.print("0");
          Serial.println(address,HEX);
        }    
    }
    if (nDevices == 0)
        Serial.println("+ No I2C devices found\n");
    else
        Serial.println("+ Scan completed.");
     
    delay(5000);
}
// -----------------------------------------------------------------------------
