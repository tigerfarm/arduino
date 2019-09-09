b n /*
  Clock pulse to CMOS 4017
  http://www.learnerswings.com/2014/08/amazing-johnson-counter-demonstration.html
*/

int CP1bar = 13; // CP1bar ( Pin 13 ) of 74HC4017 / 74HCT4017 is connected to 13th Pin of Arduino
int CP0 = 12;      // CP0 ( Pin 14 ) of 74HC4017 / 74HCT4017 is connected to 12th Pin of Arduino
int MR = 11;       // MR ( Pin 15 ) of 74HC4017 / 74HCT4017 is connected to 11th Pin of Arduino

// the setup function runs once when you press reset or power the board
void setup() {
  pinMode(CP1bar, OUTPUT);   
  pinMode(CP0, OUTPUT);
  pinMode(MR, OUTPUT);
}

void loop() {
  digitalWrite(CP1bar, LOW);   // Send LOW voltage to CP1bar
  digitalWrite(MR, LOW);         // Send LOW voltage to MR
  digitalWrite(CP0, HIGH);       // Send HIGH voltage to CP0

  delay(990);

  digitalWrite(CP1bar, LOW);   // Send LOW voltage to CP1bar
  digitalWrite(MR, LOW);         // Send LOW voltage to MR
  digitalWrite(CP0, LOW);        // Send LOW voltage to CP0

  delay(10);
}

