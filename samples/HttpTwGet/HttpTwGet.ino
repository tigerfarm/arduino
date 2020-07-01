// Code from:
//  https://github.com/TwilioDevEd/twilio_esp8266_arduino_example/blob/master/twilio_esp8266_arduino_example.ino
// Documentation:
//  https://www.twilio.com/docs/sms/tutorials/how-to-send-sms-messages-esp8266-cpp

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Your network SSID and password
const char* ssid = "b";
const char* password = "5";

// Find the api.twilio.com SHA1 fingerprint, this one was valid as
// of August 2019.
const char fingerprint[] = "06 86 86 C0 A0 ED 02 20 7A 55 CC F0 75 BB CF 24 B1 D9 C0 49";

const char* account_sid = "ACXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
const char* auth_token = "Your AUTH TOKEN";

void setup() {
  Serial.begin(115200); // 115200 or 9600
  delay(1000);        // Give the serial connection time to start before the first print.
  Serial.println(""); // Newline after garbage characters.
  Serial.println(F("+++ Setup."));

  // ----------------------------------------------------
  Serial.println("+ Connect to WiFi. ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("+ Connected to WiFi, IP address: ");
  Serial.println(WiFi.localIP());

  // ----------------------------------------------------
  // Use WiFiClientSecure class to create TLS 1.2 connection
  WiFiClientSecure client;
  client.setFingerprint(fingerprint);
  const char* host = "api.twilio.com";
  const int   httpsPort = 443;

  // Use WiFiClientSecure class to create TLS 1.2 connection
  Serial.print("+ Connecting to ");
  Serial.println(host);
  Serial.printf("+ Using fingerprint '%s'\n", fingerprint);
  if (!client.connect(host, httpsPort)) {
    Serial.println("- Connection failed.");
    return;
  }
  Serial.print("+ Connected.");

  // ----------------------------------------------------
  Serial.println(F("+ Starting the loop."));
}

void loop() {
  delay(1000);
}
