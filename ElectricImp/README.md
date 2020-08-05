--------------------------------------------------------------------------------
# Electric Imp Notes

My device is an
Electric Imp [impExplorer Kit](https://developer.electricimp.com/hardware/resources/reference-designs/explorerkit)
````
Sensor      Measured Quantity           I²C Address (8-bit) 	Library 
HTS221      Temperature and humidity    0xBE        HTS221.device.lib.nut
LPS22HB     Air pressure                0xB8        LPS22HB.class.nut
LIS2DH12    Motion in three axes        0x32*       LIS3DH.class.nut
````

--------------------------------------------------------------------------------
## Twilio Electric Imp

[Twilio library](https://developer.electricimp.com/libraries/webservices/twilio),
includes Send an SMS.

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

[I2C](https://developer.electricimp.com/api/hardware/i2c)

--------------------------------------------------------------------------------
## Program Samples

#### Hello World

Hello World server log message program that goes in the Device Code section:
````
server.log("Hello from your imp development device");
````

#### Reading A Sensor

Program to read and echo the sensor data: temperature, humidity, and air pressure

Sample output:
````
2020-08-05T16:53:56.943 +00:00 	[Status] 	Device connected
2020-08-05T16:53:56.981 +00:00 	[Device] 	+ Configure device I2C settings.
2020-08-05T16:53:56.984 +00:00 	[Device] 	+ Configure device: temperature and humidity sensor.
2020-08-05T16:53:56.990 +00:00 	[Device] 	+ Configure device: air pressure sensor.
2020-08-05T16:53:56.991 +00:00 	[Device] 	+ Read temperature humidity sensor values.
2020-08-05T16:53:56.887 +00:00 	[Status] 	Agent restarted: reload.
2020-08-05T16:53:57.492 +00:00 	[Device] 	+ Read air pressure sensor values.
2020-08-05T16:53:57.995 +00:00 	[Device] 	+ Temperature: 28.85 °C
2020-08-05T16:53:57.996 +00:00 	[Device] 	+ Humidity: 49.46
2020-08-05T16:53:57.997 +00:00 	[Device] 	+ Air pressure: 1012.88
````
Program listing:

https://developer.electricimp.com/examples/introduction-applications/reading-sensor

https://developer.electricimp.com/hardware/resources/reference-designs/explorerkit

https://developer.electricimp.com/libraries/hardware/lps22hb

````
// From:
//  https://developer.electricimp.com/examples/introduction-applications/reading-sensor
//  https://developer.electricimp.com/hardware/resources/reference-designs/explorerkit
//  https://developer.electricimp.com/libraries/hardware/lps22hb

// --------------------------------------------------------------------
// Temperature Humidity sensor Library
#require "HTS221.device.lib.nut:2.0.1"
// Air pressure Library
#require "LPS22HB.device.lib.nut:2.0.0"

// --------------------------------------------------------------------
server.log("+ Configure device I2C settings.");
local i2c = hardware.i2c89;             // impExplorer Dev Kit 001
i2c.configure(CLOCK_SPEED_400_KHZ);

server.log("+ Configure device: temperature and humidity sensor.");
const TH_I2C_ADDR = 0xBE;               // temperature and humidity
local tempHumid = HTS221(i2c, TH_I2C_ADDR);
tempHumid.setMode(HTS221_MODE.ONE_SHOT);

server.log("+ Configure device: air pressure sensor.");
const PRESSURE_I2C_ADDR = 0xB8;         // Air pressure
local airPressureSensor = LPS22HB(i2c, PRESSURE_I2C_ADDR);

// --------------------------------------------------------------------

server.log("+ Read temperature humidity sensor values.");
local resultTh = tempHumid.read();
server.log("+ Read air pressure sensor values.");
local resultAp = airPressureSensor.read();

if ("error" in resultTh) {
    server.log("+ Read sensor error:");
    server.log(resultTh.error);
} else {
    server.log(format("+ Temperature: %0.2f °C", resultTh.temperature));
    server.log(format("+ Humidity: %0.2f", resultTh.humidity));
}
if ("error" in resultAp) {
    server.log("+ Read pressure sensor error:");
    server.log(resultAp.error);
} else {
    server.log(format("+ Air pressure: %0.2f", resultAp.pressure));
}

    // Agent not configured/programmed.
    // agent.send("+ Current Temperature: %0.2f °C", result.temperature);
    // agent.send("+ Current Humidity: %0.2f", result.humidity);

// --------------------------------------------------------------------
````
--------------------------------------------------------------------------------
eof