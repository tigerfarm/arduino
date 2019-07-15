/*
  Button

  Turns on LED on, when the button is pushed.
  Else the LED is off.

  The circuit:
  - Board is either an Arduino or a NodeMCU.
  - LED ground is attached to a resister which is attached board ground.
  - LED positive is attached to board pin.
  - Button side 1 is attached board voltage +5V or 3.3V.
  - Button side 2 is attached to a 1K to 10K resistor which is attached to board ground.
  - Button side 2 is attached to board pin.

  - Note: on most Arduinos there is already an LED on the board
    attached to pin 13.
*/

// constants won't change. They're used here to set pin numbers:
const int BUTTON_PIN = 16;     // Pushbutton pin: use D2(pin 4) or D1(pin 5) on NodeMCU.
const int LED_PIN =  12;      // LED pin: 13 is for the onboard LED. D6 on NodeMCU.

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status

void blinkLed() {
  digitalWrite(LED_PIN, HIGH);   // On
  delay(1000);
  digitalWrite(LED_PIN, LOW);    // Off
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println("+++ Setup.");

  // initialize the LED pin for output.
  pinMode(LED_PIN, OUTPUT);
  
  // initialize the pushbutton pin for input:
  pinMode(BUTTON_PIN, INPUT);

  Serial.println("+ Blink LED.");
  blinkLed();
}

int loopCounter = 0;
void loop() {
  delay(100);
  ++loopCounter;
  // Serial.print("+ loopCounter = ");
  // Serial.println(loopCounter);
  
  // read the state of the pushbutton value:
  buttonState = digitalRead(BUTTON_PIN);
  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    Serial.println("+ Turn LED on.");
    digitalWrite(LED_PIN, HIGH);
  } else {
    Serial.println("+ Turn LED off.");
    digitalWrite(LED_PIN, LOW);
  }
  
}
