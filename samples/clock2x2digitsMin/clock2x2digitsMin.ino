// -----------------------------------------------------------------------------
/*
  Connect DS3231 Clock, and the LCD display, pins to the Nano:
  + VCC to Nano 5v, note, also works with 3.3v, example: NodeMCU.
  + GND to Nano ground.
  + SDA to Nano D4 (pin 4), same on Uno.
  + SCL to Nano D5 (pin 5), same on Uno.

  Connect each 2 x 7-segment common cathode displays, to each Nano.
  Needs to match digitPins as defined below:
  + Segment CA1 to 1K resister, to Nano pin 12.
  + Segment CA2 to 1K resister, to Nano pin  4.
  Needs to match segmentPins as defined below:
  + Segment A to Nano pin 9.
  + Segment B to Nano pin 8.
  + Segment C to Nano pin 5.
  + Segment D to Nano pin 7.
  + Segment E to Nano pin 6.
  + Segment F to Nano pin 10.
  + Segment G to Nano pin 11.

  7-Segment display pins:
       CA1  G  F  A  B    -> Segment mapping
        |   |  |  |  |
   ---------    ---------
   |   A   |    |   A   |
  F|       |B  F|       |B
   |---G---|    |---G---|
  E|       |C  E|       |C
   |   D   |    |   D   |
   ---------    ---------
        |   |  |  |  |
        D   DP E  C CA2   -> Segment mapping

  Connect Nanos together for communications:
  + 5v: positive
  + GND: ground
  + pins 2: clock
  + pins 3: data
*/
// -----------------------------------------------------------------------------
// For the clock board.
#include "RTClib.h"
RTC_DS3231 rtc;
DateTime now;

// -----------------------------------------------------------------------------
// For 2 x 7-segment digit displays
#include <SevSeg.h>
SevSeg sevseg;

// -----------------------------------------------------------------------------
// Nano to Nano (N2N) Communications
// Reference:
//  https://www.youtube.com/watch?v=sNkERQlK8j8
//  https://github.com/beneater/error-detection-videos
//  https://github.com/beneater/error-detection-videos/blob/master/transmitter/transmitter.ino

#define TX_CLOCK 2
#define TX_DATA 3
#define TX_RATE 5
int clockDelay = (1000 / TX_RATE) / 2;
const char *message = "hi";

// Nano to Nano (N2N) Communications: sender.
void send2nano() {
  // Transmit each bit in the byte.
  for (int byte_idx = 0; byte_idx < strlen(message); byte_idx++) {
    // Get each byte of the message.
    char tx_byte = message[byte_idx];
    for (int bit_idx = 0; bit_idx < 8; bit_idx++) {
      // Transmit each bit of the byte.
      // Get the bit to transmit.
      bool tx_bit = tx_byte & (0x80 >> bit_idx);
      // Set/write the data bit to transmit: either HIGH (1) or LOW (0) value.
      digitalWrite(TX_DATA, tx_bit);
      // Pulse the clock to transit the bit.
      delay(clockDelay);
      digitalWrite(TX_CLOCK, HIGH);
      delay(clockDelay);
      digitalWrite(TX_CLOCK, LOW);
    }
    // Set data bit to LOW (0).
    digitalWrite(TX_DATA, LOW);
  }
}

// -----------------------------------------------------------------------------
// rtc.adjust(DateTime(2019, 10, 9, 16, 22, 3));   // year, month, day, hour, minute, seconds
int theCounterYear = 0;
int theCounterMonth = 0;
int theCounterDay = 0;
int theCounterHours = 0;
int theCounterMinutes = 0;
int theCounterSeconds = 0;

void syncCountWithClock() {
  now = rtc.now();
  theCounterHours = now.hour();
  theCounterMinutes = now.minute();
  theCounterSeconds = now.second();
  //
  Serial.println("+ syncCountWithClock, current time:");
  Serial.print(" theCounterHours=");
  Serial.println(theCounterHours);
  Serial.print(" theCounterMinutes=");
  Serial.println(theCounterMinutes);
  Serial.print(" theCounterSeconds=");
  Serial.println(theCounterSeconds);
}

// -----------------------------------------------------------------------------
void processClockNow() {
  //
  now = rtc.now();
  //
  if (now.second() != theCounterSeconds) {
    // When the clock second value changes, that's a clock second pulse.
    theCounterSeconds = now.second();
    clockPulseSecond();
    if (theCounterSeconds == 0) {
      // When the clock second value changes to zero, that's a clock minute pulse.
      theCounterMinutes = now.minute();
      clockPulseMinute();
      if (theCounterMinutes == 0) {
        // When the clock minute value changes to zero, that's a clock hour pulse.
        theCounterHours = now.hour();
        clockPulseHour();
      }
    }
  }
}

void clockPulseHour() {
  Serial.print("++ clockPulseHour(), theCounterHours= ");
  Serial.println(theCounterHours);
  // I2CsendValue(theCounterHours);
  sevseg.setNumber(theCounterHours);
}
void clockPulseMinute() {
  Serial.print("+ clockPulseMinute(), theCounterMinutes= ");
  Serial.println(theCounterMinutes);
  // sevseg.setNumber(theCounterMinutes);
}
void clockPulseSecond() {
  Serial.print("+ theCounterSeconds = ");
  Serial.println(theCounterSeconds);
  // sevseg.setNumber(theCounterSeconds);
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  // RTC: Real Time Clock
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, need to reset the time.");
    // Set the RTC to the date & time this sketch was compiled,
    //  which is only seconds behind the actual time.
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  syncCountWithClock();
  Serial.println("+ Clock set and synched with program variables.");

  // Display minutes.
  byte hardwareConfig = COMMON_CATHODE; // COMMON_ANODE or COMMON_CATHODE
  byte segmentPins[] = {9, 8, 5, 7, 6, 10, 11, 13};  // Mapping segment pins A..G, to Nano pins.
  byte numDigits = 2;                 // Number of display digits.
  byte digitPins[] = {12, 4};          // Multi-digit display ground/set pins.
  bool resistorsOnSegments = true;    // Set to true when using a single resister per display digit.
  bool updateWithDelays = false;      // Doesn't work when true.
  bool leadingZeros = true;           // Clock leading 0. When true: "01" rather that " 1".
  bool disableDecPoint = true;        // Use 'true' if your decimal point doesn't exist or isn't connected
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);
  Serial.println("+ Digit display configured.");
  clockPulseMinute();

  pinMode(TX_CLOCK, OUTPUT);
  pinMode(TX_DATA, OUTPUT);
  Serial.println("+ Communications with other Nana established.");
  clockPulseHour();

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
static unsigned long timer = millis();
void loop() {

  // Each second, check the clock.
  if (millis() - timer >= 1000) {
    processClockNow();
  }

  // One digit is refreshed on one cylce, the other digit is refreshed on the next cyle.
  // If using delay(1000), one digit displays on one cycle, then the next digit displays on the next cycle.
  // The refresh (refreshDisplay) needs to be fast enough, that they look like they are always on.
  // Delay of 10 is okay. Any longer delay, example 20, the digits start to flash.
  delay(10);
  sevseg.refreshDisplay();

}
// -----------------------------------------------------------------------------
