# Superman vs Ghost Game Makefile
# Simple makefile for building with traditional make

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
DEBUGFLAGS = -std=c++17 -Wall -Wextra -g -DDEBUG

# SFML libraries
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

# Directories
SRC_DIR = .
BUILD_DIR = build
ASSETS_DIR = assets

# Source files
SOURCES = main.cpp Game.cpp Superman.cpp Ghost.cpp Projectile.cpp ParticleSystem.cpp ResourceManager.cpp
HEADERS = Game.h Superman.h Ghost.h Projectile.h ParticleSystem.h ResourceManager.h
OBJECTS = $(SOURCES:%.cpp=$(BUILD_DIR)/%.o)

# Target executable
TARGET = SupermanVsGhost

# Platform detection
UNAME_S := $(shell uname -s)
ifeq ($(OS),Windows_NT)
    # Windows settings
    TARGET := $(TARGET).exe
    RM = del /Q
    MKDIR = mkdir
    COPY = copy
    PATH_SEP = \\
else
    # Unix-like systems
    RM = rm -f
    MKDIR = mkdir -p
    COPY = cp -r
    PATH_SEP = /
    
    ifeq ($(UNAME_S),Linux)
        # Linux specific settings
    endif
    ifeq ($(UNAME_S),Darwin)
        # macOS specific settings
        SFML_LIBS += -framework OpenGL
    endif
endif

# Default target
all: $(TARGET)

# Create build directory
$(BUILD_DIR):
	$(MKDIR) $(BUILD_DIR)

# Build target
$(TARGET): $(BUILD_DIR) $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(SFML_LIBS)
	@echo "Build complete: $(TARGET)"

# Compile source files
$(BUILD_DIR)/%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Debug build
debug: CXXFLAGS = $(DEBUGFLAGS)
debug: $(TARGET)

# Clean build files
clean:
ifeq ($(OS),Windows_NT)
	if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	if exist $(TARGET) $(RM) $(TARGET)
else
	$(RM) -r $(BUILD_DIR)
	$(RM) $(TARGET)
endif

# Install (copy to system location)
install: $(TARGET)
ifeq ($(OS),Windows_NT)
	@echo "Install not implemented for Windows. Copy $(TARGET) manually."
else
	install -m 755 $(TARGET) /usr/local/bin/
	@echo "Installed $(TARGET) to /usr/local/bin/"
endif

# Uninstall
uninstall:
ifeq ($(OS),Windows_NT)
	@echo "Uninstall not implemented for Windows."
else
	$(RM) /usr/local/bin/$(TARGET)
	@echo "Uninstalled $(TARGET) from /usr/local/bin/"
endif

# Create assets directory
assets:
	$(MKDIR) $(ASSETS_DIR)$(PATH_SEP)textures
	$(MKDIR) $(ASSETS_DIR)$(PATH_SEP)sounds
	$(MKDIR) $(ASSETS_DIR)$(PATH_SEP)fonts
	@echo "Created assets directory structure"

# Run the game
run: $(TARGET)
	.$(PATH_SEP)$(TARGET)

# Package for distribution
package: $(TARGET)
ifeq ($(OS),Windows_NT)
	@echo "Creating Windows package..."
	$(MKDIR) SupermanVsGhost-Windows
	$(COPY) $(TARGET) SupermanVsGhost-Windows$(PATH_SEP)
	if exist $(ASSETS_DIR) $(COPY) $(ASSETS_DIR) SupermanVsGhost-Windows$(PATH_SEP)
	$(COPY) README.md SupermanVsGhost-Windows$(PATH_SEP)
	@echo "Package created: SupermanVsGhost-Windows"
else
	@echo "Creating $(UNAME_S) package..."
	$(MKDIR) SupermanVsGhost-$(UNAME_S)
	$(COPY) $(TARGET) SupermanVsGhost-$(UNAME_S)/
	if [ -d $(ASSETS_DIR) ]; then $(COPY) $(ASSETS_DIR) SupermanVsGhost-$(UNAME_S)/; fi
	$(COPY) README.md SupermanVsGhost-$(UNAME_S)/
	tar -czf SupermanVsGhost-$(UNAME_S).tar.gz SupermanVsGhost-$(UNAME_S)
	@echo "Package created: SupermanVsGhost-$(UNAME_S).tar.gz"
endif

# Help target
help:
	@echo "Superman vs Ghost Game - Makefile"
	@echo "================================="
	@echo "Available targets:"
	@echo "  all      - Build the game (default)"
	@echo "  debug    - Build with debug symbols"
	@echo "  clean    - Remove build files"
	@echo "  install  - Install to system (Unix only)"
	@echo "  uninstall- Remove from system (Unix only)"
	@echo "  assets   - Create assets directory structure"
	@echo "  run      - Build and run the game"
	@echo "  package  - Create distribution package"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Prerequisites:"
	@echo "  - SFML 2.5+ development libraries"
	@echo "  - C++17 compatible compiler"
	@echo ""
	@echo "Examples:"
	@echo "  make          # Build release version"
	@echo "  make debug    # Build debug version"
	@echo "  make run      # Build and run"
	@echo "  make clean    # Clean build files"

# Prevent make from considering these as file targets
.PHONY: all debug clean install uninstall assets run package help