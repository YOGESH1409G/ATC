#ifndef SIMULATIONENGINE_H
#define SIMULATIONENGINE_H

#include "Airspace.h"
#include "Radar.h"
#include "Controller.h"

// ─────────────────────────────────────────────────────────────────────────────
// SimulationEngine — Orchestrates the entire ATC simulation.
//
// Responsibilities:
//   • Run the simulation loop for a configurable number of steps (default 20)
//   • Each step:  update positions → detect collisions → display radar
//   • Add a configurable delay between steps for console readability
//
// Design:
//   - Owns Airspace, Radar, Controller via composition (no raw pointers)
//   - Single entry point: run() starts the full simulation
//   - step() can be called individually for unit testing
//   - Uses std::this_thread::sleep_for for inter-step delay
// ─────────────────────────────────────────────────────────────────────────────

class SimulationEngine {
private:
    Airspace airspace;      // The 2-D airspace grid with all aircraft
    Radar radar;            // Displays aircraft state to console
    Controller controller;  // Detects collisions between aircraft
    int maxSteps;           // Total number of simulation steps
    int currentStep;        // Current step counter (1-based during run)

public:
    // ─── Constructors ───────────────────────────────────────────────────
    // Build engine from scratch: creates Airspace, Radar, Controller internally.
    SimulationEngine(int width, int height, int maxSteps, double safeDistance = 5.0);

    // Build engine from pre-configured objects (used when main.cpp creates
    // Airspace, Radar, and Controller explicitly before starting the engine).
    SimulationEngine(Airspace airspace, Radar radar, Controller controller, int maxSteps);

    // ─── Setup ──────────────────────────────────────────────────────────
    // Add an aircraft to the simulation before calling run().
    void addAircraft(const Aircraft& aircraft);

    // ─── Simulation Control ─────────────────────────────────────────────
    // Run the full simulation loop for maxSteps iterations.
    void run();

    // Execute a single simulation step: update → detect → display.
    void step();

    // ─── Display ────────────────────────────────────────────────────────
    // Print welcome banner at simulation start.
    void printBanner() const;

    // Print summary statistics at simulation end.
    void printSummary() const;
};

#endif // SIMULATIONENGINE_H
