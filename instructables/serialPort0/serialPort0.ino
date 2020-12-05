// -----------------------------------------------------------------------------
/*
  Serial Communications:
*/
// -----------------------------------------------------------------------------
void setup() {
  // Serial speed needs to match the sending program speed such as the Arduino IDE monitor program.
  Serial.begin(9600);             // Sample baud rates: 9600 115200
  delay(1000);
  Serial.println("");               // Newline after garbage characters.
  Serial.println("+++ Setup: Ready for serial communications.");
  Serial.println("+ Go to loop.");
  Serial.println("+ Send characters using a serial program such as the Arduino IDE monitor program.");
}

// -----------------------------------------------------------------------------
// Device Loop
void loop() {
  byte readByte = 0;
  if (Serial.available() > 0) {
    // Read and process an incoming byte.
    readByte = Serial.read();
    // Process the byte.
    Serial.write(readByte);
  }
}

// -----------------------------------------------------------------------------
// eof
