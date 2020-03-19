// -----------------------------------------------------------------------------
/*
    Test Arduino board.
*/
// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  Serial.println("++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

int counter = 0;
void loop() {
  delay(1000);
  Serial.print("+ Loop counter = ");
  Serial.println(counter);
}
// -----------------------------------------------------------------------------
