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
const String hostname = "localhost";
const char *host = "192.168.1.73";
const int httpPort = 8000;

// HTTP request timeout wait time.
int secondsToWaitForResponse = 6;

String theValue = "A";
String uriName = "&name=";
String uriNameValue = "abc";
String uriBasic = "/syncdocumentupdate?identity=esp8266" + uriName + uriNameValue;
String uriValue = "&value=";
String uriValueValue = "A";
String uriPosition = "&position=";
String theUri = uriBasic  + uriValue + uriValueValue + uriPosition;
String uri = "/syncdocumentupdate?identity=esp8266&name=abc&value=" + theValue + "&position=";
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
// Device Setup

void setup() {

  pinMode(LED_PIN, OUTPUT);
  blinkLed();

  Serial.begin(115200);
  delay(100);
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
  }
  Serial.println("");
  Serial.print("++ WiFi connected on IP address: ");
  Serial.println(WiFi.localIP());

  // ------------------------------------------------
  digitalWrite(LED_PIN, LOW);
}

// -----------------------------------------------------------------------------
// Make an HTTP GET request.

void httpGetRequest(int iPosition) {

  digitalWrite(LED_PIN, HIGH);  // Turn the LED on during the request.

  Serial.println("-------------------------------------------------------");
  Serial.print("+ Connecting to: ");
  Serial.print(hostname);
  Serial.print(" IP: ");
  Serial.print(host);
  Serial.print(" Port: ");
  Serial.println(httpPort);

  // Create TCP connection.
  WiFiClient client;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // ------------------------------------------------
  // String uriRequest = uri + iPosition;
  String uriRequest = uriBasic  + uriValue + iPosition + uriPosition + iPosition;
  Serial.print("++ Connected. Make request to the URI: ");
  Serial.println(uriRequest);
  Serial.println("-----------------");
  delay(200);

  // ------------------------------------------------
  // Send GET request and headers.
  //    https://en.wikipedia.org/wiki/List_of_HTTP_header_fields
  // Host header: specifies the domain name for the server to identify the virtual hostname.
  // Optionally, include the TCP listening port:
  //    Host: <host>:<port>
  // Examples:
  //    Host: localhost
  //    Host: localhost:8000
  //
  client.print(
    String("GET ") + uriRequest + " HTTP/1.1"
    + "\r\n"
    + "Host: " + hostname + "\r\n"
    + "User-Agent: NodeMCU/1.0; ESP8266 ESP-12E NodeMCU Lua V3\r\n"
    + "Connection: close\r\n"
    + "\r\n"
  );
  // ------------------------------------------------
  // Wait for a response, and print the response.
  Serial.println("Response:");
  delay(200);

  // Times 2 (*2), because the delay is 1/2 a second.
  int timesToRetry = secondsToWaitForResponse *2;
  //
  int doRetry = 0;
  while (doRetry < timesToRetry) {
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
      doRetry = 99;
    }
    if (doRetry < timesToRetry) {
      doRetry++;
      Serial.print("++ Waiting for response, retry: ");
      Serial.println(doRetry);
      delay(500);
    }
  }

  // ------------------------------------------------
  Serial.println();
  Serial.println("-----------------");
  Serial.println("+ Connection closed.");

  digitalWrite(LED_PIN, LOW);
}

// -----------------------------------------------------------------------------
// Device Loop

int loopCounter = 0;
int iPosition = 0;

void loop() {
  delay(3000);
  ++loopCounter;
  Serial.print("+ loopCounter = ");
  Serial.println(loopCounter);
  //
  iPosition ++;
  if (iPosition > 9) {
    iPosition = 1;
    httpGetRequest(0);  // This will clear the board.
  }
  httpGetRequest(iPosition);
}

// -----------------------------------------------------------------------------
