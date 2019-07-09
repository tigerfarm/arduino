#include <ESP8266WiFi.h>

// -----------------------------------------------------------------------------

// WIFI SETTINGS: Network name (SSID) and password.
const char *ssid = "BATCAVE";  
const char *password = "";  // Note, I don't save my password on the repository.

// -----------------------------------------------------------------------------
// Host definition, for HTTP requests

// http://tigsync.herokuapp.com/
// $ ping tigsync.herokuapp.com
// PING tigsync.herokuapp.com (34.225.219.245)
// const String hostname="tigsync.herokuapp.com";
// const char *host = "34.225.219.245";
// const int httpPort = 80;

// http://localhost:8000/
// $ ping localhost
// PING localhost (127.0.0.1)
// $ ifconfig -a | grep broadcast
//     inet 192.168.1.73
const String hostname="localhost";
const char *host = "192.168.1.73";
const int httpPort = 8000;

String theValue = "A";
String uri = "/syncdocumentupdate?identity=esp8266&name=abc&value=" + theValue + "&position=" + "5";
// String uri = "/syncdocumentupdate?identity=esp8266&name=abc&position=5&value=O";
// String uri = "/hello";
// String uri = "/hello.txt";

// -----------------------------------------------------------------------------
#define LED_PIN 12
void blinkLed() {
  digitalWrite(LED_PIN, HIGH);   // On
  delay(1000);
  digitalWrite(LED_PIN, LOW);    // Off
}

// -----------------------------------------------------------------------------
// Make an HTTP GET request.

void httpGet() {

  // ------------------------------------------------
  digitalWrite(LED_PIN, HIGH);
  Serial.println("-------------------------------------------------------");
  Serial.print("+ Connecting to: ");
  Serial.println(host);

  // Create TCP connection.
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // ------------------------------------------------
  Serial.print("++ Connected. Make request to the URI: ");
  Serial.println(uri);
  Serial.println("-----------------");
  delay(200);

  // ------------------------------------------------
  // Send GET request and headers.
  // Host header: specifies the domain name of the server, for virtual hosting; and optionally, the TCP listening port number:
  //    Host: <host>:<port>
  client.print(
    String("GET ") + uri + " HTTP/1.1\r\n"
    + "Host: " + hostname + "\r\n"
    + "Connection: close\r\n\r\n"
  );
  Serial.println("Response:");
  delay(2000);
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

// -----------------------------------------------------------------------------
// Device Setup

int loopCounter = 0;
void setup() {
  pinMode(LED_PIN, OUTPUT);
  blinkLed();
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println("+++ Setup.");
  //
  // ------------------------------------------------
  digitalWrite(LED_PIN, HIGH);
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
  digitalWrite(LED_PIN, LOW);
}

// -----------------------------------------------------------------------------
// Device Loop

void loop() {
  delay(10000);
  ++loopCounter;
  Serial.print("+ loopCounter = ");
  Serial.println(loopCounter);
  httpGet();
}

// -----------------------------------------------------------------------------
