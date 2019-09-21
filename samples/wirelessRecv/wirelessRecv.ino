// -----------------------------------------------------------------------------
/*
  NRF24L01 is a low powered 2.4 GHz wireless RF Module from Nordic Semiconductors.
    https://www.instructables.com/id/NRF24L01-Wireless-Transmission-Between-Arduino/
    + Can reach up to 300 feet in open space. 3,000 with an external antenna.
    + Uses SPI protocol.
    + Baud rates from 250 kbps up to 2 Mbps.
    + Supply Voltage: 3V
    + Pin Voltage: 5V Tolerant
    + Has a 1 Mhz spacing, operating range of 2.40Ghz – 2.525 GHz.
    + Possible to have 125 independently working modems on a single network.

  Connect the NRF24L01 Wireless board to the Nano:
  + VCC  to Nano 3.3v.
  + GND  to Nano ground.
  + CE   to Nano D7.  Chip Enable.
  + CSN  to Nano D8.  Chip Select Not.
  + MOSI to Nano D11. Master Out Slave In.
  + MISO to Nano D12. Master in Slave Out
  + SCK  to Nano D13. SPI Bus Serial Clock.
  + IRQ  Not used.    Interrupt Pin (active low).

  On board pins     Nano connections
    Top  - Bottom
    VCC  - GND      3.3v - GND
    CSN  - CE        D8  - D7
    MOSI - SCK       D11 - D13
    IRQ  - MISO          - D12
    (closest to the crystal)

  // -----------------------------------------------------------------------------
  Wireless Library:
    TMRh20/RF24, https://github.com/tmrh20/RF24/

  Document from:
    https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/
*/


// -----------------------------------------------------------------------------
// For the Wireless board.

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// RF24 object, two arguments: CE and CSN pins.
RF24 radio(7, 8);

const byte address[6] = "00001";    // any 5 character identity string

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  // Give the serial connection time to start before the first print.
  delay(1000);
  Serial.println("+++ Setup.");

  Serial.println("+ Configure radio receiver.");
  radio.begin();
  radio.openReadingPipe(0, address); // For Receiver.
  radio.setPALevel(RF24_PA_MIN);     // Power Amplifier level set to minimum as rec & trans are close to each other.
  radio.startListening();   // For Receiver.

  Serial.println("+++ Go to loop.");
}

// -----------------------------------------------------------------------------
// Device Loop

void loop() {
  delay(1000);

  // For receiver.
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
  }

  // -----------------------------------------------------------------------------
}
