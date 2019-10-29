// -----------------------------------------------------------------------------
/*
  Button Sample

  LED and button options, use only one at a time:
  1. blinkLed();
  + Not button, just blink the onboard LED.
  2. checkButton();
  + If the button is not pressed (circuit open), LED blinks.
    If the button is pressed (circuit closed), LED is on.
  3. toggleButton();
  + Toggle, if 
    First press, toggle on, LED is on.
    Second press, toggle off, LED is off.

  The circuit for either an Arduino Uno, Nano, or a NodeMCU.
  - Button side 1, connect to Arduino +5V or NodeMCU +3.3V.
  - Button side 2, connect to a 10K resistor which is connected to ground.
  - Button side 2, connect to board pin (BUTTON_PIN), example: D4 on Nano.
  - Connect an external LED positive is connected to onboard LED pin.
  - Or use the on board LED: on pin 13 on Nano, pin 2 on NodeMCU.

  +++ ESP8266 ESP-12E NodeMCU pins used in this project.
  Label   Pin:GPIO
  D0      16
  D1      05          Button/toggle switch to have LED on only, not blinking
  D2      04
  D3      00
  D4(TX)  02          Built in, on board LED. Out to an LED(+). LED(-) to a resister, then to ground.
  ---
  3V      3v output
  G       Ground
  ---
  D5      14
  D6      12
  D7(RX)  13
  D8(TX)  15
  RX(D9)  03
  TX(D10) 01          .
  ---
  G       Ground      To breadboard ground (-).
  3V      3v output   To breadboard power (+).
  ---

  Note, must not have button connected to TX(D10) when uploading compiled sketch
  because the computer will not connect to the NodeMCU.
  Also, if using TX(D10), if button is pressed, Serial.print will not work.
*/
// -----------------------------------------------------------------------------

//
// Built in LED on NodeMCU, LOW is LED on.
// Built in LED on NodeMCU, HIGH is LED off.
//
// PIN X set to LOW (0) will turn the LED on.
// PIN X set to HIGH (1) will turn the LED off.

// Built in, on board LED: GPIO2 which is D04 on NodeMCU.
// Built in, on board LED: GPI13 which is D13 on Nano and Uno.
#define LED_ONBOARD_PIN 13
#define LED_PIN LED_ONBOARD_PIN

const int BUTTON_PIN = 4;   // Nano D4

// -----------------------------------------------------------------------------
// Blink the LED on and off.

void blinkLed() {
  Serial.println("+ Blink: LED on.");
  digitalWrite(LED_PIN, HIGH);
  delay(300);
  Serial.println("+ Blink: LED off.");
  digitalWrite(LED_PIN, LOW);
  delay(300);
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
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println("+++ Setup.");

  // Initialize the LED pin.
  pinMode(LED_PIN, OUTPUT);
  // Initialize the button pin.
  pinMode(BUTTON_PIN, INPUT);

  Serial.println("+ Start loop()");
}

// -----------------------------------------------------------------------------
void loop() {
  delay(60);
  // LED and button options:
  // blinkLed();
  checkButton();
  // toggleButton();
}

// -----------------------------------------------------------------------------
