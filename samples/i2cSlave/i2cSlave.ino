// -----------------------------------------------------------------------------
/*
  Connect a KY-040 rotary encoder to a Nano:
  + "+" to Nano 5v, note, also works with 3.3v, example: NodeMCU.
  + GND to Nano ground.
  + CLK (clock) to Nano pin 2, the interrupt pin. Also referred to as output A (encoder pin A).
  + DT (data) to Nano pin 3. Also referred to as output B (encoder pin B).
  + SW (switch) to Nano pin 4. Not used here. Can be used to reset the counter to zero.

  Code base from:
    https://www.brainy-bits.com/arduino-rotary-encoder-ky-040/
  More info:
    https://forum.arduino.cc/index.php?topic=242356.0
    This link suggests using a capacitor, which I'm not using:
    + Connect a 0,47µ (or 1uF) capacitor from ground to CLK, and other to DT, to handle debouncing).
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
  int x = Wire.read();              // receive byte as an integer
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
