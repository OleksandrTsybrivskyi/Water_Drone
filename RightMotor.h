#pragma once
#include <Arduino.h>

/*
 * This class represents a DC motor controlled via an H-bridge (e.g., L298N).
 * It supports bidirectional motion using two direction pins and one PWM pin for speed control.
 */
class RightMotor {
private:
    int in1Pin = 14;     // First direction control pin
    int in2Pin = 12;     // Second direction control pin
    int pwmPin = 13;     // PWM speed control pin
    int pwmChannel = 1; // ESP32 LEDC PWM channel

public:
    /*
     * Constructor initializes the motor control pins and sets up PWM.
     * @param in1  GPIO pin for IN1
     * @param in2  GPIO pin for IN2
     * @param pwm  GPIO pin for enable/PWM
     * @param channel  LEDC PWM channel (0-15)
     */
    RightMotor(){
        // Set pin modes
        pinMode(in1Pin, OUTPUT);
        pinMode(in2Pin, OUTPUT);
        pinMode(pwmPin, OUTPUT);

        // Initialize PWM on the specified channel
        ledcSetup(pwmChannel, 1000, 8); // 1000 Hz, 8-bit resolution
        ledcAttachPin(pwmPin, pwmChannel);
    }

    /*
     * Sets the motor thrust.
     * @param thrust A value from -1.0 to 1.0 where:
     *   -1.0 is full reverse
     *    0.0 is stop
     *   +1.0 is full forward
     */
    void set_thrust(float thrust) {
        thrust = constrain(thrust, -1.0, 1.0); // Clamp to valid range
        int duty = int(abs(thrust) * 255);     // Convert to 0–255 PWM duty

        if (thrust > 0) {
            digitalWrite(in1Pin, HIGH);
            digitalWrite(in2Pin, LOW);
        } else if (thrust < 0) {
            digitalWrite(in1Pin, LOW);
            digitalWrite(in2Pin, HIGH);
        } else {
            // Brake mode: both LOW (or HIGH for short brake)
            digitalWrite(in1Pin, LOW);
            digitalWrite(in2Pin, LOW);
        }

        // Apply PWM duty to control speed
        ledcWrite(pwmChannel, duty);
    }
};
