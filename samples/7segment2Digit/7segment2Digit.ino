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


  Code base from:
    https://www.tinkerhobby.com/arduino-2-digit-7-segment-display-counter/
    and
    http://www.circuitbasics.com/arduino-7-segment-display-tutorial/
    and
    http://www.arduinotutorialonline.com/2018/08/2-digit-7-segment-0-99-counter-with.html
  To install library,
    from the Arduino IDE: Sketch/include Library/Add .ZIP Library,
    then select the downloaded SevSeg ZIP file.
    http://www.circuitbasics.com/wp-content/uploads/2017/05/SevSeg.zip
*/
// -----------------------------------------------------------------------------

#include <SevSeg.h>
SevSeg sevseg;

// -----------------------------------------------------------------------------
int counter = 0;

void setup ()  {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  byte hardwareConfig = COMMON_CATHODE;   // COMMON_ANODE or COMMON_CATHODE
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};
  byte numDigits = 2;                 // Number of display digits.
  byte digitPins[] = {2, 3};          // Multi-digit display ground pins.
  bool resistorsOnSegments = true;    // Set to true when using multi-digit displays.
  bool updateWithDelays = false;      // Doesn't work when true.
  bool leadingZeros = true;
  bool disableDecPoint = false; // Use 'true' if your decimal point doesn't exist or isn't connected

  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);
  //sevseg.setBrightness(100);           // Actually, it's refresh rate. Value from 0 to 100.

  // Display a value.
  // sevseg.setNumber(counter);
  // sevseg.refreshDisplay();
  // delay(1000);

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
void loop ()  {

  int showValue = 36;
  
  // delay(60);
  static unsigned long timer = millis();
  static int deciSeconds = 0;
  if (millis() - timer >= 1000) {
    timer += 1000;
    deciSeconds++; // 100 milliSeconds is equal to 1 deciSecond
    if (deciSeconds == 100) { // Reset to 0 after counting to the max.
      deciSeconds = 0;
    }
    sevseg.setNumber(deciSeconds, 1);
    if (deciSeconds == 3) {
      sevseg.setNumber(showValue, 1);
    }
  }
  sevseg.refreshDisplay();

  /*
    delay(1000);
    counter ++;
    if (counter > 99) {
    counter = 0;
    }
    Serial.println(counter);
    sevseg.setNumber(counter);
    sevseg.refreshDisplay();
  */
}
// -----------------------------------------------------------------------------
