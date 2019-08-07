/*
  Button

  Turns on LED on, when the button is pushed.
  Else the LED is off.

  The circuit:
  - Board is either an Arduino or a NodeMCU.
  - LED ground is attached to a resister which is attached board ground.
  - LED positive is attached to board pin (LED_PIN).
  - Button side 1 is attached board voltage +5V or 3.3V.
  - Button side 2 is attached to a 1K to 10K resistor which is attached to board ground.
  - Button side 2 is attached to board pin (BUTTON_PIN).

  - Note: on most Arduinos, there is an on board LED on pin 13.
*/
// -----------------------------------------------------------------------------

// Built in, on board LED: GPIO2 which is D04 on NodeMCU.
// Built in, on board LED: GPI13 which is D13 on Nano.
//
// Built in LED on NodeMCU, LOW is LED on.
// Built in LED on NodeMCU, HIGH is LED off.
//
// PIN X set to LOW (0) will turn the LED on.
// PIN X set to HIGH (1) will turn the LED off.

const int LED_ONBOARD_PIN =  2;
const int LED_PIN =  LED_ONBOARD_PIN;

const int BUTTON_PIN = 5;    // Pushbutton pin. On NodeMCU, tested using D2(pin 4) D1(pin 5) or D0(pin 16).

void blinkLed() {
  Serial.println("+ LED on.");
  digitalWrite(LED_PIN, HIGH);   // On
  delay(1000);
  digitalWrite(LED_PIN, LOW);    // Off
  Serial.println("+ LED off.");
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println("+++ Setup.");

  // Initialize the onboard LED.
  pinMode(LED_ONBOARD_PIN, OUTPUT);
  // Turn it on for 1 seconds.
  // This is nice for powering up, or clicking the reset button.
  Serial.println("+ Turn on onboard LED for 3 seconds.");
  digitalWrite(LED_ONBOARD_PIN, LOW);   // On
  delay(3000);
  digitalWrite(LED_ONBOARD_PIN, HIGH);  // Off
  
  // Initialize the external LED pin for output.
  pinMode(LED_PIN, OUTPUT);
  // Blink the external LED.
  Serial.println("+ Turn on onboard LED for 3 seconds.");
  digitalWrite(LED_PIN, LOW);   // On
  delay(3000);
  digitalWrite(LED_PIN, HIGH);  // Off
  //
  Serial.println("+ Blink LED.");
  blinkLed();

  // Initialize the pushbutton pin for input:
  pinMode(BUTTON_PIN, INPUT);
}

// -----------------------------------------------------------------------------
void checkButton() {
  // Read the push button status.
  int buttonStatus = digitalRead(BUTTON_PIN);
  // If the button is pressed, the button status is HIGH.
  if (buttonStatus == HIGH) {
    Serial.println("+ Turn LED on.");
    digitalWrite(LED_PIN, HIGH);
  } else {
    Serial.println("+ Turn LED off.");
    digitalWrite(LED_PIN, LOW);
  }
}

// -----------------------------------------------------------------------------
int loopCounter = 0;
void loop() {
  delay(1000);
  ++loopCounter;
  // Serial.print("+ loopCounter = ");
  // Serial.println(loopCounter);

  blinkLed();
  // checkButton();
}

// -----------------------------------------------------------------------------
