#include "Aircraft.h"
#include <iomanip>

// ─── Constructor ────────────────────────────────────────────────────────────
// Initialises all private attributes via initialiser list.
Aircraft::Aircraft(const std::string& id, int x, int y, int speed, const std::string& direction)
    : id(id), x(x), y(y), speed(speed), direction(direction) {}

// ─── Movement ───────────────────────────────────────────────────────────────
// Updates position based on direction and speed (cardinal directions only).
//   N → y increases (northward)
//   S → y decreases (southward)
//   E → x increases (eastward)
//   W → x decreases (westward)
void Aircraft::move() {
    if (direction == "N") {
        y += speed;
    } else if (direction == "S") {
        y -= speed;
    } else if (direction == "E") {
        x += speed;
    } else if (direction == "W") {
        x -= speed;
    }
    // Unknown direction → no movement (safe default)
}

// ─── Getters ────────────────────────────────────────────────────────────────
std::string Aircraft::getId() const        { return id; }
int         Aircraft::getX() const         { return x; }
int         Aircraft::getY() const         { return y; }
int         Aircraft::getSpeed() const     { return speed; }
std::string Aircraft::getDirection() const { return direction; }

// Returns position as a (x, y) pair for convenient access.
std::pair<int, int> Aircraft::getPosition() const {
    return { x, y };
}

// ─── Setters ────────────────────────────────────────────────────────────────
void Aircraft::changeDirection(const std::string& newDirection) {
    // Validate: only accept cardinal directions
    if (newDirection == "N" || newDirection == "S" ||
        newDirection == "E" || newDirection == "W") {
        direction = newDirection;
    } else {
        std::cout << "  [WARNING] Invalid direction \"" << newDirection
                  << "\" for aircraft " << id << ". Keeping: " << direction << "\n";
    }
}

void Aircraft::setX(int x)         { this->x = x; }
void Aircraft::setY(int y)         { this->y = y; }
void Aircraft::setSpeed(int speed)  { this->speed = speed; }

// ─── Display ────────────────────────────────────────────────────────────────
// Prints a single-line summary of all aircraft attributes.
void Aircraft::printInfo() const {
    std::cout << "  [" << id << "]  "
              << "Pos(" << x << ", " << y << ")  "
              << "Speed: " << speed << "  "
              << "Dir: " << direction
              << std::endl;
}
