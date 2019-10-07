// -----------------------------------------------------------------------------
/*
  Connect a 7 segment display to a Nano.
  Needs to match: segmentPins and digitPins as defined below.
  + Segment CA1 to 1K resister, to Nano pin 2. If common cathode display (-).
  + Segment CA2 to 1K resister, to Nano pin 3. If common cathode display (-).
  + Segment A to Nano pin 6.
  + Segment B to Nano pin 7.
  + Segment C to Nano pin 8.
  + Segment D to Nano pin 9.
  + Segment E to Nano pin 10.
  + Segment F to Nano pin 11.
  + Segment G to Nano pin 12.

  Segment pins mapped to Nano pins:
       Rs2 12 11 6  7    -> Nano pins
       CA1 G  F  A  B    -> Segments they control
        |  |  |  |  |
   ---------    ---------
   |   A   |    |   A   |
  F|       |B  F|       |B
   |---G---|    |---G---|
  E|       |C  E|       |C
   |   D   |    |   D   |
   ---------    ---------
        |  |  |  |  |
        D  DP E  C CA2   -> Segments they control
        9     10 8 Rs3   -> Nano pins

  To install library, download the zip and use: Sketch/Include Library/Add Zip Library.
    https://github.com/DeanIsMe/SevSeg
*/
// -----------------------------------------------------------------------------

#include <SevSeg.h>
SevSeg sevseg;

// -----------------------------------------------------------------------------
void setup ()  {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  byte hardwareConfig = COMMON_CATHODE; // COMMON_ANODE or COMMON_CATHODE
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};  // Mapping segment pins A..G, to Nano pins.
  byte numDigits = 2;                 // Number of display digits.
  byte digitPins[] = {2, 3};          // Multi-digit display ground/set pins.
  bool resistorsOnSegments = true;    // Set to true when using a single resister per display digit.
  bool updateWithDelays = false;      // Doesn't work when true.
  bool leadingZeros = true;           // Clock leading 0. When true: "01" rather that " 1".
  bool disableDecPoint = true;        // Use 'true' if your decimal point doesn't exist or isn't connected
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);
  // sevseg.setBrightness(10);           // It's a refresh rate value from 0 to 100, but doesn't seem to do anything.

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

static unsigned long timer = millis();
static int counter = 0;
void loop ()  {

  // Each second, set the display value.
  if (millis() - timer >= 1000) {
    timer += 1000;
    counter++;
    if (counter > 99) {
      // Reset to 0 after counting to the max.
      counter = 0;
    }
    sevseg.setNumber(counter, 1);
  }
  
  // One digit is refreshed on one cylce, the other digit is refreshed on the next cyle.
  // If using delay(1000), one digit displays on one cycle, then the next digit displays on the next cycle.
  // The refresh (refreshDisplay) needs to be fast enough, that they look like they are always on.
  // Delay of 10 is okay. Any longer delay, example 20, the digits start to flash.
  delay(10);
  sevseg.refreshDisplay(); 

}
// -----------------------------------------------------------------------------
