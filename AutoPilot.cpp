#include "AutoPilot.h"
#include <cmath>
#include <chrono>
#include <thread>
#include <ctime>

AutoPilot::AutoPilot(Motor& left, Motor& right, GPS& gps, Compas& compas,
                     Echolot& echolot, Wifi& wifi, HardDrive& hardDrive)
    : leftMotor(left), rightMotor(right), gps(gps), compas(compas),
      echolot(echolot), wifi(wifi), hardDrive(hardDrive) {}

void AutoPilot::rotate(double newAngle) {
    double currentAngle = compas.get_azimuth();
    if (newAngle - currentAngle < 180) {
        leftMotor.set_thrust(1.0);
        rightMotor.set_thrust(-1.0);
        while (compas.get_azimuth() != newAngle) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    } else {
        leftMotor.set_thrust(-1.0);
        rightMotor.set_thrust(1.0);
        while (compas.get_azimuth() != newAngle) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    leftMotor.set_thrust(0.0);
    rightMotor.set_thrust(0.0);
}

double AutoPilot::computeAngle(std::pair<double, double> startPos, std::pair<double, double> finishPos) {
    finishPos.first -= startPos.first;
    finishPos.second -= startPos.second;
    double angle = atan2(finishPos.first, finishPos.second);
    return angle;
}

double AutoPilot::computeDistance(std::pair<double, double> startPos, std::pair<double, double> finishPos) {
    finishPos.first -= startPos.first;
    finishPos.second -= startPos.second;
    double radianDistance = std::sqrt(std::pow(finishPos.first, 2) + std::pow(finishPos.second, 2));
    double meterDistance = radianDistance * 40'000'000;
    return meterDistance;
}

void AutoPilot::moveAndScan(double time) {
    double stopTime = std::time(0) + time;
    double depth;
    std::pair<double, double> position;

    leftMotor.set_thrust(1.0);
    rightMotor.set_thrust(1.0);

    do {
        depth = echolot.get_depth();
        position = gps.get_position();
        wifi.send_data(position, depth);
        hardDrive.writeData(position, depth);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    } while (std::time(0) < stopTime);

    leftMotor.set_thrust(0.0);
    rightMotor.set_thrust(0.0);
}

void AutoPilot::goToPoint(std::pair<double, double> finishPos) {
    std::pair<double, double> position;
    double angle;
    double distance;
    double stopTime;

    do {
        position = gps.get_position();
        angle = computeAngle(position, finishPos);
        distance = computeDistance(position, finishPos);
        rotate(angle);

        stopTime = std::time(0) + distance; // assuming 1 m/s speed

        do {
            position = gps.get_position();
            distance = computeDistance(position, finishPos);
            moveAndScan(1);
        } while (distance > 5 && std::time(0) < stopTime);

    } while (distance > 5);
}
