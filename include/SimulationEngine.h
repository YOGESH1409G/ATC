#ifndef SIMULATIONENGINE_H
#define SIMULATIONENGINE_H

#include "Airspace.h"
#include "Radar.h"
#include "Controller.h"

class SimulationEngine {
private:
    Airspace airspace;
    Radar radar;
    Controller controller;
    int maxSteps;       // total simulation steps
    int currentStep;    // current step counter

public:
    // Constructor: set up engine with airspace dimensions and sim parameters
    SimulationEngine(int width, int height, int maxSteps, double safeDistance = 5.0);

    // Add an aircraft to the simulation
    void addAircraft(const Aircraft& aircraft);

    // Run the full simulation loop
    void run();

    // Execute a single simulation step
    void step();

    // Print a header / welcome banner
    void printBanner() const;

    // Print simulation summary at end
    void printSummary() const;
};

#endif // SIMULATIONENGINE_H
