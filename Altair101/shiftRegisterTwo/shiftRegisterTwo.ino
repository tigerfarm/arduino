// -----------------------------------------------------------------------------
/*
  Shifting 2 bytes, 16 bits, to Expand I/O.

  Using a 74HC595 Shift Register for serial to multiple pin outs.

  Documentation, shiftOut():
  https://www.arduino.cc/reference/tr/language/functions/advanced-io/shiftout/
  + Shifts out a byte of data one bit at a time.
  + MSBFIRST: Most signifigent first.
  + LSBFIRST: Lest signifigent first.

  Documentation, lowByte():
  https://www.arduino.cc/reference/en/language/functions/bits-and-bytes/lowbyte/
  + Extracts the low-order (rightmost) byte of a variable (e.g. a word).
  Documentation, highByte():
  https://www.arduino.cc/reference/en/language/functions/bits-and-bytes/highbyte/
  + Extracts the high-order (leftmost) byte of a word (or the second lowest byte of a larger data type).

  + Documentation:
  https://www.arduino.cc/en/Tutorial/ShiftOut
  + Samples
  https://www.arduino.cc/en/Tutorial/ShftOut21

  + Binary, Digital calculator
  https://www.calculator.net/binary-calculator.html
  
*/
// -----------------------------------------------------------------------------
// Shift Register

const int latchPin = 5;           // Latch pin of 74HC595 is connected to Digital pin 5
const int dataPin = 4;            // Data pin of 74HC595 is connected to Digital pin 4
const int clockPin = 6;           // Clock pin of 74HC595 is connected to Digital pin 6

byte dataByte = B01010101;

void displayLedData(byte dataByte) {
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
  shiftOut(dataPin, clockPin, LSBFIRST, dataByte);
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, 0);
  digitalWrite(latchPin, HIGH);

  Serial.println("+++ Start program loop.");
}

// -----------------------------------------------------------------------------
// Device Loop for processing machine code.

void loop() {
  Serial.println("+ Looping");
  delay(3000);
  // dataByte = 0;  // Initially turns all the LEDs off, by giving the variable 'leds' the value 0
  // displayLedData(B00000100);

  // 300 = 00 000 001 : 00 101 100
  int aWord16bits= 300;
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, lowByte(aWord16bits));
  shiftOut(dataPin, clockPin, LSBFIRST, highByte(aWord16bits));
  digitalWrite(latchPin, HIGH);
  delay(3000);
  // displayLedData(B00010000);
  /*
    for (int numberToDisplay = 0; numberToDisplay < 256; numberToDisplay++) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, numberToDisplay);
    digitalWrite(latchPin, HIGH);
    delay(60);
    }
  */
}
// -----------------------------------------------------------------------------
