// -----------------------------------------------------------------------------
/*
    Test DS3231 clock module.

    + Post messages to the serial port, which can be displayed using the Arduino Tools/Serial Monitor.

  Connect infrared receiver, pins from top left to right:
    Left most (next to the X) - Nano pin 9
    Center - 5V
    Right  - ground

    9 + -   - Nano pin connections
    | | |   - Infrared receiver pins
  ---------
  |S      |
  |       |
  |  ---  |
  |  | |  |
  |  ---  |
  |       |
  ---------

  ------------------------------------------------------------------------------
  DS3231 Clock Library:
    Filter your search by typing ‘rtclib’.
    There should be a couple entries. Look for RTClib by Adafruit.
  https://github.com/adafruit/RTClib
*/
// -----------------------------------------------------------------------
// For the infrared receiver.
#include <IRremote.h>
int IR_PIN = A1;
IRrecv irrecv(IR_PIN);
decode_results results;

void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");


  Serial.println("++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

int counter = 0;
boolean ledOn = false;
void loop() {
  delay(1000);
  Serial.print("+ Loop counter = ");
  Serial.println(counter);
}
// -----------------------------------------------------------------------------
