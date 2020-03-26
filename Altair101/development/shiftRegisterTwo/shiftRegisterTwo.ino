// -----------------------------------------------------------------------------
/*
  Shifting 2 bytes, 16 bits, to Expand I/O.

  Using a 74HC595 Shift Register for serial to multiple pin outs.

  First 74HC595:
        ---------
  LED 1 |01 | | 16| 5V+
  LED 2 |02  -  15| LED 0
  LED 3 |03     14| Data pin (SRCLK) to Nano pin 4, data transfer from Nano to 595.
  LED 4 |04     13| Ground (-)
  LED 5 |05     12| Latch pin (RCLK) to Nano pin 5, which does the Parallel-Out task to the 8 output pins.
  LED 6 |06     11| Clock pin (SER)  to Nano pin 6, clock signal to say that the data is ready.
  LED 7 |07     10| 5V+
  GND(-)|08     09| Daisy chain to next 74HC595, pin 14 (data). Not used in single 74HC595.
         ---------

  Second 74HC595 pins are the same as above, except:
  + First 74HC595 pin 09 is connected to second 74HC595 pin 14 (data).
  + This daisy chains data from the first 74HC595 to next 74HC595.
  + Clock(pin 11) and latch(pin 14) are the same for each 74HC595.
  + 2 pins to +5V:   10 and 16.
  + 2 pins to ground: 9 and 13.

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

  + Binary, Digital calculator
  https://www.calculator.net/binary-calculator.html

*/
// -----------------------------------------------------------------------------
// Shift Register

const int dataPin = 4;            // 74HC595 Data  pin 14 is connected to Digital pin 4
const int latchPin = 5;           // 74HC595 Latch pin 12 is connected to Digital pin 5
const int clockPin = 6;           // 74HC595 Clock pin 11 is connected to Digital pin 6

byte dataByte = B01010101;

void displayLedByte(byte data8bits) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, data8bits);
  digitalWrite(latchPin, HIGH);
}

void displayLedWord(unsigned int data16bits) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, lowByte(data16bits));
  shiftOut(dataPin, clockPin, LSBFIRST, highByte(data16bits));
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

  digitalWrite(latchPin, LOW);
  dataByte = B11000011;
  shiftOut(dataPin, clockPin, LSBFIRST, dataByte);
  dataByte = B01010101;
  shiftOut(dataPin, clockPin, LSBFIRST, B11000011);
  digitalWrite(latchPin, HIGH);

  Serial.println("+++ Start program loop.");
}

// -----------------------------------------------------------------------------
// Device Loop for processing machine code.

void loop() {
  Serial.println("+ Looping");
  delay(3000);

  unsigned int aWord16bits = B11000011 * 256 + B10101010; // 17322, 195 = 11 000 011 : 170 = 10101010
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, highByte(aWord16bits));
  shiftOut(dataPin, clockPin, LSBFIRST, lowByte(aWord16bits));
  digitalWrite(latchPin, HIGH);
  delay(3000);
  displayLedByte(B00010000);
  for (int numberToDisplay = 0; numberToDisplay < 256; numberToDisplay++) {
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, numberToDisplay);
    shiftOut(dataPin, clockPin, MSBFIRST, numberToDisplay);
    digitalWrite(latchPin, HIGH);
    delay(60);
  }
}
// -----------------------------------------------------------------------------
