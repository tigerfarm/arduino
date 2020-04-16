\
// -----------------------------------------------------------------------------
/*
  Shifting 8 bits to Expand I/O.

  Using a 74HC595 Shift Register for serial to multiple pin outs.

  74HC595 is a SIPO (Serial-In-Parallel-Out) shift registers, example: Texas Instruments SN74HC595N.
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

//           Mega/Nano pins            74HC595 Pins
const int dataPinLed = A13;     // pin 14 Data pin.
const int latchPinLed = A14;    // pin 12 Latch pin.
const int clockPinLed = A15;    // pin 11 Clock pin.

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
