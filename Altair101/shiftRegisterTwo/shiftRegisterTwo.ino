// -----------------------------------------------------------------------------
/*
  Shifting 2 bytes, 16 bits, to Expand I/O.

  Using a 74HC595 Shift Register for serial to multiple pin outs.

  First 74HC595:
  + 74HC595 pin 16: to 5V+.
  + 74HC595 pin 15: LED 0.
  + 74HC595 pin 14: Latch pin (RCLK) to Nano pin 5, which does the Parallel-Out task to the 8 output pins.
  + 74HC595 pin 13: to ground (-).
  + 74HC595 pin 12: Data-in pin (SRCLK) to Nano pin 4(data out), data transfer from Nano to 595.
  + 74HC595 pin 11: Clock pin (SER)  to Nano pin 6, clock signal to say that the data is ready.
  + 74HC595 pin 10: to 5V+.
  + 74HC595 pin 09: Not used in single 74HC595. Used to daisy chain to next 74HC595, pin 14 (data).
  + 74HC595 pin 08: to ground (-).
  + 74HC595 pin 07: LED 7.
  + 74HC595 pin 06: LED 6.
  + 74HC595 pin 05: LED 5.
  + 74HC595 pin 04: LED 4.
  + 74HC595 pin 03: LED 3.
  + 74HC595 pin 02: LED 2.
  + 74HC595 pin 01: LED 1.

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

const int latchPin = 5;           // Latch pin of 74HC595 is connected to Digital pin 5
const int dataPin = 4;            // Data pin of 74HC595 is connected to Digital pin 4
const int clockPin = 6;           // Clock pin of 74HC595 is connected to Digital pin 6

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
