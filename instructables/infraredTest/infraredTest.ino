// -----------------------------------------------------------------------------
/*
    Test DS3231 clock module.

    + Post messages to the serial port, which can be displayed using the Arduino Tools/Serial Monitor.

  ------------------------------------------------------------------------------
  DS3231 Clock Library:
    Filter your search by typing ‘rtclib’.
    There should be a couple entries. Look for RTClib by Adafruit.
  https://github.com/adafruit/RTClib
*/
// -----------------------------------------------------------------------------
// For the clock board.
#include "RTClib.h"
RTC_DS3231 rtc;
DateTime now;

void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);    // Off
  Serial.println("+ Initialized the on board LED digital pin for output. LED is off.");

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
  if (ledOn) {
    digitalWrite(LED_PIN, LOW);   // Off
  } else {
    digitalWrite(LED_PIN, HIGH);    // On
  }
}
// -----------------------------------------------------------------------------
