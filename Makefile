# Makefile for OpenConstruct C API

.PHONY: all clean example test run

# Directories
RUST_CRATE = /tmp/openconstruct-abi
BUILD_DIR = build

# Rust static library output
RUST_LIB = $(BUILD_DIR)/libopenconstruct_abi.a

# C compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I.
LDFLAGS = -L$(BUILD_DIR) -lopenconstruct_abi -lpthread -ldl -lm

# Targets
all: $(RUST_LIB) example test

# Build Rust crate as static library
$(RUST_LIB): | $(BUILD_DIR)
	cargo build --manifest-path $(RUST_CRATE)/Cargo.toml --release
	cp $(RUST_CRATE)/target/release/libopenconstruct_abi.a $@

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Build example program
example: $(RUST_LIB) example.c
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/example example.c $(LDFLAGS)

# Build test program
test: $(RUST_LIB) test.c
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/test test.c $(LDFLAGS)

# Run example
run: example
	@echo "Running example program..."
	LD_LIBRARY_PATH=$(BUILD_DIR):$$LD_LIBRARY_PATH $(BUILD_DIR)/example

# Run tests
run-test: test
	@echo "Running tests..."
	LD_LIBRARY_PATH=$(BUILD_DIR):$$LD_LIBRARY_PATH $(BUILD_DIR)/test

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR)
	cargo clean --manifest-path $(RUST_CRATE)/Cargo.toml

# Help
help:
	@echo "OpenConstruct C API Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build everything (default)"
	@echo "  example   - Build example program"
	@echo "  test      - Build test program"
	@echo "  run       - Build and run example"
	@echo "  run-test  - Build and run tests"
	@echo "  clean     - Remove build artifacts"
	@echo "  help      - Show this help message"