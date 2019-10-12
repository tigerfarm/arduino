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

byte x = 0;
void I2CsendValue (int sendValue)  {
  Serial.print("+ Send value: ");
  Serial.println(sendValue);
  x = (byte) sendValue;
  Wire.beginTransmission(8);    // transmit to device #8
  //          12345
  Wire.write("x is ");          // sends five bytes
  Wire.write(x);                // sends one byte.
  Wire.endTransmission();       // stop transmitting
  x++;
}
  
// -----------------------------------------------------------------------------
// Rotary Encoder module connections
const int PinCLK = 2; // Generating interrupts using CLK signal
const int PinDT = 3;  // Reading DT signal

// Interrupt routine runs if rotary encoder CLK pin changes state.

volatile boolean TurnDetected;  // Type volatile for interrupts.
volatile boolean turnRight;

void rotarydetect ()  {
  // Set TurnDetected, and in which direction: turnRight or left (!turnRight).
  TurnDetected = false;
  if (digitalRead(PinDT) == 1) {
    TurnDetected = true;
    turnRight = false;
    if (digitalRead(PinCLK) == 0) {
      turnRight = true;
    }
  }
}

void setup ()  {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  attachInterrupt (0, rotarydetect, CHANGE); // Interrupt 0 is pin 2 on Arduino.
  Serial.println("+ Rotary Encoder setup.");

  Wire.begin(); // Note, address optional for master.
  Serial.println("+ Joined I2C bus.");

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

void loop ()  {
  static long virtualPosition = 0;  // static is required to count correctly.

  delay(20);

  if (TurnDetected)  {
    TurnDetected = false;  // Reset, until new rotation detected
    if (turnRight) {
      virtualPosition++;
      Serial.print (" > right count = ");
      Serial.println (virtualPosition);
      I2CsendValue(virtualPosition);
    } else {
      virtualPosition--;
      Serial.print (" > left  count = ");
      Serial.println (virtualPosition);
    }
    delay(10);
  }

  // -----------------------------------------------------------------------------
}
