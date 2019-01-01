int pin;
void setup() {
  Serial.begin(9600);
  Serial.println("+++ Start setup.");
  for (pin = 2; pin < 12; pin++) {
    pinMode(pin, OUTPUT);
  }
  Serial.println("+ Setup completed.");
  delay(6000);
}

void loop() {
  for (pin = 2; pin < 12; pin++) {
    Serial.print("+ Pin: ");
    Serial.println(pin);
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
  }
  for (pin = 11; pin > 1; pin--) {
    Serial.print("+ Pin: ");
    Serial.println(pin);
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
  }
}
