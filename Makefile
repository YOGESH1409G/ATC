# ──────────────────────────────────────────────────────────────────────────────
# Air Traffic Control Simulator — Makefile
# ──────────────────────────────────────────────────────────────────────────────

CXX       = g++
CXXFLAGS  = -std=c++17 -Wall -Wextra -pedantic -I include
LDFLAGS   =

SRC_DIR   = src
OBJ_DIR   = build
TARGET    = atc_simulator

# Source files and corresponding object files
SRCS      = $(wildcard $(SRC_DIR)/*.cpp)
OBJS      = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# ─── Default target ──────────────────────────────────────────────────────────
all: $(TARGET)

# ─── Link ────────────────────────────────────────────────────────────────────
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

# ─── Compile ─────────────────────────────────────────────────────────────────
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ─── Create build directory ─────────────────────────────────────────────────
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# ─── Clean ───────────────────────────────────────────────────────────────────
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# ─── Run ─────────────────────────────────────────────────────────────────────
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
