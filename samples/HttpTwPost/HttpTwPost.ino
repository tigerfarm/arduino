// Code from:
//  https://github.com/TwilioDevEd/twilio_esp8266_arduino_example
// Documentation:
//  https://www.twilio.com/docs/sms/tutorials/how-to-send-sms-messages-esp8266-cpp
// Fingerprint note,
//  https://github.com/TwilioDevEd/twilio_esp8266_arduino_example/issues/1
//  echo | openssl s_client -connect api.twilio.com:443 | openssl x509 -fingerprint
//  ...
//  SHA1 Fingerprint=BC:B0:1A:32:80:5D:E6:E4:A2:29:66:2B:08:C8:E0:4C:45:29:3F:D0
//  ...

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

// Your network SSID and password
const char* ssid = "b";
const char* password = "5";

const char* account_sid = "ACXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
const char* auth_token = "Your AUTH TOKEN";
String from_number      = "16505551111";  // After encoding is added, add "+".
String to_number        = "16505552222";
String message_body     = "Hello.";

// Find the api.twilio.com SHA1 fingerprint using,
//  echo | openssl s_client -connect api.twilio.com:443 | openssl x509 -fingerprint
const char fingerprint[] = "BC B0 1A 32 80 5D E6 E4 A2 29 66 2B 08 C8 E0 4C 45 29 3F D0";

String urlencode(String aString) {
  // Need to actually add this.
  return aString;
}

String get_auth_header(const String& user, const String& password) {
  size_t toencodeLen = user.length() + password.length() + 2;
  char toencode[toencodeLen];
  memset(toencode, 0, toencodeLen);
  snprintf(toencode, toencodeLen, "%s:%s", user.c_str(), password.c_str());
  String encoded = base64::encode((uint8_t*)toencode, toencodeLen - 1);
  String encoded_string = String(encoded);
  std::string::size_type i = 0;
  // Strip newlines (after every 72 characters in spec)
  while (i < encoded_string.length()) {
    i = encoded_string.indexOf('\n', i);
    if (i == -1) {
      break;
    }
    encoded_string.remove(i, 1);
  }
  return "Authorization: Basic " + encoded_string;
}

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
  // Use WiFiClientSecure to create a TLS 1.2 connection.
  //  Note, using a cert fingerprint is required.
  WiFiClientSecure client;
  client.setFingerprint(fingerprint);
  Serial.printf("+ Using fingerprint '%s'\n", fingerprint);
  const char* host = "api.twilio.com";
  const int   httpsPort = 443;
  Serial.print("+ Connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("- Connection failed.");
    return; // Skips to loop();
  }
  Serial.println("+ Connected.");
  Serial.println("+ Send an SMS.");
  String post_data = "To=" + urlencode(to_number)
                     + "&From=" + urlencode(from_number)
                     + "&Body=" + urlencode(message_body);
  String auth_header = get_auth_header(account_sid, auth_token);
  String http_request = "POST /2010-04-01/Accounts/"
                        + String(account_sid) + "/Messages HTTP/1.1\r\n"
                        + auth_header + "\r\n" + "Host: " + host + "\r\n"
                        + "Cache-control: no-cache\r\n"
                        + "User-Agent: ESP8266 Twilio Example\r\n"
                        + "Content-Type: "
                        + "application/x-www-form-urlencoded\r\n"
                        + "Content-Length: " + post_data.length() + "\r\n"
                        + "Connection: close\r\n"
                        + "\r\n"
                        + post_data
                        + "\r\n";
  client.println(http_request);
  // Read the response into the 'response' string
  String response = "";
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    response += (line);
    response += ("\r\n");
  }
  Serial.println("+ Connection is closed.");
  Serial.println("+ Response:");
  Serial.println(response);

  // ----------------------------------------------------
  Serial.println(F("+ Starting the loop."));
}

void loop() {
  delay(1000);
}