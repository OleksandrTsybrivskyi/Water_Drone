// Echolot.cpp
#include "Echolot.h"
#include <wiringPi.h>
#include <chrono>
#include <thread>

Echolot::Echolot(int trigPin, int echoPin)
    : trigPin(trigPin), echoPin(echoPin) {
    wiringPiSetupGpio();
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
}

double Echolot::get_depth() {
    digitalWrite(trigPin, LOW);
    std::this_thread::sleep_for(std::chrono::microseconds(2));
    digitalWrite(trigPin, HIGH);
    std::this_thread::sleep_for(std::chrono::microseconds(10));
    digitalWrite(trigPin, LOW);

    while (digitalRead(echoPin) == LOW);
    auto start = std::chrono::high_resolution_clock::now();

    while (digitalRead(echoPin) == HIGH);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    double distance = (duration * 0.0343) / 2 / 100; // in meters
    return distance;
}
