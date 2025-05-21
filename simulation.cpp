#include <iostream>
#include <cmath>


class Simulation{
public:

    double latitude, longitude, azimuth, batteryCharge, leftMotorThrust, rightMotorThrust;
    std::string name;
    
    const double MOTOR_MAX_FORWARD_SPEED = 1;  // m/s
    const double MOTOR_MAX_ROTATION_SPEED = 30; // grad/s

    Simulation(std::string droneName, double latitude, double longitude, double azimuth){
        latitude = latitude;
        longitude = longitude;
        azimuth = azimuth;
        batteryCharge = 100.0;
        leftMotorThrust = 0.0;
        rightMotorThrust = 0.0;
    }

    // GPS Module Simulation
    std::pair<double, double> getGps(){
        return std::make_pair(latitude, longitude);
    }

    // Compass Module Simulation
    double getAzimuth(){
        return azimuth;
    }

    // Echolot Module Simulation
    double getDepth(){
        double depth = 50 + 10 * sin(latitude / 10) + 10 * cos(longitude / 10);
        return depth;
    }

    // Battery Charge Module Simulation
    double getBatteryCharge(){
        return batteryCharge;
    }
    void updateBatteryCharge(double newBatteryCharge){
        batteryCharge = newBatteryCharge;
    }

    // Left Motor Control Simulation
    void updateLeftMotorThrust(double newThrust){
        leftMotorThrust = newThrust;
    }

    // Right Motor Control Simulation
    void updateRightMotorThrust(double newThrust){
        rightMotorThrust = newThrust;
    }
};