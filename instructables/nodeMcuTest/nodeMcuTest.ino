// -----------------------------------------------------------------------------
/*
  Test ESP8266 Node MCU board.

  + Blink the on board LED light on and off.
  + Post messages to the serial port, which can be displayed using the Arduino Tools/Serial Monitor.

  https://en.wikipedia.org/wiki/NodeMCU

  Features,
  + Clock Speed: 80 MHz
  + Operating Voltage: 3.3V
  + Storage Flash Memory: 4 MB
  + SRAM: 64 KB
  + SPI and I2C.
  + NodeMCU based ESP8266 has interrupt feature on GPIO pins D0-D8.
  + UART0 RX and TX, for uploading programs from the IDE.
  + UART1 D7(RX) and D8(TX).
  
  NodeMCU
  Label   Pin:GPIO
  D0      16          GPIO read/write, only.
  D1      5           Blinks LED.
  D2      4           Blinks LED.
  D3      0           Blinks LED.
  D4(TX)  2           Built in, on board LED
  3V      3v output   Infrared receive: power
  G       Ground      Infrared receive: ground
  D5      14
  D6      12          Out to a resister, to an LED, to ground.
  D7(RX)  13          Blinks LED. Infrared receive: input (Didn't work on D8 which is TX)
  D8(TX)  15          Blinks LED.
  RX      03
  TX      01
  G       Ground
  3V      3v output
  ------------------
  Label   Pin:GPIO
  S3      10
  S2      09
  VIN     7-12V+

  Libaries,
  + I installed IRremoteESP8266 version 2.6.3.
*/
// -----------------------------------------------------------------------------
// Built in, on board LED: GPI13 which is D13 on Mega, Nano, and Uno. LED is red on Nano.
// Built in, on board LED: GPIO2 which is D04 on NodeMCU.

#define LED_PIN 2       // If using an external LED, use NodeMCU pin labeled: D4.

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
