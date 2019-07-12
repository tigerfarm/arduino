/****************************
  Infrared Wireless Remote Control Kit for Arduino DIY Project
  https://www.instructables.com/id/Universal-Remote-Using-ESP8266Wifi-Controlled/

*/

// -----------------------------------------------------------------------------
// Infrared settings

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

int IR_PIN = 13;          // Pin D7
IRrecv irrecv(IR_PIN);
decode_results results;

// -----------------------------------------------------------------------------
#define LED_PIN 12
void blinkLed() {
  digitalWrite(LED_PIN, HIGH);   // On
  delay(1000);
  digitalWrite(LED_PIN, LOW);    // Off
}

// -----------------------------------------------------------------------------
// Device Setup

void setup() {

  pinMode(LED_PIN, OUTPUT);
  blinkLed();

  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println("+++ Setup.");
  //
  irrecv.enableIRIn();
}

// -----------------------------------------------------------------------
void infraredSwitch() {
  // Serial.println("+ infraredSwitch");
  //
  switch (results.value) {
    case 0xFFFFFFFF:
      // Ignore. This is from holding the key down.
      break;
    case 0xFF10EF:
    case 0xE0E0A659:
      Serial.println("+ Key < - previous");
      break;
    case 0xE0E016E9:
      Serial.println("+ Key center - E0E016E9");
      break;
    // -----------------------------------
    default:
      Serial.print("+ Result value: ");
      serialPrintUint64(results.value, 16);
      Serial.println("");
      // -----------------------------------
  } // end switch
}

// -----------------------------------------------------------------------------
// Device Loop

int loopCounter = 0;
int iPosition = 0;

void loop() {
  delay(500);
  blinkLed();
  ++loopCounter;
  Serial.print("+ loopCounter = ");
  Serial.println(loopCounter);
  //
  if (irrecv.decode(&results)) {
    // serialPrintUint64(results.value, 16);
    infraredSwitch();
    irrecv.resume();
  }
}

// -----------------------------------------------------------------------------
// eof
