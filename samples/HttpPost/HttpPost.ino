#include "WiFly.h"
#include "Credentials.h"

// Code from: https://forum.arduino.cc/index.php?topic=155218.0

Timer t;
long lastUpdate=0;
byte server[] = { 106,187,94,198 }; // artiswrong.com
Client client("artiswrong.com", 80);

void setup() {
  Serial.begin(115200);
  WiFly.begin();
  if (!WiFly.join(ssid, passphrase)) {
    Serial.println("Association failed.");
    while (1) {
      // Hang on failure.
    }
  } 
}

void post()
{
  my_account_SID = "my_account_SID";

  Serial.println("connecting...");
  String PostData = "From=16505551111&To=16505552222&Body=Hello1."; 
  Serial.println(PostData);
  String encodedBasicAuthorization = ""; // Base64 encoded string: my_account_SID + ":" + my_account_auth_token;
  if (client.connect()) {
    Serial.println("connected");
    client.println(POST /2010-04-01/Accounts/" + my_account_SID + "/Messages.json HTTP/1.1);
    client.println("Host:  example.com");
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.println("Authorization: Basic " + encodedBasicAuthorization);
    client.println("Content-Type: application/x-www-form-urlencoded;");
    client.print("Content-Length: ");
    client.println(PostData.length());
    client.println();
    client.println(PostData);
    } else {
      Serial.println("connection failed");
    }
}

void loop() {
    post();
    delay(500);
}
