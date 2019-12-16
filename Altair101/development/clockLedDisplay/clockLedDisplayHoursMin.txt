// -----------------------------------------------------------------------------
/*
  Clock with LED lights for diplay.
*/
// ------------------------------------------------------------------------
byte programCounter = B11111111;
byte dataByte       = B11111111;
byte statusByte     = B11111111;

// ------------------------------------------------------------------------
// Output LED light shift register(SN74HC595N) pins

//                Nano pin               74HC595 Pins
const PROGMEM int dataPinLed = 7;     // pin 14 Data pin.
const PROGMEM int latchPinLed = 8;    // pin 12 Latch pin.
const PROGMEM int clockPinLed = 9;    // pin 11 Clock pin.

void lightsStatusAddressData( byte status8bits, byte address16bits, byte data8bits) {
  digitalWrite(latchPinLed, LOW);
  // MSBFIRST matches the bit to LED mapping.
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, data8bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, address16bits);
  shiftOut(dataPinLed, clockPinLed, LSBFIRST, status8bits);
  digitalWrite(latchPinLed, HIGH);
}

void printByte(byte b) {
  for (int i = 7; i >= 0; i--)
    Serial.print(bitRead(b, i));
}

// ------------------------------------------------------------------------
// Data LED lights displayed using shift registers.

byte theCounterYear = 0;
byte theCounterMonth = 0;
byte theCounterDay = 0;
byte theCounterHours = 0;
byte theCounterMinutes = 0;
byte theCounterSeconds = 0;

// ------------------------------------------------------------------------
// Display hours and minutes on LED lights.

byte theCounterMinuteBinary = 0;
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
  lightsStatusAddressData(theCounterMinuteBinary, theCounterHours1, theCounterHours2);
}

void displayTheMinute(byte theMinute) {
  byte theCounterMinuteOnes = 0;
  byte theCounterMinuteTens = 0;
  if (theMinute < 10) {
    theCounterMinuteBinary = theMinute;
  } else {
    // There are 3 bits for the tens: 0, 10, 20, 30, 40, or 50.
    // There are 4 bits for the ones: 0 ... 9.
    //                         Tens & Minutes: B-tttmmmm
    //                                         B00001111 = 2 ^ 4 = 16
    // theMinute = 10, theCounterMinuteBinary = 00010000
    theCounterMinuteTens = theMinute / 10;
    theCounterMinuteOnes = theMinute - theCounterMinuteTens * 10;
    theCounterMinuteBinary = 16 * theCounterMinuteTens + theCounterMinuteOnes;
    /*
    Serial.print(F(", theMinute = "));
    Serial.print(theMinute);
    Serial.print(F(", theCounterMinuteOnes = "));
    Serial.print(theCounterMinuteOnes);
    Serial.print(F(", theCounterMinuteTens = "));
    Serial.print(theCounterMinuteTens);
    Serial.print(F(", theCounterMinuteBinary = "));
    printByte(theCounterMinuteBinary);
    */
  }
  lightsStatusAddressData(theCounterMinuteBinary, theCounterHours1, theCounterHours2);
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
  // lightsStatusAddressData(statusByte, programCounter, dataByte);
  // delay(3000);
  Serial.println(F("+ All LED lights off."));
  lightsStatusAddressData(0, 0, 0);
  Serial.println("------------------------");
  for (byte numberToDisplay = 1; numberToDisplay <= 12; numberToDisplay++) {
    Serial.print("+ Hour to display: ");
    Serial.print(numberToDisplay);
    displayTheHour(numberToDisplay);
    delay(500);
    Serial.println("");
  }
  Serial.println("+++ Start program loop.");
}

// -----------------------------------------------------------------------------
// Loop for displaying hours and minutes.

void loop() {
  Serial.println("------------------------");
  for (byte numberToDisplay = 0; numberToDisplay < 60; numberToDisplay++) {
    Serial.print("+ Minute to display: ");
    Serial.print(numberToDisplay);
    displayTheMinute(numberToDisplay);
    delay(500);
    Serial.println("");
  }
}
// -----------------------------------------------------------------------------
