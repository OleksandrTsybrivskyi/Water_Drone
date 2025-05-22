// Motor.cpp
#include "Motor.h"
#include <wiringPi.h>
#include <softPwm.h>

Motor::Motor(int pwmPin) : pwmPin(pwmPin) {
    wiringPiSetupGpio(); // Use BCM GPIO
    softPwmCreate(pwmPin, 0, 100);
}

void Motor::set_thrust(double power) {
    int pwmValue = static_cast<int>((power + 1.0) * 50); // scale -1..1 to 0..100
    setPWM(pwmValue);
}

void Motor::setPWM(int value) {
    softPwmWrite(pwmPin, value);
}
