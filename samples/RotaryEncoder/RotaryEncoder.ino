// -----------------------------------------------------------------------------
/*
  Connect a KY-040 rotary encoder to a Nano:
  + + to Nano 5v, note, also works with 3.3v, example: NodeMCU.
  + GND to Nano ground.
  + CLK (clock) to Nano pin 2, the interrupt pin. Also referred to as output A.
  + SW (switch) to Nano pin 3. Optional, can be used to reset the counter to zero.
  + DT (data) to Nano pin 4. Also referred to as output B.

  Code base from:
    https://www.brainy-bits.com/arduino-rotary-encoder-ky-040/
*/
// -----------------------------------------------------------------------------

volatile boolean TurnDetected;  // Type volatile for interrupts.

// Rotary Encoder Module connections
const int PinCLK=2;   // Generating interrupts using CLK signal
const int PinDT=4;    // Reading DT signal

// Interrupt routine runs if CLK pin changes state
void rotarydetect ()  {
  TurnDetected = true;
}

void setup ()  {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  attachInterrupt (0,rotarydetect,CHANGE); // interrupt 0 always connected to pin 2 on Arduino

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

void loop ()  {
  delay(10);
  if (TurnDetected)  {
      // rotary has moved
      TurnDetected = false;  // do NOT repeat IF loop until new rotation detected 
      Serial.print("CLK Pin: ");
      Serial.println(digitalRead(PinCLK));
      Serial.print("DT Pin: ");
      Serial.println(digitalRead(PinDT));
      delay(5);
  }

  // -----------------------------------------------------------------------------
}
