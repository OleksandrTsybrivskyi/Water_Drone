// Motor.h
#pragma once

class Motor {
public:
    Motor(int pwmPin); // GPIO pin
    void set_thrust(double power); // -1.0 to 1.0

private:
    int pwmPin;
    void setPWM(int value); // internal
};
