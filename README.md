# Air Traffic Control Simulator

A **C++ Object-Oriented** console-based simulation that models an air traffic control system. Aircraft move across a 2D airspace grid, a radar displays their positions, and a controller detects potential collisions in real time.

---

## UML Class Diagram

> **Standalone file:** [`docs/class_diagram.mmd`](docs/class_diagram.mmd) — open in [Mermaid Live Editor](https://mermaid.live) or render with `mmdc -i docs/class_diagram.mmd -o docs/class_diagram.svg`

```mermaid
classDiagram
    direction TB

    class Aircraft {
        -string id
        -int x
        -int y
        -int speed
        -string direction
        +Aircraft(id, x, y, speed, direction)
        +move() void
        +getId() string
        +getPosition() pair~int,int~
        +getX() int
        +getY() int
        +getSpeed() int
        +getDirection() string
        +changeDirection(newDir) void
        +setX(x) void
        +setY(y) void
        +setSpeed(speed) void
        +printInfo() void
    }

    class WeatherZone {
        &lt;&lt;struct&gt;&gt;
        +string name
        +int x
        +int y
        +int width
        +int height
        +contains(px, py) bool
    }

    class Airspace {
        -int width
        -int height
        -vector~Aircraft~ aircraftList
        -vector~WeatherZone~ weatherZones
        -reverseDirection(dir)$ string
        +Airspace(width, height)
        +addAircraft(aircraft) void
        +removeAircraft(id) bool
        +updateAircraftPositions() void
        +isInBounds(x, y) bool
        +addWeatherZone(zone) void
        +getWeatherZones() const vector~WeatherZone~
        +isInWeatherZone(x, y) bool
        +getAircraftList() const vector~Aircraft~
        +getAircraftListMutable() vector~Aircraft~
        +getWidth() int
        +getHeight() int
        +getAircraftCount() int
    }

    class Radar {
        +displayAirspace(airspace) void
        +printGrid(airspace) void
    }

    class Controller {
        -double minSafeDistance
        -computeDistance(a, b) double
        -raiseWarning(a, b, dist) void
        +Controller(minSafeDistance)
        +checkCollisions(airspace) int
        +getMinSafeDistance() double
        +setMinSafeDistance(d) void
    }

    class SimulationEngine {
        -Airspace airspace
        -Radar radar
        -Controller controller
        -int maxSteps
        -int currentStep
        +SimulationEngine(w, h, steps, safeDist)
        +SimulationEngine(airspace, radar, controller, steps)
        +addAircraft(aircraft) void
        +run() void
        +step() void
        +printBanner() void
        +printSummary() void
    }

    SimulationEngine *-- Airspace : owns
    SimulationEngine *-- Radar : owns
    SimulationEngine *-- Controller : owns
    Airspace *-- "0..*" Aircraft : manages
    Airspace *-- "0..*" WeatherZone : enforces
    Radar ..> Airspace : reads
    Controller ..> Airspace : inspects
    Controller ..> Aircraft : computes distance
```

### Relationship Key
| Symbol | Meaning | Example |
|--------|---------|---------|
| `*--` | **Composition** (owns, lifecycle-bound) | SimulationEngine owns Airspace |
| `..>` | **Dependency** (uses, read-only) | Radar reads Airspace |
| `0..*` | **Multiplicity** (zero-to-many) | Airspace manages 0..* Aircraft |

### OOP Design Patterns Used
- **Composition over Inheritance** — SimulationEngine owns Airspace, Radar, Controller as value members (no raw pointers, no inheritance hierarchies)
- **Single Responsibility Principle** — Each class has exactly one job: Aircraft moves, Airspace manages boundaries + zones, Radar displays, Controller detects, Engine orchestrates
- **Encapsulation** — All data members are `private`; access only through getters/setters
- **Observer Pattern (lightweight)** — Radar and Controller read Airspace state without modifying it (dependency, not composition)
- **Strategy-ready** — Collision detection and avoidance logic are isolated in Controller and Airspace, easily replaceable

---

## Stage 1 — Core Architecture & Simulation Engine

### Features Implemented
- **Aircraft** objects with ID, position (x, y), speed, and 4-direction compass heading
- **Airspace** grid that holds aircraft, updates positions, and enforces boundaries (clamping)
- **Radar** system that displays a formatted status table and an ASCII grid view
- **Controller** that checks every aircraft pair for proximity violations (Euclidean distance)
- **SimulationEngine** that orchestrates the time-step loop: move → detect → display

---

## Stage 2 — Aircraft Class (Full Implementation)

### Changes Made
Refactored the `Aircraft` class to match the clean OOP specification:

### Aircraft Attributes (all private, encapsulated)
| Attribute | Type | Description |
|-----------|------|-------------|
| `id` | `string` | Unique aircraft identifier (e.g. "AA101") |
| `x` | `int` | X position on the airspace grid |
| `y` | `int` | Y position on the airspace grid |
| `speed` | `int` | Units moved per simulation step |
| `direction` | `string` | Compass heading: "N", "S", "E", "W" |

### Aircraft Methods
| Method | Return | Description |
|--------|--------|-------------|
| `Aircraft(id, x, y, speed, direction)` | — | Constructor with full initialisation |
| `move()` | `void` | Updates position based on direction and speed |
| `getId()` | `string` | Returns the aircraft ID |
| `getPosition()` | `pair<int,int>` | Returns (x, y) as a pair |
| `getX()`, `getY()` | `int` | Returns individual coordinates |
| `getSpeed()` | `int` | Returns speed |
| `getDirection()` | `string` | Returns direction string |
| `changeDirection(newDir)` | `void` | Sets new heading with validation |
| `setX()`, `setY()`, `setSpeed()` | `void` | Direct setters for Airspace use |
| `printInfo()` | `void` | Prints formatted aircraft summary |

### Movement Logic
```
N → y += speed   (northward)
S → y -= speed   (southward)
E → x += speed   (eastward)
W → x -= speed   (westward)
```

### OOP Principles Applied
- **Encapsulation** — all attributes are `private`; access only via getters/setters
- **Validation** — `changeDirection()` rejects invalid directions with a warning
- **Clean interface** — `getPosition()` returns a `std::pair` for convenient access
- **Integer grid** — positions and speed are `int` for discrete grid-based movement

### Project Structure
```
AIR TRAFIC CONTROLLER/
├── include/
│   ├── Aircraft.h            # Aircraft class declaration
│   ├── Airspace.h            # Airspace manager declaration
│   ├── Radar.h               # Radar display declaration
│   ├── Controller.h          # Collision detection declaration
│   └── SimulationEngine.h    # Simulation orchestrator declaration
├── src/
│   ├── Aircraft.cpp          # Aircraft implementation
│   ├── Airspace.cpp          # Airspace implementation
│   ├── Radar.cpp             # Radar implementation
│   ├── Controller.cpp        # Controller implementation
│   ├── SimulationEngine.cpp  # Simulation engine implementation
│   └── main.cpp              # Entry point with sample aircraft
├── Makefile                  # Build automation
└── README.md                 # This file
```

### Class Responsibilities

| Class | Responsibility |
|-------|---------------|
| `Aircraft` | Represents a single aircraft with position, speed, direction. Moves itself each step. |
| `Airspace` | Manages the 2D grid boundaries and all active aircraft. Updates positions, enforces bounds. |
| `Radar` | Read-only observer. Prints formatted aircraft table and ASCII grid to console. |
| `Controller` | Safety system. Scans all aircraft pairs, raises warnings when distance < safe threshold. |
| `SimulationEngine` | Orchestrator. Owns Airspace, Radar, Controller. Runs the main time-step loop. |

### Simulation Workflow
```
main()
  └─► SimulationEngine::run()
        ├── Display initial state
        └── FOR each step (1 → maxSteps):
              ├── 1. Airspace::updateAircraftPositions()  →  move + clamp bounds
              ├── 2. Controller::checkCollisions()  →  warn if distance < threshold
              ├── 3. Radar::display()  →  print status table
              └── 4. Radar::printGrid()  →  print ASCII grid
```

### OOP Concepts Used
- **Encapsulation** — private data with getter/setter access
- **Composition** — SimulationEngine owns Airspace, Radar, Controller
- **Single Responsibility Principle** — each class has one clear job
- **Separation of Concerns** — movement, detection, display are independent

### How to Build & Run

**Prerequisites:** C++17 compatible compiler (g++, clang++)

```bash
# Build
make

# Run
make run

# Clean build artifacts
make clean
```

### Configuration (in main.cpp)
| Parameter | Default | Description |
|-----------|---------|-------------|
| `AIRSPACE_WIDTH` | 40 | Grid width |
| `AIRSPACE_HEIGHT` | 40 | Grid height |
| `MAX_STEPS` | 20 | Simulation duration |
| `SAFE_DISTANCE` | 5.0 | Collision warning threshold |

### Sample Aircraft
| ID | Start Position | Speed | Direction | Notes |
|----|---------------|-------|-----------|-------|
| A1 | (10, 10) | 1 | E | Heads toward A2 — collision scenario |
| A2 | (20, 10) | 1 | W | Heads toward A1 — collision scenario |
| A3 | (5, 30) | 1 | N | Independent — demonstrates boundary clamping |

---

## Stage 3 — Airspace Class (Full Implementation)

### Changes Made
Refactored the `Airspace` class with clean method naming, STL usage, and rich documentation.

### Airspace Attributes (private)
| Attribute | Type | Description |
|-----------|------|-------------|
| `width` | `int` | Grid width (valid x: 0 to width-1) |
| `height` | `int` | Grid height (valid y: 0 to height-1) |
| `aircraftList` | `vector<Aircraft>` | STL vector holding all active aircraft |

### Airspace Methods
| Method | Return | Description |
|--------|--------|-------------|
| `Airspace(width, height)` | — | Constructor, sets grid dimensions |
| `addAircraft(aircraft)` | `void` | Adds aircraft to vector (O(1) amortised) |
| `removeAircraft(id)` | `bool` | Removes by ID using STL remove_if + erase |
| `updateAircraftPositions()` | `void` | Moves all aircraft, clamps to bounds |
| `getAircraftList()` | `const vector&` | Read-only access to aircraft |
| `getAircraftListMutable()` | `vector&` | Mutable access for advanced use |
| `isInBounds(x, y)` | `bool` | Checks if coordinate is inside grid |
| `getWidth()`, `getHeight()` | `int` | Grid dimensions |
| `getAircraftCount()` | `int` | Number of active aircraft |

### Boundary Enforcement
Aircraft that move beyond the grid edges are **clamped** to the boundary:
```
if x < 0       → x = 0
if x >= width  → x = width - 1
if y < 0       → y = 0
if y >= height → y = height - 1
```

### STL Features Used
- `std::vector<Aircraft>` — dynamic aircraft storage
- `std::remove_if` + `erase` — idiomatic removal by predicate
- Lambda capture `[&id]` — for ID matching

---

## Stage 4 — Radar Class (Full Implementation)

### Changes Made
Refactored the `Radar` class with the required `displayAirspace()` method and clean output format.

### Radar Methods
| Method | Return | Description |
|--------|--------|-------------|
| `displayAirspace(airspace)` | `void` | Prints each aircraft ID and position |
| `printGrid(airspace)` | `void` | Renders ASCII grid of the airspace |

### Output Format
```
Aircraft AA101 → (2,3)
Aircraft BB202 → (25,15)
Aircraft CC303 → (10,10)
Aircraft DD404 → (15,5)
Aircraft EE505 → (5,18)
```

### Design Principles
- **Read-only observer** — does not modify aircraft or airspace data
- **Simple interface** — one primary method `displayAirspace()`
- **Clean output** — minimal, scannable `Aircraft <ID> → (<x>,<y>)` format

---

## Stage 5 — Controller Class (Collision Detection)

### Changes Made
Implemented the `Controller` class with Euclidean distance-based collision detection and the required warning format.

### Controller Attributes (private)
| Attribute | Type | Description |
|-----------|------|-------------|
| `minSafeDistance` | `double` | Minimum safe separation (default: 5.0 units) |

### Controller Methods
| Method | Return | Description |
|--------|--------|-------------|
| `Controller(safeDistance)` | — | Constructor, sets safe distance threshold |
| `checkCollisions(airspace)` | `int` | Checks all pairs, returns warning count |
| `getMinSafeDistance()` | `double` | Returns current safe distance |
| `setMinSafeDistance(d)` | `void` | Updates safe distance threshold |
| `computeDistance(a, b)` | `double` | Private — Euclidean distance between two aircraft |
| `raiseWarning(a, b, dist)` | `void` | Private — prints collision warning |

### Collision Rule
Euclidean distance formula:
```
d = sqrt((x2 - x1)^2 + (y2 - y1)^2)

If d < safe_distance → WARNING
```

### Warning Output Format
```
WARNING: Potential collision between Aircraft AA101 and BB202
```

### Algorithm
- Checks all unique pairs: O(n*(n-1)/2)
- Stateless — operates on current airspace snapshot each step
- Configurable threshold via `setMinSafeDistance()`

---

## Stage 6 — SimulationEngine (20-Step Loop with Delay)

### Changes Made
Enhanced the `SimulationEngine` class with full documentation, a **20-step** simulation loop, and a **500 ms** inter-step delay for console readability.

### SimulationEngine Attributes (private)
| Attribute | Type | Description |
|-----------|------|-------------|
| `airspace` | `Airspace` | The 2-D grid holding all active aircraft |
| `radar` | `Radar` | Console display system (read-only observer) |
| `controller` | `Controller` | Collision detection system |
| `maxSteps` | `int` | Total number of simulation steps (default 20) |
| `currentStep` | `int` | 1-based counter tracking the current step |

### SimulationEngine Methods
| Method | Return | Description |
|--------|--------|-------------|
| `SimulationEngine(w, h, steps, safeDist)` | — | Constructor, initialises all subsystems |
| `addAircraft(aircraft)` | `void` | Registers an aircraft before the simulation starts |
| `run()` | `void` | Main entry point — runs the full 20-step loop |
| `step()` | `void` | Executes one step: update → detect → display |
| `printBanner()` | `void` | Prints the welcome banner at simulation start |
| `printSummary()` | `void` | Prints summary statistics at simulation end |

### Simulation Loop (per step)
```
FOR currentStep = 1 TO maxSteps (20):
  1. Airspace::updateAircraftPositions()   → move all aircraft
  2. Controller::checkCollisions()         → Euclidean distance check
  3. Radar::displayAirspace()              → print "Aircraft <ID> → (x,y)"
     Radar::printGrid()                    → render ASCII grid
  ── sleep 500 ms ──
```

### Delay Mechanism
- Uses `std::this_thread::sleep_for(std::chrono::milliseconds(500))`
- Provides a half-second pause between steps so the console output is readable
- Requires `<thread>` and `<chrono>` headers

### Design Principles
- **Composition** — owns Airspace, Radar, Controller (no raw pointers)
- **Orchestrator pattern** — coordinates subsystems without exposing internals
- **Single entry point** — `run()` is the only method main() needs to call
- **Testable** — `step()` is public and can be called independently for unit testing

---

## Weather Zones — Restricted Airspace

### Overview
Rectangular **weather zones** can be placed anywhere in the airspace. Aircraft attempting to enter a zone will have their move **reverted** and their direction **reversed** (N↔S, E↔W), effectively bouncing away.

### WeatherZone Struct (`include/WeatherZone.h`)
| Field | Type | Description |
|-------|------|-------------|
| `name` | `string` | Human-readable label (e.g. "Storm") |
| `x` | `int` | Bottom-left corner X |
| `y` | `int` | Bottom-left corner Y |
| `width` | `int` | Extends rightward from x |
| `height` | `int` | Extends upward from y |
| `contains(px, py)` | `bool` | Returns true if (px, py) is inside the zone |

### Avoidance Logic (in `Airspace::updateAircraftPositions`)
```
for each aircraft:
  1. Save old position (oldX, oldY)
  2. Call move()
  3. If new position is inside a weather zone:
       → Revert to (oldX, oldY)
       → Reverse direction (N↔S, E↔W)
       → Print "[AIRSPACE] Aircraft <ID> reversed — weather zone ahead"
       → Skip boundary clamping
  4. Otherwise clamp to grid boundaries as usual
```

### Radar Display
- Weather zone cells rendered as `#` on the ASCII grid
- Aircraft markers overwrite `#` if they share a cell
- Zone summary printed below the aircraft table

### Demo Zones (in `main.cpp`)
| Name | Position | Size | Purpose |
|------|----------|------|---------|
| Storm | (14, 8) | 5×5 | Blocks A1/A2 on row y=10 — forces reversal |
| Fog | (3, 33) | 4×4 | Blocks A3's northbound path — forces reversal |

### New Airspace Methods
| Method | Return | Description |
|--------|--------|-------------|
| `addWeatherZone(zone)` | `void` | Registers a restricted weather zone |
| `getWeatherZones()` | `const vector&` | Read-only access to all zones |
| `isInWeatherZone(x, y)` | `bool` | Checks if a point is inside any zone |

---

> **Next stages** will add more features on top of this core engine.
