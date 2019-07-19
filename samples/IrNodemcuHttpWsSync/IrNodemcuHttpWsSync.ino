// -----------------------------------------------------------------------------
/***
+++ ESP8266 ESP-12E NodeMCU pins used in this project.

Label   Pin:GPIO
D0      16          Button to turn LED on or off.
D1      05          
D2      04
D3      00
D4(TX)  02          Built in, on board LED
---
3V      3v output   Infrared receive: power
G       Ground      Infrared receive: ground
---
D5      14
D6      12          Out to an LED(+), LED(-) to a resister, then to ground.
D7(RX)  13          Infrared receive(RX). Didn't work on D8 which is TX.
D8(TX)  15
RX      03
TX      01
---
G       Ground      To breadboard ground (-).
3V      3v output   To breadboard power (+).
***/

// -----------------------------------------------------------------------------
#include <ESP8266WiFi.h>

// -----------------------------------------------------------------------------
// Infrared settings

#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

int IR_PIN = 13;          // Pin D7
IRrecv irrecv(IR_PIN);
decode_results results;

// -----------------------------------------------------------------------------
// LEDs to identity activities.

// Built in, on board LED: GPIO2 (Arduino pin 2) which is pin D4 on NodeMCU.
// PIN 2 set to LOW (0) will turn the LED on.
// PIN 2 set to HIGH (1) will turn the LED off.
const int LED_ONBOARD_PIN =  2;

#define LED_PIN 12

void blinkLed() {
  digitalWrite(LED_PIN, HIGH);   // On
  delay(1000);
  digitalWrite(LED_PIN, LOW);    // Off
}

// -----------------------------------------------------------------------------
// Push button to reset the game. 

// On NodeMCU, tested using D2(pin 4) D1(pin 5) or D0(pin 16).
const int BUTTON_PIN = 16;

int buttonToggleStatus = 0;
void checkButton() {
  // If the button is pressed, the button status is HIGH.
  if (digitalRead(BUTTON_PIN) == HIGH) {
    Serial.println("+ Button pressed.");
    digitalWrite(LED_PIN, HIGH);
    if (buttonToggleStatus == 0) {
       // Toggle: for the case when the person holds the button down.
       //   Then the HTTP request is only sent once.
       httpGetRequestWithRetry(0, ""); // This will clear the board.
    }
    buttonToggleStatus = 1;
  } else {
    // Serial.println("+ Button not pressed..");
    digitalWrite(LED_PIN, LOW);
    buttonToggleStatus = 0;
  }
}

// -------------------------------------------------------------------------------
// Keypad: 3x3

#include <Keypad.h>

// For a 3x3 keypad. Match the number of rows and columns to keypad.
const byte ROWS = 3;
const byte COLS = 3;

// For keys: 1 ... 9: 3x3.
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'}
};
byte rowPins[ROWS] = { 5, 4, 0};    // D1 D2     D3
byte colPins[COLS] = {14, 3, 9};    // D5 D9(RX) S3+S2: S3 to keypad, use S2 as the pin

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void checkKeypad() {
  char customKey = customKeypad.getKey();
  if (customKey) {
    Serial.print("+ Key pressed: ");
    Serial.println(customKey);
  }
}

// -----------------------------------------------------------------------------
// WIFI SETTINGS: Network name (SSID) and password.

const char *ssid = "BATCAVE";
const char *password = "";  // Note, I don't save my password on the repository.

// -------------------------------------

// For creating a TCP client connection.
WiFiClient client;

// ---------------------------
// Host definition, for HTTP requests
//
// http://tigsync.herokuapp.com/
// $ ping tigsync.herokuapp.com
// PING tigsync.herokuapp.com (34.225.219.245)
// ---
const String hostname = "tigsync.herokuapp.com";
const char *host = "34.225.219.245";
const int httpPort = 80;

// http://localhost:8000/
// $ ping localhost
// PING localhost (127.0.0.1)
// $ ifconfig -a | grep broadcast
//     inet 192.168.1.73
// ---
// const String hostname = "localhost";
// const char *host = "192.168.1.73";
// const int httpPort = 8000;

// ---------------------------
// HTTP request timeout wait time.
int secondsToWaitForResponse = 15;
int responseWaitTime = 500; // Wait between retry, is 1/2 a second.
// Times 2 (*2), because the delay is 1/2 a second.
int timesToRetry = secondsToWaitForResponse * 2;

