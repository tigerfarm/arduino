// -----------------------------------------------------------------------------
/*
  Serial Communications:

  + Second serial port module:
  + The one I bought, required a new driver.
  ++ CP2102 USB 2.0 to TTL UART Module 6Pin Serial Converter STC FT232 26.5mm*15.6mm
  ++ Download driver:https://www.silabs.com/community/interface/knowledge-base.entry.html/2017/01/10/legacy_os_softwarea-bgvU
  +++ http://www.silabs.com/Support%20Documents/Software/Mac_OSX_VCP_Driver_10_6.zip
  +++ I unzipped it and renamed it: CP2102-MacSerialDriver.dmg.
  + Consider buying a serial port module that matches the Arduino current CH340 USB driver:
  ++ CH340 USB To RS232 TTL Auto Converter Module Serial Port FOR Arduino STC TA-02L

  View serial ports on a Mac:
  $ ls /dev/tty.*

  I'm following the video,
    https://www.youtube.com/watch?v=BdzzyEuUWYk

  Reference, has notes about wiring:
    https://www.arduino.cc/en/Tutorial/SoftwareSerialExample
      Note: for receive (RX), use one of the Mega and Mega 2560 interrupt supported pins,
        10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69
    https://forum.arduino.cc/index.php?topic=396450
      The 64 byte serial input buffer does not limit the number of receive characters
      because the code examples can empty the buffer faster than new data arrives.

  Strings for testing.
  0         1         2         3         4         5         6         7         8         9         0         1         2         3
  01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
  01214111811111116111111111111111211111111111111111111111111111114111111111111111111111111111111111111111111111111111111111111111811

*/

// -----------------------------------------------------------------------------
// Add another serial port settings, to connect to the new serial hardware module.
#include <SoftwareSerial.h>
// Connections:
// Since not transmitting, the second parameter pin doesn't need to be connected.
// Parameters: (receive, transmit).
// Receive needs to be an interrupt pin.
// Computer USB >> serial2 module TXD >> RX pin for the Arduino to receive the bytes.
//                                TXD transmits received bytes to the Arduino receive (RX) pin.
const int PIN_RX = 12;  // Arduino receive is connected to TXD on the serial module.
const int PIN_TX = 11;  // Arduino transmit is not used, and therefore not connected to RXD pin on the serial module.
SoftwareSerial serial2(PIN_RX, PIN_TX);

// Then, to read from the new serial port, use:
//    serial2.begin(9600);
//    serial2.available()
//    serial2.read();

// -----------------------------------------------------------------------------
// Memory definitions

const int memoryBytes = 1024;       // Simulate emulator memory
byte memoryData[memoryBytes];
unsigned int programCounter = 0;    // Program address value

// -----------------------------------------------------------------------------
void printByte(byte b) {
  for (int i = 7; i >= 0; i--)
    Serial.print(bitRead(b, i));
}

void printOctal(byte b) {
  String sOctal = String(b, OCT);
  for (int i = 1; i <= 3 - sOctal.length(); i++) {
    Serial.print("0");
  }
  Serial.print(sOctal);
}

// -----------------------------------------------------------------------------
void setup() {
  // Logging port speed.
  Serial.begin(115200); // 9600 115200
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");
  Serial.println("+ Ready for serial communications.");

  // ------------------------
  serial2.begin(9600);
  // serial2.listen(); // Not required when only using one serial module.
  if (serial2.isListening()) {
    Serial.println("+ serial2 is listening.");
    Serial.println("+ Ready to use the second serial port for receiving program bytes.");
  }

  // ------------------------
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
byte readByte = 0;
int readByteCount = 0;
void loop() {
  if (Serial.available() > 0) {
    // Input on the serial log message port.
    // Read and process an incoming byte.
    readByte = Serial.read();
    if (readByte == 10) {
      // New line character.
      Serial.println("");
    } else {
      Serial.write(readByte);
    }
    readByteCount = 0;  // Reset for doing multiple upload tests.
  }
  if (serial2.available() > 0) {
    // Input on the external serial port module.
    // Read and process an incoming byte.
    Serial.print("++ Byte array number: ");
    if (readByteCount<10) {
      Serial.print(" ");
    }
    if (readByteCount<100) {
      Serial.print(" ");
    }
    Serial.print(readByteCount);
    readByte = serial2.read();
    memoryData[readByteCount] = readByte;
    readByteCount++;
    Serial.print(", Byte: ");
    printByte(readByte);
    Serial.print(" Octal:");
    printOctal(readByte);
    Serial.print(" Decimal");
    Serial.print(readByte, DEC);
    Serial.println("");
  }
  // delay(30); // Arduino sample code, doesn't use a delay.
}
// -----------------------------------------------------------------------------
