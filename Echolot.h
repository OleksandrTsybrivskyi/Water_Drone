// Echolot.h
#pragma once

class Echolot {
public:
    Echolot(int trigPin, int echoPin);
    double get_depth(); // in meters

private:
    int trigPin;
    int echoPin;
};
