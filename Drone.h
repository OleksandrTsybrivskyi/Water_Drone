#pragma once
#include <utility>
#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <ctime>
#include "LeftMotor.h"
#include "RightMotor.h"
#include "GPS.h"
#include "Compas.h"


class Drone{
public:
    static constexpr double DRONE_SPEED = 3.0; // m/s
    LeftMotor leftMotor;
    RightMotor rightMotor;
    Compas compas;
    GPS gps;
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
    double computeAngle(GPSPosition startPos, GPSPosition finishPos){
        finishPos.latitude -= startPos.latitude;
        finishPos.longitude -= startPos.longitude;
        double angle = atan2(finishPos.latitude, finishPos.longitude);
        return angle;
    }
    // get distance between 2 points
    // pair<latitude, longitude>
    double computeDistance(GPSPosition startPos, GPSPosition finishPos){
        finishPos.latitude -= startPos.latitude;
        finishPos.longitude -= startPos.longitude;
        double radianDistance = sqrt(pow(finishPos.latitude, 2) + pow(finishPos.longitude, 2));
        double meterDistance = radianDistance * 40'000'000;
        return meterDistance;
    }
    // move for certain amount of time and scan the depth of the sea
    // send data using another function
    void move(double time){
        double stopTime = std::time(0) + time;
        double depth;
        leftMotor.set_thrust(1.0);
        rightMotor.set_thrust(1.0);
        do{
            std::chrono::milliseconds timespan(500);
            std::this_thread::sleep_for(timespan);
        }while(std::time(0) < stopTime);
        leftMotor.set_thrust(0.0);
        rightMotor.set_thrust(0.0);
    }
    // go to point with new GPS cords
    // pair<latitude, longitude>
    void goToPoint(GPSPosition finishPos){
        GPSPosition position;
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
                move(1);
            }while(distance > 5 && std::time(0) < stopTime);
        }while(distance > 5);
    }
};