#include "SimulationEngine.h"
#include <iostream>
#include <thread>
#include <chrono>

// ─── Constructor ────────────────────────────────────────────────────────────
SimulationEngine::SimulationEngine(int width, int height, int maxSteps, double safeDistance)
    : airspace(width, height),
      controller(safeDistance),
      maxSteps(maxSteps),
      currentStep(0) {}

// ─── Add Aircraft ───────────────────────────────────────────────────────────
void SimulationEngine::addAircraft(const Aircraft& aircraft) {
    airspace.addAircraft(aircraft);
}

// ─── Run Full Simulation ────────────────────────────────────────────────────
void SimulationEngine::run() {
    printBanner();

    std::cout << "\n  Airspace: " << airspace.getWidth() << " x " << airspace.getHeight()
              << "  |  Aircraft: " << airspace.getAircraftCount()
              << "  |  Steps: " << maxSteps
              << "  |  Safe Distance: " << controller.getMinSafeDistance()
              << "\n";

    // Show initial state
    std::cout << "\n  ─── INITIAL STATE ───────────────────────────────────\n";
    radar.display(airspace);
    radar.printGrid(airspace);

    // Main simulation loop
    for (currentStep = 1; currentStep <= maxSteps; ++currentStep) {
        step();

        // Pause between steps for readability
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    printSummary();
}

// ─── Single Step ────────────────────────────────────────────────────────────
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
    radar.display(airspace);
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
