// -----------------------------------------------------------------------------
/*
  Button Sample

  If the button is not pressed (circuit open), LED blinks.
  If the button is pressed (circuit closed), LED is on.

  The circuit:
  - Board is either an Arduino Uno, Nano, or a NodeMCU.
  - LED positive is connected to onboard LED pin.
  - Button side 1, connected to Arduino +5V or NodeMCU +3.3V.
  - Button side 2, connected to a 10K resistor which is connected to ground.
  - Button side 2, connected to board pin (BUTTON_PIN), example: D11 on Nano.

  - Note: on most Arduinos, there is an on board LED on pin 13.

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

// const int BUTTON_PIN = 5;   // NodeMCU D1
const int BUTTON_PIN = 2;   // Nano D2

// -----------------------------------------------------------------------------
// Blink the LED on and off.

void blinkLed() {
  Serial.println("+ Blink: LED on.");
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  Serial.println("+ Blink: LED off.");
  digitalWrite(LED_PIN, LOW);
}

// -----------------------------------------------------------------------------
// Turn light on when the button is pressed.

void checkButton() {
  
  // Read the push button status.
  int buttonStatus = digitalRead(BUTTON_PIN);
  
  // If the button is pressed (circuit closed), the button status is HIGH.
  if (buttonStatus == HIGH) {
    Serial.println("+ checkButton(), turn LED on.");
    digitalWrite(LED_PIN, HIGH);
  } else {
    Serial.println("+ checkButton(), turn LED off.");
    digitalWrite(LED_PIN, LOW);
    // blinkLed();
  }
}

// -----------------------------------------------------------------------------
// Toggle light on and off each time the button is pressed.

boolean theToggle = true;
boolean buttonAction = true;  // Case the button is pressed and held, only toggle once.

void toggleButton() {
  
  // Read the push button status.
  int buttonStatus = digitalRead(BUTTON_PIN);
  
  // If the button is pressed (circuit closed), the button status is HIGH.  
  if (buttonStatus == HIGH) {
    if (buttonAction) {
      if (theToggle) {
        theToggle = false;
        Serial.println("+ toggleButton(), turn LED off.");
        digitalWrite(LED_PIN, LOW);
      } else {
        theToggle = true;
        Serial.println("+ toggleButton(), turn LED on.");
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

  // Initialize the onboard LED.
  pinMode(LED_PIN, OUTPUT);
  // Blink the external LED.
  Serial.println("+ Start loop()");
}

// -----------------------------------------------------------------------------
int loopCounter = 0;
void loop() {
  delay(500);
  ++loopCounter;
  // Serial.print("+ loopCounter = ");
  // Serial.println(loopCounter);

  // LED and button options:
  // blinkLed();
  // checkButton();
  toggleButton();
}

// -----------------------------------------------------------------------------
