// HardDrive.cpp
#include "HardDrive.h"
#include <fstream>

void HardDrive::writeData(std::pair<double, double> pos, double depth) {
    std::ofstream file("log.txt", std::ios::app);
    file << pos.first << "," << pos.second << "," << depth << "\n";
}
