#include "AutoPilot.h"

int main() {
    // Create component objects
    Motor leftMotor, rightMotor;
    GPS gps;
    Compas compas;
    Echolot echolot;
    Wifi wifi;
    HardDrive hardDrive;

    // Initialize autopilot with components
    AutoPilot autopilot(leftMotor, rightMotor, gps, compas, echolot, wifi, hardDrive);

    // Example usage:
    // autopilot.goToPoint({48.45, 34.98});

    return 0;
}
