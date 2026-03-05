#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Airspace.h"

class Controller {
private:
    double minSafeDistance;  // minimum safe separation between aircraft

public:
    // Constructor: set minimum safe distance
    Controller(double minSafeDistance = 5.0);

    // Check all aircraft pairs for potential collisions
    // Returns the number of warnings raised
    int checkCollisions(const Airspace& airspace) const;

    // Getter / Setter
    double getMinSafeDistance() const;
    void setMinSafeDistance(double distance);

private:
    // Compute Euclidean distance between two aircraft
    double computeDistance(const Aircraft& a, const Aircraft& b) const;

    // Raise a collision warning to console
    void raiseWarning(const Aircraft& a, const Aircraft& b, double distance) const;
};

#endif // CONTROLLER_H
