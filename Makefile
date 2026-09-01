# ==== Compiler ====
CXX      := g++
CXXFLAGS := --std=c++23 -Wall -Wextra -MMD -MP
LDFLAGS  :=

# ==== Directories ====
SRC_DIR  := cppsrc
ALGO_DIR := cppsrc/algo
UTIL_DIR := cppsrc/geoUtil

BIN_DIR      := bin
OBJ_DIR      := bin/obj
DEBUG_OBJ_DIR:= bin/debug_obj

TARGET       := $(BIN_DIR)/BARDS
DEBUG_TARGET := $(BIN_DIR)/BARDS_debug

# ==== Source files ====
SRC := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(ALGO_DIR)/*.cpp) $(wildcard $(UTIL_DIR)/*.cpp)

# ==== Object files (release) ====
OBJS := $(patsubst $(SRC_DIR)/%.cpp,  $(OBJ_DIR)/%.o,        $(SRC))
OBJS := $(patsubst $(ALGO_DIR)/%.cpp, $(OBJ_DIR)/algo/%.o,   $(OBJS))
OBJS := $(patsubst $(UTIL_DIR)/%.cpp, $(OBJ_DIR)/geoUtil/%.o,$(OBJS))

# ==== Object files (debug) ====
DEBUG_OBJS := $(patsubst $(SRC_DIR)/%.cpp,  $(DEBUG_OBJ_DIR)/%.o,        $(SRC))
DEBUG_OBJS := $(patsubst $(ALGO_DIR)/%.cpp, $(DEBUG_OBJ_DIR)/algo/%.o,   $(DEBUG_OBJS))
DEBUG_OBJS := $(patsubst $(UTIL_DIR)/%.cpp, $(DEBUG_OBJ_DIR)/geoUtil/%.o,$(DEBUG_OBJS))

# ==== Auto-generated header dependencies ====
-include $(OBJS:.o=.d)
-include $(DEBUG_OBJS:.o=.d)

# ==== Default target ====
.PHONY: all
all: compile

# ==== Release build ====
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/algo/%.o: $(ALGO_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/geoUtil/%.o: $(UTIL_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: compile
compile: $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)

# ==== Debug build (adds -g, no optimization) ====
$(DEBUG_OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -g -O0 -c $< -o $@

$(DEBUG_OBJ_DIR)/algo/%.o: $(ALGO_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -g -O0 -c $< -o $@

$(DEBUG_OBJ_DIR)/geoUtil/%.o: $(UTIL_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -g -O0 -c $< -o $@

.PHONY: debug
debug: $(DEBUG_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -g -O0 $(DEBUG_OBJS) -o $(DEBUG_TARGET) $(LDFLAGS)

# Build the debug binary and immediately launch it under gdb
.PHONY: gdb
gdb: debug
	gdb $(DEBUG_TARGET)

# ==== Housekeeping ====
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(DEBUG_OBJ_DIR) $(TARGET) $(DEBUG_TARGET)

.PHONY: run
run: compile
	$(TARGET)
