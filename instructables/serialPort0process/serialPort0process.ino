// -----------------------------------------------------------------------------
/*
  Serial Communication processing
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

void processReadByte(byte readByte) {
  Serial.print("++  ");
  switch (readByte) {
    // ----------------------------
    case 10:
      Serial.print("<LF>");
      break;
    case 13:
      Serial.print("<CR>");
    case 32:
      Serial.print("' ' ");
      break;
    // ----------------------------
    default:
      Serial.print(" ");
      Serial.write(readByte);
      Serial.print("  ");
  }
  Serial.print("  ");
  if (readByte < 100) {
    Serial.print(" ");
  }
  if (readByte < 10) {
    Serial.print(" ");
  }
  Serial.print(readByte);
  Serial.print("    ");
  printByte(readByte);
  Serial.print("  ");
  printOctal(readByte);
  Serial.println();
}

// -----------------------------------------------------------------------------
void setup() {
  // Serial speed needs to match the sending program speed such as the Arduino IDE monitor program.
  Serial.begin(9600);             // Sample baud rates: 9600 115200
  delay(1000);
  Serial.println("");               // Newline after garbage characters.
  Serial.println("+++ Setup: Ready for serial communications.");
  Serial.println("+ Go to loop.");
  Serial.println("+   Char  ASCII  Binary    Octal");
}

// -----------------------------------------------------------------------------
// Device Loop
byte readByte = 0;
int readByteCount = 0;
void loop() {
  if (Serial.available() > 0) {
    //
    // Read and process an incoming byte.
    //
    readByte = Serial.read();
    //
    readByteCount++;
    // Re-print the title each 10 lines.
    if (readByteCount > 10) {
      Serial.println("+   Char  ASCII  Binary    Octal");
      // Sample:     "++   a     97    01100001  141"
      readByteCount = 0;
    }
    processReadByte(readByte);
    delay(30);
  }
}
// -----------------------------------------------------------------------------
// eof
