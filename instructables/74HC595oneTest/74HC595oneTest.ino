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
  + MSBFIRST: Most significant first.
  + LSBFIRST: Lest significant first.

  Documentation:
  https://www.arduino.cc/en/Tutorial/ShiftOut
  + How 74HC595 Shift Register Works & Interface it with Arduino
  https://lastminuteengineers.com/74hc595-shift-register-arduino-tutorial/
  + Video
  https://www.youtube.com/watch?v=N7CAboD1jU0

*/
// -----------------------------------------------------------------------------
// Shift Register

//           Mega/Nano pins            74HC595 Pins
const int dataPinLed = A14;     // pin 14 Data pin.
const int latchPinLed = A12;    // pin 12 Latch pin.
const int clockPinLed = A11;    // pin 11 Clock pin.

byte dataByte = B01010101;
unsigned int addressWord;

void updateShiftRegister(byte dataByte) {
  digitalWrite(latchPinLed, LOW);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, dataByte);  // MSBFIRST or LSBFIRST
  digitalWrite(latchPinLed, HIGH);
}

void lightsStatusAddressData( byte status8bits, unsigned int address16bits, byte data8bits) {
  // Status, data, lower address byte, higher address byte.
  digitalWrite(latchPinLed, LOW);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, status8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, data8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, lowByte(address16bits));
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, highByte(address16bits));
  digitalWrite(latchPinLed, HIGH);
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  pinMode(latchPinLed, OUTPUT);
  pinMode(clockPinLed, OUTPUT);
  pinMode(dataPinLed, OUTPUT);
  delay(300);
  Serial.println("+ 74HC595 chip control pins are set.");

  // Turns all the LEDs off.
  lightsStatusAddressData(0, 0, 0);
  delay(3000);
  //
  // Turns all the LEDs on.
  addressWord = B11111111 + B11111111 * 256;
  lightsStatusAddressData(B11111111, addressWord, B11111111);
  delay(3000);
  //
  // Turns all the LEDs off.
  lightsStatusAddressData(0, 0, 0);
  delay(1000);
  //
  // Every other LED.
  digitalWrite(latchPinLed, LOW);
  shiftOut(dataPinLed, clockPinLed, MSBFIRST, B01010101);  // MSBFIRST or LSBFIRST
  shiftOut(dataPinLed, clockPinLed, MSBFIRST, B01010101);  // MSBFIRST or LSBFIRST
  shiftOut(dataPinLed, clockPinLed, MSBFIRST, B01010101);  // MSBFIRST or LSBFIRST
  shiftOut(dataPinLed, clockPinLed, MSBFIRST, B01010101);  // MSBFIRST or LSBFIRST
  digitalWrite(latchPinLed, HIGH);
  delay(3000);

  // 1, 2, 3, 4.
  addressWord = 3 +  4 * 256;
  lightsStatusAddressData(1, addressWord, 2);
  delay(3000);

  Serial.println("+++ Start program loop.");
}

// -----------------------------------------------------------------------------
// Device Loop.

void loop() {
  Serial.println("+ Looping");
  delay(500);
  lightsStatusAddressData(0, 0, 0); // Turns all the LEDs off.
  delay(500);
  for (int numberToDisplay = 0; numberToDisplay < 256; numberToDisplay++) {
    updateShiftRegister(numberToDisplay);   // Status lights
    updateShiftRegister(numberToDisplay);   // Data lights
    updateShiftRegister(numberToDisplay);   // Address lower byte
    updateShiftRegister(numberToDisplay);   // Address higher byte
    delay(60);
  }
}
// -----------------------------------------------------------------------------
