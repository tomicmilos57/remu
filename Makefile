# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

# Source and build directories
SRC_DIR := src
OBJ_DIR := build
BIN := main

# Find all .cpp files under src/ and convert to corresponding .o paths in build/
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Default target
all: $(BIN)

# Link all object files into the final binary
$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile each .cpp file to .o in the build directory
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Build and run the application
run: all
	./$(BIN)

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(BIN)
