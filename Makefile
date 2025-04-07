# Makefile for VRP_EA_Optimization project

# Directories
BUILD_DIR := build
BIN_DIR := bin

# Project settings
PROJECT_NAME := VRP_EA_Optimization
CMAKE_GENERATOR ?= "Unix Makefiles"

# Default target: build the project
.PHONY: all
all: build

# Build target: creates build directory if needed, runs CMake, then builds
.PHONY: build
build:
	@echo "Creating build directory (if it does not exist)..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake -G $(CMAKE_GENERATOR) .. && $(MAKE)
	@echo "Build complete."

# Run target: builds and then runs the executable
.PHONY: run
run: build
	@echo "Running project..."
	@./$(BIN_DIR)/$(PROJECT_NAME)

# Test target: builds and runs tests using CTest (if tests are defined)
.PHONY: test
test: build
	@echo "Running tests..."
	@cd $(BUILD_DIR) && ctest

# Clean target: removes build and bin directories
.PHONY: clean
clean:
	@echo "Cleaning build and bin directories..."
	@rm -rf $(BUILD_DIR) $(BIN_DIR)
	@echo "Clean complete."
