#include <ESP8266WiFi.h>

const char *ssid = "BATCAVE";  // WIFI SETTINGS: Network name (SSID) and password.
// const char *password = "mypassword";
const char *password = "5198427760";  // Note, I don't save my password when I upload to the repository.

// const char* host = "tigerfarmpress.com";
const char *host = "192.168.1.73";

#define LED_PIN 12
void blinkLed() {
  digitalWrite(LED_PIN, HIGH);   // On
  delay(1000);
  digitalWrite(LED_PIN, LOW);    // Off
}

void httpGet() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("-------------------------------------------------------");
  Serial.print("+ Connecting to: ");
  Serial.println(host);
  WiFiClient client;  // Use WiFiClient class to create TCP connections
  const int httpPort = 8000;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  // ------------------------------------------------
  digitalWrite(LED_PIN, LOW);
  String url = "/hello.txt";
  Serial.print("++ Connected. Make request to the URL: ");
  Serial.println(url);
  client.print(
    String("GET ") + url + " HTTP/1.1\r\n"
    + "Host: " + host + "\r\n"
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
  // ------------------------------------------------
  digitalWrite(LED_PIN, LOW);
  Serial.println();
  Serial.println("-----------------");
  Serial.println("+ Connection closed.");
}

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

void loop() {
  delay(10000);
  ++loopCounter;
  Serial.print("+ loopCounter = ");
  Serial.println(loopCounter);
  httpGet();
}
