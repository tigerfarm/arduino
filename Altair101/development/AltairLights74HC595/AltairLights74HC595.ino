// -----------------------------------------------------------------------------
/*
  Testing front panel LED light implementations for both the Tablet and the Desktop modules.
  + Test display functions: processDataLights and lightsStatusAddressData: Status, data, and address lights
  + Confirmed that the status bits matching both Tablet and Desktop front panels
  + This program does not test WAIT and HLDA, thoses are tested in the PCF toggle program.
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

byte statusByte = B00000000;        // By default, all are OFF.
byte dataByte = B01010101;
unsigned int curProgramCounter = 0;     // Current program address value

// -----------------------------------------------------------------------------
// Status lights: INT, WO, STACK, HLTA, OUT, MI, INP, MEMR

// Use OR to turn ON. Example:
//  statusByte = statusByte | MEMR_ON;
const byte MEMR_ON =    B10000000;  // MEMR   The memory bus will be used for memory read data.
const byte INP_ON =     B01000000;  // INP    The address bus containing the address of an input device. The input data should be placed on the data bus when the data bus is in the input mode
const byte M1_ON =      B00100000;  // M1     Machine cycle 1, fetch opcode.
const byte OUT_ON =     B00010000;  // OUT    The address contains the address of an output device and the data bus will contain the out- put data when the CPU is ready.
const byte HLTA_ON =    B00001000;  // HLTA   Machine opcode hlt, has halted the machine.
const byte STACK_ON =   B00000100;  // STACK  Stack process
const byte WO_ON =      B00000010;  // WO     Write out (inverse logic)
const byte INT_ON =     B00000001;  // INT    Interrupt

// Use AND to turn OFF. Example:
//  statusByte = statusByte & M1_OFF;
const byte MEMR_OFF =   B01111111;
const byte INP_OFF =    B10111111;
const byte M1_OFF =     B11011111;
const byte OUT_OFF =    B11101111;
const byte HLTA_OFF =   B11110111;
const byte STACK_OFF =  B11111011;
const byte WO_OFF =     B11111101;
const byte INT_OFF =    B11111110;

// -----------------------------------------------------------------------------
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

  Serial.println("+ Turns all the LEDs OFF.");
  lightsStatusAddressData(0, 0, 0);
  delay(3000);
  //
  Serial.println("+ Turns all the LEDs ON.");
  curProgramCounter = B11111111 + B11111111 * 256;
  lightsStatusAddressData(B11111111, curProgramCounter, B11111111);
  Serial.print("+ Value to turn all Address LED lights on: ");
  Serial.println(curProgramCounter);
  delay(6000);

  Serial.println("+ A counter in each section:");
  Serial.println("++ 1) Status lights, 2) Data byte 3) lower byte address, 4) Higher byte address) ");
  // 1, 2, 3, 4.
  curProgramCounter = 3 +  4 * 256;
  lightsStatusAddressData(1, curProgramCounter, 2);
  delay(6000);

  Serial.println("+ Turns all the LEDs off.");
  // lightsStatusAddressData(0, 0, 0);
  statusByte = 0;
  dataByte = 0;
  curProgramCounter = 0;
  processDataLights();
  delay(1000);

  Serial.println("+ Turns every other LED on.");
  // lightsStatusAddressData(B01010101, curProgramCounter, B01010101);
  statusByte = B01010101;
  dataByte = B01010101;
  curProgramCounter = B01010101 + B01010101 * 256;
  processDataLights();
  delay(3000);

  // -------------------------
  // To test, status lights.
  Serial.println("+ Turn the status LED lights on, one at a time, from left to right, from MEMR to INT.");
  statusByte = 0;
  dataByte = 0;
  curProgramCounter = 0;
  processDataLights();
  delay(3000);
  // -------------------------
  Serial.println("+ MEMR_ON");
  statusByte = 0;
  statusByte = MEMR_ON;
  processDataLights();
  delay(3000);
  // -------------------------
  Serial.println("+ INP_ON");
  statusByte = 0;
  statusByte = INP_ON;
  processDataLights();
  delay(3000);
  // -------------------------
  Serial.println("+ M1_ON");
  statusByte = 0;
  statusByte = M1_ON;
  processDataLights();
  delay(3000);
  // -------------------------
  Serial.println("+ OUT_ON");
  statusByte = 0;
  statusByte = OUT_ON;
  processDataLights();
  delay(3000);
  // -------------------------
  Serial.println("+ HLTA_ON");
  statusByte = 0;
  statusByte = HLTA_ON;
  processDataLights();
  delay(3000);
  // -------------------------
  Serial.println("+ STACK_ON");
  statusByte = 0;
  statusByte = STACK_ON;
  processDataLights();
  delay(3000);
  // -------------------------
  Serial.println("+ WO_ON");
  statusByte = 0;
  statusByte = WO_ON;
  processDataLights();
  delay(3000);
  // -------------------------
  Serial.println("+ INT_ON");
  statusByte = 0;
  statusByte = INT_ON;
  processDataLights();
  delay(3000);
  // -------------------------

  Serial.println("+ Turn on status LED lights: MEMR M1 WO.");
  statusByte = 0;
  statusByte = statusByte | MEMR_ON;
  statusByte = statusByte | M1_ON;
  statusByte = statusByte | WO_ON;
  processDataLights();
  delay(6000);
  //
  Serial.println("+ Turn M1 status LED light off, leaving the following on: MEMR WO.");
  statusByte = statusByte & M1_OFF;
  processDataLights();
  delay(6000);
  //
  Serial.println("+ Turn on status LED lights: MEMR WO.");
  statusByte = 0;
  statusByte = MEMR_ON | WO_ON;
  processDataLights();
  delay(6000);
  //
  Serial.println("+ Turn M1 status LED light off, leaving the following on: MEMR WO.");
  statusByte = (MEMR_ON | WO_ON) & M1_OFF;
  processDataLights();
  delay(6000);

  // -------------------------------------------------------------
  Serial.println("+ Turn every other LED light on.");
  statusByte = B01010101;
  dataByte = B01010101;
  curProgramCounter = B01010101 + B01010101 * 256;
  lightsStatusAddressData(0, 0, 0); // Turns all the LEDs off.
  delay(500);

  // -------------------------------------------------------------
  Serial.println("+ Turn each LED on light, from right to left to.");
  statusByte = 1;
  dataByte = 1;
  curProgramCounter = 1 + 1 * 256;
  for (int numberToDisplay = 0; numberToDisplay < 7; numberToDisplay++) {
    dataByte = dataByte << 1;
    curProgramCounter = dataByte +  dataByte * 256;
    lightsStatusAddressData(dataByte, curProgramCounter, dataByte);
    delay(500);
  }

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
    // Counts up from 0 (no lights on), 1, 2, 3, ..., to 255 (all lights on).
    curProgramCounter = numberToDisplay +  numberToDisplay * 256;
    lightsStatusAddressData(numberToDisplay, curProgramCounter, numberToDisplay);
    delay(100);
  }
}
// -----------------------------------------------------------------------------
