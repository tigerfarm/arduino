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
int inPinUp = 5;
int inPinDown = 6;

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println(""); // Newline after garbage characters.
  Serial.println("+++ Setup.");

  // ------------------------------
  pcfSwitches.begin();
  Serial.println("+ PCF PCF8574 I2C Two Wire module initialized.");

  // ------------------------------
  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

int counter = 0;
void loop() {
  delay (50);
  counter++;
  // 20 is 1 second (20 x 50 = 1000). 40 is every 2 seconds.
  if (counter == 40) {
    Serial.println("---------------------------");
    echoSwitchData();
    counter = 0;
  }
}
// -----------------------------------------------------------------------------
