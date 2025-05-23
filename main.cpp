#include "Drone.h"
#include <chrono>
#include <thread>
#include <HTTPClient.h>
#include <ArduinoJson.h>


const char* serverURL = "http://your-server.com/api/endpoint";  

DynamicJsonDocument send_position_to_server(float lon, float lat, float direction) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    // Create Json file for request
    DynamicJsonDocument jsonOut(256);
    jsonOut["position"]["lon"] = lon;
    jsonOut["position"]["lat"] = lat;
    jsonOut["direction"] = direction;

    String requestBody;
    serializeJson(jsonOut, requestBody);

    // Send JSON request
    int httpResponseCode = http.POST(requestBody);

    // prepare object for response
    DynamicJsonDocument response(512);

    if (httpResponseCode > 0) {
        String payload = http.getString();
        deserializeJson(response, payload);
    } else {
        response["err"] = 1;
        response["error_msg"] = http.errorToString(httpResponseCode);
    }

    http.end();
    return response;
}


int main() {
    // Initialize autopilot with components
    Drone drone;

    while(true){
        // Get drone position and compas
        GPSPosition position = drone.gps.get_position();
        double facing = drone.compas.get_azimuth();
        // Send a request to server
        DynamicJsonDocument resp = send_position_to_server(position.longitude, position.latitude, facing);
        if(resp["err"] == 1){
            continue;
        }
        // no updates case
        if(resp["type_"] == "void"){
            continue;
        }
        // auto_pilot_path mode case
        if(resp["type_"] == "auto_pilot_path"){
            for(int i = 0; i < sizeof(resp["points"])/sizeof(resp["points"][0]); i++){
                GPSPosition position;
                position.latitude = resp["points"][i]["lat"];
                position.longitude = resp["points"][i]["lon"];
                drone.goToPoint(position);
            }
        }
        // manual mode case
        if(resp["type_"] == "manual"){
            double time = resp["movement"]["duration"];
            double rotation = resp["rotation"]["angle"];
            if(!resp["movement"]["direction"]){
                rotation = 3.14 - rotation;
            }
            drone.rotate(rotation);
            drone.move(time);
        }
    }

    return 0;
}
