# ATC Simulator — Interview Preparation Guide

> A comprehensive guide that starts from **key OOP concepts**, progresses to **implementation details**, and includes **interviewer questions + follow-ups with answers** for every topic.

---

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [OOP Concepts (The Foundation)](#2-oop-concepts-the-foundation)
3. [C++ Specific Concepts](#3-c-specific-concepts)
4. [STL (Standard Template Library)](#4-stl-standard-template-library)
5. [Class-by-Class Deep Dive](#5-class-by-class-deep-dive)
6. [Design Patterns Used](#6-design-patterns-used)
7. [Algorithm & Logic Questions](#7-algorithm--logic-questions)
8. [System Design & Architecture](#8-system-design--architecture)
9. [Build System & Tooling](#9-build-system--tooling)
10. [Scenario-Based Questions](#10-scenario-based-questions)
11. [How to Present This Project in an Interview](#11-how-to-present-this-project)
12. [Automatic Collision Avoidance — Q&A](#12-automatic-collision-avoidance--qa)
13. [ANSI Color Visualization — Q&A](#13-ansi-color-visualization--qa)

---

## 1. Project Overview

### What is this project?
A **C++17 console-based Air Traffic Control Simulator** that models aircraft moving across a 2D grid airspace. It features:
- Aircraft with position, speed, and direction
- A bounded airspace with restricted weather zones
- Real-time collision detection using Euclidean distance
- **Automatic collision avoidance** — lower-priority aircraft rerouted 90° right
- **ANSI color-coded** radar display (green=safe, red=danger, yellow=caution, blue=weather zones)
- Animated console with clear-screen, progress bar, and summary dashboard
- A simulation engine running a 20-step time-loop

### Architecture (memorise this!)
```
SimulationEngine (Orchestrator)
    ├── owns Airspace       → manages Aircraft[] + WeatherZone[]
    ├── owns Radar          → color-coded display (reads dangerIds)
    └── owns Controller     → collision detection + auto-avoidance
```

### The Simulation Loop (per step)
```
1. system("clear")                               → animated screen refresh
2. Print progress bar                             → [########-----------] 40%
3. Airspace::updateAircraftPositions()             → move + weather zone avoidance + boundary clamping
4. Controller::resolveCollisions()                 → detect collisions + auto-reroute (turn 90° right)
5. Build danger set (IDs within safe distance)     → std::set<string>
6. Radar::displayAirspace(airspace, dangerIds)     → color-coded aircraft list
7. Radar::printGrid(airspace, dangerIds)           → color-coded ASCII grid + legend
── sleep 500ms ──
```

---

## 2. OOP Concepts (The Foundation)

### 2.1 Encapsulation

**What it is:** Bundling data + methods together, hiding internal state behind `private`, exposing only necessary operations through `public` methods.

**Where we use it:**
```cpp
class Aircraft {
private:                          // ← hidden from outside
    std::string id;
    int x, y, speed;
    std::string direction;
public:                           // ← controlled access
    int getX() const;             // read-only getter
    void setX(int x);            // controlled setter
    void changeDirection(const std::string& dir);  // validates input
};
```

**Why it matters in this project:**
- `changeDirection()` **validates** input — rejects anything other than N/S/E/W
- You can't set `x` directly from outside — only through `setX()` or `move()`
- This prevents invalid state (e.g., `direction = "BANANA"`)

---

#### Interviewer Questions:

**Q1: "What is encapsulation and where did you use it in your project?"**

**Answer:** Encapsulation is the principle of hiding internal data behind access controls and exposing functionality through public methods. In my Aircraft class, all attributes — `id`, `x`, `y`, `speed`, `direction` — are `private`. External code accesses them through getters like `getX()` and `getPosition()`. The setter `changeDirection()` validates the input before accepting it — if you pass an invalid direction, it prints a warning and keeps the old value. This prevents the aircraft from entering an invalid state.

**Follow-up Q: "Why not just make everything public? It's simpler."**

**Answer:** If `direction` were public, any code could set `direction = "UP"` or `direction = ""`, which would break the `move()` method (it only handles N/S/E/W). Encapsulation ensures the class is always in a **valid state**. It also means I can change the internal representation later (e.g., switch from string to an enum) without affecting any code that uses the class — they just call the same getters/setters.

**Follow-up Q: "Your Airspace exposes `getAircraftListMutable()` — doesn't that break encapsulation?"**

**Answer:** Yes, it partially does. I included it as a deliberate escape hatch for advanced scenarios where external code needs to modify aircraft directly. The primary interface is `getAircraftList()` which returns a **const reference** — true read-only access. The mutable version is documented as "for advanced use". In a production system, I'd consider removing it or replacing it with targeted methods like `modifyAircraft(id, callback)`.

---

### 2.2 Composition

**What it is:** Building complex classes by containing instances of other classes as members (has-a relationship), instead of using inheritance (is-a).

**Where we use it:**
```cpp
class SimulationEngine {
private:
    Airspace airspace;       // SimulationEngine HAS-A Airspace
    Radar radar;             // SimulationEngine HAS-A Radar
    Controller controller;   // SimulationEngine HAS-A Controller
};

class Airspace {
private:
    std::vector<Aircraft> aircraftList;      // Airspace HAS-MANY Aircraft
    std::vector<WeatherZone> weatherZones;   // Airspace HAS-MANY WeatherZone
};
```

---

#### Interviewer Questions:

**Q2: "Why did you use composition instead of inheritance?"**

**Answer:** Because none of my classes have an "is-a" relationship. A SimulationEngine is not a type of Airspace — it *uses* an Airspace. Composition gives me:
1. **Loose coupling** — I can swap the Radar implementation without touching SimulationEngine
2. **No diamond problem** — no multiple inheritance headaches
3. **Lifetime control** — when SimulationEngine is destroyed, all its members are automatically destroyed
4. **Flexibility** — I can have two constructors: one that builds internally, one that accepts pre-built objects

**Follow-up Q: "When would inheritance be appropriate in this project?"**

**Answer:** If I had different *types* of aircraft — like `FighterJet`, `CommercialPlane`, `Helicopter` — that share the basic Aircraft interface but have different movement patterns. I'd make `Aircraft` a base class with a `virtual move()` method, and each subtype would override it. That's a genuine "is-a" relationship.

**Follow-up Q: "Your members are value types, not pointers. Why?"**

**Answer:** I store `Airspace airspace` not `Airspace* airspace` because:
- No manual memory management needed (no `new`/`delete`)
- No null pointer bugs
- Automatic cleanup via RAII — when the engine is destroyed, members are destroyed
- In C++17 this is idiomatic; pointers are used when you need polymorphism or shared ownership

---

### 2.3 Abstraction

**What it is:** Hiding complex implementation behind a simple interface.

**Where we use it:**
```cpp
// User of SimulationEngine only needs to know:
engine.addAircraft(a1);
engine.run();                // ← one call does EVERYTHING

// They don't need to know about:
// - The 20-step loop inside run()
// - How updateAircraftPositions() handles weather zone avoidance
// - How checkCollisions() uses Euclidean distance
// - How printGrid() flips the Y-axis
```

---

#### Interviewer Questions:

**Q3: "How does your project demonstrate abstraction?"**

**Answer:** The `SimulationEngine::run()` method is the single entry point. The caller just calls `run()` and the entire simulation executes — 20 steps of movement, collision detection, radar display, with delays. The caller doesn't know or care about the Euclidean distance formula, the Y-axis flip in the grid, or how weather zone avoidance works internally. Similarly, `Airspace::updateAircraftPositions()` abstracts away three concerns — movement, weather zone avoidance, and boundary clamping — behind one method call.

**Follow-up Q: "How is abstraction different from encapsulation?"**

**Answer:** **Encapsulation** is about *access control* — hiding data with `private` and exposing it through methods. **Abstraction** is about *complexity hiding* — presenting a simple interface while hiding the complex logic underneath. Encapsulation is the mechanism (private/public), abstraction is the design philosophy (simple interface, complex internals). My `changeDirection()` demonstrates both: it's encapsulation because it controls access to the `direction` field, and it's abstraction because the validation logic is hidden — the caller just passes a string.

---

### 2.4 Single Responsibility Principle (SRP)

**Where we use it:**

| Class | Single Responsibility |
|-------|----------------------|
| `Aircraft` | Represents one aircraft, knows how to move itself |
| `Airspace` | Manages the grid, boundaries, weather zones |
| `Radar` | Display only — never modifies data |
| `Controller` | Collision detection only |
| `SimulationEngine` | Orchestrates the loop — delegates to others |
| `WeatherZone` | Defines a rectangular restricted area |

---

#### Interviewer Questions:

**Q4: "How did you apply SRP in your design?"**

**Answer:** Each class has exactly one reason to change. If I need to change how aircraft are displayed, I only modify `Radar` — the movement logic in `Aircraft` and collision logic in `Controller` are untouched. If I need a new collision algorithm, I only modify `Controller`. The `SimulationEngine` doesn't contain any domain logic itself — it just calls the right methods in the right order. This makes the code modular, testable, and easy to extend.

**Follow-up Q: "Is weather zone logic in the right place? It's in Airspace, not in a separate class."**

**Answer:** I put weather zone avoidance in `Airspace::updateAircraftPositions()` because weather zones are spatial constraints — just like boundaries. Airspace already handles "what happens when an aircraft tries to go out of bounds" (clamping), so "what happens when an aircraft tries to enter a restricted zone" (revert + reverse) is the same category of responsibility. If weather zones needed complex behaviour (movement, expiration, severity levels), I'd extract it into a `WeatherSystem` class.

---

## 3. C++ Specific Concepts

### 3.1 `const` Correctness

**Where we use it:**
```cpp
// 1. const member functions — promise not to modify the object
std::string getId() const;
int getX() const;
void displayAirspace(const Airspace& airspace) const;

// 2. const references — avoid copying, prevent modification
void addAircraft(const Aircraft& aircraft);
const std::vector<Aircraft>& getAircraftList() const;

// 3. const local variables
const int AIRSPACE_WIDTH = 40;
```

---

#### Interviewer Questions:

**Q5: "Explain const correctness in your project."**

**Answer:** I use `const` at three levels:

1. **`const` member functions** — Every getter (`getId()`, `getX()`, etc.) and every method that doesn't modify state (`displayAirspace()`, `checkCollisions()`) is marked `const`. This tells the compiler *and* the reader that calling these methods won't change the object.

2. **`const` references in parameters** — Methods like `addAircraft(const Aircraft& aircraft)` take a const reference. This avoids the cost of copying an Aircraft object while guaranteeing the function won't modify the original.

3. **`const` return types** — `getAircraftList()` returns `const std::vector<Aircraft>&` — a read-only reference. This lets Radar and Controller read the list without being able to accidentally modify it.

**Follow-up Q: "What would happen if you forgot `const` on `checkCollisions()`?"**

**Answer:** The `SimulationEngine` holds the Controller as a value member. If `checkCollisions()` weren't marked `const`, I couldn't call it from a const context or through a const reference. More importantly, it would signal to other developers that the method *might modify* the Controller's state, which it doesn't — the method is stateless. `const` is both a compiler constraint and documentation.

**Follow-up Q: "What is the difference between `const Aircraft&` and `Aircraft` as a parameter?"**

**Answer:** `Aircraft` passes by value — the compiler copies the entire Aircraft object (id string, 3 ints, direction string). `const Aircraft&` passes by const reference — no copy, just a reference to the original. For objects larger than a pointer (which Aircraft is, because it contains `std::string`), passing by const reference is faster. The `const` guarantees we won't modify the caller's original.

---

### 3.2 Member Initialiser Lists

**Where we use it:**
```cpp
Aircraft::Aircraft(const std::string& id, int x, int y, int speed, const std::string& direction)
    : id(id), x(x), y(y), speed(speed), direction(direction) {}
//  ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//  This is the member initialiser list
```

---

#### Interviewer Questions:

**Q6: "Why do you use member initialiser lists instead of assignment in the constructor body?"**

**Answer:** Three reasons:
1. **Efficiency** — With initialiser lists, members are constructed directly with the given values. With assignment in the body, members are first default-constructed, then assigned — that's two operations instead of one. For `std::string`, this means avoiding an unnecessary empty-string construction.
2. **Required for `const` and reference members** — If I had a `const int maxSpeed` member, it *must* be initialised in the initialiser list because you can't assign to a const after construction.
3. **Required for members without default constructors** — My `Airspace` takes `(width, height)` in its constructor. In `SimulationEngine`, I must write `: airspace(width, height)` in the initialiser list because `Airspace` has no default constructor.

**Follow-up Q: "In `SimulationEngine`, why does `radar` work in the initialiser list without arguments?"**

**Answer:** Because `Radar` is a stateless class with no private data members. The compiler generates a default constructor for it automatically. So `: radar()` or just having `radar` in the member list works fine — it's default-constructed.

---

### 3.3 `this` Pointer

**Where we use it:**
```cpp
void Aircraft::setX(int x) {
    this->x = x;   // this->x is the member, x is the parameter
}
```

---

#### Interviewer Questions:

**Q7: "When is `this` pointer needed?"**

**Answer:** When a parameter name shadows a member variable name. In `setX(int x)`, both the parameter and the member are called `x`. Writing `this->x = x` makes it explicit: `this->x` is the member variable, `x` is the parameter. Without `this->`, the compiler would interpret `x = x` as assigning the parameter to itself — a no-op. I could also avoid this by naming the parameter differently (e.g., `newX`), but `this->` is a common C++ idiom.

---

### 3.4 `static` Methods

**Where we use it:**
```cpp
class Airspace {
private:
    static std::string reverseDirection(const std::string& dir);
};
```

---

#### Interviewer Questions:

**Q8: "Why is `reverseDirection()` static?"**

**Answer:** Because it doesn't access any member variables of the Airspace object. It's a pure utility function — given "N", it returns "S"; given "E", it returns "W". Making it `static` communicates that it doesn't depend on any particular Airspace instance's state. It's a class-level helper, not an instance-level method. You could even call it as `Airspace::reverseDirection("N")` without creating an Airspace object.

---

### 3.5 `auto` and Range-Based For Loops

**Where we use it:**
```cpp
for (auto& aircraft : aircraftList) {    // mutable reference — we modify aircraft
    aircraft.move();
}

for (const auto& a : list) {             // const reference — read-only
    std::cout << a.getId();
}
```

---

#### Interviewer Questions:

**Q9: "Explain the difference between `auto&` and `const auto&` in your for loops."**

**Answer:** In `updateAircraftPositions()`, I use `auto&` (mutable reference) because I need to call `move()`, `setX()`, `setY()`, `changeDirection()` — all of which modify the aircraft. In `displayAirspace()` and `checkCollisions()`, I use `const auto&` (const reference) because I only read data via getters like `getId()`, `getX()`. Using `const auto&` prevents accidental modification and clearly signals read-only intent.

**Follow-up Q: "What if you used `auto` without the `&`?"**

**Answer:** Without `&`, each loop iteration would **copy** the Aircraft object. Modifications would be made to the copy, not the original in the vector. The `move()` calls would do nothing useful. With `&`, I get a reference to the actual element in the vector, so changes persist.

---

## 4. STL (Standard Template Library)

### 4.1 `std::vector`

**Where we use it:**
```cpp
std::vector<Aircraft> aircraftList;      // dynamic array of aircraft
std::vector<WeatherZone> weatherZones;   // dynamic array of zones
std::vector<std::vector<char>> grid(h, std::vector<char>(w, '.'));  // 2D grid
```

---

#### Interviewer Questions:

**Q10: "Why did you choose `std::vector` over arrays or `std::list`?"**

**Answer:**
- **vs raw arrays** — vectors handle dynamic sizing. I don't know at compile time how many aircraft there'll be. Vectors manage their own memory (`push_back` grows automatically).
- **vs `std::list`** — Vectors are cache-friendly because elements are contiguous in memory. For my use case (iterate all aircraft every step, rarely remove), vector is much faster than list. List would be better if I had frequent insertions/removals in the middle, but I don't.
- **Complexity**: `push_back` is O(1) amortised, iteration is O(n) with cache-friendly access, random access is O(1).

**Follow-up Q: "Your `removeAircraft()` uses remove-erase idiom. Explain it."**

**Answer:**
```cpp
auto it = std::remove_if(aircraftList.begin(), aircraftList.end(),
    [&id](const Aircraft& a) { return a.getId() == id; });
aircraftList.erase(it, aircraftList.end());
```
`std::remove_if` doesn't actually remove elements — it moves matching elements to the end and returns an iterator to the "new end". The elements after that iterator are in a valid-but-unspecified state. `erase()` then actually removes them from the vector. This is the standard C++ idiom for removing elements by predicate from a vector.

**Follow-up Q: "What's the `[&id]` in the lambda?"**

**Answer:** It's a **capture clause**. `[&id]` captures the local variable `id` by reference, making it available inside the lambda function. The lambda `(const Aircraft& a) { return a.getId() == id; }` is a predicate that returns true for aircraft whose ID matches. `&` means capture by reference (efficient, no copy). I could also use `[=]` to capture by value, but since `id` is a `const std::string&`, capturing by reference is more appropriate.

---

### 4.2 `std::pair`

**Where we use it:**
```cpp
std::pair<int, int> Aircraft::getPosition() const {
    return { x, y };
}
```

---

#### Interviewer Questions:

**Q11: "Why return a `std::pair` from `getPosition()` instead of two separate getters?"**

**Answer:** A pair bundles x and y into a single return value, which is useful when you want both coordinates together (e.g., for distance calculations or display). I also provide separate `getX()` and `getY()` when the caller only needs one value. The pair approach lets callers use structured bindings: `auto [x, y] = aircraft.getPosition();` — clean and readable C++17 syntax.

---

## 5. Class-by-Class Deep Dive

### 5.1 Aircraft Class

**Role:** Represents a single aircraft in the simulation.

**Key implementation details:**
- `move()` uses `if/else` on direction string — simple but clear
- No validation in `move()` for unknown directions — it just does nothing (safe default)
- `changeDirection()` validates input and warns on invalid values
- Constructor uses member initialiser list for efficiency

---

#### Interviewer Questions:

**Q12: "Walk me through what happens when `move()` is called on an aircraft with direction 'E' and speed 2."**

**Answer:** The method checks `direction == "E"`, which is true, so it executes `x += speed`, which is `x += 2`. If x was 10, it becomes 12. The aircraft has moved 2 units to the right (east) on the grid. The method doesn't check boundaries — that's Airspace's responsibility (SRP). If the new x exceeds the grid width, Airspace will clamp it.

**Follow-up Q: "What if speed is negative?"**

**Answer:** It would reverse the direction. `"E"` with speed `-2` would decrease x. This isn't validated in the current design — I could add validation in `setSpeed()` to reject negative values. The current design assumes the caller provides valid speed values, which is reasonable for a simulation where we control all inputs.

**Follow-up Q: "Why use `string` for direction instead of an `enum`?"**

**Answer:** I chose string for readability and simplicity of the user-facing interface (`Aircraft a1("A1", 10, 10, 1, "E")`). An enum would be more type-safe and faster (integer comparison vs string comparison). In a performance-critical system, I'd use:
```cpp
enum class Direction { N, S, E, W };
```
This gives compile-time checking — you can't accidentally pass "BANANA" as a direction. The tradeoff is that string is more flexible for extension (e.g., "NE", "NW"), while enum is safer and faster.

---

### 5.2 Airspace Class

**Role:** Manages the 2D grid, all aircraft, and weather zones.

**Key implementation detail — `updateAircraftPositions()`:**
```
for each aircraft:
    1. Save old position (oldX, oldY)
    2. Call aircraft.move()
    3. Check: is new position in a weather zone?
       YES → revert to (oldX, oldY), reverse direction, skip clamping
       NO  → clamp to grid boundaries
```

---

#### Interviewer Questions:

**Q13: "Explain the weather zone avoidance logic step by step."**

**Answer:** In `updateAircraftPositions()`, for each aircraft:
1. I save the old position: `int oldX = aircraft.getX(); int oldY = aircraft.getY();`
2. I call `aircraft.move()` which updates the position
3. I check if the *new* position is inside any weather zone using `isInWeatherZone()`
4. If yes — the move would put the aircraft inside a restricted zone — I **revert** the position by calling `setX(oldX)` and `setY(oldY)`, then **reverse** the direction by calling `changeDirection(reverseDirection(currentDirection))`. The aircraft bounces back.
5. I use `continue` to skip the boundary clamping step — since the position didn't change, there's no need to check bounds.
6. If no weather zone conflict, I proceed with the normal boundary clamping.

**Follow-up Q: "What happens if an aircraft starts inside a weather zone?"**

**Answer:** Good catch — in the current implementation, if an aircraft starts inside a zone, its first `move()` will move it to a new position. If that new position is *still* inside the zone, it gets reverted and reversed. So it would oscillate, moving and reverting every step. A fix would be to check the starting position during `addAircraft()` and reject or warn. This is a valid edge case I'd address in a production version.

**Follow-up Q: "What if two weather zones are adjacent and an aircraft bounces between them?"**

**Answer:** The aircraft would reverse at zone 1, then on the next step move toward zone 2, reverse again, then move back toward zone 1, and so on — oscillating between two zones. This is a limitation of the simple "reverse direction" strategy. A more sophisticated approach would be pathfinding (e.g., reroute the aircraft around the zone) or queueing the aircraft to hold position until a safe path is available.

---

### 5.3 Radar Class

**Role:** Color-coded read-only display — never modifies any data.

**Key features:**
- Accepts `std::set<std::string> dangerIds` to color-code aircraft by threat level
- Uses `isNearWeatherZone()` helper (2-cell margin) for caution tagging
- Grid cells are colored: green=safe, red=danger, yellow=caution, blue=weather zone, dim=empty
- Color legend printed below the grid

**Key implementation detail — Y-axis flip in `printGrid()`:**
```cpp
grid[h - 1 - gy][gx] = marker;
// Why: In console, row 0 is the top of the screen.
// In our grid, y=0 is the bottom (south).
// So we flip: console row 0 = max y (north).
```

---

#### Interviewer Questions:

**Q14: "How does the ASCII grid work? Explain the Y-axis flip."**

**Answer:** In our simulation, y=0 is the bottom (south) and y increases going north. But in a console, line 0 prints at the top of the screen. So if I just did `grid[gy][gx]`, north (high y) would appear at the bottom of the console — inverted. The formula `grid[h - 1 - gy][gx]` maps:
- y=0 → row `h-1` (bottom of console) ✓
- y=h-1 → row 0 (top of console) ✓

This makes the grid visually correct: north is up, south is down.

**Follow-up Q: "How does the color-coding work in the grid?"**

**Answer:** I maintain a parallel `colorMap` grid alongside the character grid. Each cell gets a color type: 0=empty (dim), 1=weather zone (blue), 2=safe aircraft (green), 3=danger (red), 4=caution/near weather zone (yellow). The `dangerIds` set — built by `buildDangerSet()` in SimulationEngine — identifies aircraft within safe distance. The `isNearWeatherZone()` helper flags aircraft within 2 cells of any zone boundary. When rendering, a `switch` on the color type selects the ANSI escape code.

**Follow-up Q: "Why is Radar stateless — no member variables at all?"**

**Answer:** Radar only needs to display the *current* state of the airspace. It doesn't need to remember anything between calls. By being stateless:
- It's trivially copyable (used in SimulationEngine's pre-built constructor)
- It's thread-safe (no shared mutable state)
- It follows SRP — purely display, nothing else
- Testing is simple — just pass an Airspace and check the output

---

### 5.4 Controller Class

**Role:** Collision detection **and automatic avoidance** using Euclidean distance.

**Key algorithm (resolveCollisions):**
```cpp
for (i = 0; i < n; ++i)
    for (j = i+1; j < n; ++j)        // j starts at i+1 to avoid duplicates
        if (distance(i, j) < safe) {
            warn();                   // red warning
            turnRight90(list[j]);     // reroute lower-priority aircraft
            ++totalAvoidances;        // track across simulation
        }
```

---

#### Interviewer Questions:

**Q15: "Explain your collision detection and avoidance algorithm."**

**Answer:** I check every unique pair of aircraft. The outer loop goes from 0 to n-1, the inner loop goes from i+1 to n-1. This gives me $\frac{n(n-1)}{2}$ comparisons — that's $O(n^2)$. For each pair, I compute the Euclidean distance: $d = \sqrt{(x_2-x_1)^2 + (y_2-y_1)^2}$. If the distance is less than `minSafeDistance` (5.0 units), I:
1. Print a colour-coded **red warning**
2. **Reroute** the lower-priority aircraft (the one with the higher index) by turning it **90° right** (N→E, E→S, S→W, W→N)
3. Print a **yellow rerouting** message
4. Increment `totalAvoidances` for the final summary

The `checkCollisions()` method is the read-only version (detection only), while `resolveCollisions()` does both detection **and** avoidance.

Starting the inner loop at `j = i + 1` is critical — it ensures I check A1-A2 but not A2-A1 (same pair). It also ensures I never check A1-A1 (same aircraft).

**Follow-up Q: "Why turn 90° right instead of reversing direction?"**

**Answer:** Reversing (180°) would send the aircraft back the way it came, which could create oscillating collisions with other aircraft behind it. A 90° turn moves the aircraft **perpendicular** to the collision course, separating the paths laterally. In real ATC, this is similar to a TCAS Resolution Advisory (RA) that commands a climb or descent — a change perpendicular to the horizontal conflict.

**Follow-up Q: "How would you optimise this for 10,000 aircraft?"**

**Answer:** $O(n^2)$ is fine for small n (3-50 aircraft), but for 10,000 I'd use **spatial partitioning**:
1. **Grid-based spatial hashing** — divide the airspace into cells of size `safeDistance × safeDistance`. Only check pairs in the same cell or adjacent cells. This reduces comparisons from $O(n^2)$ to $O(n \cdot k)$ where k is the average number of neighbours.
2. **Quadtree** — recursively subdivide the space. Query for aircraft within range of each aircraft.
3. **Sweep-and-prune** — sort aircraft by x-coordinate, use the sorted order to skip pairs that are definitely too far apart on x alone.

**Follow-up Q: "Why use Euclidean distance and not Manhattan distance?"**

**Answer:** Euclidean distance measures straight-line distance, which is physically accurate — aircraft don't only move on grid axes in real life. Manhattan distance ($|x_2-x_1| + |y_2-y_1|$) would overestimate diagonal distances. For example, two aircraft at (0,0) and (3,4) have Euclidean distance 5.0 but Manhattan distance 7. Using Manhattan would cause false alarms for aircraft that are actually a safe distance apart.

---

### 5.5 SimulationEngine Class

**Role:** Orchestrates the entire simulation.

**Key design decisions:**
- Two constructors — one builds from scratch, one accepts pre-built objects
- `step()` is public and can be called independently (testability)
- 500ms delay between steps using `<thread>` and `<chrono>`

---

#### Interviewer Questions:

**Q16: "Why does SimulationEngine have two constructors?"**

**Answer:** 
1. **`SimulationEngine(width, height, steps, safeDist)`** — creates everything internally. Simple to use: one line creates the entire engine.
2. **`SimulationEngine(airspace, radar, controller, steps)`** — accepts pre-built objects. This is used in `main.cpp` where we explicitly create each class (Steps 1-5) and then hand them to the engine. It supports the assignment requirement of demonstrating each class being created independently.

The second constructor also enables **dependency injection** — in testing, I could pass a mock Radar or a pre-configured Airspace with specific aircraft positions.

**Follow-up Q: "Why is `step()` public?"**

**Answer:** For testability. If I want to unit-test a single simulation step — verify that positions update correctly, collisions are detected, and the radar output is correct — I can call `step()` directly without running the full 20-step loop with delays. It follows the **tell, don't ask** principle: the engine exposes granular operations that tests can exercise.

---

### 5.6 WeatherZone Struct

**Role:** Defines a rectangular restricted area.

---

#### Interviewer Questions:

**Q17: "Why is WeatherZone a `struct` and not a `class`?"**

**Answer:** In C++, `struct` and `class` are identical except for the default access modifier (`struct` = `public`, `class` = `private`). I used `struct` because WeatherZone is a plain data type with no invariants to protect — all fields are public and there's no internal state to encapsulate. It's just a rectangle with a name. The single method `contains()` is a convenience — it doesn't modify any fields. Using `struct` signals to the reader: "this is a simple data holder".

**Follow-up Q: "What's aggregate initialisation and why can you write `{\"Storm\", 14, 8, 5, 5}` ?"**

**Answer:** Since WeatherZone is an aggregate (struct with all public members and no user-declared constructor), C++ allows brace-enclosed initialiser lists. The values are assigned in order: `name="Storm"`, `x=14`, `y=8`, `width=5`, `height=5`. This is cleaner than writing a constructor with 5 parameters. It works in C++17 and later.

---

## 6. Design Patterns Used

### 6.1 Composition (over Inheritance)
Already covered in Section 2.2.

### 6.2 Observer Pattern (Lightweight)

**How we use it:** Radar and Controller **observe** Airspace without modifying it. They take `const Airspace&` and only call `getAircraftList()` (const).

---

#### Interviewer Questions:

**Q18: "You mentioned Observer pattern. How is it implemented?"**

**Answer:** It's a lightweight, simplified version. In the classic pattern, observers register with a subject and get notified on changes. In my project, it's simpler: `SimulationEngine::step()` explicitly calls `radar.displayAirspace(airspace)` and `controller.checkCollisions(airspace)` each step, passing the airspace by const reference. Radar and Controller are observers — they read the state but never modify it. It's not a formal event-driven observer, but the principle is the same: separation between the data source (Airspace) and the consumers (Radar, Controller).

### 6.3 Strategy Pattern (Ready)

---

#### Interviewer Questions:

**Q19: "How would you add different collision detection strategies?"**

**Answer:** I'd define an interface:
```cpp
class CollisionStrategy {
public:
    virtual int detect(const Airspace& airspace) const = 0;
    virtual ~CollisionStrategy() = default;
};
```
Then implement `EuclideanStrategy`, `ManhattanStrategy`, `SpatialHashStrategy`, etc. `Controller` would hold a `unique_ptr<CollisionStrategy>` and delegate to it. The engine wouldn't change at all — it just calls `controller.checkCollisions()`. This is the **Strategy Pattern** — algorithms are interchangeable without changing the client code.

---

## 7. Algorithm & Logic Questions

### 7.1 Boundary Clamping

---

#### Interviewer Questions:

**Q20: "What is clamping and why do you use it?"**

**Answer:** Clamping restricts a value to a range. When an aircraft moves beyond the grid boundary, instead of wrapping around or removing it, I clamp it to the edge:
```cpp
if (ax < 0)        aircraft.setX(0);           // can't go left of 0
if (ax >= width)   aircraft.setX(width - 1);   // can't go right of width-1
if (ay < 0)        aircraft.setY(0);           // can't go below 0
if (ay >= height)  aircraft.setY(height - 1);  // can't go above height-1
```
The aircraft "sticks" at the wall. This is a deliberate design choice — the alternative is wrapping (appear on the other side) or removal. Clamping is visible (you can see the aircraft stuck at the edge on the grid) and safe (no index-out-of-bounds).

---

### 7.2 Weather Zone Containment Check

---

#### Interviewer Questions:

**Q21: "How does `contains(px, py)` work?"**

**Answer:**
```cpp
bool contains(int px, int py) const {
    return px >= x && px < x + width
        && py >= y && py < y + height;
}
```
This checks if point (px, py) falls inside the rectangle `[x, x+width) × [y, y+height)`. Note the **half-open interval**: the left/bottom edges are inclusive (`>=`), but the right/top edges are exclusive (`<`). This prevents overlap when two zones are placed side by side — a point at `x + width` belongs to the next zone, not this one. This is a standard convention in computer graphics and grid systems.

---

## 8. System Design & Architecture

#### Interviewer Questions:

**Q22: "If you were to scale this to handle 1,000 aircraft in a real-time system, what would you change?"**

**Answer:**
1. **Data structures** — Replace `std::vector` aircraft scanning with spatial partitioning (quadtree or grid hash) for O(1) neighbour lookups instead of O(n²) collision checks
2. **Multithreading** — Run movement updates, collision detection, and display on separate threads. Use mutex or lock-free queues for communication
3. **Network layer** — Aircraft positions would come from real sensors via UDP/TCP. Add a `DataIngestionModule` class
4. **Persistent state** — Use a database or in-memory store (Redis) for flight tracking across system restarts
5. **GUI** — Replace ASCII grid with a graphical frontend (Qt, OpenGL, or web-based with WebSocket)
6. **Event system** — Replace the polling loop with an event-driven architecture for real-time responsiveness

**Follow-up Q: "How would you add unit testing?"**

**Answer:** I'd use Google Test (gtest):
```cpp
TEST(AircraftTest, MoveEast) {
    Aircraft a("T1", 0, 0, 1, "E");
    a.move();
    EXPECT_EQ(a.getX(), 1);
    EXPECT_EQ(a.getY(), 0);
}

TEST(ControllerTest, DetectsCollision) {
    Airspace space(100, 100);
    space.addAircraft(Aircraft("A1", 0, 0, 0, "N"));
    space.addAircraft(Aircraft("A2", 1, 0, 0, "N"));
    Controller ctrl(5.0);
    EXPECT_EQ(ctrl.checkCollisions(space), 1);
}
```
Since `step()` is public and classes are loosely coupled, each class can be tested independently.

---

**Q23: "Walk me through the flow when the program starts."**

**Answer:**
1. `main()` creates an `Airspace(40, 40)` — empty 40×40 grid
2. Adds 2 weather zones: Storm at (14,8) size 5×5, Fog at (3,33) size 4×4
3. Creates 5 Aircraft objects: A1 at (10,10) East, A2 at (20,10) West, A3 at (5,30) North, A4 at (10,20) East, A5 at (14,20) West
4. Adds all 5 to Airspace via `addAircraft()`
5. Creates Radar (stateless) and Controller (safe distance = 5.0)
6. Creates SimulationEngine by passing the pre-built objects
7. Calls `engine.run()` which:
   - Clears screen, prints color banner (v2.0)
   - Shows initial state (color-coded radar display + grid)
   - Waits 2 seconds
   - Runs 20 steps: each step → clear screen → progress bar → update positions → resolve collisions → build danger set → color display → sleep 500ms
   - Prints colored summary dashboard

During the simulation:
- A1 (East) and A2 (West) converge on row y=10 but hit the Storm zone first → reverse
- A3 (North) hits the Fog zone around y=33 → reverses to South
- A4 (East) and A5 (West) converge on row y=20 → A5 is automatically rerouted 90° right
- Aircraft near weather zones are shown in **yellow** (CAUTION)
- Aircraft in collision range are shown in **red** (DANGER)
- Final summary shows **Collisions Avoided: 10**

---

## 9. Build System & Tooling

#### Interviewer Questions:

**Q24: "Explain your Makefile and build process."**

**Answer:** My Makefile compiles with `g++ -std=c++17 -Wall -Wextra -pedantic`:
- `-std=c++17` — enables C++17 features (structured bindings, aggregate initialisation)
- `-Wall -Wextra` — enables all common warnings to catch bugs early
- `-pedantic` — enforces strict ISO C++ compliance
- `-I include` — header search path

Each `.cpp` file compiles to a `.o` object file in the `build/` directory, then all objects are linked into the `atc_simulator` executable. Targets: `make` (build), `make run` (build + execute), `make clean` (remove artifacts).

**Follow-up Q: "Why separate compilation instead of compiling everything in one command?"**

**Answer:** Separate compilation means if I change `Radar.cpp`, only `Radar.o` gets recompiled — not all 6 files. For a small project it doesn't matter much, but for large projects it saves minutes of build time. Make handles dependency tracking automatically.

---

## 10. Scenario-Based Questions

**Q25: "Two aircraft are heading straight at each other on the same row. Walk me through what happens step by step."**

**Answer:** With A1 at (10,10) moving East and A2 at (20,10) moving West:
- **Step 1**: A1→(11,10), A2→(19,10). Distance = 8.0. No warning.
- **Step 2**: A1→(12,10), A2→(18,10). Distance = 6.0. No warning.
- **Step 3**: A1→(13,10), A2→(17,10). Distance = 4.0. **WARNING** (< 5.0).
- **Step 4**: A1→(14,10) — but wait! (14,10) is inside the Storm zone (14,8 to 19,13). A1 is reverted to (13,10) and reverses to West.
- Similarly, A2 at step ~2-3 hits the Storm zone from the other side and reverses to East.
- After reversal, they move **away** from each other.

This demonstrates weather zone avoidance preventing the head-on collision.

---

**Q26: "What if I want to add diagonal movement (NE, NW, SE, SW)?"**

**Answer:** I'd modify three things:
1. **`Aircraft::move()`** — add cases for "NE", "NW", "SE", "SW" that modify both x and y
2. **`Aircraft::changeDirection()`** — update the validation to accept the 4 new directions
3. **`Airspace::reverseDirection()`** — add reverse mappings: "NE"→"SW", "NW"→"SE", etc.

No changes needed in Radar, Controller, or SimulationEngine — they don't depend on direction values. This demonstrates the benefit of SRP and loose coupling.

---

**Q27: "How would you add different types of aircraft (helicopter, jet, commercial)?"**

**Answer:** I'd use **inheritance with polymorphism**:
```cpp
class Aircraft {                    // base class
public:
    virtual void move() = 0;       // pure virtual
    virtual ~Aircraft() = default;
};

class Jet : public Aircraft {
    void move() override { /* moves fast, straight lines */ }
};

class Helicopter : public Aircraft {
    void move() override { /* can hover, move in any direction */ }
};
```
Airspace would store `std::vector<std::unique_ptr<Aircraft>>` instead of `std::vector<Aircraft>`. The `move()` call is polymorphic — each type moves differently. This is the **Open/Closed Principle** — open for extension (new aircraft types), closed for modification (existing code doesn't change).

---

## 11. How to Present This Project

### 30-Second Elevator Pitch
> "I built a C++ Air Traffic Control Simulator using object-oriented design. It has 6 classes — Aircraft, Airspace with weather zones, color-coded Radar display, collision detection **and avoidance** Controller, and a SimulationEngine with animated console output. I used composition over inheritance, SRP, const correctness, STL containers, and the remove-erase idiom. The collision algorithm uses Euclidean distance with O(n²) pairwise checking — when aircraft are too close, the lower-priority one is **automatically rerouted 90° right**. Aircraft near weather zones get yellow caution warnings, and the entire output is **ANSI color-coded** with a progress bar and summary dashboard."

### Key Technical Points to Mention
1. **OOP** — Encapsulation, Composition, SRP, Abstraction
2. **C++17** — const correctness, member initialiser lists, auto, range-based for, structured bindings, aggregate initialisation
3. **STL** — vector, pair, set, remove_if with lambdas, algorithm header
4. **Algorithms** — Euclidean distance collision detection O(n²), rectangular containment check, boundary clamping, 90° right-turn avoidance
5. **Design** — Composition over inheritance, Observer pattern, Strategy-ready architecture
6. **Visualization** — ANSI escape codes (AnsiColors.h with `clr::` namespace), danger set tracking with `std::set<std::string>`, animated clear-screen + progress bar
7. **Build** — Makefile with separate compilation, g++ with strict warnings

### Questions YOU Should Ask Back
If the interviewer asks "any questions?", these show depth:
- "In production ATC systems, is collision detection event-driven or polling-based?"
- "What data structures do you use for spatial queries with large numbers of entities?"
- "Do you prefer composition or inheritance for entity systems in your codebase?"
- "How do real TCAS systems decide between climb and descend resolution advisories?"

---

## 12. Automatic Collision Avoidance — Q&A

#### Interviewer Questions:

**Q28: "How does your automatic collision avoidance work?"**

**Answer:** In `Controller::resolveCollisions()`, I check every unique aircraft pair using the same O(n²) Euclidean distance algorithm. When a pair is within `minSafeDistance`, instead of just warning, I **reroute the lower-priority aircraft** — the one with the higher index `j` — by turning it 90° right. The mapping is: N→E, E→S, S→W, W→N. This ensures the avoidance is deterministic and repeatable. I also increment a `totalAvoidances` counter that's displayed in the final summary dashboard.

**Follow-up Q: "Why is the higher-index aircraft rerouted, not the lower?"**

**Answer:** In my design, aircraft registered earlier (lower index) have implicit priority — like a first-come-first-served queue. In production ATC, priority is typically based on emergency status, fuel level, or aircraft type. My approach is simple but extensible — I could add an explicit `priority` field to `Aircraft` and reroute the one with lower priority instead.

**Follow-up Q: "What happens if both aircraft need to be rerouted?"**

**Answer:** Currently, only one is rerouted per pair per step. If after the reroute they're still too close on the next step, the avoidance triggers again. This iterative approach works because 90° turns quickly separate converging paths. In a more sophisticated system, I'd implement a cooperative avoidance protocol where both aircraft receive complementary manoeuvres (like TCAS — one climbs, one descends).

**Follow-up Q: "Could the 90° turn send an aircraft into a weather zone?"**

**Answer:** Yes, that's possible. The avoidance happens in `resolveCollisions()` (step 4 of the loop), while weather zone avoidance happens in `updateAircraftPositions()` (step 3). So on the *next* step, if the rerouted aircraft enters a weather zone, the Airspace will revert + reverse it. The system is self-correcting across steps, though not optimal within a single step. A production fix would be to check the resulting path before committing the reroute.

---

## 13. ANSI Color Visualization — Q&A

#### Interviewer Questions:

**Q29: "How did you implement the color-coded console output?"**

**Answer:** I created an `AnsiColors.h` header with a `clr::` namespace containing `constexpr const char*` constants for common ANSI escape sequences (e.g., `"\033[1;31m"` for bold red). Each output statement wraps the text with the appropriate color and always appends `clr::RESET` afterwards. The Radar methods accept a `std::set<std::string> dangerIds` parameter — built by the engine each step — to determine which aircraft are in danger. A helper `isNearWeatherZone()` checks if an aircraft is within 2 cells of any weather zone boundary for caution tagging.

**Follow-up Q: "Why use `constexpr const char*` instead of `std::string`?"**

**Answer:** ANSI escape codes are compile-time constants that never change. `constexpr const char*` stores them as string literals with zero runtime overhead — no heap allocation, no constructor calls. Using `std::string` would allocate heap memory at program start for each color code, which is wasteful for immutable constant data. The `constexpr` also enables the compiler to inline the pointer values directly.

**Follow-up Q: "How does the danger set work?"**

**Answer:** `buildDangerSet()` is a static helper in `SimulationEngine.cpp`. Each step, after collision avoidance runs, it iterates all aircraft pairs and checks if any are within `minSafeDistance`. Both aircraft IDs are inserted into a `std::set<std::string>`. This set is then passed to `Radar::displayAirspace()` and `Radar::printGrid()`. If an aircraft's ID is in the set → red. If it's near a weather zone → yellow. Otherwise → green. The set uses `std::set::count()` for O(log n) lookup.

**Follow-up Q: "What happens on terminals that don't support ANSI colors?"**

**Answer:** The escape codes would appear as raw text (e.g., `[1;31m`), making output unreadable. To handle this gracefully, I could: (1) check the `TERM` environment variable, (2) use `isatty(STDOUT_FILENO)` to detect if stdout is a terminal, or (3) add a `--no-color` command-line flag that replaces all `clr::` values with empty strings. For this educational project, ANSI support is assumed since it works in virtually all modern terminals.

**Q30: "Explain the progress bar implementation."**

**Answer:** Each step, I compute the fill percentage: `filled = (currentStep * barWidth) / maxSteps`. I then print `barWidth` characters: `#` for filled positions and `-` (dimmed) for empty positions. The percentage `(currentStep * 100 / maxSteps)` is appended. Combined with `system("clear")` at the start of each step, this creates an animated progress bar that updates in place. The 500ms delay between steps makes the animation visible.

---

*End of Interview Preparation Guide*
