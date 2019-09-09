/*
  Clock pulse to counters
*/

int pin8Read = 8;
int pin9Read = 9;
int pin11pulse = 11;
int pin12pulse = 12;
int pulseThis = pin11pulse;
int switch1 = 0;
int switch2 = 0;

int counter = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("+++ Start.");
  pinMode(pin8Read, INPUT);   
  pinMode(pin9Read, INPUT);   
  pinMode(pin11pulse, OUTPUT);
  pinMode(pin12pulse, OUTPUT);
  // ---------------------------------------
  
  Serial.println("++ Set 1.");
  pulseThis = pin11pulse;
  switch1 = 0;
  while (switch1 != 1) {
  switch1 = digitalRead(pin8Read);
  Serial.print(", switch1 = ");
  Serial.println(switch1);
  digitalWrite(pulseThis, HIGH);
  delay(900);
  digitalWrite(pulseThis, LOW);
  delay(100);
  }
  Serial.println("++ Set 2.");
  pulseThis = pin12pulse;
  switch2 = 0;
  while (switch2 != 1) {
  switch2 = digitalRead(pin9Read);
  Serial.print(", switch2 = ");
  Serial.println(switch2);
  digitalWrite(pulseThis, HIGH);
  delay(900);
  digitalWrite(pulseThis, LOW);
  delay(100);
  }
  Serial.println("++ Set.");
  delay(6000);
}

void loop() {
  counter++;
  int switch1 = digitalRead(pin8Read);
  int switch2 = digitalRead(pin9Read);
  if ( switch1 == 1) {
    pulseThis =  pin12pulse;
    digitalWrite(pulseThis, HIGH);
  }
  if (switch2 == 1) {
    pulseThis =  pin11pulse;
  }
  // ---------------------------------------
  Serial.print("Counter = ");
  Serial.print(counter);
  Serial.print(", pulseThis = ");
  Serial.print(pulseThis);
  Serial.print(", switch1 = ");
  Serial.print(switch1);
  Serial.print(", switch2 = ");
  Serial.println(switch2);
  // ---------------------------------------
  digitalWrite(pulseThis, HIGH);
  delay(990);
  digitalWrite(pulseThis, LOW);
  delay(10);
}

