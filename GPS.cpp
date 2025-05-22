// GPS.cpp
#include "GPS.h"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string>
#include <sstream>

GPS::GPS(const char* device) {
    serial_fd = open(device, O_RDONLY | O_NOCTTY);
    termios tty{};
    tcgetattr(serial_fd, &tty);
    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);
    tty.c_cflag |= (CLOCAL | CREAD);
    tcsetattr(serial_fd, TCSANOW, &tty);
}

std::pair<double, double> GPS::get_position() {
    char buf[256];
    read(serial_fd, buf, sizeof(buf));
    std::string line(buf);

    // Parse GPGGA or GPRMC (very simplified!)
    size_t pos = line.find("$GPGGA");
    if (pos != std::string::npos) {
        std::istringstream ss(line.substr(pos));
        std::string token;
        std::getline(ss, token, ','); // $GPGGA
        std::getline(ss, token, ','); // time
        std::getline(ss, token, ','); // lat
        double lat = std::stod(token.substr(0, 2)) + std::stod(token.substr(2)) / 60.0;
        std::getline(ss, token, ','); // N/S
        if (token == "S") lat = -lat;
        std::getline(ss, token, ','); // lon
        double lon = std::stod(token.substr(0, 3)) + std::stod(token.substr(3)) / 60.0;
        std::getline(ss, token, ','); // E/W
        if (token == "W") lon = -lon;
        return {lat, lon};
    }

    return {0.0, 0.0}; // fallback
}
