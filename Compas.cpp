// Compas.cpp (Mock)
#include "Compas.h"
#include <cstdlib>
#include <ctime>

Compas::Compas() {
    std::srand(std::time(nullptr));
}

double Compas::get_azimuth() {
    return std::rand() % 360; // stub
}
