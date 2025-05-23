#include "AutoPilot.h"

int main() {
    // Initialize autopilot with components
    AutoPilot autopilot(leftMotor, rightMotor, gps, compas, echolot, wifi, hardDrive);

    GPSPosition pos;
    pos.latitude = 10.0;
    pos.longitude = 10.0;
    autopilot.goToPoint(pos);
    // Example usage:
    // autopilot.goToPoint({48.45, 34.98});

    return 0;
}
