// -----------------------------------------------------------------------------
/*
  Shifting 2 bytes, 16 bits, to Expand I/O.

  Using a 74HC595 Shift Register for serial to multiple pin outs.

  + Documentation:
  https://www.arduino.cc/en/Tutorial/ShiftOut
  + Samples
  https://www.arduino.cc/en/Tutorial/ShftOut21


*/
// -----------------------------------------------------------------------------
// Shift Register

const int latchPin = 5;           // Latch pin of 74HC595 is connected to Digital pin 5
const int dataPin = 4;            // Data pin of 74HC595 is connected to Digital pin 4
const int clockPin = 6;           // Clock pin of 74HC595 is connected to Digital pin 6

byte dataByte = B01010101;

void updateShiftRegister() {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, dataByte);
  digitalWrite(latchPin, HIGH);
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  delay(300);
  Serial.println("+ Connection to the 595 is set.");

  dataByte = B01010101;
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, dataByte);  // MSBFIRST or LSBFIRST
  digitalWrite(latchPin, HIGH);

  Serial.println("+++ Start program loop.");
}

// -----------------------------------------------------------------------------
// Device Loop for processing machine code.

void loop() {
  Serial.println("+ Looping");
  delay(500);
  dataByte = 0;  // Initially turns all the LEDs off, by giving the variable 'leds' the value 0
  updateShiftRegister();
  delay(500);
  for (int numberToDisplay = 0; numberToDisplay < 256; numberToDisplay++) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, numberToDisplay);
    digitalWrite(latchPin, HIGH);
    delay(60);
  }
}
// -----------------------------------------------------------------------------
