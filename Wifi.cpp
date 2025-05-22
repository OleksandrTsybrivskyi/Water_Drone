// Wifi.cpp
#include "Wifi.h"
#include <cstdlib>
#include <string>

void Wifi::send_data(std::pair<double, double> pos, double depth) {
    std::string cmd = "curl -X POST http://your-server/api/data -d \"lat=" +
        std::to_string(pos.first) + "&lon=" + std::to_string(pos.second) +
        "&depth=" + std::to_string(depth) + "\"";
    system(cmd.c_str());
}
