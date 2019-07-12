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
  Serial.println("+ Setup completed.");
}

// -----------------------------------------------------------------------
void infraredSwitch() {
  // Serial.println("+ infraredSwitch");
  //
  // Top case value is for small remote controller.
  // Lower case value is for Samsung TV remote controller.
  //
  switch (results.value) {
    case 0xFFFFFFFF:
    case 0xFFFFFFFFFFFFFFFF:
      // Ignore. This is from holding the key down on small remote controller.
      // When holding a key down on the Samsung remote, get arbitrary result values.
      Serial.print(".");
      break;
    // -----------------------------------
    case 0xFFA25D:
    case 0xE0E020DF:
      Serial.println("+ Key 1: ");
      break;
    case 0xFF629D:
    case 0xE0E0A05F:
      Serial.println("+ Key 2: ");
      break;
    case 0xFFE21D:
    case 0xE0E0609F:
      Serial.println("+ Key 3: ");
      break;
    case 0xFF22DD:
    case 0xE0E010EF:
      Serial.println("+ Key 4: ");
      break;
    case 0xFF02FD:
    case 0xE0E0906F:
      Serial.println("+ Key 5: ");
      break;
    case 0xFFC23D:
    case 0xE0E050AF:
      Serial.println("+ Key 6: ");
      break;
    case 0xFFE01F:
    case 0xE0E030CF:
      Serial.println("+ Key 7: ");
      break;
    case 0xFFA857:
    case 0xE0E0B04F:
      Serial.println("+ Key 8: ");
      break;
    case 0xFF906F:
    case 0xE0E0708F:
      Serial.println("+ Key 9: ");
      break;
    case 0xFF9867:
    case 0xE0E08877:
      Serial.println("+ Key 0: ");
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
    case 0xFF38C7:
    case 0xE0E016E9:
      Serial.println("+ Key center, OK key");
      break;
    case 0xFF18E7:
    case 0xE0E006F9:
      Serial.println("+ Key up");
      break;
    case 0xFF4AB5:
    case 0xE0E08679:
      Serial.println("+ Key down");
      break;
    // -----------------------------------
    case 0xFF6897:
    case 0xE0E01AE5:
    case 0xE0E0C43B:
      Serial.println("+ Key *, button left of '0' or Return.");
      break;
    case 0xFFB04F:
    case 0xE0E0B44B:
    case 0xE0E0C837:
      Serial.println("+ Key #, button right of '0' or Exit.");
      break;
    // -----------------------------------
    default:
      Serial.print("+ Result value: ");
      serialPrintUint64(results.value, 16);
      Serial.println("");
      // -----------------------------------
  } // end switch
  
  delay(60);  // To reduce repeat key results.
}

// -----------------------------------------------------------------------------
// Device Loop

int loopCounter = 0;

void loop() {
  // blinkLed();
  delay(60);
  ++loopCounter;
  // Serial.print("+ loopCounter = ");
  // Serial.println(loopCounter);
  //
  if (irrecv.decode(&results)) {
    infraredSwitch();
    irrecv.resume();
  }
}

// -----------------------------------------------------------------------------
// eof
