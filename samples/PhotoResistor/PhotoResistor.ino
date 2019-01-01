const int pResistor = A0;
const int ledPin=11;

void setup() {
  Serial.begin(9600);
  pinMode(pResistor, INPUT);
  pinMode(ledPin, OUTPUT);
}

int value = 0;
void loop() {
  value = analogRead(pResistor);
  Serial.print("Value = ");
  Serial.println(value);
    if (value < 800) {
    digitalWrite(ledPin, HIGH);
  }
  else{
    digitalWrite(ledPin, LOW);
  }
  delay(500);
}
