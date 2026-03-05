#include "Controller.h"
#include <cmath>
#include <iostream>
#include <iomanip>

// ─── Constructor ────────────────────────────────────────────────────────────
Controller::Controller(double minSafeDistance)
    : minSafeDistance(minSafeDistance) {}

// ─── Collision Detection ────────────────────────────────────────────────────
int Controller::checkCollisions(const Airspace& airspace) const {
    const auto& list = airspace.getAircraftList();
    int warnings = 0;

    // Check every unique pair
    for (size_t i = 0; i < list.size(); ++i) {
        for (size_t j = i + 1; j < list.size(); ++j) {
            double dist = computeDistance(list[i], list[j]);
            if (dist < minSafeDistance) {
                raiseWarning(list[i], list[j], dist);
                ++warnings;
            }
        }
    }
    return warnings;
}

// ─── Getters / Setters ─────────────────────────────────────────────────────
double Controller::getMinSafeDistance() const       { return minSafeDistance; }
void   Controller::setMinSafeDistance(double dist)  { minSafeDistance = dist; }

// ─── Euclidean Distance ─────────────────────────────────────────────────────
double Controller::computeDistance(const Aircraft& a, const Aircraft& b) const {
    double dx = static_cast<double>(a.getX() - b.getX());
    double dy = static_cast<double>(a.getY() - b.getY());
    return std::sqrt(dx * dx + dy * dy);
}

// ─── Warning Output ─────────────────────────────────────────────────────────
void Controller::raiseWarning(const Aircraft& a, const Aircraft& b, double distance) const {
    std::cout << "  ⚠  COLLISION WARNING: "
              << a.getId() << " <-> " << b.getId()
              << "  |  Distance: " << std::fixed << std::setprecision(2) << distance
              << "  |  Min Safe: " << minSafeDistance
              << std::endl;
}
