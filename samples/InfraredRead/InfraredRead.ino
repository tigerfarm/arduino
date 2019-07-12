/****************************
  Infrared Wireless Remote Control Kit for Arduino DIY Project
  https://tutorial45.com/arduino-ir-receiver/

  Install library: IRremote by Shirriff, I installed version: 2.2.3
*/

// -----------------------------------------------------------------------
#include <IRremote.h>

int IR_PIN = 9;
IRrecv irrecv(IR_PIN);
decode_results results;

// -----------------------------------------------------------------------
void setup()
{
  Serial.begin(9600);
  // Serial.begin(115200);
  Serial.println("+ Setup to ");
  //
  irrecv.enableIRIn();
}

// -----------------------------------------------------------------------
void loop() {
  delay(50);
  if (irrecv.decode(&results)) {
    switch (results.value) {
      case 0xFFFFFFFF:
        // Ignore. This is from holding the key down.
        break;
      case 0xFF38C7:
        Serial.println("+ OK");
        break;
      case 0xFF10EF:
        Serial.println("+ < - previous");
        break;
      case 0xFF5AA5:
        Serial.println("+ > - next");
        break;
      case 0xFF18E7:
        Serial.println("+ up - next");
        break;
      case 0xFF4AB5:
        Serial.println("+ down - previous");
        break;
      case 0xFF6897:
        Serial.println("+ * - Loop single on");
        break;
      case 0xFFB04F:
        Serial.println("+ # - Loop single off");
        break;
      case 0xFF9867:
        Serial.println("+ 0 - Pause");
        break;
      //
      case 0x3E81CB71:
        digitalWrite(5, !digitalRead(5));
        break;
      //
      default:
        Serial.print("+ Result value: ");
        Serial.println(results.value, HEX);
    }
    //  For the infrared mini-keypad:
    //  1 : FFA25D
    //  2 : FF629D
    //  3 : FFE21D
    //  4 : FF22DD
    //  5 : FF02FD
    //  6 : FFC23D
    //  7 : FFE01F
    //  8 : FFA857
    //  9 : FF906F
    //  0 : FF9867
    //
    //  * : FF6897
    //  # : FFB04F
    //
    // OK : FF38C7
    //  < : FF10EF
    //  > : FF5AA5
    // up : FF18E7
    // dn : FF4AB5
    //
    //
    irrecv.resume();
  }
}

// -----------------------------------------------------------------------
// eof
