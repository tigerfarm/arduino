/*
  Blink
  http://www.arduino.cc/en/Tutorial/Blink
*/

#define LED_PIN 11

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pins for output.
  pinMode(LED_PIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_PIN, HIGH);   // On
  delay(500);                       // wait a second
  digitalWrite(LED_PIN, LOW);    // Off
  delay(500);
}

