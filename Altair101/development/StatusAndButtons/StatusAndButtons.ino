// -----------------------------------------------------------------------------
/*
    For testing status LED lights, with check buttons.
*/

// -----------------------------------------------------------------------------
byte statusByte = B00000000;        // By default, all are OFF.
unsigned int programCounter = 6;    // Program address value
byte dataByte = B11000011;
int testLights = B10011001 * 256 + B01100110;

void printByte(byte b) {
  Serial.print("+ Byte = ");
  for (int i = 7; i >= 0; i--)
    Serial.print(bitRead(b, i));
  Serial.println("");
}

// ------------------------------
// Status LEDs
//
const byte MEMR_ON =    B10000000;  // Use OR  to turn ON.
const byte INP_ON =     B01000000;
const byte M1_ON =      B00100000;
const byte OUT_ON =     B00010000;
const byte HLTA_ON =    B00001000;
const byte STACK_ON =   B00000100;
const byte WO_ON =      B00000010;
const byte WAIT_ON =    B00000001;

const byte MEMR_OFF =   B01111111;  // Use AND to turn OFF.
const byte INP_OFF =    B10111111;
const byte M1_OFF =     B11011111;
const byte OUT_OFF =    B11101111;
const byte HLTA_OFF =   B11110111;
const byte STACK_OFF =  B11111011;
const byte WO_OFF =     B11111101;
const byte WAIT_OFF =   B11111110;

// -----------------------------------------------------------------------------
// Shift Register

const int dataPin = 7;            // 74HC595 Data  pin 14 is connected to Nano pin.
const int latchPin = 8;           // 74HC595 Latch pin 12 is connected to Nano pin.
const int clockPin = 9;           // 74HC595 Clock pin 11 is connected to Nano pin.

void lightsStatusAddressData( byte status8bits, unsigned int address16bits, byte data8bits) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, data8bits);
  shiftOut(dataPin, clockPin, LSBFIRST, lowByte(address16bits));
  shiftOut(dataPin, clockPin, LSBFIRST, highByte(address16bits));
  shiftOut(dataPin, clockPin, MSBFIRST, status8bits);
  digitalWrite(latchPin, HIGH);
}

// -----------------------------------------------------------------------------
// Input control
//
const int dataPinIn = 4;            // 74HC595 Data  pin 14 is connected to Digital pin 7
const int latchPinIn = 5;           // 74HC595 Latch pin 12 is connected to Digital pin 8
const int clockPinIn = 6;           // 74HC595 Clock pin 11 is connected to Digital pin 9
const int dataInputPin = A0;      // Nano data input check pin. Digital pins work and some analog pins work.

// Only do the action once, don't repeat if the button is held down.
// Don't repeat action if the button is not pressed.

const int numberOfSwitches = 8;
boolean switchState[numberOfSwitches] = {
  false, false, false, false, false, false, false, false
};
void buttonCheck() {
  byte dataByte = B10000000;
  for (int i = 0; i < numberOfSwitches; i++) {
    digitalWrite(latchPinIn, LOW);
    shiftOut(dataPinIn, clockPinIn, LSBFIRST, dataByte);
    digitalWrite(latchPinIn, HIGH);
    //
    if (digitalRead(dataInputPin) == HIGH) {
      if (!switchState[i]) {
        switchState[i] = true;
        Serial.print("+ Button pressed: ");
        Serial.println(i);
      }
    } else if (switchState[i]) {
      switchState[i] = false;
      //
      if (i == 1) {
        Serial.println(F("> hlt, halt the processor."));
        statusByte = 0;
        statusByte = statusByte | WAIT_ON;
        statusByte = statusByte | HLTA_ON;
        lightsStatusAddressData(statusByte, programCounter, dataByte);
      } else if (i == 2) {
        Serial.print("+ Shift statusByte: ");
        statusByte = statusByte >> 1;
        if (statusByte == 0) {
          statusByte = B10000000;
        }
        printByte(statusByte);
        lightsStatusAddressData(statusByte, testLights, dataByte);
      } else {
        Serial.print("+ Button released: ");
        Serial.println(i);
      }
    }
    //
    dataByte = dataByte >> 1;
  }
  digitalWrite(latchPinIn, LOW);
  shiftOut(dataPinIn, clockPinIn, LSBFIRST, B1111111);
  digitalWrite(latchPinIn, HIGH);
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  // ----------------------------------------------------
  pinMode(latchPinIn, OUTPUT);
  pinMode(clockPinIn, OUTPUT);
  pinMode(dataPinIn, OUTPUT);
  pinMode(dataInputPin, INPUT);
  delay(300);
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, LSBFIRST, B11111111);
  digitalWrite(latchPin, HIGH);
  Serial.println("+ Ready to monitor input switches.");

  // ----------------------------------------------------
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  delay(300);
  Serial.println(F("+ Front panel LED shift registers ready."));
  //
  // Status lights are off by default.
  statusByte = statusByte | WAIT_ON;
  statusByte = statusByte | MEMR_ON;
  statusByte = statusByte | M1_ON;
  statusByte = statusByte | WO_ON;  // WO: on, Inverse logic: off when writing out. On when not.
  Serial.println(F("+ Front panel LED lights initialized."));

  testLights = B10011001 * 256 + B01100110;
  // lightsStatusAddressData(statusByte, programCounter, dataByte);
  printByte(statusByte);
  lightsStatusAddressData(statusByte, testLights, dataByte);
  delay(3000);
  statusByte = B10000000;
  printByte(statusByte);
  lightsStatusAddressData(statusByte, testLights, dataByte);

  Serial.println("+++ Start program loop.");
}

// -----------------------------------------------------------------------------
// Device Loop for processing machine code.

void loop() {
  // Serial.println("+ Looping");
  buttonCheck();
  delay(60);
}
// -----------------------------------------------------------------------------
