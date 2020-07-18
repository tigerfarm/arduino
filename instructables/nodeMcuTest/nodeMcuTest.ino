// -----------------------------------------------------------------------------
/*
    Test ESP8266 Node MCU board.

    + Blink the on board LED light on and off.
    + Post messages to the serial port, which can be displayed using the Arduino Tools/Serial Monitor.
*/
// -----------------------------------------------------------------------------
// Built in, on board LED: GPI13 which is D13 on Mega, Nano, and Uno. LED is red on Nano.
// Built in, on board LED: GPIO2 which is D04 on NodeMCU.

#define LED_PIN 2       // If using an external LED, use pin D04

boolean ledOn = false;

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200); // 9600 or 115200
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);    // Off
  Serial.println("+ Initialized the on board LED digital pin for output. LED is off.");

  Serial.println("++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

int counter = 0;
void loop() {
  delay(1000);
  counter++;
  Serial.print("+ Loop counter = ");
  Serial.println(counter);
  if (ledOn) {
    digitalWrite(LED_PIN, LOW);   // Off
    ledOn = false;
  } else {
    digitalWrite(LED_PIN, HIGH);    // On
    ledOn = true;
  }
}
// -----------------------------------------------------------------------------
