// -----------------------------------------------------------------------------
/*
  Nano to Nano (N2N) Communications: Receiver.
  
  Connect Nanos together for communications:
  + 5v: positive
  + GND: ground
  + pins 2: clock
  + pins 3: data

  Connect a 2 x 7 segment display to a Nano.
  Needs to match: segmentPins and digitPins as defined below.
  + Segment CA1 to 1K resister, to Nano pin 4. If common cathode display (-).
  + Segment CA2 to 1K resister, to Nano pin 5. If common cathode display (-).
  + Segment A to Nano pin 6.
  + Segment B to Nano pin 7.
  + Segment C to Nano pin 8.
  + Segment D to Nano pin 9.
  + Segment E to Nano pin 10.
  + Segment F to Nano pin 11.
  + Segment G to Nano pin 12.

  2 x 7 segment display pins:
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
// Used with the 2 x 7 segment display
#include <SevSeg.h>
SevSeg sevseg;

// -----------------------------------------------------------------------------
// Nano to Nano (N2N) Communications: transmission sender
#define TX_CLOCK 4
#define TX_DATA A1

// Rate notes:
//  300 nice to watch the bits show.
//   10 fast for transfer.
//    1 works fine, even with serial print statements.
#define TX_RATE 10
int clockDelay = (TX_RATE);
// int clockDelay = (1000 / TX_RATE) / 2;  // original, where TX_RATE = 5.

const char *message = "TX";

void sendBit2nano(bool tx_bit) {
  Serial.print(tx_bit);
  // Set/write the data bit to transmit: either HIGH (1) or LOW (0) value.
  digitalWrite(TX_DATA, tx_bit);
  // Pulse the clock to transit the bit.
  delay(clockDelay);
  digitalWrite(TX_CLOCK, HIGH);
  delay(clockDelay);
  digitalWrite(TX_CLOCK, LOW);
}
void sendByte2nano(char tx_byte) {
  Serial.print("+ Send byte:");
  Serial.print(tx_byte, DEC);   // Note, BIN prints the binary value, example: DEC:12: BIN: 1100.
  Serial.print(": bits: ");
  for (int bit_idx = 0; bit_idx < 8; bit_idx++) {
    // Transmit each bit of the byte.
    // Get the bit to transmit, and transmit it.
    bool tx_bit = tx_byte & (0x80 >> bit_idx);
    sendBit2nano(tx_bit);
  }
  Serial.println(".");
  //
  // Set data bit to LOW (0).
  digitalWrite(TX_DATA, LOW);
}
// Nano to Nano (N2N) Communications: sender.
void sendMessage2nano() {
  // Transmit each bit in the byte.
  for (int byte_idx = 0; byte_idx < strlen(message); byte_idx++) {
    // Get each byte of the message, and transmit it.
    char tx_byte = message[byte_idx];
    sendByte2nano(tx_byte);
  }
}

// -----------------------------------------------------------------------------
// Nano to Nano (N2N) Communications: Receiver.

#define RX_CLOCK 2
#define RX_DATA A0

volatile byte rx_bit = 0;
volatile byte rx_byte = 0;
volatile int bit_position = 0;
volatile bool bitReceived = false;
volatile bool byteReceived = false;
volatile byte messageByte = 0;

void onClockPulse() {
  bitReceived = true;
  // Read a bit.
  rx_bit = digitalRead(RX_DATA);
  // Add bit to byte.
  if (rx_bit) {
    rx_byte |= (0x80 >> bit_position);
  }
  //  Serial.print("+");
  //  Serial.print(" bit_position: ");
  //  Serial.print(bit_position);
  //  Serial.print(" bit: ");
  //  Serial.println(rx_bit);
  bit_position++;
  if (bit_position == 8) {
    // 8 bits is a byte.
    byteReceived = true;
    messageByte = rx_byte;
    bit_position = 0;
    rx_byte = 0;
  }
}

// -----------------------------------------------------------------------------
// Rotary Encoder module connections
const int PinCLK = 3; // Generating interrupts using CLK signal
const int PinDT = A6;  // Reading DT signal

// Interrupt routine runs if rotary encoder CLK pin changes state.
volatile boolean TurnDetected;  // Type volatile for interrupts.
volatile boolean turnRight;
void rotarydetect ()  {
  // Set direction for TurnDetected: turnRight or left (!turnRight).
  TurnDetected = false;
  if (digitalRead(PinDT) == 1) {
    TurnDetected = true;
    turnRight = false;
    if (digitalRead(PinCLK) == 0) {
      turnRight = true;
    }
  }
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  byte hardwareConfig = COMMON_CATHODE; // COMMON_ANODE or COMMON_CATHODE
  byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12};  // Mapping segment pins A..G, to Nano pins.
  byte numDigits = 2;                 // Number of display digits.
  byte digitPins[] = {5, 13};          // Multi-digit display ground/set pins: can use pin 13.
  bool resistorsOnSegments = true;    // Set to true when using a single resister per display digit.
  bool updateWithDelays = false;      // Doesn't work when true.
  bool leadingZeros = true;           // Clock leading 0. When true: "01" rather that " 1".
  bool disableDecPoint = true;        // Use 'true' if your decimal point doesn't exist or isn't connected
  sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);
  Serial.println("+ Digit display configured.");
  sevseg.setNumber(42, 1);

  pinMode(TX_CLOCK, OUTPUT);
  pinMode(TX_DATA, OUTPUT);
  Serial.println("+ Configured: Nano to Nano (N2N) Communications for sending.");

  pinMode(RX_DATA, INPUT);
  attachInterrupt(digitalPinToInterrupt(RX_CLOCK), onClockPulse, RISING);
  Serial.println("+ Ready to receive communications from the other Nano.");

   attachInterrupt (digitalPinToInterrupt(PinCLK), rotarydetect, CHANGE);
  Serial.println("+ Configured: rotary encoder.");

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

static int virtualPosition = 0;
void loop() {

  if (byteReceived) {
    Serial.print("+");
    Serial.print(" messageByte: ");
    Serial.println(messageByte);
    if (messageByte < 13) {
      sevseg.setNumber(messageByte, 1);
    }
    byteReceived = false;
  }

  if (TurnDetected)  {
    TurnDetected = false;  // Reset, until new rotation detected
    if (turnRight) {
      virtualPosition++;
      if (virtualPosition > 12) {
        virtualPosition = 1;
      }
      Serial.print (" > right count = ");
      Serial.println (virtualPosition);
    } else {
      virtualPosition--;
      if (virtualPosition < 0) {
        virtualPosition = 12;
      }
      Serial.print (" > left  count = ");
      Serial.println (virtualPosition);
    }
    // sendMessage2nano();
    sendByte2nano(virtualPosition);
  }

  // Delay of 10 is okay. Any longer delay, example 20, the digits start to flash.
  delay(10);
  sevseg.refreshDisplay();

}
// -----------------------------------------------------------------------------
