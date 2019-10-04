// -----------------------------------------------------------------------------
/*
  Connect a 7 segment display to a Nano.
  Needs to match: segmentPins and digitPins as defined below.
  + Segment ds1 to 1K resister, to Nano pin 10. If common cathode display.
  + Segment ds2 to 1K resister, to Nano pin 11. If common cathode display.
  + Segment A to Nano pin 6.
  + Segment B to Nano pin 5.
  + Segment C to Nano pin 2.
  + Segment D to Nano pin 3.
  + Segment E to Nano pin 4.
  + Segment F to Nano pin 7.
  + Segment G to Nano pin 8.

  Segment pins mapped to Nano pins:
      6 5         7 Nano pins
      A B ds2 ds1 F
       ---   ---
      |   | |   |
       ---   ---
      |   | |   |
       ---.  ---.
      C DP E D G
      2    4 3 8    Nano pins

  Code base from:
    http://www.circuitbasics.com/arduino-7-segment-display-tutorial/
    and
    http://www.arduinotutorialonline.com/2018/08/2-digit-7-segment-0-99-counter-with.html
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

char tempString[10]; //Used for sprintf

void setup ()  {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  byte hardwareConfig = COMMON_CATHODE;   // Or COMMON_ANODE
  byte segmentPins[] = {6, 5, 2, 3, 4, 7, 8, 9};
  byte numDigits = 2;                 // Number of display digits.
  byte digitPins[] = {10, 11};        // Multi-digit display ground pins.
  bool resistorsOnSegments = true;    // Set to true when using multi-digit displays.

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments);
  // sevseg.setBrightness(3);           // Does effect my setup: Value from 0 to 100.
  // Display a value.
  sprintf(tempString, "%2d", counter);    // Convert deciSecond into a string that is right adjusted
  myDisplay.DisplayString(tempString, 0); // (numberToDisplay, decimal point location) where 0 is no decimal point.
  // sevseg.setNumber(counter);
  // sevseg.refreshDisplay();
  delay(1000);

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
void loop ()  {

  delay(1000);
  counter ++;
  if (counter > 99) {
    counter = 0;
  }
  sprintf(tempString, "%2d", counter);    // Convert deciSecond into a string that is right adjusted
  myDisplay.DisplayString(tempString, 0); // (numberToDisplay, decimal point location) where 0 is no decimal point.
  // sevseg.setNumber(counter);
  // sevseg.refreshDisplay();

}
// -----------------------------------------------------------------------------
