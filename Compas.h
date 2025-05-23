#pragma once
#include <Wire.h> // I2C communication library

class Compas {
private:
    int sdaPin = 21;         // I2C SDA pin number
    int sclPin = 22;         // I2C SCL pin number
    const uint8_t address = 0x0D; // I2C address of QMC5883L sensor (default)

public:
    // Constructor takes SDA and SCL pins for I2C, starts I2C and initializes sensor
    Compas(){
        Wire.begin(sdaPin, sclPin); // Initialize I2C with custom pins
        initSensor();               // Set sensor to continuous mode
    }

    // Send setup bytes to the compass sensor to enable continuous reading
    void initSensor() {
        Wire.beginTransmission(address);
        Wire.write(0x0B);               // Control register on QMC5883L
        Wire.write(0b00011101);         // Continuous mode, 200Hz, 2G, oversampling x512
        Wire.endTransmission();
    }

    // Read azimuth (heading) in degrees from sensor using X and Y axis
    float get_azimuth() {
        int16_t x, y;

        // Request reading starting from register 0x00 (X LSB)
        Wire.beginTransmission(address);
        Wire.write(0x00);
        Wire.endTransmission();

        // Request 6 bytes: X (2), Y (2), Z (2)
        Wire.requestFrom(address, (uint8_t)6);
        if (Wire.available() == 6) {
            // Read X-axis value (low byte + high byte)
            x = Wire.read() | (Wire.read() << 8);
            // Read Y-axis value
            y = Wire.read() | (Wire.read() << 8);
            // Skip Z-axis (not needed for azimuth)
            Wire.read(); Wire.read();
        } else {
            return -1.0; // Error case: not enough bytes received
        }

        // Calculate heading (angle) in degrees from X and Y
        float angle = atan2((float)y, (float)x) * 180 / PI;

        // Ensure angle is in range [0, 360)
        if (angle < 0) angle += 360;

        return angle;
    }
};
