// -----------------------------------------------------------------------------
/*
  Use Rotary Encoder to count from 0-59 using a 2 digit, 7 segment, display.

  Connect Nanos together for IC2 communications:
  + 5v: positive
  + GND: ground
  + pins 4: SDA, data
  + pins 5: SCL, clock

  // -----------------------------------------------------------------------------
  Connect a KY-040 rotary encoder to a Nano.
  + "+" to Nano 5v, note, also works with 3.3v, example: NodeMCU.
  + GND to Nano ground.
  + CLK (clock) to Nano pin 2, an interrupt pin. Also referred to as output A.
  + DT  (data)  to Nano pin 3. Also referred to as output B (encoder pin B).

  // -----------------------------------------------------------------------------
  Connect a 7 segment display to a Nano.
  Needs to match: segmentPins and digitPins as defined below.
  + Segment CA1 to 1K resister, to Nano pin 4. If common cathode display (-).
  + Segment CA2 to 1K resister, to Nano pin 5. If common cathode display (-).
  + Segment A to Nano pin 6.
  + Segment B to Nano pin 7.
  + Segment C to Nano pin 8.
  + Segment D to Nano pin 9.
  + Segment E to Nano pin 10.
  + Segment F to Nano pin 11.
  + Segment G to Nano pin 12.

  Segment pins mapped to Nano pins:
       Rs-4 12 11 6  7    -> Nano pins
       CA1  G  F  A  B    -> Segment pins and segment values they control
        |   |  |  |  |
   ---------    ---------
   |   A   |    |   A   |
  F|       |B  F|       |B
   |---G---|    |---G---|
  E|       |C  E|       |C
   |   D   |    |   D   |
   ---------    ---------
        |   |  |  |  |
        D   DP E  C CA2   -> Segment pins
        9     10 8 Rs-5   -> Nano pins
*/
// -----------------------------------------------------------------------------
// Include the seven segment library and declare an object using it.
#include <SevSeg.h>
SevSeg sevseg;

#define MIN_VALUE 0
#define MAX_VALUE 59

// -----------------------------------------------------------------------------
// Rotary Encoder module

const int PinCLK = 2; // Generating interrupts using CLK signal
const int PinDT = 3;  // Reading DT signal

volatile boolean turnDetected;  // Type volatile for interrupts.
volatile boolean turnRight;

// Interrupt routine runs if rotary encoder CLK pin changes state.
void rotarydetect ()  {
  // Set turnDetected, and in which direction: turnRight or left (!turnRight).
  turnDetected = false;
  if (digitalRead(PinDT) == 1) {
    turnDetected = true;
    turnRight = false;
    if (digitalRead(PinCLK) == 0) {
      turnRight = true;
    }
  }
}

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
  sevseg.setNumber(x, 1);
}

// -----------------------------------------------------------------------------
void setup ()  {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  // Setup: rotary encoder interupt on Arduino pin 2.
  attachInterrupt (digitalPinToInterrupt(2), rotarydetect, CHANGE);

  // Set up the libraries values that are used to write to the segment digits.
  byte hardwareConfig = COMMON_CATHODE; // COMMON_ANODE or COMMON_CATHODE
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};  // Mapping segment pins A..G, to Nano pins.
  byte numDigits = 2;                 // Number of display digits.
  byte digitPins[] = {4, 5};          // Multi-digit display ground/set pins.
  bool resistorsOnSegments = true;    // Set to true when using a single resister per display digit.
  bool updateWithDelays = false;      // Doesn't work when true.
  bool leadingZeros = true;           // Clock leading 0. When true: "01" rather that " 1".
  bool disableDecPoint = true;        // Use 'true' if your decimal point doesn't exist or isn't connected
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);
  // Display "00" to start.
  sevseg.setNumber(0);
  sevseg.refreshDisplay();

  Wire.begin(8);
  Wire.onReceive(I2CreceiveEvent);     // register event function
  Serial.println("+ Joined I2C bus on address: 8.");

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

void loop ()  {

  // static is required to count correctly.
  static long virtualPosition = 0;
  if (turnDetected)  {
    turnDetected = false;  // Reset, until new rotation detected
    if (turnRight) {
      virtualPosition++;
      Serial.print (" > right count = ");
      Serial.println (virtualPosition);
    } else {
      virtualPosition--;
      Serial.print (" > left  count = ");
      Serial.println (virtualPosition);
    }
    if (virtualPosition > MAX_VALUE) {
      // Reset to 0 after counting to the max.
      virtualPosition = MIN_VALUE;
    } else if (virtualPosition < MIN_VALUE) {
      // Set max, if less than zero.
      virtualPosition = MAX_VALUE;
    }
    sevseg.setNumber(virtualPosition, 1);
    delay(10);
  }

  // Display the digits.
  sevseg.refreshDisplay();
  // At most, use a delay of 10, else the digits being displayed will flash.
  delay(10);

  // -----------------------------------------------------------------------------
}
