#include "SimulationEngine.h"

int main() {
    // ─── Configuration ──────────────────────────────────────────────────
    const int AIRSPACE_WIDTH  = 30;
    const int AIRSPACE_HEIGHT = 20;
    const int MAX_STEPS       = 20;
    const double SAFE_DISTANCE = 5.0;

    // ─── Create Simulation Engine ───────────────────────────────────────
    SimulationEngine engine(AIRSPACE_WIDTH, AIRSPACE_HEIGHT, MAX_STEPS, SAFE_DISTANCE);

    // ─── Add Aircraft ───────────────────────────────────────────────────
    engine.addAircraft(Aircraft("AA101",  2,  3,  2, "N"));
    engine.addAircraft(Aircraft("BB202", 25, 15,  1, "S"));
    engine.addAircraft(Aircraft("CC303", 10, 10,  1, "E"));
    engine.addAircraft(Aircraft("DD404", 15,  5,  2, "N"));
    engine.addAircraft(Aircraft("EE505",  5, 18,  1, "W"));

    // ─── Run Simulation ─────────────────────────────────────────────────
    engine.run();

    return 0;
}
