/*
  Blink
  http://www.arduino.cc/en/Tutorial/Blink
*/

// #define LED_PIN 11
#define LED_PIN 13  // Onboard LED

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pins for output.
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("+ Setup complete.");
}

// the loop function runs over and over again forever
int counter = 0;
void loop() {
  counter = counter + 1;
  Serial.print("Counter = ");
  Serial.println(counter);
  digitalWrite(LED_PIN, HIGH);   // On
  delay(500);                    // On:  for 1/2 a second
  digitalWrite(LED_PIN, LOW);    // Off
  delay(2000);                   // Off: for 2 seconds
}
