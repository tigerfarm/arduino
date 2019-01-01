/*
  Clock pulse to counters
*/
int counter1pin = 8;   // counter chip 1
int counter1clock = 11;
int counter2pin = 9;   // counter chip 2
int counter2clock = 12;
//
int counter1readValue = 0;
int counter2readValue = 0;
int counterOutput = 0;
int currentClock = counter1clock;

void pulseSet(int thePin) {
  digitalWrite(thePin, HIGH);
  delay(100);
  digitalWrite(thePin, LOW);
  delay(10);
  // delay(2000);
}

void pulseOneSecond(int thePin) {
  digitalWrite(thePin, HIGH);
  delay(990);
  digitalWrite(thePin, LOW);
  delay(10);
  // delay(2000);
}

void pulse10ms(int thePin) {
  digitalWrite(thePin, HIGH);
  delay(10);
  digitalWrite(thePin, LOW);
  // delay(10);
}

void echoData() {
  if (counterOutput == 7 ){
    counterOutput = 1;
  }
  Serial.print("+ LED = ");
  Serial.print(counterOutput);
  // Serial.print(", counterOutput = ");
  // Serial.print(counterOutput - 1);
  Serial.print(", currentClock = ");
  Serial.print(currentClock);
  Serial.print(", counter1readValue = ");
  Serial.print(digitalRead(counter1pin));
  Serial.print(", counter2readValue = ");
  Serial.println(digitalRead(counter2pin));
}

void setup() {
  Serial.begin(9600);
  Serial.println("+++ Start v1.");
  pinMode(counter1pin, INPUT);
  pinMode(counter2pin, INPUT);
  pinMode(counter1clock, OUTPUT);
  pinMode(counter2clock, OUTPUT);
  // ---------------------------------------
  Serial.println("++ Set counter 1 to LEDs off.");
  currentClock = counter1clock;
  counter1readValue = 0;
  while (counter1readValue != 1) {
    pulseSet(currentClock);
    counter1readValue = digitalRead(counter1pin);
    Serial.print("+ counter1readValue = ");
    Serial.println(counter1readValue);
  }
  Serial.println("++ Set counter 2 to LEDs off.");
  currentClock = counter2clock;
  counter2readValue = 0;
  while (counter2readValue != 1) {
    pulseSet(currentClock);
    counter2readValue = digitalRead(counter2pin);
    Serial.print("+ counter2readValue = ");
    Serial.println(counter2readValue);
  }
  Serial.println("++ Setup completed.");
  delay(2000);  // for testing
  //
  // Set Counter 1, LED 1: On.
  // Counter 2, all LEDs: off
  currentClock = counter1clock;
  pulseOneSecond(currentClock);
  counterOutput = 0;
}

void loop() {
  //
  counterOutput++;
  echoData();
  delay(980);
  pulse10ms(currentClock);
  //
  counter1readValue = digitalRead(counter1pin);
  counter2readValue = digitalRead(counter2pin);
  if ( counter1readValue == 1 &&  counter2readValue == 1) {
    // toggle the counters.
    if (currentClock == counter1clock) {
      currentClock = counter2clock;
    } else {
      currentClock = counter1clock;
    }
    counterOutput = 0;
    //
    pulse10ms(currentClock); // 980ms + pulse10ms + 10 = 1 second.
  } else {
    delay(10); // 980ms + pulse10ms + 10 = 1 second.
  }
}
