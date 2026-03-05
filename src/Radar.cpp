#include "Radar.h"
#include <iostream>
#include <iomanip>
#include <vector>

// ─── Display Aircraft Table ─────────────────────────────────────────────────
void Radar::display(const Airspace& airspace) const {
    const auto& list = airspace.getAircraftList();

    std::cout << "\n  ╔══════════════════════════════════════════════════╗\n";
    std::cout << "  ║               RADAR — Aircraft Status            ║\n";
    std::cout << "  ╠══════════════════════════════════════════════════╣\n";
    std::cout << "  ║  ID        X        Y    Speed   Dir            ║\n";
    std::cout << "  ╠══════════════════════════════════════════════════╣\n";

    for (const auto& a : list) {
        std::cout << "  ║  "
                  << std::left  << std::setw(8)  << a.getId()
                  << std::right << std::setw(6)  << a.getX()
                  << std::setw(9) << a.getY()
                  << std::setw(8) << a.getSpeed()
                  << "   " << std::left << std::setw(4) << a.getDirection()
                  << "         ║" << std::endl;
    }

    std::cout << "  ╚══════════════════════════════════════════════════╝\n";
    std::cout << "  Total aircraft: " << list.size() << "\n";
}

// ─── Print ASCII Grid ───────────────────────────────────────────────────────
void Radar::printGrid(const Airspace& airspace) const {
    int w = airspace.getWidth();
    int h = airspace.getHeight();
    const auto& list = airspace.getAircraftList();

    // Create empty grid
    std::vector<std::vector<char>> grid(h, std::vector<char>(w, '.'));

    // Place aircraft on grid (use first char of ID)
    for (const auto& a : list) {
        int gx = a.getX();
        int gy = a.getY();

        // Clamp to grid bounds
        if (gx >= 0 && gx < w && gy >= 0 && gy < h) {
            char marker = a.getId().empty() ? '*' : a.getId()[0];
            // Flip Y so row 0 = top of screen = max Y
            grid[h - 1 - gy][gx] = marker;
        }
    }

    // Print grid with border
    std::cout << "\n  +" << std::string(w, '-') << "+\n";
    for (int row = 0; row < h; ++row) {
        std::cout << "  |";
        for (int col = 0; col < w; ++col) {
            std::cout << grid[row][col];
        }
        std::cout << "|\n";
    }
    std::cout << "  +" << std::string(w, '-') << "+\n";
}
