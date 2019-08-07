#include <ESP8266WiFi.h>

const char *ssid = "BATCAVE";  // WIFI SETTINGS: Network name (SSID) and password.
// const char *password = "mypassword";
const char *password = "";  // Note, I don't save my password when I upload to the repository.

// -------------------------------------------------------------------------------
// IP address or hostname of the webserver.
const char* host = "tigerfarmpress.com";
// const char *host = "192.168.1.73";
// const char *host = "34.200.203.60";
// String uri = "/syncdocumentupdate?identity=esp8266&name=abc&position=5&value=O"; // "/hello.txt";
//
const String hostname="tigerfarmpress.com";
//
const int httpPort = 80;
String uri = "/hello.txt";

// -------------------------------------------------------------------------------
#define LED_PIN 12
void blinkLed() {
  digitalWrite(LED_PIN, HIGH);   // On
  delay(1000);
  digitalWrite(LED_PIN, LOW);    // Off
}

// -------------------------------------------------------------------------------
void httpGet() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("-------------------------------------------------------");
  Serial.print("+ Connecting to: ");
  Serial.println(host);
  WiFiClient client;  // Use WiFiClient class to create TCP connections
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  // ----------------
  digitalWrite(LED_PIN, LOW);
  Serial.print("++ Connected. Make request to the URL: ");
  Serial.println(uri);
  client.print(
    String("GET ") + uri + " HTTP/1.1\r\n"
    + "Host: " + hostname + "\r\n"
    + "Connection: close\r\n\r\n"
  );
  Serial.println("-----------------");
  delay(200);
  digitalWrite(LED_PIN, HIGH);
  Serial.println("Response:");
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  // -----------------
  digitalWrite(LED_PIN, LOW);
  Serial.println();
  Serial.println("-----------------");
  Serial.println("+ Connection closed.");
}

// -------------------------------------------------------------------------------
int loopCounter = 0;
void setup() {
  pinMode(LED_PIN, OUTPUT);
  blinkLed();
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  // ------------------------------------------------
  Serial.print("+ Connecting to the WiFi network: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
    ++loopCounter;
  }
  Serial.println("");
  Serial.print("++ WiFi connected on IP address: ");
  Serial.println(WiFi.localIP());
  // ------------------------------------------------
  //
  loopCounter = 0;
  digitalWrite(LED_PIN, HIGH);
}

// -------------------------------------------------------------------------------
void loop() {
  delay(10000);
  ++loopCounter;
  Serial.print("+ loopCounter = ");
  Serial.println(loopCounter);
  httpGet();
}
