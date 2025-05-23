# ESP32 Drone Control System

This project implements a basic control system for an ESP32-powered drone. It integrates GPS, compass, and motor control functionality with server communication for automatic and manual navigation.

## Features

- Sends current GPS position and facing direction to a remote server
- Handles different types of server responses:
  - `void`: No change needed
  - `auto_pilot_path`: Receives a list of GPS waypoints and navigates automatically
  - `manual`: Receives direct instructions to rotate and move
- Uses onboard components:
  - GPS sensor via UART
  - Compass (e.g. QMC5883L) via I2C
  - Motor drivers with PWM support

---

## Project Structure
├── Drone.h # Main class encapsulating autopilot logic

├── Compas.h # Class to manage compass data

├── GPS.h # Class to manage gps module

├── LeftMotor.h # Class to manage left motor

├── Right.Motor.h # Class to manage right motor

├── main.cpp # Entry point, contains main loop

├── README.md # This file


---

## Hardware Connections

| Component | Pins Used | Notes |
|----------|-----------|-------|
| GPS      | TX → GPIO16, RX → GPIO17 | UART communication |
| Compass  | SDA → GPIO21, SCL → GPIO22 | I2C |
| Motors   | PWM & direction pins     | Controlled via `LeftMotor` and `RightMotor` classes |

---

## Server Communication

- **Endpoint**: `http://your-server.com/api/endpoint`
- **Request**:
```json
{
  "position": {
    "lon": 54.2,
    "lat": 57.2
  },
  "direction": 3.14
}
```
Responses:

Type: void
```json
{
  "time": "timestamp",
  "err": 0,
  "type_": "void"
}
```
Type: auto_pilot_path
```json
{
  "time": "timestamp",
  "err": 0,
  "type_": "auto_pilot_path",
  "points": [
    { "lon": 54.2, "lat": 57.2 },
    ...
  ]
}
```
Type: manual
```json
{
  "time": "timestamp",
  "err": 0,
  "type_": "manual",
  "movement": {
    "direction": true,
    "duration": 10
  },
  "rotation": {
    "angle": 3.14
  }
}
```
🚀 Getting Started

Connect your ESP32 and sensors according to the scheme.
Upload the firmware using the Arduino IDE or PlatformIO.
Launch drone.

