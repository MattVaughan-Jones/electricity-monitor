CFLAGS ?= -Wall -Wextra -g -MMD
CONTROLLER_SRCS = $(wildcard controllers/*.c)
CORE_SRCS = server.c router.c validators.c
SRCS = $(CORE_SRCS) $(CONTROLLER_SRCS)
BUILD_DIR = build
SHARED = ../shared/build/shared.a
TARGET = $(BUILD_DIR)/server
OBJS = $(SRCS:%.c=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

$(TARGET): $(OBJS) $(SHARED) | $(BUILD_DIR)
	clang $(CFLAGS) $(OBJS) $(SHARED) -o $(TARGET)

$(SHARED):
	$(MAKE) -C ../shared

# Object files
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	clang $(CFLAGS) -I../shared -c $< -o $@

# Create build directory
build:
	mkdir -p $(BUILD_DIR)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)

# Run the server (you'll need to add port argument later)
run: $(TARGET)
	./$(TARGET) 8080

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
	@echo "  run      - Build and run server on port 8080"
	@echo "  debug    - Build with debug flags"
	@echo "  release  - Build optimized release version"
	@echo "  help     - Show this help"

# Include dependency files
-include $(DEPS)
