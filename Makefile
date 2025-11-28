# --- Variables ---
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -pedantic -g
TARGET = FileMonitor
SRC = main.c

# Define where object files and the log file will go
BUILD_DIR = build
LOG_FILE = FileMonitor.log

# --- Default Target ---
.PHONY: all
all: $(BUILD_DIR) $(TARGET)

# --- Compilation Rules ---

# Rule to create the build directory
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Rule to compile the executable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(BUILD_DIR)/$(TARGET)
	@echo "--- Compilation successful: $(BUILD_DIR)/$(TARGET) ---"

# --- Cleanup Options ---

.PHONY: clean
clean:
	@echo "--- Cleaning up build files ---"
	@rm -f $(BUILD_DIR)/$(TARGET)
	@rmdir $(BUILD_DIR) 2>/dev/null || true

.PHONY: log_clean
log_clean:
	@echo "--- Cleaning up log file(s) ---"
	@rm -f $(LOG_FILE)

.PHONY: full_clean
full_clean: clean log_clean
	@echo "--- Performed full cleanup ---"
