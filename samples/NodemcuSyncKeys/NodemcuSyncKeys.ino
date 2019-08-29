// -----------------------------------------------------------------------------
/***
  +++ ESP8266 ESP-12E NodeMCU pins used in this project.

  IDE: Tools/NodeMCU 1.0 (ESP-12E) Module

  Label   Pin:GPIO
  D0      16          Keypad: row 4
  D1      05          Keypad: row 1
  D2      04          Keypad: row 2
  D3      00          Keypad: row 3
  D4(TX)  02          Built in, on board LED. Out to an LED(+). LED(-) to a resister, then to ground.
  ---
  3V      3v output
  G       Ground
  ---
  D5      14          Keypad: column 1
  D6      12          Keypad: column 2
  D7(RX)  13          
  D8(TX)  15
  RX(D9)  03          Keypad: column 3
  TX(D10) 01          
  ---
  G       Ground      To breadboard ground (-)
  3V      3v output   To breadboard power (+)

  Keypad pins are: first the rows(left), then the columns(right).
  Keypad:
    1-9 game squares
    0   Reset game
    *   Use X.
    #   Use O.
***/
// -----------------------------------------------------------------------------
#include <ESP8266WiFi.h>

#include <Keypad.h>

// Built in, on board LED: GPIO2 (Arduino pin 2) which is pin D4 on NodeMCU.
// PIN 2 set to LOW (0) will turn the LED on.
// PIN 2 set to HIGH (1) will turn the LED off.
#define LED_ONBOARD_PIN 2

#define LED_PIN 2

// -----------------------------------------------------------------------------
// WIFI SETTINGS: Network name (SSID) and password.

const char *ssid = "BATCAVE";
const char *password = "";  // Note, I don't save my password on the repository.

// -----------------------------------------------------------------------------
// For Making HTTP Requests

// For creating a TCP client connection.
WiFiClient client;

// ---------------------------
// Host definition, for HTTP requests
//
// IP address or hostname of the webserver.
const char* host = "tigerfarmpress.com";
const String hostname  = "tigerfarmpress.com";
// ---
const int httpPort = 80;

// ---------------------------
// HTTP request timeout wait time.
int secondsToWaitForResponse = 15;
int responseWaitTime = 500; // Wait between retry, is 1/2 a second.
// Times 2 (*2), because the delay is 1/2 a second.
int timesToRetry = secondsToWaitForResponse * 2;

// Number of times to retry the HTTP request.
int timesToRetryRequest = 3;

// ---------------------------
// http://example.com/syncdocumentupdate?identity=browser&name=abc&position=1&value=B

String uriIdentity = "identity=";
String uriIdentityValue = "nodemcu";
//
String uriName = "name=";
String uriNameValue = "abc";  // Twilio Sync document name.
//
String uriPosition = "position=";
String uriPositionValue = "1";
//
String uriValue = "value=";
String uriValueValue = "X";
//
String uriBasic = "/syncdocumentupdate?"
                  + uriIdentity + uriIdentityValue
                  + "&" + uriName + uriNameValue;
String theUri = uriBasic
                + "&" + uriPosition + uriPositionValue
                + "&" + uriValue + uriValueValue;

// -----------------------------------------------------------------------------
// Make an HTTP GET request.

