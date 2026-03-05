#include "SimulationEngine.h"
#include <iostream>
#include <thread>
#include <chrono>

// ─── Constructor (from dimensions) ──────────────────────────────────────────
SimulationEngine::SimulationEngine(int width, int height, int maxSteps, double safeDistance)
    : airspace(width, height),
      controller(safeDistance),
      maxSteps(maxSteps),
      currentStep(0) {}

// ─── Constructor (from pre-built objects) ───────────────────────────────────
// Accepts an Airspace that already has aircraft added, plus Radar & Controller.
SimulationEngine::SimulationEngine(Airspace airspace, Radar radar, Controller controller, int maxSteps)
    : airspace(airspace),
      radar(radar),
      controller(controller),
      maxSteps(maxSteps),
      currentStep(0) {}

// ─── Add Aircraft ───────────────────────────────────────────────────────────
void SimulationEngine::addAircraft(const Aircraft& aircraft) {
    airspace.addAircraft(aircraft);
}

// ─── Run Full Simulation ────────────────────────────────────────────────────
// Main entry point.  Runs the loop for maxSteps iterations.
// Each step executes:  update positions → detect collisions → display radar.
// A 500 ms delay is inserted between steps for console readability.
void SimulationEngine::run() {
    printBanner();

    std::cout << "\n  Airspace: " << airspace.getWidth() << " x " << airspace.getHeight()
              << "  |  Aircraft: " << airspace.getAircraftCount()
              << "  |  Steps: " << maxSteps
              << "  |  Safe Distance: " << controller.getMinSafeDistance()
              << "\n";

    // Show initial state before the loop begins
    std::cout << "\n  ─── INITIAL STATE ───────────────────────────────────\n";
    radar.displayAirspace(airspace);
    radar.printGrid(airspace);

    // ─── Main simulation loop (20 steps by default) ─────────────────────
    // For each step:
    //   1. Airspace::updateAircraftPositions()  — move all aircraft
    //   2. Controller::checkCollisions()        — detect proximity violations
    //   3. Radar::displayAirspace()             — print positions & grid
    for (currentStep = 1; currentStep <= maxSteps; ++currentStep) {
        step();

        // Delay between steps for readability (500 ms)
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    printSummary();
}

// ─── Single Step ────────────────────────────────────────────────────────────
// Executes one simulation time-step in three phases:
//   1. updateAircraftPositions() — move every aircraft by its speed/direction
//   2. checkCollisions()         — Euclidean distance check for all pairs
//   3. displayAirspace()         — print status table + ASCII grid
void SimulationEngine::step() {
    std::cout << "\n  ═══ STEP " << currentStep << " / " << maxSteps
              << " ═══════════════════════════════════════════\n";

    // 1. Update all aircraft positions
    airspace.updateAircraftPositions();

    // 2. Check for collisions
    int warnings = controller.checkCollisions(airspace);
    if (warnings == 0) {
        std::cout << "  ✓  No collision warnings.\n";
    }

    // 3. Display radar
    radar.displayAirspace(airspace);
    radar.printGrid(airspace);
}

// ─── Banner ─────────────────────────────────────────────────────────────────
void SimulationEngine::printBanner() const {
    std::cout << "\n";
    std::cout << "  ╔══════════════════════════════════════════════════════╗\n";
    std::cout << "  ║                                                      ║\n";
    std::cout << "  ║       AIR TRAFFIC CONTROL SIMULATOR  v1.0            ║\n";
    std::cout << "  ║       ─────────────────────────────────              ║\n";
    std::cout << "  ║       C++ OOP Simulation Engine                      ║\n";
    std::cout << "  ║                                                      ║\n";
    std::cout << "  ╚══════════════════════════════════════════════════════╝\n";
}

// ─── Summary ────────────────────────────────────────────────────────────────
void SimulationEngine::printSummary() const {
    std::cout << "\n  ╔══════════════════════════════════════════════════════╗\n";
    std::cout << "  ║              SIMULATION COMPLETE                     ║\n";
    std::cout << "  ╠══════════════════════════════════════════════════════╣\n";
    std::cout << "  ║  Total Steps Run: " << maxSteps << std::string(35 - std::to_string(maxSteps).length(), ' ') << "║\n";
    std::cout << "  ║  Aircraft Tracked: " << airspace.getAircraftCount() << std::string(34 - std::to_string(airspace.getAircraftCount()).length(), ' ') << "║\n";
    std::cout << "  ╚══════════════════════════════════════════════════════╝\n\n";
}