// Number of times to retry the HTTP request.
int timesToRetryRequest = 3;

// ---------------------------
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
  //
  // Wait for a response, and print the response.
  Serial.println("--- Response ---");
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
  Serial.println();
  Serial.println("-----------------");
  Serial.println("+ Connection closed.");

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

// -----------------------------------------------------------------------
void infraredSwitch() {
  //
  // Top switch case value is for small remote controller.
  // Lower switch case value is for Samsung TV remote controller.
  //
  switch (results.value) {
    case 0xFFFFFFFF:
    case 0xFFFFFFFFFFFFFFFF:
      // Ignore. This is from holding the key down on small remote controller.
      // When holding a key down on the Samsung remote, get arbitrary result values.
      // Serial.print(".");
      break;
    // -----------------------------------
    case 0xFFA25D:
    case 0xE0E020DF:
      Serial.println("+ Key 1: ");
      httpGetRequestWithRetry(1, uriValueValue);
      break;
    case 0xFF629D:
    case 0xE0E0A05F:
      Serial.println("+ Key 2: ");
      httpGetRequestWithRetry(2, uriValueValue);
      break;
    case 0xFFE21D:
    case 0xE0E0609F:
      Serial.println("+ Key 3: ");
      httpGetRequestWithRetry(3, uriValueValue);
      break;
    case 0xFF22DD:
    case 0xE0E010EF:
      Serial.println("+ Key 4: ");
      httpGetRequestWithRetry(4, uriValueValue);
      break;
    case 0xFF02FD:
    case 0xE0E0906F:
      Serial.println("+ Key 5: ");
      httpGetRequestWithRetry(5, uriValueValue);
      break;
    case 0xFFC23D:
    case 0xE0E050AF:
      Serial.println("+ Key 6: ");
      httpGetRequestWithRetry(6, uriValueValue);
      break;
    case 0xFFE01F:
    case 0xE0E030CF:
      Serial.println("+ Key 7: ");
      httpGetRequestWithRetry(7, uriValueValue);
      break;
    case 0xFFA857:
    case 0xE0E0B04F:
      Serial.println("+ Key 8: ");
      httpGetRequestWithRetry(8, uriValueValue);
      break;
    case 0xFF906F:
    case 0xE0E0708F:
      Serial.println("+ Key 9: ");
      httpGetRequestWithRetry(9, uriValueValue);
      break;
    case 0xFF9867:
    case 0xE0E08877:
      Serial.println("+ Key 0: ");
      httpGetRequestWithRetry(0, ""); // This will clear the board.
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
      Serial.println("+ Set to use: X.");
      uriValueValue = "X";
      break;
    case 0xFFB04F:
    case 0xE0E0B44B:
    case 0xE0E0C837:
      Serial.println("+ Key #, button right of '0' or Exit.");
      Serial.println("+ Set to use: O.");
      uriValueValue = "O";
      break;
    // -----------------------------------
    default:
      // Serial.print("+ Result value: ");
      // serialPrintUint64(results.value, 16);
      Serial.print(".");
      // -----------------------------------
  } // end switch

  delay(60);  // To reduce repeat key results.
}

// -----------------------------------------------------------------------------
// Device Setup

void setup() {

  // Initialize the onboard LED.
  pinMode(LED_ONBOARD_PIN, OUTPUT);
  // Turn it on for 1 seconds.
  // This is nice for powering up, or clicking the reset button.
  digitalWrite(LED_ONBOARD_PIN, LOW);   // On
  delay(1000);
  digitalWrite(LED_ONBOARD_PIN, HIGH);  // Off

  pinMode(LED_PIN, OUTPUT);
  // blinkLed();

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

  httpGetRequestWithRetry(0, ""); // Start by clearing the board.

  // Initialized Infrared reader.
  irrecv.enableIRIn();

  // Initialize the pushbutton pin for input:
  pinMode(BUTTON_PIN, INPUT);
}

// -----------------------------------------------------------------------------
// Device Loop

int loopCounter = 0;
int iPosition = 0;

void loop() {
  delay(60);
  ++loopCounter;
  // Serial.print("+ loopCounter = ");
  // Serial.println(loopCounter);
  //
  // Infrared controls
  if (irrecv.decode(&results)) {
    infraredSwitch();
    irrecv.resume();
  }
  //
  // Used to reset the game board.
  checkButton();
  //
  // Used to set game board squares: 1..9.
  // checkKeypad();
}

// -----------------------------------------------------------------------------
