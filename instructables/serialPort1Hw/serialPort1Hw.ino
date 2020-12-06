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

  View serial ports on a Mac for the Mega default serial port(14120) and the componet's port(14110):
  $ ls -l /dev/tty.*
crw-rw-rw-  1 root  wheel   20,   0 Nov 13 15:20 /dev/tty.Bluetooth-Incoming-Port
crw-rw-rw-  1 root  wheel   20, 0x00000104 Dec  5 20:13 /dev/tty.wchusbserial14110
crw-rw-rw-  1 root  wheel   20, 0x00000106 Dec  5 20:13 /dev/tty.wchusbserial14120

    https://www.arduino.cc/reference/en/language/functions/communication/serial/
*/
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
  Serial2.begin(9600);
  Serial.println("+ SerialHw on the second port is listening.");
  Serial.println("+ Ready to use the second serial port.");

  // ------------------------
  Serial.println("+ Go to loop.");
  Serial.println("++ Send characters using the Arduino Serial Monitor.");
  Serial.print("+ Port 0: ");
  // ------------------------
  Serial2.println("+ Go to loop.");
  Serial2.println("++ Send characters to the software serial port.");
  Serial2.print("+ Port SerialSw: ");
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
      Serial.print("+ Port 0: ");
    }
  }
  //--------------------------------------
  // Software serial port processing
  if (Serial2.available() > 0) {
    // Read and process an incoming byte.
    readByte = Serial2.read();
    // Process the byte.
    Serial2.write(readByte);
    if (readByte == 13) {
      // Mac terminal uses carriage return (CR).
      Serial2.println();
      Serial2.print("+ Port Serial2: ");
    }
  }
}

// -----------------------------------------------------------------------------
// eof
