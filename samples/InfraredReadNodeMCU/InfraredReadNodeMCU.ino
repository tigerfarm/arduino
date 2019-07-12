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

    // -----------------------------------
    case 0xFFA25D:
    case 0xFFA25D:
      Serial.print("+ Key 1: ");
      break;
    case 0xFF629D:
    case 0xFF629D:
      Serial.print("+ Key 2: ");
      break;
    case 0xFFE21D:
    case 0xFFE21D:
      Serial.print("+ Key 3: ");
      break;
    case 0xFF22DD:
    case 0xFF22DD:
      Serial.print("+ Key 4: ");
      break;
    case 0xFF02FD:
    case 0xFF02FD:
      Serial.print("+ Key 5: ");
      break;
    case 0xFFC23D:
    case 0xFFC23D:
      Serial.print("+ Key 6: ");
      break;
    case 0xFFE01F:
    case 0xFFE01F:
      Serial.print("+ Key 7: ");
      break;
    case 0xFFA857:
    case 0xFFA857:
      Serial.print("+ Key 8: ");
      break;
    case 0xFF906F:
    case 0xFF906F:
      Serial.print("+ Key 9: ");
      break;
    // -----------------------------------
    case 0xFF10EF:
    case 0xE0E0A659:
      Serial.println("+ Key < - previous");
      break;
    case 0xFF5AA5:
    case 0xE0E046B9:
      Serial.println("+ Key > - next");
      break;
    case 0xE0E016E9:
      Serial.println("+ Key center, OK key");
      break;
    case 0xFF18E7:
    case 0xE0E006F9:
      Serial.print("+ Key up - next directory");
      break;
    case 0xFF4AB5:
    case 0xE0E08679:
      Serial.print("+ Key down - previous directory");
      break;
    // -----------------------------------
    // Single song loop
    case 0xFF6897:
    case 0xE0E01AE5:
      Serial.println("+ Key * (Return)");
      break;
    case 0xFFB04F:
    case 0xE0E0B44B:
      Serial.println("+ Key # (Exit)");
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
  delay(2000);
  blinkLed();
  ++loopCounter;
  Serial.print("+ loopCounter = ");
  Serial.println(loopCounter);
  //
  if (irrecv.decode(&results)) {
    infraredSwitch();
    irrecv.resume();
  }
}

// -----------------------------------------------------------------------------
// eof
