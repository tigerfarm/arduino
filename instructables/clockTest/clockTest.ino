// -----------------------------------------------------------------------------
/*
  Test DS3231 clock module.

  + Post clock data messages to the serial port, which can be displayed using the Arduino Tools/Serial Monitor.

  ------------------------------------------------------------------------------
  DS3231 Clock Library:
    Filter your search by typing ‘rtclib’.
    There should be a couple entries. Look for RTClib by Adafruit.
  https://github.com/adafruit/RTClib
*/
// -----------------------------------------------------------------------------
// For the clock module.

#include "RTClib.h"
RTC_DS3231 rtc;
DateTime now;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void printClockData() {
  DateTime now = rtc.now();
  Serial.println("----------------------------------------");
  Serial.print("+ Current Date & Time: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  // Initialize the Real Time Clock (RTC).
  if (!rtc.begin()) {
    Serial.println("--- Error: RTC not found.");
    while (1);
  }
  //
  // Set the time for testing. Example, test for testing AM/PM.
  // rtc.adjust(DateTime(2019, 10, 22, 23, 59, 56)); // DateTime(year, month, day, hour, minute, second)
  // delay(100);
  //
  Serial.println("+ Clock set.");

  Serial.println("++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

void loop() {
  delay(1000);
  printClockData();
}
// -----------------------------------------------------------------------------
