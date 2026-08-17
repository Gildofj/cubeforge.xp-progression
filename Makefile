# PyroProgression - Automation Makefile
# Target: Cube World x86_64 Mod DLL

BUILD_DIR = build
CONFIG = Release
DIST_DIR = dist

.PHONY: all build test clean dist run-test help

# Default target: build the DLL and copy to dist/
all: build dist

# Configure and compile the mod DLL
build:
	@echo [1/2] Configuring CMake (x64)...
	@cmake -B $(BUILD_DIR) -S . -A x64
	@echo [2/2] Building $(CONFIG) configuration...
	@cmake --build $(BUILD_DIR) --config $(CONFIG) --target PyroProgression

# Build and execute the automated test suite
test:
	@echo [1/2] Building test suite...
	@cmake -B $(BUILD_DIR) -S . -A x64
	@cmake --build $(BUILD_DIR) --config $(CONFIG) --target pyro_tests
	@echo [2/2] Running automated tests...
	@ctest --test-dir $(BUILD_DIR) -C $(CONFIG) --output-on-failure

# Copy generated DLL to dist/ directory
dist:
	@if not exist $(DIST_DIR) mkdir $(DIST_DIR)
	@if exist $(BUILD_DIR)\$(CONFIG)\PyroProgression.dll copy /Y $(BUILD_DIR)\$(CONFIG)\PyroProgression.dll $(DIST_DIR)\PyroProgression.dll >nul
	@echo Build complete: $(DIST_DIR)\PyroProgression.dll

# Clean build artifacts
clean:
	@if exist $(BUILD_DIR) rmdir /S /Q $(BUILD_DIR)
	@if exist $(DIST_DIR) rmdir /S /Q $(DIST_DIR)
	@echo Cleaned build and dist directories.

# Quick Help
help:
	@echo Available make targets:
	@echo   make         - Build PyroProgression.dll in Release mode and copy to dist/
	@echo   make test    - Build and run unit tests
	@echo   make clean   - Remove build and dist directories
	@echo   make dist    - Copy output DLL to dist/
