# Compiler and executable name
CXX      ?= g++
TARGET   := main

# Directorios del proyecto
SRC_DIR      := src
INC_DIR      := include
BUILD_DIR    := build
BIN_DIR      := bin
ARQUITECTURE := 64

# Default mode
MODE ?= debug

# ------------------------------------------------------------------------------
# Path detection and lib config
# ------------------------------------------------------------------------------
ifeq ($(OS),Windows_NT)
    DETECTED_OS := Windows
    TARGET_EXT  := .exe
    
    # MinGW paths
    PLATFORM_INC := -I lib/libsdl3/64/include
    PLATFORM_LIB := -L lib/libsdl3/64/lib -lmingw32 -lSDL3 -lSDL3_image
    
    # Windows console commands
    MKDIR     = if not exist "$(subst /,\,$1)" mkdir "$(subst /,\,$1)"
    RMDIR     = rmdir /S /Q "$(subst /,\,$1)" 2>nul || exit 0
    KILL      = taskkill /f /im $(TARGET)$(TARGET_EXT) 2>nul || exit 0
    
    # Borra solo los ejecutables (.exe) recursivamente dentro de bin/, respetando las DLLs
    CLEAN_BIN = del /S /Q "$(subst /,\,$(BIN_DIR))\*.exe" 2>nul || exit 0
else
    DETECTED_OS := $(shell uname -s)
    TARGET_EXT  :=
    KILL        := pkill -f $(TARGET) 2>/dev/null || true
    
    # Automatic Linux macOS detection
    PLATFORM_INC := $(shell pkg-config --cflags sdl3 sdl3-image 2>/dev/null || echo "-I/usr/local/include")
    PLATFORM_LIB := $(shell pkg-config --libs sdl3 sdl3-image 2>/dev/null || echo "-lSDL3 -lSDL3_image")
    
    # POSIX commands
    MKDIR     = mkdir -p "$1"
    RMDIR     = rm -rf "$1"
    
    # Borra los binarios dentro de bin/ ignorando explícitamente cualquier archivo .dll
    CLEAN_BIN = find $(BIN_DIR) -type f ! -name "*.dll" -delete 2>/dev/null || true
endif

# ------------------------------------------------------------------------------
# Compilation flags (-MMD -MP generan archivos .d con las dependencias .hpp)
# ------------------------------------------------------------------------------
BASE_CXXFLAGS := -std=c++17 -Wall -Wextra -I $(INC_DIR) $(PLATFORM_INC) -MMD -MP

ifeq ($(MODE),release)
    CXXFLAGS := $(BASE_CXXFLAGS) -O3 -DNDEBUG
    MODE_DIR := release
else
    CXXFLAGS := $(BASE_CXXFLAGS) -g -DDEBUG
    MODE_DIR := debug
endif

# Detección de archivos fuente, objetos y dependencias
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/$(MODE_DIR)/%.o, $(SRCS))
DEPS := $(OBJS:.o=.d)
EXEC := $(BIN_DIR)/$(ARQUITECTURE)/$(MODE_DIR)/$(TARGET)$(TARGET_EXT)

# ------------------------------------------------------------------------------
# Makefile rules
# ------------------------------------------------------------------------------
.PHONY: all debug release clean run help

# Default compilation
all: $(EXEC)

# Direct access for debug mode
debug:
	@$(MAKE) MODE=debug

# Direct access for release mode
release:
	@$(MAKE) MODE=release

# Objects link to generate the executable
$(EXEC): $(OBJS)
	@$(call MKDIR,$(dir $@))
	$(CXX) $(OBJS) -o $@ $(PLATFORM_LIB)
	@echo Compilacion exitosa en modo [$(MODE)] para [$(DETECTED_OS)]

# Individual compilation from cpp to o
$(BUILD_DIR)/$(MODE_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(call MKDIR,$(dir $@))
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Incluye las dependencias .d generadas automáticamente por el compilador
-include $(DEPS)

# Closes active instances and executes the program
run: all
	-@$(KILL)
	@$(EXEC)

# Clean
clean:
	-@$(call RMDIR,$(BUILD_DIR))
	-@$(CLEAN_BIN)
	@echo Archivos temporales y ejecutables eliminados (DLLs conservadas).