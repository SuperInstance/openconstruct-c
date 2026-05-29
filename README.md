# OpenConstruct C Bindings

C bindings for the OpenConstruct C ABI, enabling any language that can call C functions to use OpenConstruct.

## Overview

OpenConstruct is a polyglot agent framework for modular onboarding workflows. This repository provides the C consumer side of the ABI, with:

- **Complete API coverage** - All functions, types, and phase constants
- **Example program** - Full workflow demonstration
- **Test suite** - Basic API verification
- **Static linking** - No runtime dependencies beyond standard C library

## Files

- `openconstruct.h` - Complete C header with all type and function declarations
- `example.c` - Full example showing registry creation, session management, and config generation
- `test.c` - Basic tests for API correctness
- `Makefile` - Build system for compiling Rust crate and linking C programs
- `README.md` - This file

## Requirements

- GCC or Clang (C compiler)
- Cargo and Rust (for building the Rust ABI)
- Make (build tool)

## Building

### Prerequisites

The Rust crate must already exist at `/tmp/openconstruct-abi`.

### Build Everything

```bash
make all
```

This will:
1. Build the Rust crate as a static library (`libopenconstruct_abi.a`)
2. Compile the example program
3. Compile the test program

### Build Individual Components

```bash
make example    # Build example program only
make test       # Build test program only
```

## Running

### Run Example

```bash
make run
```

Or manually:

```bash
LD_LIBRARY_PATH=build:$LD_LIBRARY_PATH ./build/example
```

Expected output:
```
=== OpenConstruct C API Example ===

Step 1: Creating module registry...
Registry created with 10 modules:

[0]
  ID:       spectral-graph-core
  Domain:   math
  Name:     Spectral Graph Core
  Summary:  Laplacian eigenvalues, conservation ratio, Fiedler analysis

[1]
  ID:       conservation-regime
  Domain:   math
  Name:     Conservation Regime
  Summary:  CR regime detection, anomaly analysis, spectral forecasting
...

Step 2: Starting new onboarding session...
Session started
Current phase: 1 (Self Declaration)

Step 3: Declaring agent identity (Phase 1)...
Agent declared successfully
Result message: Agent declared
Current phase: 2 (Module Selection)

Step 4: Selecting modules (Phase 2)...
Selecting modules: ["spectral-graph-core","plato-room","plato-puppeteer"]
Modules selected successfully
Result message: Modules selected
Current phase: 3 (Interface Selection)

Step 5: Generating final configuration...
Configuration JSON:
{"session_id": "oc-1716937200000", "agent": "ExampleAgent", "model": "gpt-4", "modules": ["spectral-graph-core", "plato-room", "plato-puppeteer"], "interfaces": ["cli", "api"], "phase": 3}

Step 6: Cleaning up...
All resources freed successfully

=== Example completed successfully ===
```

### Run Tests

```bash
make run-test
```

## API Reference

### Core Types

#### `OcSession`
Opaque handle for an active onboarding session.

#### `OcRegistry`
Opaque handle for the module registry.

#### `OcModule`
Module descriptor with fields:
- `id` - Module identifier
- `domain` - Module domain (math, plato, agents)
- `name` - Human-readable name
- `one_line` - Short description

#### `OcResult`
Operation result with fields:
- `success` - Whether operation succeeded
- `phase` - Current phase after operation
- `message` - Human-readable message (must be freed)
- `data_json` - JSON data (must be freed)

### Phase Constants

- `OC_PHASE_SELF_DECLARATION` (1) - Initial phase, declare agent identity
- `OC_PHASE_MODULE_SELECTION` (2) - Select modules for agent
- `OC_PHASE_INTERFACE_SELECTION` (3) - Choose communication interfaces
- `OC_PHASE_CONNECTION_SETUP` (4) - Establish connections
- `OC_PHASE_ENVIRONMENT_GEN` (5) - Generate final environment

### Key Functions

#### Registry Functions
- `oc_registry_create()` - Create new registry with default modules
- `oc_registry_free(reg)` - Free registry
- `oc_registry_count(reg)` - Get module count
- `oc_registry_get(reg, index)` - Get module by index

#### Session Functions
- `oc_session_start()` - Start new session
- `oc_session_free(session)` - Free session
- `oc_session_phase(session)` - Get current phase

#### Onboarding Functions
- `oc_declare_agent(session, name, model)` - Phase 1: Declare identity
- `oc_select_modules(session, module_ids_json)` - Phase 2: Select modules
- `oc_generate_config(session)` - Generate final JSON config

#### Utility Functions
- `oc_string_free(s)` - Free returned strings
- `oc_result_free(result)` - Free result strings

## Memory Management

- **Registry and Session**: Created with `_create()` or `_start()`, freed with `_free()`
- **Module pointers**: Returned by `oc_registry_get()`, owned by registry, do not free
- **String returns**: From `oc_generate_config()`, must be freed with `oc_string_free()`
- **Result strings**: Both `message` and `data_json` must be freed, use `oc_result_free()`

## Error Handling

All functions handle NULL pointers gracefully:
- `oc_registry_count(NULL)` returns 0
- `oc_registry_get(NULL, index)` returns NULL
- `oc_session_phase(NULL)` returns 0
- Operations on NULL sessions return error results with `success = false`

## Cleaning Up

```bash
make clean
```

Removes all build artifacts and cleans Rust crate.

## License

Same as the OpenConstruct Rust crate.