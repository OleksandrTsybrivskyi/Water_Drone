#pragma once
#include <utility>
#include "Motor.h"
#include "GPS.h"
#include "Compas.h"
#include "Echolot.h"
#include "Wifi.h"
#include "HardDrive.h"

class AutoPilot {
public:
    AutoPilot(Motor& left, Motor& right, GPS& gps, Compas& compas,
              Echolot& echolot, Wifi& wifi, HardDrive& hardDrive);

    void rotate(double newAngle);
    double computeAngle(std::pair<double, double> startPos, std::pair<double, double> finishPos);
    double computeDistance(std::pair<double, double> startPos, std::pair<double, double> finishPos);
    void moveAndScan(double time);
    void goToPoint(std::pair<double, double> finishPos);

private:
    Motor& leftMotor;
    Motor& rightMotor;
    GPS& gps;
    Compas& compas;
    Echolot& echolot;
    Wifi& wifi;
    HardDrive& hardDrive;
};
