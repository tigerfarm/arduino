// --------------------------------------------------------------------
// Initial State Library
#require "InitialState.class.nut:1.0.0"

local agentID = split(http.agenturl(), "/").top();
server.log("+ Agent running, ID: " + agentID);
server.log("+ Agent running, URL: " + http.agenturl());

// --------------------------------------------------------------------
const STREAMING_ACCESS_KEY = "i...l";
local iState = InitialState(STREAMING_ACCESS_KEY);
local sensorValues;
local sensorTime;
local sensorTimePtHour;
local sensorTimeMin;
// Open listener for "reading" messages from the device
device.on("tempHumidity", function(tempHumidity) {
    //
    sensorTime = date();
    sensorTimePtHour = sensorTime.hour - 7;
    if (sensorTimePtHour < 0) {
        sensorTimePtHour = sensorTimePtHour + 24;
    }
    if (sensorTime.min < 10) {
        sensorTimeMin = "0" + sensorTime.min;
        // sensorTimeMin = sensorTime.min;
    } else {
        sensorTimeMin = sensorTime.min;
    }
    //
    sensorValues = tempHumidity;
    server.log(
          format("++ Agent, temperature:  %0.1f °C", sensorValues.temperature)
        + format(", humidity:  %0.0f", sensorValues.humidity)
        );
})
local sensorAp;
device.on("airPressure", function(airPressure) {
    sensorAp = airPressure;
    server.log(format("++ Agent, air pressure:  %0.1f °C", sensorAp.temperature));
})

// --------------------------------------------------------------------
// Webserver
function requestHandler(request, response) {
    try {
        // https://agent.electricimp.com/.../abc/def?ghi=123
        // query:(table : 0x7f8ba80a3fb0)
        server.log("+ Incoming request method:" + request.method);
        server.log("+ Incoming request query:" + request.query);
        // path:/abc/def
        server.log("+ Incoming request path:" + request.path);
        if ("ghi" in request.query) {
            server.log("++ Contains: ghi.");
        }
        //
        // Response:
        local theResponse = 
                     "Reading time:           " + sensorTimePtHour + ":" + sensorTime.min
            + "\n"
            + format("+ Sensor, temperature:  %0.1fc", sensorValues.temperature)
            + "\n"
            + format("+ Sensor, humidity:     %0.0f", sensorValues.humidity)
            + "\n"
            + format("+ Sensor, air pressure: %0.0f", sensorAp.pressure);
        response.send(200, theResponse);
    } catch (exp) {
        response.send(500, "Error from here");
    }
}
http.onrequest(requestHandler);

// --------------------------------------------------------------------