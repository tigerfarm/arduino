// -----------------------------------------------------------------------------
/*
  On the Slave, connect to the computer's USB port. This allows serial printing.
  
  Connect Nanos together:
  + 5v: positive
  + GND: ground
  + pins 4: SDA, data
  + pins 5: SCL, clock

  Code base from:
    https://www.arduino.cc/en/Tutorial/MasterWriter
*/
// -----------------------------------------------------------------------------
// I2C Communications

#include <Wire.h> 

void I2CreceiveEvent (int howMany)  {
  Serial.print("+ I2CreceiveEvent: ");
  while (1 < Wire.available()) {    // Loop through all but the last byte.
    char c = Wire.read();           // Receive each byte as a character.
    Serial.print(c);                // Print the character.
  }
  int x = Wire.read();              // receive byte as an integer: 0 to 255.
  Serial.print(x);                  // print the integer
  Serial.println(".");
}

void setup ()  {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  Wire.begin(8);
  Wire.onReceive(I2CreceiveEvent);     // register event function
  Serial.println("+ Joined I2C bus on address: 8.");

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

int counter = 0;
void loop ()  {

  delay(1000);
  counter ++;
  Serial.print("+ counter: ");
  Serial.println(counter);

  // -----------------------------------------------------------------------------
}
