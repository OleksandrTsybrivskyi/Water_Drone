#pragma once
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <ctime>


const double DRONE_SPEED = 3; // m/s

class AutoPilot{
    Motor leftMotor;
    Motor rightMotor;
    Compas compas;
    GPS gps;
    Echolot echolot;
    Wifi wifi;
    HardDrive hardDrive;
public:
    // stop and rotate to a new angle
    void rotate(double newAngle){
        double currentAngle = compas.get_azimuth();
        if(newAngle - currentAngle < 180){
            leftMotor.set_thrust(1.0);
            rightMotor.set_thrust(-1.0);
            while(compas.get_azimuth() != newAngle){
                std::chrono::milliseconds timespan(100);
                std::this_thread::sleep_for(timespan);
            }
            leftMotor.set_thrust(0.0);
            rightMotor.set_thrust(0.0);
        }
        else{
            leftMotor.set_thrust(-1.0);
            rightMotor.set_thrust(1.0);
            while(compas.get_azimuth() != newAngle){
                std::chrono::milliseconds timespan(100);
                std::this_thread::sleep_for(timespan);
            }
            leftMotor.set_thrust(0.0);
            rightMotor.set_thrust(0.0);
        }
    }
    // compute steering to new point
    // pair<latitude, longitude>
    double computeAngle(std::pair<double, double> startPos, std::pair<double, double> finishPos){
        finishPos.first -= startPos.first;
        finishPos.second -= startPos.second;
        double angle = atan2(finishPos.first, finishPos.second);
        return angle;
    }
    // get distance between 2 points
    // pair<latitude, longitude>
    double computeDistance(std::pair<double, double> startPos, std::pair<double, double> finishPos){
        finishPos.first -= startPos.first;
        finishPos.second -= startPos.second;
        double radianDistance = sqrt(pow(finishPos.first, 2) + pow(finishPos.second, 2));
        double meterDistance = radianDistance * 40'000'000;
        return meterDistance;
    }
    // move for certain amount of time and scan the depth of the sea
    // send data using another function
    void moveAndScan(double time){
        double stopTime = std::time(0) + time;
        double depth;
        leftMotor.set_thrust(1.0);
        rightMotor.set_thrust(1.0);
        do{
            depth = echolot.get_depth();
            position = gps.get_position();
            wifi.send_data(position, depth);
            hardDrive.writeData(position, depth);
            std::chrono::milliseconds timespan(500);
            std::this_thread::sleep_for(timespan);
        }while(std::time(0) < stopTime);
        leftMotor.set_thrust(0.0);
        rightMotor.set_thrust(0.0);
    }
    // go to point with new GPS cords
    // pair<latitude, longitude>
    void goToPoint(std::pair<double, double> finishPos){
        std::pair<double, double> position;
        double angle;
        double distance;
        double stopTime;
        do{
            position = gps.get_position();
            angle = computeAngle(position, finishPos);
            distance = computeDistance(position, finishPos);
            rotate(angle);
            // move to the point while distance to it is bigger than 5 meters
            // or while certain amount of time didnt pass
            // scan depth each second
            stopTime = std::time(0) + distance / DRONE_SPEED;
            do{
                position = gps.get_position();
                distance = computeDistance(position, finishPos);
                moveAndScan(1);
            }while(distance > 5 && std::time(0) < stopTime);
        }while(distance > 5);
    }
};