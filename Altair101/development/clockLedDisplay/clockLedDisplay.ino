// -----------------------------------------------------------------------------
/*
  Clock with LED lights for diplay.

*/
// ------------------------------------------------------------------------
// Output LED light shift register(SN74HC595N) pins

//                Nano pin               74HC595 Pins
const PROGMEM int dataPinLed = 7;     // pin 14 Data pin.
const PROGMEM int latchPinLed = 8;    // pin 12 Latch pin.
const PROGMEM int clockPinLed = 9;    // pin 11 Clock pin.

// ------------------------------------------------------------------------
byte programCounter = B11111111;
byte dataByte       = B11111111;
byte statusByte     = B11111111;

// ------------------------------------------------------------------------
// Data LED lights displayed using shift registers.

byte theCounterYear = 0;
byte theCounterMonth = 0;
byte theCounterDay = 0;
byte theCounterHours = 0;
byte theCounterMinutes = 0;
byte theCounterSeconds = 0;

byte theCounterHours1 = 0;
byte theCounterHours2 = 0;
void displayTheHour( byte theHour) {
  switch (theHour) {
    case 1:
      //                 B11111111
      theCounterHours1 = B00000010;
      theCounterHours2 = 0;
      break;
    case 2:
      //                 B11111111
      theCounterHours1 = B00000100;
      theCounterHours2 = 0;
      break;
    case 3:
      //                 B11111111
      theCounterHours1 = B00001000;
      theCounterHours2 = 0;
      break;
    case 4:
      //                 B11111111
      theCounterHours1 = B00010000;
      theCounterHours2 = 0;
      break;
    case 5:
      //                 B11111111
      theCounterHours1 = B00100000;
      theCounterHours2 = 0;
      break;
    case 6:
      //                 B11111111
      theCounterHours1 = B01000000;
      theCounterHours2 = 0;
      break;
    case 7:
      //                 B11111111
      theCounterHours1 = 0;
      theCounterHours2 = B00000010;
      break;
    case 8:
      //                 B11111111
      theCounterHours1 = 0;
      theCounterHours2 = B00000100;
      break;
    case 9:
      //                 B11111111
      theCounterHours1 = 0;
      theCounterHours2 = B00001000;
      break;
    case 10:
      //                 B11111111
      theCounterHours1 = 0;
      theCounterHours2 = B00010000;
      break;
    case 11:
      //                 B11111111
      theCounterHours1 = 0;
      theCounterHours2 = B00100000;
      break;
    case 12:
      //                 B11111111
      theCounterHours1 = 0;
      theCounterHours2 = B01000000;
      break;
  }
  lightsStatusAddressData(theCounterMinutes, theCounterHours1, theCounterHours2);
}

void lightsStatusAddressData( byte status8bits, byte address16bits, byte data8bits) {
  digitalWrite(latchPinLed, LOW);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, data8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, address16bits);
  shiftOut(dataPinLed, clockPinLed, MSBFIRST, status8bits); // MSBFIRST matches the bit to LED mapping.
  digitalWrite(latchPinLed, HIGH);
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  // ----------------------------------------------------
  pinMode(latchPinLed, OUTPUT);
  pinMode(clockPinLed, OUTPUT);
  pinMode(dataPinLed, OUTPUT);
  delay(300);
  Serial.println(F("+ Front panel LED shift registers ready."));

  Serial.println(F("+ All LED lights on."));
  lightsStatusAddressData(statusByte, programCounter, dataByte);
  delay(3000);
  Serial.println(F("+ All LED lights off."));
  lightsStatusAddressData(0, 0, 0);
  Serial.println("+++ Start program loop.");
}

// -----------------------------------------------------------------------------
// Device Loop for processing machine code.

void loop() {
  for (byte numberToDisplay = 1; numberToDisplay <= 12; numberToDisplay++) {
    Serial.print("+ Hour to display: ");
    Serial.print(numberToDisplay);
    displayTheHour(numberToDisplay);
    delay(1000);
    Serial.println("");
  }
}
// -----------------------------------------------------------------------------
