# --- Variables ---
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic -g
TARGET = FileMonitor
SRC = main.c

# Define where object files and the log file will go
BUILD_DIR = build
LOG_FILE = FileMonitor.log

# libpvars Submodule Variables
PVARS_DIR = libpvars
PVARS_LIB = $(PVARS_DIR)/libpvars.a
PVARS_INCLUDE = -I$(PVARS_DIR)/include
PVARS_LINK = -L$(PVARS_DIR) -lpvars

# Combined flags for ease of use
COMPILER_FLAGS = $(CFLAGS) $(PVARS_INCLUDE)
LINKER_FLAGS = $(PVARS_LINK)

# --- Default Target ---
.PHONY: all
all: $(BUILD_DIR) $(PVARS_LIB) $(TARGET)

# --- Compilation Rules ---

# Rule to create the build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Rule to build libpvars
$(PVARS_LIB):
	@echo "--- Building libpvars dependency ---"
	@$(MAKE) -C $(PVARS_DIR)

# Rule to compile the executable
$(TARGET): $(SRC) $(PVARS_LIB)
	$(CC) $(COMPILER_FLAGS) $(SRC) -o $(BUILD_DIR)/$(TARGET) $(LINKER_FLAGS)
	@echo "--- Compilation successful: $(BUILD_DIR)/$(TARGET) ---"

# --- Cleanup Options ---

.PHONY: clean
clean:
	@echo "--- Cleaning up build files ---"
	@rm -f $(BUILD_DIR)/$(TARGET)
	@rmdir $(BUILD_DIR) 2>/dev/null || true
	# clean up of libpvars submodule
	@echo "--- Cleaning up libpvars build files ---"
	@$(MAKE) -C $(PVARS_DIR) clean

.PHONY: log_clean
log_clean:
	@echo "--- Cleaning up log file(s) ---"
	@rm -f $(LOG_FILE)

.PHONY: full_clean
full_clean: clean log_clean
	@echo "--- Performed full cleanup ---"
