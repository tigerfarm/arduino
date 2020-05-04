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
    Arduino        Switch
    Digital pin to one side of the switch or button.
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

const int SWITCH_PIN = 4;

int inPinUp = 5;
int inPinDown = 6;

// -----------------------------------------------------------------------------
// Toggle light on and off each time the button is pressed.

boolean theToggle = true;
boolean buttonAction = true;  // Case the button is pressed and held, only toggle once.
void toggleButton() {
  // If the button is pressed (circuit closed), the button status is HIGH.  
  if (digitalRead(BUTTON_PIN) == HIGH) {
    if (buttonAction) {
      if (theToggle) {
        theToggle = false;
        Serial.println("+ toggleButton(), turn off.");
        digitalWrite(LED_PIN, LOW);
      } else {
        theToggle = true;
        Serial.println("+ toggleButton(), turn on.");
        digitalWrite(LED_PIN, HIGH);
      }
    }
    buttonAction = false;
  } else {
    buttonAction = true;
  }
}

// -----------------------------------------------------------------------------
// Turn light on when the button is pressed.

// Only do the action once, don't repeat if the button is held down.
// Don't repeat action if the button is not pressed.
boolean setButtonState = true;

void checkButton() {
  // If the button is pressed (circuit closed), the button status is HIGH.
  if (digitalRead(BUTTON_PIN) == HIGH) {
    if (!setButtonState) {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("+ checkButton(), turn LED on.");
      setButtonState = false;
    }
    setButtonState = true;
  } else {
    if (setButtonState) {
      digitalWrite(LED_PIN, LOW);
      Serial.println("+ checkButton(), turn LED off.");
      setButtonState = false;
    }
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
  // Initialize the LED pin.
  pinMode(LED_PIN, OUTPUT);
  // Initialize the button pin.
  pinMode(SWITCH_PIN, INPUT);

  // ------------------------------
  Serial.println("+++ Go to loop and check for switch, switched.");
}

// -----------------------------------------------------------------------------
// Device Loop

void loop() {
  delay (50);
  checkButton();
  counter++;
}
// -----------------------------------------------------------------------------
