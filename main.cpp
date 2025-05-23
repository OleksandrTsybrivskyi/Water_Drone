#include "Drone.h"
#include <chrono>
#include <thread>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include "sonar_config.h"

const char* serverURL = "http://server.com/api/endpoint";

// ���������� � ��������
SoftwareSerial sonarSerial(16, 17); // RX, TX ���� ��� ESP32

// ����� ��� ����� �������
struct SonarData {
    float depth;
    float distance;
    float temperature;
    int signalStrength;
    uint32_t timestamp;
    String objectType;
    bool vesselDetected;
};

SonarData currentSonarReading;
unsigned long lastSonarTransmission = 0;
const unsigned long sonarTransmissionInterval = 5000; // ��������� ���� 5 ������

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
    }
    else {
        response["err"] = 1;
        response["error_msg"] = http.errorToString(httpResponseCode);
    }
    http.end();
    return response;
}

void initializeSonar() {
    // ������� ����������� ��� Cerulean Omniscan 450
    sonarSerial.begin(9600);
    sonarSerial.println("$PCOM,1,115200*");  // ���������� ��������
    delay(500);
    sonarSerial.println("$PCOM,2,1*");       // �������� side scan
    delay(500);
    sonarSerial.println("$PCOM,3,50*");      // ���������� ������� 50�
    delay(500);
    sonarSerial.println("$PCOM,4,HIGH*");    // ������ ����������
    delay(500);
}

bool readSonarData() {
    if (sonarSerial.available()) {
        String data = sonarSerial.readStringUntil('\n');

        // ������� NMEA ���������� �� �������
        if (data.startsWith("$SDDPT")) {
            // �������
            parseDepthData(data);
            return true;
        }
        else if (data.startsWith("$SDMTW")) {
            // ����������� ����
            parseTemperatureData(data);
            return true;
        }
        else if (data.startsWith("$SDVDR")) {
            // Side scan ���
            parseSideScanData(data);
            return true;
        }
    }
    return false;
}

void parseDepthData(String data) {
    // ������� $SDDPT,depth,offset,range*checksum
    int comma1 = data.indexOf(',', 7);
    if (comma1 > 0) {
        currentSonarReading.depth = data.substring(7, comma1).toFloat();
        currentSonarReading.timestamp = millis();
    }
}

void parseTemperatureData(String data) {
    // ������� $SDMTW,temp,C*checksum
    int comma1 = data.indexOf(',', 7);
    if (comma1 > 0) {
        currentSonarReading.temperature = data.substring(7, comma1).toFloat();
    }
}

void parseSideScanData(String data) {
    // ������� side scan ����� ��� ��������� ��'����
    int comma1 = data.indexOf(',', 7);
    int comma2 = data.indexOf(',', comma1 + 1);

    if (comma1 > 0 && comma2 > 0) {
        int signalLeft = data.substring(comma1 + 1, comma2).toInt();
        int signalRight = data.substring(comma2 + 1).toInt();

        currentSonarReading.signalStrength = max(signalLeft, signalRight);
        currentSonarReading.distance = calculateDistance(signalLeft, signalRight);
    }
}

float calculateDistance(int leftSignal, int rightSignal) {
    // ���������� ������ �� ����� ���� ���������� �������
    float timeOfFlight = abs(leftSignal - rightSignal) * 0.001; // �� � �
    float soundSpeed = 1500; // �/� � ���
    return (timeOfFlight * soundSpeed) / 2;
}

void analyzeForVessels() {
    // ������� �������� ��������� �����
    bool largeObject = currentSonarReading.signalStrength > VESSEL_SIGNAL_THRESHOLD;
    bool appropriateDepth = currentSonarReading.depth > MIN_VESSEL_DEPTH &&
        currentSonarReading.depth < MAX_VESSEL_DEPTH;
    bool metallic = currentSonarReading.signalStrength > METAL_REFLECTION_THRESHOLD;

    if (largeObject && appropriateDepth && metallic) {
        currentSonarReading.vesselDetected = true;
        currentSonarReading.objectType = "POTENTIAL_VESSEL";
    }
    else {
        currentSonarReading.vesselDetected = false;
        currentSonarReading.objectType = "CLEAR";
    }
}

