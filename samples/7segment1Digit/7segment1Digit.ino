// -----------------------------------------------------------------------------
/*
  Connect a 7 segment display to a Nano.
  Needs to match: segmentPins as defined below.
  + Segment + to 1K resister, to Nano 5V. If common anode display.
  + Segment - to 1K resister, to Nano ground. If common cathode display.
  + Segment A to Nano pin 6.
  + Segment B to Nano pin 5.
  + Segment C to Nano pin 2.
  + Segment D to Nano pin 3.
  + Segment E to Nano pin 4.
  + Segment F to Nano pin 7.
  + Segment G to Nano pin 8.
  + Segment DP to Nano pin 9.

  Segment pins for common cathode display (-):
    G F - A B
       ---
      |   |
       ---
      |   |
       --- .
    E D - C DP

  Code base from:
    http://www.circuitbasics.com/arduino-7-segment-display-tutorial/
  To install library,
    from the Arduino IDE: Sketch/include Library/Add .ZIP Library,
    then select the downloaded SevSeg ZIP file.
*/
// -----------------------------------------------------------------------------

#include <SevSeg.h>
// #include "SevSeg.h"
SevSeg sevseg;

// -----------------------------------------------------------------------------
int counter = 0;

void setup ()  {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  byte hardwareConfig = COMMON_CATHODE;   // Or COMMON_ANODE
  byte segmentPins[] = {6, 5, 2, 3, 4, 7, 8, 9};
  byte numDigits = 1;                 // Number of display digits.
  byte digitPins[] = {};              // Multi-digit display ground pins.
  bool resistorsOnSegments = true;    // Set to true when using single resistor for the display.

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  // sevseg.setBrightness(3);           // Does effect my setup: Value from 0 to 100.
  // Display a value.
  sevseg.setNumber(counter);
  sevseg.refreshDisplay();
  delay(1000);

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
void loop ()  {

  delay(1000);
  counter ++;
  if (counter > 9) {
    counter = 0;
  }
  sevseg.setNumber(counter);
  sevseg.refreshDisplay();

}
// -----------------------------------------------------------------------------
