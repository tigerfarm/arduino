// -----------------------------------------------------------------------------
/*
  Test ESP8266 Node MCU board.

  + Blink the on board LED light on and off.
  + Post messages to the serial port, which can be displayed using the Arduino Tools/Serial Monitor.

  https://en.wikipedia.org/wiki/NodeMCU

  ESP8266 NodeMCU Features,
  + 80 MHz clock speed
  + Operating voltage: 3.3V
  + Storage flash memory: 4 MB, SRAM: 64 KB
  + 9 usable digital GPIO pins labeled: D0 to D8.
  + Of the 9, 4 pins can be used for SPI, 2 pins for I2C.
  + Interrupt GPIO pins D0-D8.
  + Do not use the 6 pins: CLK, SD0, CMD, SD1, SD2, SD3 (GPIO 6-11), because they are in use. 
  + Tested: button input using D0-D02.
  + Tested: blink LED using D0-D08. Out to a resister, to an LED, to ground.
  + Need to test, UART1 (TX = GPIO2), Serial1 object: D4 or D7 and D8.

  NodeMCU
  Label   GPIO pin#
  D0      16          GPIO read/write, only. May not have interrupt feature.
  D1      5           Digital GPIO.
  --------------------
  D2      4           Digital GPIO. 
  D3      0           Digital GPIO.
  ----------
  D2      4           I2C:SCL, clock DS3231, PCF8574 input modules
  D3      0           I2C:SDA
  ----------
  D4(TX)  2           Built in, on board LED.
  ----------
  3V      3v output
  G       Ground
  --------------------
  D5      14          Digital GPIO.
  D6      12          Digital GPIO.
  D7(RX)  13          Works for input, for example, infrared receive.
  D8(TX)  15          Doesn't work for input, for example, infrared receive.
  ----------
  D5      14          SD card: SPI SCK
  D6      12          SD card: SPI MISO
  D7(RX)  13          SD card: SPI MOSI
  D8      15          SD card: CS for SPI enable/disable a device. Can use other digital pins.
  --------------------
  RX      03          System uplod from the IDE, which cause reboot after upload.
  TX      01          System uplod.
  G       Ground
  3V      3v output
  ------------------

  Libaries,
  + I installed IRremoteESP8266 version 2.6.3.
  + I also have Arduino WiFi libary installed.

  Interrupt sample:
    https://www.electronicwings.com/nodemcu/nodemcu-gpio-interrupts-with-arduino-ide
  Pin reference,
    https://randomnerdtutorials.com/esp8266-pinout-reference-gpios/
  SPI reference,
    https://nodemcu.readthedocs.io/en/master/modules/spi/
  Reference,
    https://tttapa.github.io/ESP8266/Chap01%20-%20ESP8266.html
*/
// -----------------------------------------------------------------------------
// Built in, on board LED: GPIO2 which is labeled D04 on NodeMCU.

#define LED_PIN 2

boolean ledOn = false;

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200); // 9600 or 115200
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);    // Off
  Serial.println("+ Initialized the on board LED digital pin for output. Onboard LED is off.");

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
