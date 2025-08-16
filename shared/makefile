CFLAGS ?= -Wall -Wextra -g -MMD
SRCS = http.c websocket.c sha1.c
BUILD_DIR = build
TARGET = $(BUILD_DIR)/shared.a
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

$(TARGET): $(OBJS) | $(BUILD_DIR)
	ar rcs $(TARGET) $(OBJS)

# Object files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	clang $(CFLAGS) -c $< -o $@

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Debug target with more verbose output
debug: CFLAGS += -DDEBUG -O0
debug: $(TARGET)

# Release build
release: CFLAGS += -O2 -DNDEBUG
release: clean $(TARGET)

# Show help
help:
	@echo "Available targets:"
	@echo "  clean    - Remove build artifacts"
	@echo "  debug    - Build with debug flags"
	@echo "  release  - Build optimized release version"
	@echo "  help     - Show this help"

# Include dependency files
-include $(DEPS)
