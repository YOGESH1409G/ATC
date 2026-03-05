#ifndef AIRCRAFT_H
#define AIRCRAFT_H

#include <string>
#include <utility>   // std::pair
#include <iostream>

// ─────────────────────────────────────────────────────────────────────────────
// Aircraft — Represents a single plane in the simulation.
//
// Attributes (private, encapsulated):
//   id        — unique identifier string (e.g. "AA101")
//   x, y      — position on the 2D airspace grid (integers)
//   speed     — units moved per simulation step (integer)
//   direction — compass heading as a string: "N", "S", "E", "W"
//
// Key methods:
//   move()            — advances position based on speed and direction
//   getPosition()     — returns (x, y) as a pair
//   getId()           — returns the aircraft ID
//   changeDirection() — sets a new compass heading
// ─────────────────────────────────────────────────────────────────────────────

class Aircraft {
private:
    std::string id;         // Unique aircraft identifier
    int x;                  // X position on the airspace grid
    int y;                  // Y position on the airspace grid
    int speed;              // Movement units per simulation step
    std::string direction;  // Compass heading: "N", "S", "E", "W"

public:
    // ─── Constructor ────────────────────────────────────────────────────
    // Initialises an aircraft with all required attributes.
    Aircraft(const std::string& id, int x, int y, int speed, const std::string& direction);

    // ─── Movement ───────────────────────────────────────────────────────
    // Updates the aircraft's (x, y) position based on its current
    // direction and speed.  Called once per simulation time step.
    //   N → y += speed   S → y -= speed
    //   E → x += speed   W → x -= speed
    void move();

    // ─── Getters ────────────────────────────────────────────────────────
    // Returns the aircraft's unique ID.
    std::string getId() const;

    // Returns current position as a (x, y) pair.
    std::pair<int, int> getPosition() const;

    // Returns individual coordinate components.
    int getX() const;
    int getY() const;

    // Returns current speed.
    int getSpeed() const;

    // Returns current direction string.
    std::string getDirection() const;

    // ─── Setters ────────────────────────────────────────────────────────
    // Change the aircraft's compass heading.
    // Accepts: "N", "S", "E", "W"
    void changeDirection(const std::string& newDirection);

    // Set position directly (used by Airspace for boundary clamping).
    void setX(int x);
    void setY(int y);

    // Set speed.
    void setSpeed(int speed);

    // ─── Display ────────────────────────────────────────────────────────
    // Prints a formatted line with all aircraft attributes.
    void printInfo() const;
};

#endif // AIRCRAFT_H
