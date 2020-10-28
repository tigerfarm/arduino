--------------------------------------------------------------------------------
# Electric Imp Notes

Development environment: [ImpCentral](https://impcentral.electricimp.com).

### Development Plan

Next:
````
Update Twilio colleagues on my progress, and include the sensor data link.
Send an SMS from my Imp device.
Low battery use. Example, have my Imp outside to get outside sensor values.
Use NodeMCU to retrieve the sensor data.
Use I2C to communications with Arduino devices.
````

Done:
````
Initialize device and online environment.
Program and run Hello world.
Read and echo onboard sensor data.
Send sensor data to the Agent.
Basic webserver processing of a request and making a response.
Basic webserver processing of a GET request when the parameter names are known.
Post sensor data on a webserver.
Setup and use development environment such as: STREAMING_ACCESS_KEY.
````
--------------------------------------------------------------------------------
## Program Samples

#### Hello World

Hello World server log message program that goes in the Device Code section:
````
server.log("Hello from your imp development device");
````

#### Web server functions.

[WebAgent.nut](WebAgent.nut)

#### Reading A Sensor

Program to read and echo the sensor data: temperature, humidity, and air pressure.

Program listing:
[SensorReadingDevice.nut](SensorReadingDevice.nut) and
[SensorReadingAgent.nut](SensorReadingAgent.nut)

Links I used when developing the program:

[Reading a sensor](https://developer.electricimp.com/examples/introduction-applications/reading-sensor),
[Electric Imp imp001 Explorer Kit](https://developer.electricimp.com/hardware/resources/reference-designs/explorerkit)(my hardware),
and
Air pressure Library: [LPS22HB](https://developer.electricimp.com/libraries/hardware/lps22hb).

Sample output:
````
2020-08-05T17:41:33.627 +00:00 	[Status] 	Agent restarted: reload.
2020-08-05T17:41:34.079 +00:00 	[Status] 	Downloading new code; 14.43% program storage used
2020-08-05T17:41:34.491 +00:00 	[Device] 	+++ Read sensor data and send it to the Agent.
2020-08-05T17:41:34.492 +00:00 	[Device] 	+ Configure device: I2C settings.
2020-08-05T17:41:34.494 +00:00 	[Device] 	+ Configure device: temperature and humidity sensor.
2020-08-05T17:41:34.498 +00:00 	[Device] 	+ Configure device: air pressure sensor.
2020-08-05T17:41:34.500 +00:00 	[Device] 	+ Read temperature and humidity sensor values.
2020-08-05T17:41:35.002 +00:00 	[Device] 	+ Read air pressure sensor values.
2020-08-05T17:41:35.505 +00:00 	[Device] 	++ Temperature: 29.9 °C
2020-08-05T17:41:35.506 +00:00 	[Device] 	++ Humidity: 48
2020-08-05T17:41:35.507 +00:00 	[Device] 	++ Air pressure: 1013
````

--------------------------------------------------------------------------------
#### Post Sensor Values on the Internet

[SensorWebDevice.nut](SensorWebDevice.nut)

Inside, home office,
````
Reading date time:      2020:07:06 12:40 Pacific time
+ Sensor, temperature:  30.8c
+ Sensor, humidity:     47
+ Sensor, air pressure: 1012
Reading date time:      2020:07:07 09:31 Pacific time
+ Sensor, temperature:  26.8c
+ Sensor, humidity:     52
+ Sensor, air pressure: 1013
Reading date time:      2020:07:07 11:19 Pacific time
+ Sensor, temperature:  27.9c
+ Sensor, humidity:     48
+ Sensor, air pressure: 1013
Reading date time:      2020:07:10 10:21 Pacific time
+ Sensor, temperature:  27.8c
+ Sensor, humidity:     45
+ Sensor, air pressure: 1011
````
Outside,
````
Reading date time:      2020:07:17 13:30 Pacific time
+ Sensor, temperature:  37.1c
+ Sensor, humidity:     37
+ Sensor, air pressure: 1014
Reading date time:      2020:07:19 17:25 Pacific time
+ Sensor, temperature:  40.2c
+ Sensor, humidity:     32
+ Sensor, air pressure: 1011
Reading date time:      2020:07:24 08:43 Pacific time
+ Sensor, temperature:  27.6c
+ Sensor, humidity:     65
+ Sensor, air pressure: 1011
Reading date time:      2020:08:08 10:31 Pacific time
+ Sensor, temperature:  30.7c
+ Sensor, humidity:     24
+ Sensor, air pressure: 1006
Reading date time:      2020:09:27 08:56 Pacific time
+ Sensor, temperature:  14.6c
+ Sensor, humidity:     52
+ Sensor, air pressure: 1021
````
--------------------------------------------------------------------------------
### Twilio Electric Imp

[Twilio library](https://developer.electricimp.com/libraries/webservices/twilio),
includes Send an SMS.

{
    "ACCOUNT_SID": "your_account_sid",
    "ACCOUNT_AUTH_TOKEN": "your_account_auth_token",
    "ACCOUNT_PHONE_NUMBER": "your_account_phone_number"
}

My device is an
Electric Imp [impExplorer Kit](https://developer.electricimp.com/hardware/resources/reference-designs/explorerkit).

The impExplorer™ Developer Kit
[store link](https://store.electricimp.com/collections/featured-products/products/impexplorer-developer-kit?variant=32325242585148),
$25 for a single unit.

Onboard:
````
Sensor      Measured Quantity           I²C Address (8-bit)  Library (sample program below)
HTS221      Temperature and humidity    0xBE                 HTS221.device.lib.nut
LPS22HB     Air pressure                0xB8                 LPS22HB.class.nut
LIS2DH12    Motion in three axes        0x32*                LIS3DH.class.nut
````
--------------------------------------------------------------------------------
### Links

[Getting started guide](https://developer.electricimp.com/gettingstarted/generic),
which I went through to initialize my device and developer environment.

[User Guide](https://developer.electricimp.com/tools/impcentral/impcentralintroduction)

[Sample code](https://developer.electricimp.com/examples)

##### Documents

[API](https://developer.electricimp.com/api),
[Libraries](https://developer.electricimp.com/integrations),
[Language intro](https://developer.electricimp.com/squirrel/squirrel-guide/introduction)

[format()](https://developer.electricimp.com/squirrel/string/format) for printing out variables.

[http.onrequest(callback)](https://developer.electricimp.com/api/http/onrequest)
Registers a function to be executed on receipt of an incoming HTTP request

[httpresponse.send(statusCode, responseBody)](https://developer.electricimp.com/api/httpresponse/send)

[httpresponse.header(headerName, headerValue)](https://developer.electricimp.com/api/httpresponse/header)

[http.get(url, headers)](https://developer.electricimp.com/api/http/get)

[table.rawget(key)](https://developer.electricimp.com/squirrel/table/rawget)

[System date()](https://developer.electricimp.com/squirrel/system/date)

[Environment variables](https://developer.electricimp.com/tools/impcentral/environmentvariables#user-defined-environment-variables)

[Introduction](https://developer.electricimp.com/resources/i2c)
[I2C](https://developer.electricimp.com/api/hardware/i2c)

--------------------------------------------------------------------------------
Cheers