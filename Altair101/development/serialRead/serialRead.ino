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

  I'm following the video,
    https://www.youtube.com/watch?v=BdzzyEuUWYk
*/

// -----------------------------------------------------------------------------
// Add another serial port settings, to connect to the new serial hardware module.
#include <SoftwareSerial.h>
//
// 1st is receive, second is for sending.
// If not sending, then the second pin doesn't need to be connect or used.
// SoftwareSerical Serial2(5,6);
// Then, read from the new serial port, use:
//    Serial2.begin(9600);
//    Serial2.available()
//    Serial2.read();

// -----------------------------------------------------------------------------
void printByte(byte b) {
  for (int i = 7; i >= 0; i--)
    Serial.print(bitRead(b, i));
}

// -----------------------------------------------------------------------------
void setup() {
  // Speed for serial read, which matches the sending program.
  Serial.begin(9600);
  //
  // Speed for logging, only.
  // Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");
  Serial.println("+ Ready for serial communications.");

  //  Serial2.begin(9600);
  //  Serial.println("+ Ready to use the second serial port.");


  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
byte readByte = 0;
void loop() {
  // send data only when you receive data:
  if (Serial.available() > 0) {
    // read the incoming byte:
    readByte = Serial.read();
    if (readByte == 10) {
      // New line character.
      Serial.println("");
    } else {
      Serial.write(readByte);
      /*
      Serial.print("++ Character: ");
      Serial.write(readByte);
      Serial.print(", Byte: ");
      printByte(readByte);
      Serial.print(" = ");
      Serial.print(readByte, DEC);
      */
    }
  }
  delay(30);
}
// -----------------------------------------------------------------------------
