// Initial State Library
#require "InitialState.class.nut:1.0.0"

local agentID = split(http.agenturl(), "/").top();
server.log("+ Agent running, ID: " + agentID);
server.log("+ Agent running, URL: " + http.agenturl());

// -------------------------------------------
function requestHandler(request, response) {
    try {
        local url = http.agenturl();
        local urlParts = split(url, "/");
        server.log("+ Incoming request 0:" + urlParts[0]
            + " 1:" + urlParts[1]
            + " 2:" + urlParts[2]);
        //
        // https://agent.electricimp.com/.../abc/def?ghi=123
        // query:(table : 0x7f8ba80a3fb0)
        server.log("+ Incoming request method:" + request.method);
        server.log("+ Incoming request query:" + request.query);
        // path:/abc/def
        server.log("+ Incoming request path:" + request.path);
        server.log("+ Number of query parameters:" + request.query.len());
        // local queryArray = [];
        // queryArray.push(request.query);
        //
        // Response:
        if ("ghi" in request.query) {
            server.log("++ request.query ghi value:" + request.query.rawget("ghi"));
            response.send(200, "++ Contains a parameter with ghi, value: "
                + request.query.rawget("ghi"));
        } else {
            response.send(200, "++ Doesn't contains a parameter with ghi.");
        }
    } catch (exp) {
        response.send(500, "Error from here");
    }
}
http.onrequest(requestHandler);

// -------------------------------------------
