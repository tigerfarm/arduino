/*
  Blink
  http://www.arduino.cc/en/Tutorial/Blink
*/

// #define LED_PIN 11
#define LED_PIN 13  // Onboard LED

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  // initialize digital pin for output.
  pinMode(LED_PIN, OUTPUT);

  Serial.println("++ Go to loop.");
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
  delay(500);                    // Off: for 1/2 a second
}
