// -----------------------------------------------------------------------------
/*
  Merge RotaryEncoder.ino with 7segment2Digit.ino.

  Connect a KY-040 rotary encoder to a Nano:
  + "+" to Nano 5v, note, also works with 3.3v, example: NodeMCU.
  + GND to Nano ground.
  + CLK (clock) to Nano pin 2, the interrupt pin. Also referred to as output A.
  + SW (switch) to Nano pin 3. Optional, can be used to reset the counter to zero.
  + DT (data) to Nano pin 4. Also referred to as output B. Can be used to reset the counter value.

  
*/
// -----------------------------------------------------------------------------

// Rotary Encoder module connections
const int PinCLK = 2; // Generating interrupts using CLK signal
const int PinDT = 4;  // Reading DT signal

// -----------------------------------------------------------------------------
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
    } else {
      virtualPosition--;
      Serial.print (" > left  count = ");
      Serial.println (virtualPosition);
    }
    delay(10);
  }

  // -----------------------------------------------------------------------------
}
