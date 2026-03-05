#include "Airspace.h"
#include <algorithm>
#include <iostream>

// ─── Constructor ────────────────────────────────────────────────────────────
// Stores grid dimensions. Aircraft list starts empty.
Airspace::Airspace(int width, int height)
    : width(width), height(height) {}

// ─── addAircraft ────────────────────────────────────────────────────────────
// Adds an Aircraft object to the internal STL vector.
// O(1) amortised time via push_back.
void Airspace::addAircraft(const Aircraft& aircraft) {
    aircraftList.push_back(aircraft);
    std::cout << "  [AIRSPACE] Aircraft " << aircraft.getId()
              << " added at (" << aircraft.getX() << ", " << aircraft.getY() << ")\n";
}

// ─── removeAircraft ─────────────────────────────────────────────────────────
// Uses STL remove_if + erase idiom to remove by ID.
bool Airspace::removeAircraft(const std::string& id) {
    auto it = std::remove_if(aircraftList.begin(), aircraftList.end(),
        [&id](const Aircraft& a) { return a.getId() == id; });

    if (it != aircraftList.end()) {
        aircraftList.erase(it, aircraftList.end());
        std::cout << "  [AIRSPACE] Aircraft " << id << " removed.\n";
        return true;
    }
    return false;
}

// ─── updateAircraftPositions ────────────────────────────────────────────────
// Called once per simulation step.
// 1. Calls move() on every aircraft (updates position by speed + direction).
// 2. Clamps positions so aircraft stay within the airspace boundaries.
//    Aircraft that would fly out of bounds are stopped at the edge.
void Airspace::updateAircraftPositions() {
    for (auto& aircraft : aircraftList) {
        // Step 1 — Move based on speed and direction
        aircraft.move();

        // Step 2 — Enforce grid boundaries (clamping)
        int ax = aircraft.getX();
        int ay = aircraft.getY();

        if (ax < 0)          aircraft.setX(0);
        if (ax >= width)     aircraft.setX(width - 1);
        if (ay < 0)          aircraft.setY(0);
        if (ay >= height)    aircraft.setY(height - 1);
    }
}

// ─── isInBounds ─────────────────────────────────────────────────────────────
// Utility check — returns true if (x, y) is inside the grid.
bool Airspace::isInBounds(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
}

// ─── Getters ────────────────────────────────────────────────────────────────
// getAircraftList() — read-only access for Radar and Controller.
const std::vector<Aircraft>& Airspace::getAircraftList() const {
    return aircraftList;
}

// getAircraftListMutable() — mutable access for advanced manipulation.
std::vector<Aircraft>& Airspace::getAircraftListMutable() {
    return aircraftList;
}

int Airspace::getWidth() const  { return width; }
int Airspace::getHeight() const { return height; }

int Airspace::getAircraftCount() const {
    return static_cast<int>(aircraftList.size());
}
