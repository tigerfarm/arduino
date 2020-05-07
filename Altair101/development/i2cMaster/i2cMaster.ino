// -----------------------------------------------------------------------------
/*
    Scan for I2C devices

    I2C Between Arduinos
    https://www.instructables.com/id/I2C-between-Arduinos/
*/

#include<Wire.h>

// -----------------------------------------------------------------------------
void setup() {
  // Serial.begin(115200);
  Serial.begin(9600);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  // ------------------------------
  Serial.println("Scanning...");
  byte count = 0;
  Wire.begin();
  for (byte i = 8; i < 120; i++) {
    Wire.beginTransmission (i);
    if (Wire.endTransmission () == 0) {
      Serial.print ("+ Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      delay (1);
    }
  }
  Serial.print("+ Scan completed. Number of devices found = ");
  Serial.println(count, DEC);

  // ------------------------------
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
void loop() {
  // Serial.println("+ Loop.");
  delay(5000);
}
// -----------------------------------------------------------------------------
