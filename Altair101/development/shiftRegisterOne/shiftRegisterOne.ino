\
// -----------------------------------------------------------------------------
/*
  Shifting 8 bits to Expand I/O.

  Using a 74HC595 Shift Register for serial to multiple pin outs.

  74HC595 is a SIPO (Serial-In-Parallel-Out) shift registers, example: Texas Instruments SN74HC595N.
  + 74HC595 pin 16: 5V+
  + 74HC595 pin 15: LED 0
  + 74HC595 pin 14: Data    pin (SRCLK) to Nano pin 4, data transfer from Nano to 595.
  + 74HC595 pin 13: Ground (-)
  + 74HC595 pin 12: Latch   pin (RCLK) to Nano pin 5, which does the Parallel-Out task to the 8 output pins.
  + 74HC595 pin 11: Clock   pin (SER)  to Nano pin 6, clock signal to say that the data is ready.
  + 74HC595 pin 10: 5V+
  + 74HC595 pin 09: Daisy chain to next 74HC595, pin 14 (data). Not used in single 74HC595.
  + 74HC595 pin 08: Ground (-)
  + 74HC595 pin 07: LED 7.
  + 74HC595 pin 06: LED 6.
  + 74HC595 pin 05: LED 5.
  + 74HC595 pin 04: LED 4.
  + 74HC595 pin 03: LED 3.
  + 74HC595 pin 02: LED 2.
  + 74HC595 pin 01: LED 1.

  Documentation, shiftOut():
  https://www.arduino.cc/reference/tr/language/functions/advanced-io/shiftout/
  + Shifts out a byte of data one bit at a time.
  + Shifts out a byte of data one bit at a time.
  + MSBFIRST: Most significant first.
  + LSBFIRST: Lest significant first.

  Documentation:
  https://www.arduino.cc/en/Tutorial/ShiftOut
  + How 74HC595 Shift Register Works & Interface it with Arduino
  https://lastminuteengineers.com/74hc595-shift-register-arduino-tutorial/
  + Video
  https://www.youtube.com/watch?v=N7CAboD1jU0

  + Binary, Digital calculator
  https://www.calculator.net/binary-calculator.html
  
*/
// -----------------------------------------------------------------------------
// Shift Register
const int dataPin = 7;      // Connected to 74HC595 Data  pin 14.
const int latchPin = 8;     // Connected to 74HC595 Latch pin 12.
const int clockPin = 9;     // Connected to 74HC595 Clock pin 11.

byte dataByte = B01010101;

void updateShiftRegister(byte dataByte) {
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

  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, B01010101);  // MSBFIRST or LSBFIRST
  digitalWrite(latchPin, HIGH);

  Serial.println("+++ Start program loop.");
}

// -----------------------------------------------------------------------------
// Device Loop.

void loop() {
  Serial.println("+ Looping");
  delay(500);
  // updateShiftRegister(0); // Turns all the LEDs off.
  delay(500);
  for (int numberToDisplay = 0; numberToDisplay < 256; numberToDisplay++) {
    // updateShiftRegister(numberToDisplay);
    delay(60);
  }
}
// -----------------------------------------------------------------------------
