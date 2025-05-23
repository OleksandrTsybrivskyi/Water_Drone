#pragma once
#include <HardwareSerial.h>

struct GPSPosition {
    float latitude;
    float longitude;
};

class GPS {
private:
    HardwareSerial& gpsSerial = Serial2;   
    const int txPin = 17;            // UART0 TX pin (GPIO1)
    const int rxPin = 16;            // UART0 RX pin (GPIO3)
    String buffer = "";             // Buffer for incoming serial data

public:
    // Constructor takes which UART to use (UART0 in this case)
    GPS(){
        gpsSerial.begin(9600, SERIAL_8N1, rxPin, txPin); // 9600 baud, 8 data bits, no parity, 1 stop bit
    }

    // Parse latitude or longitude from NMEA format
    float convertToDecimalDegrees(String raw, String direction) {
        float degMin = raw.toFloat();
        int degrees = int(degMin / 100);
        float minutes = degMin - (degrees * 100);
        float decimal = degrees + (minutes / 60.0);

        if (direction == "S" || direction == "W") {
            decimal = -decimal;
        }

        return decimal;
    }

    // Read GPS position from serial, parse NMEA sentence and return latitude + longitude
    GPSPosition get_position() {
        GPSPosition pos = {0.0, 0.0};

        while (gpsSerial.available()) {
            char c = gpsSerial.read();

            if (c == '\n') {
                if (buffer.startsWith("$GPGGA") || buffer.startsWith("$GPRMC")) {
                    // Split sentence into comma-separated parts
                    int fieldIndex = 0;
                    String fields[15];
                    for (int i = 0; i < buffer.length(); i++) {
                        if (buffer[i] == ',' || buffer[i] == '*') {
                            fieldIndex++;
                        } else {
                            fields[fieldIndex] += buffer[i];
                        }
                    }

                    // Example: $GPRMC sentence
                    // Field 3: latitude, Field 4: N/S
                    // Field 5: longitude, Field 6: E/W
                    if (fields[3].length() > 0 && fields[5].length() > 0) {
                        pos.latitude = convertToDecimalDegrees(fields[3], fields[4]);
                        pos.longitude = convertToDecimalDegrees(fields[5], fields[6]);
                    }
                }

                buffer = ""; // Clear buffer for next sentence
            } else {
                buffer += c; // Keep building buffer
            }
        }

        return pos;
    }
};