void transmitSonarDataToServer(float lon, float lat) {
    HTTPClient http;
    http.begin("http://server.com/api/sonar-data");
    http.addHeader("Content-Type", "application/json");

    // ��������� JSON payload ��� �������
    DynamicJsonDocument doc(1024);
    doc["drone_id"] = "DRONE_SONAR_001";
    doc["timestamp"] = currentSonarReading.timestamp;
    doc["position"]["lon"] = lon;
    doc["position"]["lat"] = lat;
    doc["sonar_data"]["depth"] = currentSonarReading.depth;
    doc["sonar_data"]["distance"] = currentSonarReading.distance;
    doc["sonar_data"]["temperature"] = currentSonarReading.temperature;
    doc["sonar_data"]["signal_strength"] = currentSonarReading.signalStrength;
    doc["sonar_data"]["vessel_detected"] = currentSonarReading.vesselDetected;
    doc["sonar_data"]["object_type"] = currentSonarReading.objectType;

    String jsonString;
    serializeJson(doc, jsonString);

    int httpResponseCode = http.POST(jsonString);
    http.end();
}

void transmitEmergencySonarData(float lon, float lat) {
    // ������� �������� ��� �������� �����
    HTTPClient http;
    http.begin("http://server.com/api/sonar-emergency");
    http.addHeader("Content-Type", "application/json");

    DynamicJsonDocument doc(512);
    doc["alert_type"] = "VESSEL_DETECTED";
    doc["timestamp"] = currentSonarReading.timestamp;
    doc["position"]["lon"] = lon;
    doc["position"]["lat"] = lat;
    doc["sonar_data"]["depth"] = currentSonarReading.depth;
    doc["sonar_data"]["signal_strength"] = currentSonarReading.signalStrength;
    doc["confidence"] = calculateDetectionConfidence();

    String jsonString;
    serializeJson(doc, jsonString);

    http.POST(jsonString);
    http.end();
}

float calculateDetectionConfidence() {
    // ���������� ���������� ���������
    float confidence = 0.0;

    if (currentSonarReading.signalStrength > VESSEL_SIGNAL_THRESHOLD) confidence += 0.4;
    if (currentSonarReading.depth > MIN_VESSEL_DEPTH && currentSonarReading.depth < MAX_VESSEL_DEPTH) confidence += 0.3;
    if (currentSonarReading.signalStrength > METAL_REFLECTION_THRESHOLD) confidence += 0.3;

    return min(confidence, 1.0);
}

int main() {
    // Initialize autopilot with components
    Drone drone;

    // ����������� �������
    initializeSonar();

    while (true) {
        // Get drone position and compas
        GPSPosition position = drone.gps.get_position();
        double facing = drone.compas.get_azimuth();

        // ������� ����� � �������
        if (readSonarData()) {
            // ����� ����� �� �������� �������� ��'����
            analyzeForVessels();

            // �������� ����� ������� �� ������ � ����������
            if (millis() - lastSonarTransmission > sonarTransmissionInterval) {
                transmitSonarDataToServer(position.longitude, position.latitude);
                lastSonarTransmission = millis();
            }

            // ���� �������� ����� - ������� ��������
            if (currentSonarReading.vesselDetected) {
                transmitEmergencySonarData(position.longitude, position.latitude);
            }
        }

        // Send a request to server
        DynamicJsonDocument resp = send_position_to_server(position.longitude, position.latitude, facing);
        if (resp["err"] == 1) {
            continue;
        }
        // no updates case
        if (resp["type_"] == "void") {
            continue;
        }
        // auto_pilot_path mode case
        if (resp["type_"] == "auto_pilot_path") {
            for (int i = 0; i < sizeof(resp["points"]) / sizeof(resp["points"][0]); i++) {
                GPSPosition position;
                position.latitude = resp["points"][i]["lat"];
                position.longitude = resp["points"][i]["lon"];
                drone.goToPoint(position);
            }
        }
        // manual mode case
        if (resp["type_"] == "manual") {
            double time = resp["movement"]["duration"];
            double rotation = resp["rotation"]["angle"];
            if (!resp["movement"]["direction"]) {
                rotation = 3.14 - rotation;
            }
            drone.rotate(rotation);
            drone.move(time);
        }
    }
    return 0;
}