// -----------------------------------------------------------------------------
/*
  Nano to Nano (N2N) Communications
  Connect Nanos together for communications:
  + 5v: positive
  + GND: ground
  + pins 2: clock
  + pins 3: data
*/
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
char message[16];

void onClockPulse() {
  // Read a bit.
  rx_bit = digitalRead(RX_DATA);
  // Add bit to byte.
  if (rx_bit) {
    rx_byte |= (0x80 >> bit_position);
  }
  /*
  */
  Serial.print("+");
  Serial.print(" bit_position: ");
  Serial.print(bit_position);
  Serial.print(" bit: ");
  Serial.println(rx_bit);
  //
  bit_position++;
  if (bit_position == 8) {
    // 8 bits is a byte.
    byteReceived = true;
    strncat(message, (const char *)&rx_byte, 1);
    messageByte = rx_byte;
    bit_position = 0;
    rx_byte = 0;
  }
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  // Prep to receive a message.
  strcpy(message, "");
  pinMode(RX_DATA, INPUT);
  attachInterrupt(digitalPinToInterrupt(RX_CLOCK), onClockPulse, RISING);
  Serial.println("+ Ready to receive communications from the other Nano.");

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
void loop() {

  if (bitReceived) {
    if (byteReceived) {
      Serial.print("+");
      Serial.print(" messageByte :");
      Serial.print(messageByte);
      Serial.print(": message :");
      Serial.print(message);
      Serial.println(":");
      byteReceived = false;
    }
    bitReceived = false;
  }

  delay(100);
}
// -----------------------------------------------------------------------------
