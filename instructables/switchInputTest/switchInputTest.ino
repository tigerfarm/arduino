// -----------------------------------------------------------------------------
/*
  Switch Input Test Logic

  Pull up circuit. Connections,
    Arduino        Switch
    Digital pin to one side of the switch or button.
    Ground to      other side of the switch.
  Use,
    pinMode(inPinUp, INPUT_PULLUP); // Which doesn't require any resister.
  Arduino reads HIGH from the digital pin.
    When the switch is closed (button pushed), the Arduino reads LOW from the digital pin.

  Pull down circuit. Connections,
    Arduino digital pin to one side of the switch or button.
    Ground to 10K ohm resister, to the same side of the switch.
    5V+            to the other side of the switch.
  Where as,
    pinMode(inPinDown, INPUT), requires a pull-down resistor.
  Arduino reads LOW from the digital pin.
    When the switch is closed (button pushed), the Arduino reads HIGH from the digital pin.

  References,
  Digital Input Pull-Up resistor
    https://www.arduino.cc/en/Tutorial/DigitalInputPullup
  Using switches with an Arduino – Input Pull-Up / Pull-Down
    https://www.brainy-bits.com/arduino-input-pullup-tutorial/
*/
// -----------------------------------------------------------------------------
// Button Controls

// Built in LED on NodeMCU, LOW is LED on.
// Built in LED on NodeMCU, HIGH is LED off.
//
// PIN X set to LOW (0) will turn the LED on.
// PIN X set to HIGH (1) will turn the LED off.

// Built in, on board LED: GPIO2 which is D04 on NodeMCU.
// Built in, on board LED: GPI13 which is D13 on Nano and Uno.

#define LED_ONBOARD_PIN 13
#define LED_PIN LED_ONBOARD_PIN

#define LED_TOGGLE_PIN1 A1
#define LED_TOGGLE_PIN2 A2

#define PIN_PULL_DOWN1  6
#define PIN_PULL_DOWN2  7
#define PIN_PULL_UP1    8
#define PIN_PULL_UP2    9

// -----------------------------------------------------------------------------
// Turn light on when the button is pressed.

// Only do the action once, don't repeat if the button is held down.
// Don't repeat action if the button is not pressed.
boolean setPullUpState1 = false;
void checkPullUp1() {
  // When using the PULLUP option, HIGH is not switched, LOW is switched.
  if (digitalRead(PIN_PULL_UP1) == LOW) {
    if (!setPullUpState1) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("+ Pull up pin switch 1, LED on.");
      setPullUpState1 = false;
    }
    setPullUpState1 = true;
  } else {
    if (setPullUpState1) {
      digitalWrite(LED_PIN, LOW);
      Serial.println("+ Pull up pin switch 1, LED off.");
      setPullUpState1 = false;
    }
  }
}

// Toggle light on and off each time the button is pressed.
boolean theTogglePullUp = false;
boolean setPullUpState2 = false;  // Case the button is pressed and held, only toggle once.
void checkPullUp2() {
  // If the button is pressed (circuit closed), the button status is HIGH.
  if (digitalRead(PIN_PULL_UP2) == LOW) {
    if (setPullUpState2) {
      if (theTogglePullUp) {
        theTogglePullUp = false;
        Serial.println("+ Pull up pin switch 2, toggle off.");
        digitalWrite(LED_TOGGLE_PIN1, LOW);
      } else {
        theTogglePullUp = true;
        Serial.println("+ Pull up pin switch 2, toggle on.");
        digitalWrite(LED_TOGGLE_PIN1, HIGH);
      }
    }
    setPullUpState2 = false;
  } else {
    setPullUpState2 = true;
  }
}

// -----------------------------------------------------------------------------
boolean setPullDownState1 = false;
void checkPullDown1() {
  // When using a pull down resister, LOW is not switched, HIGH is switched.
  if (digitalRead(PIN_PULL_DOWN1) == HIGH) {
    if (!setPullDownState1) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("+ Pull down pin switch 1, LED on.");
      setPullDownState1 = false;
    }
    setPullDownState1 = true;
  } else {
    if (setPullDownState1) {
      digitalWrite(LED_PIN, LOW);
      Serial.println("+ Pull down pin switch 1, LED off.");
      setPullDownState1 = false;
    }
  }
}

boolean theTogglePullDown = false;
boolean setPullDownState2 = false;  // Case the button is pressed and held, only toggle once.
void checkPullDown2() {
  // If the button is pressed (circuit closed), the button status is HIGH.
  if (digitalRead(PIN_PULL_DOWN2) == HIGH) {
    if (setPullDownState2) {
      if (theTogglePullDown) {
        theTogglePullDown = false;
        Serial.println("+ Pull down pin switch 2, toggle off.");
        digitalWrite(LED_TOGGLE_PIN2, LOW);
      } else {
        theTogglePullDown = true;
        Serial.println("+ Pull down pin switch 2, toggle on.");
        digitalWrite(LED_TOGGLE_PIN2, HIGH);
      }
    }
    setPullDownState2 = false;
  } else {
    setPullDownState2 = true;
  }
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  // ------------------------------
  pinMode(LED_PIN, OUTPUT);
  pinMode(LED_TOGGLE_PIN1, OUTPUT);
  pinMode(LED_TOGGLE_PIN2, OUTPUT);
  Serial.println("+ LED output pins initialized.");

  // Initialize the button pin.
  pinMode(PIN_PULL_DOWN1, INPUT);         // Requires a 10K resister.
  pinMode(PIN_PULL_DOWN2, INPUT);         // Requires a 10K resister.
  pinMode(PIN_PULL_UP1, INPUT_PULLUP);    // Doesn't require a resister.
  pinMode(PIN_PULL_UP2, INPUT_PULLUP);    // Doesn't require a resister.
  Serial.println("+ Input pins initialized.");

  // ------------------------------
  Serial.println("+++ Go to loop and check for switch, switched.");
}

// -----------------------------------------------------------------------------
// Device Loop

void loop() {
  checkPullUp1();
  checkPullUp2();
  checkPullDown1();
  checkPullDown2();
  delay (100);
}
// -----------------------------------------------------------------------------
