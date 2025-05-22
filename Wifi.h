// Wifi.h
#pragma once
#include <utility>

class Wifi {
public:
    void send_data(std::pair<double, double> pos, double depth);
};
