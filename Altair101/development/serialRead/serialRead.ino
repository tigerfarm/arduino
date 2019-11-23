// -----------------------------------------------------------------------------
/*
  Serial Communications: Read and Write

  + Read/Write to SD card
  https://www.youtube.com/watch?v=8MvRRNYxy9c

*/

// -----------------------------------------------------------------------------
void printByte(byte b) {
  for (int i = 7; i >= 0; i--)
    Serial.print(bitRead(b, i));
}

// -----------------------------------------------------------------------------
void setup() {
  // Speed for serial read.
  Serial.begin(9600);
  // Speed for logging, only.
  // Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");
  Serial.println("+ Ready for serial communications.");

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
    // Serial.print("++ Byte: ");
    // printByte(readByte);
    // Serial.print(" = ");
    // Serial.println(readByte, DEC);
    Serial.write(readByte);
  }
  delay(30);
}
// -----------------------------------------------------------------------------
