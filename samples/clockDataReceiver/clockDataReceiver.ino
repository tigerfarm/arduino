// -----------------------------------------------------------------------------
/*
  Connect both 2 x 7 segment common cathode displays to each Nano.
  Needs to match: segmentPins and digitPins as defined below.
  + Segment CA1 to 1K resister, to Nano pin 12.
  + Segment CA2 to 1K resister, to Nano pin  4.
  + Segment A to Nano pin 9.
  + Segment B to Nano pin 8.
  + Segment C to Nano pin 5.
  + Segment D to Nano pin 7.
  + Segment E to Nano pin 6.
  + Segment F to Nano pin 10.
  + Segment G to Nano pin 11.

  Segment pins mapped to Nano pins:
       Rs-4 12 11 6  7    -> Nano pins
       CA1  G  F  A  B    -> Segments they control
        |   |  |  |  |
   ---------    ---------
   |   A   |    |   A   |
  F|       |B  F|       |B
   |---G---|    |---G---|
  E|       |C  E|       |C
   |   D   |    |   D   |
   ---------    ---------
        |   |  |  |  |
        D   DP E  C CA2   -> Segments they control
        9      10 8 Rs-5  -> Nano pins

  Communications programs that don't use IC2.
    https://github.com/beneater/error-detection-videos

*/
// -----------------------------------------------------------------------------
// For digit displays
#include <SevSeg.h>
SevSeg sevseg;

// -----------------------------------------------------------------------------
// Nano to Nano (N2N) Communications: Receiver.
// Reference:
//  https://www.youtube.com/watch?v=eq5YpKHXJDM
//  https://github.com/beneater/error-detection-videos
//  https://github.com/beneater/error-detection-videos/blob/master/receiver/receiver.ino

#define TX_CLOCK 2
#define TX_DATA 3

char message[16];
volatile byte rx_byte = 0;
volatile int bit_position = 0;
volatile bool bitReceived = false;

void onClockPulse() {
  // Read a bit and add it to the byte.
  bool rx_bit = digitalRead(RX_DATA);
  if (bit_position == 8) {
    rx_byte = 0;
    bit_position = 0;
  }
  if (rx_bit) {
    rx_byte |= (0x80 >> bit_position);
  }
  bit_position += 1;
  if (bit_position == 8) {
    strncat(message, (const char *)&rx_byte, 1);
  }
  bitReceived = true;
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

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
  sevseg.setNumber(42, 1);

  // Prep to receive a message.
  strcpy(message, "");
  pinMode(RX_DATA, INPUT);
  attachInterrupt(digitalPinToInterrupt(RX_CLOCK), onClockPulse, RISING);
  Serial.println("+ Ready to receive communications from the other Nano.");

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
static unsigned long timer = millis();
static int counter = 0;
void loop() {

  // Each second, set the display value.
  if (millis() - timer >= 1000) {
    timer += 1000;
    counter++;
    if (counter > 59) {
      // Reset to 0 after counting to the max.
      counter = 0;
    }
    // sevseg.setNumber(counter, 1);
  }

  if (bitReceived) {
    for (int i = 0; i < 8; i += 1) {
      if (i < bit_position) {
        // lcd.print(rx_byte & (0x80 >> i) ? "1" : "0");
      } else {
        // lcd.print(" ");
      }
    }
    Serial.print("+ bit :");
    Serial.println(message);
    Serial.println(":");
    //
    bitReceived = false;
  }

  // One digit is refreshed on one cylce, the other digit is refreshed on the next cyle.
  // If using delay(1000), one digit displays on one cycle, then the next digit displays on the next cycle.
  // The refresh (refreshDisplay) needs to be fast enough, that they look like they are always on.
  // Delay of 10 is okay. Any longer delay, example 20, the digits start to flash.
  delay(10);
  sevseg.refreshDisplay();

}
// -----------------------------------------------------------------------------
