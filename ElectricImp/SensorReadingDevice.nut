// --------------------------------------------------------------------
// Temperature Humidity sensor Library
#require "HTS221.device.lib.nut:2.0.1"
// Air pressure Library
#require "LPS22HB.device.lib.nut:2.0.0"

server.log("+++ Read sensor data and send it to the Agent.");

// --------------------------------------------------------------------
server.log("+ Configure device: I2C settings.");
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

server.log("+ Read temperature and humidity sensor values.");
local resultTh = tempHumid.read();
server.log("+ Read air pressure sensor values.");
local resultAp = airPressureSensor.read();

if ("error" in resultTh) {
    server.log("-- Read sensor error:");
    server.log(resultTh.error);
} else {
    server.log(format("++ Temperature: %0.1f °C", resultTh.temperature));
    server.log(format("++ Humidity: %0.0f", resultTh.humidity));
}
if ("error" in resultAp) {
    server.log("+ Read pressure sensor error:");
    server.log(resultAp.error);
} else {
    server.log(format("++ Air pressure: %0.0f", resultAp.pressure));
}

local counter = 0;
function loop() {
    server.log(format("+ Loop counter = %x", counter++));
    agent.send("reading", resultTh);
    //
    // Loop wait time: 10 seconds.
    imp.wakeup(10, loop);
}

server.log("+ Start the loop.");
loop();

// --------------------------------------------------------------------