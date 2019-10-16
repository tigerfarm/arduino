// -----------------------------------------------------------------------------
/*
  Nano to Nano (N2N) Communications: Receiver.

  Connect Nanos together for communications:
  + 5v: positive
  + GND: ground
  + pins 2: clock
  + pins 3: data

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
       CA1  G  F  A  B    -> Pins mapped to segments
        |   |  |  |  |
   ---------    ---------
   |   A   |    |   A   |
  F|       |B  F|       |B
   |---G---|    |---G---|
  E|       |C  E|       |C
   |   D   |    |   D   |
   ---------    ---------
        |   |  |  |  |
        D   DP E  C CA2   -> Pins mapped to segments

*/
// -----------------------------------------------------------------------------
// For digit displays
#include <SevSeg.h>
SevSeg sevseg;

// -----------------------------------------------------------------------------
// Nano to Nano (N2N) Communications: Receiver.
#define RX_CLOCK 2
#define RX_DATA 3

volatile byte rx_bit = 0;
volatile byte rx_byte = 0;
volatile int bit_position = 0;
volatile bool bitReceived = false;
volatile bool byteReceived = false;
volatile byte messageByte = 0;

void onClockPulse() {
  bitReceived = true;
  // Read a bit and add it to the byte.
  rx_bit = digitalRead(RX_DATA);
  /*
    Serial.print("+");
    Serial.print(" bit_position: ");
    Serial.print(bit_position);
    Serial.print(" bit: ");
    Serial.println(rx_bit);
  */
  // Add bit to byte.
  if (rx_bit) {
    rx_byte |= (0x80 >> bit_position);
  }
  bit_position++;
  if (bit_position == 8) {
    // 8 bits is a byte.
    bit_position = 0;
    byteReceived = true;
    messageByte = rx_byte;
    rx_byte = 0;
  }
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

  pinMode(RX_DATA, INPUT);
  attachInterrupt(digitalPinToInterrupt(RX_CLOCK), onClockPulse, RISING);
  Serial.println("+ Ready to receive communications from the other Nano.");

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

void loop() {

  if (byteReceived) {
    Serial.print("+");
    Serial.print(" messageByte: ");
    Serial.println(messageByte);
    sevseg.setNumber(messageByte, 1);
    byteReceived = false;
  }

  // Delay of 10 is okay. Any longer delay, example 20, the digits start to flash.
  delay(10);
  sevseg.refreshDisplay();

}
// -----------------------------------------------------------------------------
