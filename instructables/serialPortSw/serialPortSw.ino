// -----------------------------------------------------------------------------
/*
  Serial Communications:

  Second serial port module that I bought, requireed a new driver.
  + CP2102 USB 2.0 to TTL UART Module 6 Pin Serial Converter STC FT232
  ++ Download driver:
      https://www.silabs.com/community/interface/knowledge-base.entry.html/2017/01/10/legacy_os_softwarea-bgvU
      http://www.silabs.com/Support%20Documents/Software/Mac_OSX_VCP_Driver_10_6.zip
  ++ I unzipped it and renamed it: CP2102-MacSerialDriver.dmg.
  + Next time consider buying a serial port module that matches the Arduino current CH340 USB driver:
  ++ CH340 USB To RS232 TTL Auto Converter Module Serial Port FOR Arduino STC TA-02L

  ​Wire the serial component to the Uno.
  The pin numbers are configured on the program to match the values used on the SoftwareSerial library.
  Serial component RX to Uno TX pin 11.
  Serial component TX to Uno RX pin 12.

  On a Mac, list serial ports and connect to the port:
  $ ls /dev/tty.*
  /dev/tty.Bluetooth-Incoming-Port /dev/tty.usbmodem14241 /dev/tty.wchusbserial14220
  $ screen /dev/tty.wchusbserial14220
*/
// -----------------------------------------------------------------------------
// Add another serial port settings, to connect to the new serial hardware module.
#include <SoftwareSerial.h>
// Connections:
// Note receive needs to be an interrupt pin.
// Computer USB >> SerialSw module TXD >> RX pin for the Arduino to receive the bytes.
//                                TXD transmits received bytes to the Arduino receive (RX) pin.
const int PIN_RX = 12;  // Arduino receive  pin (12) is connected to TXD pin on the serial module.
const int PIN_TX = 11;  // Arduino transmit pin (11) is connected to RXD pin on the serial module.
// Parameters: SerialSw(receive, transmit);
SoftwareSerial SerialSw(PIN_RX, PIN_TX);
// Then, to read from the new serial port, use:
//    SerialSw.begin(9600);
//    SerialSw.available()
//    SerialSw.read();

// -----------------------------------------------------------------------------
void setup() {
  // ------------------------
  // Serial speed needs to match the sending program speed such as the Arduino IDE monitor program.
  Serial.begin(9600);             // Sample baud rates: 9600 115200
  delay(1000);
  Serial.println("");               // Newline after garbage characters.
  Serial.println("+++ Setup");
  //
  Serial.println("+ Ready to use port 0 serial port with the Arduino Serial Monitor.");

  // ------------------------
  SerialSw.begin(9600);
  // SerialSw.listen(); // Not required when only using one serial module.
  if (SerialSw.isListening()) {
    Serial.println("+ SerialSw is listening.");
    Serial.println("+ Ready to use the second serial port.");
  }

  // ------------------------
  Serial.println("+ Go to loop.");
  Serial.println("++ Send characters using the Arduino Serial Monitor.");
  Serial.print("+ Port Serial: ");
  // ------------------------
  SerialSw.print("+ Port SerialSw: ");
}

// -----------------------------------------------------------------------------
// Device Loop
void loop() {
  byte readByte = 0;
  //--------------------------------------
  // Serial port 0 processing
  if (Serial.available() > 0) {
    // Read and process an incoming byte.
    readByte = Serial.read();
    // Process the byte.
    Serial.write(readByte);
    if (readByte == 10) {
      // Arduino IDE monitor line feed (LF).
      Serial.print("+ Port Serial: ");
    } else if (readByte == 13) {
      // Terminal uses carriage return (CR).
      Serial.println();
      Serial.print("+ Port Serial: ");
    }
  }
  //--------------------------------------
  // Software serial port processing
  if (SerialSw.available() > 0) {
    // Read and process an incoming byte.
    readByte = SerialSw.read();
    // Process the byte.
    Serial.write(readByte);                 // For testing with a serial upload program.
    SerialSw.write(readByte);
    if (readByte == 10) {
      // IDE monitor (LF).
      SerialSw.print("+ Port SerialSw: ");
      Serial.print("+ Port SerialSw: ");    // For testing with a serial upload program.
    } else if (readByte == 13) {
      // Terminal uses carriage return (CR).
      SerialSw.println();
      SerialSw.print("+ Port SerialSw: ");
      Serial.println();                     // For testing with a serial upload program.
      Serial.print("+ Port SerialSw: ");
    }
  }
}

// -----------------------------------------------------------------------------
// eof
