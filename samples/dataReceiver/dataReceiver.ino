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
  // Read a bit and add it to the byte.
  bitReceived = true;
  rx_bit = digitalRead(RX_DATA);
  /*
  */
  Serial.print("+");
  Serial.print(" bit_position: ");
  Serial.print(bit_position);
  Serial.print(" bit: ");
  Serial.println(rx_bit);
  // lcd.print(rx_byte & (0x80 >> i) ? "1" : "0");
  //
  if (rx_bit) {
    rx_byte |= (0x80 >> bit_position);
  }
  bit_position++;
  if (bit_position == 8) {
    messageByte = rx_byte;
    strncat(message, (const char *)&rx_byte, 1);
    byteReceived = true;
    rx_byte = 0;
    bit_position = 0;
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
    // Serial.print("+ bit :");
    // Serial.println(counter);
  }

  if (bitReceived) {
    bitReceived = false;
    if (byteReceived) {
      Serial.print("+");
      Serial.print(" messageByte :");
      Serial.print(messageByte);
      Serial.print(" message :");
      Serial.print(message);
      Serial.println(":");
      byteReceived = false;
    }
  }

  delay(100);
}
// -----------------------------------------------------------------------------
