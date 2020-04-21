// -----------------------------------------------------------------------------
/*
  Testing LED light implementations for both the Tablet and the Desktop modules.

*/
// -----------------------------------------------------------------------------
// If defined, Desktop module. Else Tablet module.
// #define DESKTOP_MODULE 1
//
// #ifdef DESKTOP_MODULE
//    ... Desktop module code ...
// #else
//    ... Tablet module code ...
// #endif

// -----------------------------------------------------------------------------
// Shift Register

//           Mega/Nano pins        74HC595 Pins
const int dataPinLed  = A14;    // pin 14 Data pin.
const int latchPinLed = A12;    // pin 12 Latch pin.
const int clockPinLed = A11;    // pin 11 Clock pin.

// Status lights: INT, WO, STACK, HLTA, OUT, MI, INP, MEMR
byte statusByte = B00000000;        // By default, all are OFF.
byte dataByte = B01010101;
unsigned int curProgramCounter = 0;     // Current program address value

unsigned int addressWord;

// ------------------------------------------------
// Data LED lights displayed using shift registers.

void processDataLights() {
  // Use the current program values: statusByte, curProgramCounter, and dataByte.
  digitalWrite(latchPinLed, LOW);
#ifdef DESKTOP_MODULE
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, statusByte);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, dataByte);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, lowByte(curProgramCounter));
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, highByte(curProgramCounter));
#else
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, dataByte);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, lowByte(curProgramCounter));
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, highByte(curProgramCounter));
  shiftOut(dataPinLed, clockPinLed, MSBFIRST, statusByte); // MSBFIRST matches the bit to LED mapping.
#endif
  digitalWrite(latchPinLed, HIGH);
}

void lightsStatusAddressData( byte status8bits, unsigned int address16bits, byte data8bits) {
  // Status, data, lower address byte, higher address byte.
  digitalWrite(latchPinLed, LOW);
#ifdef DESKTOP_MODULE
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, status8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, data8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, lowByte(address16bits));
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, highByte(address16bits));
#else
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, data8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, lowByte(address16bits));
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, highByte(address16bits));
  shiftOut(dataPinLed, clockPinLed, MSBFIRST, status8bits);
#endif
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
  addressWord = B01010101 + B01010101 * 256;
  lightsStatusAddressData(B01010101, addressWord, B01010101);
  delay(3000);

  // 1, 2, 3, 4.
  addressWord = 3 +  4 * 256;
  lightsStatusAddressData(1, addressWord, 2);
  delay(6000);

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
    // Counts up: 0, 1, 2, 3, ..., 255
    addressWord = numberToDisplay +  numberToDisplay * 256;
    lightsStatusAddressData(numberToDisplay, addressWord, numberToDisplay);
    delay(60);
  }
}
// -----------------------------------------------------------------------------
