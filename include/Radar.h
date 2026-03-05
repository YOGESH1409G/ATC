#ifndef RADAR_H
#define RADAR_H

#include "Airspace.h"

class Radar {
public:
    // Display a formatted table of all aircraft in the airspace
    void display(const Airspace& airspace) const;

    // Render a simple ASCII grid showing aircraft positions
    void printGrid(const Airspace& airspace) const;
};

#endif // RADAR_H
