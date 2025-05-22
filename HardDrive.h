// HardDrive.h
#pragma once
#include <utility>

class HardDrive {
public:
    void writeData(std::pair<double, double> pos, double depth);
};
