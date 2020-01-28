// -----------------------------------------------------------------------------
/*
  Serial Communications:
  1) Test that the Java program runs with this program, using only the monitor serial port.
  ++ Write to monitor serial port using a Java program.
  ++ Read monitor serial port in the Arduino.
  +++ No output.
  +++ Data not confirmed.
  +++ Java program confirms that the port was open and that bytes were successfully sent.
  2) Add a second serial port hardware module to the Arduino.
  ++ Write to Serial2 using the Java program.
  ++ Read Serial2 in the Arduino.
  ++ Arduino will echo the incoming bytes to the monitor serial port.

  For receiving a binary program file,
  + Once the receive function is started,
  ++ Have a interrupt option to exit.
  ++ Wait for the first character then start loading a 2k buffer.
  ++ During the receive, if no characters received in a 1/2 second interval, end receive.
  + Echo information about the received buffer.

  + Second port module:
  ++ CH340 USB To RS232 TTL Auto Converter Module Serial Port FOR Arduino STC TA-02L
  + The one I bought, requires a new driver.
  ++ CP2102 USB 2.0 to TTL UART Module 6Pin Serial Converter STC FT232 26.5mm*15.6mm
  ++ Download driver:https://www.silabs.com/community/interface/knowledge-base.entry.html/2017/01/10/legacy_os_softwarea-bgvU
  +++ http://www.silabs.com/Support%20Documents/Software/Mac_OSX_VCP_Driver_10_6.zip

  View serial ports:
  $ ls /dev/tty.*

  I'm following the video,
    https://www.youtube.com/watch?v=BdzzyEuUWYk

  Reference,
    +
    https://www.arduino.cc/en/Tutorial/SoftwareSerialExample
       Note:, Not all pins on the Mega and Mega 2560 support change interrupts,
        so only the following can be used for RX:
        10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

    https://forum.arduino.cc/index.php?topic=396450
    + The 64 byte serial input buffer does not limit the number of receive characters
      because the code in the examples can empty the buffer faster than new data arrives.

  0         1         2         3         4         5         6         7         8         9         0         1         2         3
  01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
  01214111811111116111111111111111211111111111111111111111111111114111111111111111111111111111111111111111111111111111111111111111811
  01214111811111116111111111111111211111111111111111111111111111114111111111111111111111111111111111111111111111111111111111111111811

*/

// -----------------------------------------------------------------------------
// Add another serial port settings, to connect to the new serial hardware module.
#include <SoftwareSerial.h>
// Connections:
//  If not transmiting, then the second pin doesn't need to be connected.
//
// Parameters: (receive, transmit).
// Receive needs to be on an interrupt pin.
// Pin 10 or 12 (tested) is connected to TXD on the serial module. TXD transmits received bytes to the Arduino.
// Pin ? is not connected to RXD on the serial module.
SoftwareSerial serial2(12,11);

// Then, to read from the new serial port, use:
//    serial2.begin(9600);
//    serial2.available()
//    serial2.read();

// -----------------------------------------------------------------------------
// Memory definitions

const int memoryBytes = 1024;  // When using Mega: 1024, for Nano: 256
byte memoryData[memoryBytes];
unsigned int programCounter = 0;     // Program address value

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
  // Speed port for logging.
  Serial.begin(9600); // 9600 115200
  while (!Serial) {
    ;
  }
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");
  Serial.println("+ Ready for serial communications.");

  serial2.begin(9600);
  while (!serial2) {
    ;
  }
  // serial2.listen(); // Not likely required.
  if (serial2.isListening()) {
    Serial.println("+ serial2 is listening.");
  }
  Serial.println("+ Ready to use the second serial port.");

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
byte readByte = 0;
int readByteCount = 0;
void loop() {
  /*
  */
  if (Serial.available() > 0) {
    // Read and process an incoming byte.
    readByte = Serial.read();
    if (readByte == 10) {
      // New line character.
      Serial.println("");
    } else {
      Serial.write(readByte);
    }
  }
  if (serial2.available() > 0) {
    // Read and process an incoming byte.
    Serial.print("++ Byte: ");
    readByte = serial2.read();
    // When displaying only binary data.
    memoryData[readByteCount];
    readByteCount++;
    printByte(readByte);
    Serial.print(" Octal:");
    printOctal(readByte);
    Serial.print(" Decimal");
    Serial.print(readByte, DEC);
    // Serial.print(", Character: ");
    // Serial.write(readByte);
    Serial.println("");
    /*
    */
  }
  // delay(30); // Arduino sample code, doesn't use a delay.
}
// -----------------------------------------------------------------------------
