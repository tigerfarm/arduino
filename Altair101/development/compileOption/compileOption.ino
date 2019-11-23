// -----------------------------------------------------------------------------
// Test a something to add to Processor.ino.

#define RUN_DELAY 1

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println(F("+++ Setup."));

  Serial.println(F("+++ Start the processor loop."));
}

// -----------------------------------------------------------------------------
// Device Loop for processing each byte of machine code.

#ifdef RUN_DELAY
static unsigned long timer = millis();
#endif
int counter = 0;
void loop() {

#ifdef RUN_DELAY
  Serial.println("+ Delay.");
  // When testing, can add a cycle delay.
  // Clock process timing is controlled by the timer.
  // Example, 50000 : once every 1/2 second.
  if (millis() - timer >= 2000) {
    //  timer = millis();
  }
#endif

  Serial.print(F("+ Loop: "));
  Serial.println(counter);

  delay(500);

}
// -----------------------------------------------------------------------------
