// -----------------------------------------------------------------------------
/*
  Nano to Nano (N2N) Communications
  Connect Nanos together for communications:
  + 5v: positive
  + GND: ground
  + pin 2: clock
  + pin 5: data

  Reference:
    https://www.youtube.com/watch?v=eq5YpKHXJDM
    https://github.com/beneater/error-detection-videos
  Sender:
    https://github.com/beneater/error-detection-videos/blob/master/transmitter/transmitter.ino
  Receiver:
    https://github.com/beneater/error-detection-videos/blob/master/receiver/receiver.ino

  Connect a KY-040 rotary encoder to a Nano:
  + "+" to Nano 5v, note, also works with 3.3v, example: NodeMCU.
  + GND to Nano ground.
  + CLK (clock) to Nano pin 3, the interrupt pin. Also referred to as output A (encoder pin A).
  + DT (data)   to Nano pin 4. Also referred to as output B (encoder pin B).

  +++ Note, Nana has 2 interrupt pins: 2 and 3.
  + pin 2: Communications clock
  + pin 3: rotary encoder
*/
// -----------------------------------------------------------------------------
// Nano to Nano (N2N) Communications: transmission sender
#define TX_CLOCK 2
#define TX_DATA 5

// Rate notes:
//  300 nice to watch the bits show.
//   10 fast for transfer.
//    1 works fine, even with serial print statements.
#define TX_RATE 1
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
  Serial.print("+ byte:");
  Serial.print(tx_byte);
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
// Rotary Encoder module connections
const int PinCLK = 3; // Generating interrupts using CLK signal
const int PinDT = 4;  // Reading DT signal

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

  pinMode(TX_CLOCK, OUTPUT);
  pinMode(TX_DATA, OUTPUT);
  Serial.println("+ Configured: Nano to Nano (N2N) Communications for sending.");

  attachInterrupt (digitalPinToInterrupt(PinCLK), rotarydetect, CHANGE); // Interrupt 0 is pin 2 on Arduino.
  Serial.println("+ Configured: rotary encoder.");

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
static unsigned long timer = millis();
static int virtualPosition = 0;
void loop() {

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

  delay(10);

}
// -----------------------------------------------------------------------------
