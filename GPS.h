// GPS.h
#pragma once
#include <utility>

class GPS {
public:
    GPS(const char* device = "/dev/serial0");
    std::pair<double, double> get_position(); // lat, lon

private:
    int serial_fd;
};
