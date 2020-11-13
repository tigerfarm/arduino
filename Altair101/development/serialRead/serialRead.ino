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

  I'm following the video,
    https://www.youtube.com/watch?v=BdzzyEuUWYk
    
  Reference,
    https://forum.arduino.cc/index.php?topic=396450
    + The 64 byte serial input buffer does not limit the number of receive characters
      because the code in the examples can empty the buffer faster than new data arrives.

0         1         2         3         4         5         6         7         8         9         0         1         2         3
01234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
01214111811111116111111111111111211111111111111111111111111111114111111111111111111111111111111111111111111111111111111111111111811
01214111811111116111111111111111211111111111111111111111111111114111111111111111111111111111111111111111111111111111111111111111811

*/

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
  
  // Speed for serial read, which matches the sending program.
  Serial.begin(115200);   // 115200
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");
  Serial.println("+ Ready for serial communications.");

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop
byte readByte = 0;
int readByteCount = 0;
void loop() {

  if (Serial.available() > 0) {
    // Read and process an incoming byte.
    readByte = Serial.read();
    memoryData[readByteCount];
    readByteCount++;
    //
    if (readByte == 10) {
      // New line character.
      Serial.println("");
    } else {
      Serial.write(readByte);
      if (readByte == 'r') {
        Serial.print("+ r: RUN");
      }
    }
    /*  When displaying only binary data.
      Serial.print("++ Byte: ");
      printByte(readByte);
      Serial.print(" = ");
      printOctal(readByte);
      Serial.print(" = ");
      Serial.print(readByte, DEC);
      Serial.print(", Character: ");
      Serial.write(readByte);
    */
  }
  // delay(30); // Arduino sample code, doesn't use a delay.
}
// -----------------------------------------------------------------------------