int httpGetRequest(int iPosition, String sValue) {
  digitalWrite(LED_PIN, HIGH);  // Turn the LED on during the request.

  int returnValue = 0;

  Serial.println("-------------------------------------------------------");
  String uriRequest = uriBasic
                      + "&" + uriValue + sValue
                      + "&" + uriPosition + iPosition;
  Serial.print("+ Make request to the URI: ");
  Serial.println(uriRequest);
  //
  // ------------------------------------------------
  Serial.print("+ Connecting to: ");
  Serial.print(hostname);
  Serial.print(" IP: ");
  Serial.print(host);
  Serial.print(" Port: ");
  Serial.println(httpPort);
  //
  // I should test using the keep-alive header.
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    digitalWrite(LED_PIN, LOW);
    return 1;
  }
  // delay(200);

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
  // Should test:
  //    Connection: keep-alive
  // Sample response header:
  //    Keep-Alive: timeout=5, max=1000
  //
  client.print(
    String("GET ") + uriRequest + " HTTP/1.1"
    + "\r\n"
    + "Host: " + hostname + "\r\n"
    + "User-Agent: NodeMCU/1.0; ESP8266 ESP-12E NodeMCU Lua V3\r\n"
    + "Connection: close\r\n"
    + "\r\n"
  );
  Serial.println("+ HTTP Request sent.");
  //
  // Wait for a response, and print the response.
  // Serial.println("--- Response ---");
  // delay(200);
  // int doRetry = 0;  // Use to get the response.
  int doRetry = 99;  // Use to bypass waiting for the response.
  while (doRetry < timesToRetry) {
    while (client.available()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
      doRetry = 99;
    }
    if (doRetry < timesToRetry) {
      if (doRetry != 0) {
        Serial.print("++ Waiting for response, retry: ");
        Serial.println(doRetry);
      }
      doRetry++;
      delay(responseWaitTime);
    }
  }
  if (doRetry != 99) {
    returnValue = 2;
  }
  // ------------------------------------------------
  // Serial.println();
  Serial.println("+ Connection closed.");
  Serial.println("--------------------");

  digitalWrite(LED_PIN, LOW);
  return returnValue;
}

int httpGetRequestWithRetry(int iPosition, String sValue) {
  //
  // Need further testing on the retry, if the HTTP request failes.
  //
  int doRetry = 0;
  int returnValue = 1;
  while (doRetry < timesToRetryRequest && returnValue != 0) {
    returnValue = httpGetRequest(iPosition, sValue);
    if (returnValue != 0) {
      doRetry++;
      Serial.print("++ HTTP GET failed, retry: ");
      Serial.println(doRetry);
      delay(responseWaitTime);
    }
  }
}

// -------------------------------------------------------------------------------
// For a 4x3 keypad. Match the number of rows and columns to keypad.
const byte ROWS = 4;
const byte COLS = 3;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = { 5,  4, 0, 16};  // D1 D2 D3 D0
byte colPins[COLS] = {14, 12, 3};      // D5 D6 D9(RX)

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

char keyPressed;
void keypadProcess() {
  Serial.print("+ Key pressed: ");
  Serial.println(keyPressed);
  //
  int keyInt = keyPressed - '0';
  if (keyInt >= 0 && keyInt <= 9) {
    Serial.print("+ keyPressed: ");
    Serial.println(keyPressed);
    httpGetRequestWithRetry(keyInt, uriValueValue);
  }
  //
  switch (keyPressed) {
    case '*':
      Serial.println("+ Use: X");
      uriValueValue = "X";
      break;
    case '#':
      Serial.println("+ Use: O");
      uriValueValue = "O";
      break;
  }
}

// -----------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println();
  Serial.println("+++ Setup.");

  // Initialize the onboard LED.
  pinMode(LED_ONBOARD_PIN, OUTPUT);
  // Turn it on for 1 seconds.
  // This is nice for powering up, or clicking the reset button.
  digitalWrite(LED_ONBOARD_PIN, LOW);   // On
  delay(1000);
  digitalWrite(LED_ONBOARD_PIN, HIGH);  // Off

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);   // On

  // ------------------------------------------------
  Serial.print("+ Connecting to the WiFi network: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected on IP address: ");
  Serial.println(WiFi.localIP());

  // ------------------------------------------------
  digitalWrite(LED_PIN, LOW);
  // Initialize the pushbutton pin for input:
  Serial.println("+ Start loop()");
}

// -----------------------------------------------------------------------------
int loopCounter = 0;
int iPosition = 0;
void loop() {
  delay(60);
  // ++loopCounter;
  // Serial.print("+ loopCounter = ");
  // Serial.println(loopCounter);
  //
  if (keyPressed = customKeypad.getKey()) {
    keypadProcess();;
  }
}

// -----------------------------------------------------------------------------
