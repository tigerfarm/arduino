// Initial State Library
#require "InitialState.class.nut:1.0.0"

// Create an account on the following and create an access key.
// https://iot.app.initialstate.com/#/account
//  Account email address/password.
// Add the key as user defined environment variable.
local STREAMING_ACCESS_KEY = __VARS.STREAMING_ACCESS_KEY_1;  // Environment variable
local iState = InitialState(STREAMING_ACCESS_KEY); 

local iState = InitialState(STREAMING_ACCESS_KEY);
local agentID = split(http.agenturl(), "/").top();
server.log("+ Agent running, ID: " + agentID);

// Open listener for "reading" messages from the device
device.on("reading", function(reading) {
    // Log the reading from the device. The reading is a 
    // table, so use JSON encodeing method convert to a string
    server.log(http.jsonencode(reading));
    // Initial State requires the data in a specific structre
    // Build an array with the data from our reading.
    local events = [];
    events.push({"key" : "temperature", "value" : reading.temperature, "epoch" : time()});
    events.push({"key" : "humidity", "value" : reading.humidity, "epoch" : time()});

    // Send reading to Initial State
    iState.sendEvents(events, function(err, resp) {
        if (err != null) {
            // We had trouble sending to Initial State, log the error
            server.error("Error sending to Initial State: " + err);
        } else {
            // A successful send. The response is an empty string, so
            // just log a generic send message
            server.log("Reading sent to Initial State.");
        }
    })
})