/*
  Blink
  http://www.arduino.cc/en/Tutorial/Blink
*/

// Built in, on board LED: GPIO2 which is D04 on NodeMCU.
// Built in, on board LED: GPI13 which is D13 on Nano and Uno. LED is red on Nano.
#define LED_PIN 13

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pins for output.
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
}

// the loop function runs over and over again forever
int counter = 0;
void loop() {
  counter = counter + 1;
  Serial.print("Counter = ");
  Serial.println(counter);
  digitalWrite(LED_PIN, HIGH);   // On
  delay(500);                       // wait a second
  digitalWrite(LED_PIN, LOW);    // Off
  delay(500);
}
