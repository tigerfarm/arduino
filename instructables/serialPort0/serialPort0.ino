// -----------------------------------------------------------------------------
/*
  Serial Communications:

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
  Serial.begin(115200);             // 115200
  delay(1000);
  Serial.println("");               // Newline after garbage characters.
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
