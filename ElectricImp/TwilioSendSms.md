--------------------------------------------------------------------------------
# Electric Imp Notes

Overview of steps to make an HTTP request to Twilio.

### Sending a Twilio HTTP Request

[Docs for "http"](https://developer.electricimp.com/api/http)

[Docs for "http post"](https://developer.electricimp.com/api/http/post)

The headers can be any supported by "http", for instance:
````
{ "Content-Type" : "text/xml" }
````

Sample code.
The added header is for basic authentication where the username and password are base64 encoded.
You'll need to replace "AC123...89" and "twilio_account_authtoken", with your own Twilio account SID and auth token.
````
function SendSms(TextMessage) {
   local twilio_account_sid = "AC123...89";
   local url = "https://api.twilio.com/2010-04-01/Accounts/"+twilio_account_sid+"/Messages.json";
   local Body = "From=" + twilioNumber + "&To=" + toNumber + "&Body=" + TextMessage;
   local userpassword = "Basic " + http.base64decode(twilio_account_sid + ":" + "twilio_account_authtoken");
   local request = http.post(url, {"Authorization" : userpassword}, Body);
   request.sendasync(processSmsResponse);
}
````

--------------------------------------------------------------------------------
Cheers