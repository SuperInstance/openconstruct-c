# OpenConstruct C — C Bindings for the OpenConstruct ABI

Complete C consumer for the [OpenConstruct ABI](https://github.com/SuperInstance/openconstruct-abi). Any C or C++ project can onboard agents into the SuperInstance ecosystem with a single header include.

**Part of [SuperInstance OpenConstruct](https://github.com/SuperInstance/OpenConstruct).**

## What This Gives You

- **Single header** — `openconstruct.h` with all types, functions, and phase constants
- **Full onboarding flow** — registry → declare → select → interface → config
- **Example program** — complete workflow demonstration in `example.c`
- **Test suite** — API verification in `test.c`
- **Static linking** — links against the Rust-built `libopenconstruct_abi.a`

## Quick Start

```c
#include "openconstruct.h"
#include <stdio.h>

int main() {
    OcRegistry* reg = oc_registry_create();
    OcSession* sess = oc_session_create(reg);

    oc_session_declare(sess, "my-agent", "glm-5.1");
    oc_session_select_modules(sess, "spectral-graph-core,plato-room");
    oc_session_choose_interface(sess, "cli");
    oc_session_connect(sess, "localhost:9142");

    OcResult* result = oc_session_generate_config(sess);
    printf("Config: %s\n", result->data_json);

    oc_result_free(result);
    oc_session_free(sess);
    oc_registry_free(reg);
    return 0;
}
```

## Building

```bash
# Build everything (Rust ABI + example + tests)
make all

# Run example
make run

# Run tests
make check
```

### Prerequisites

- GCC or Clang
- Cargo and Rust (for building the ABI static library)
- Make

## How It Fits

This is the C consumer of [openconstruct-abi](https://github.com/SuperInstance/openconstruct-abi). See also the [ESP32 binding](https://github.com/SuperInstance/openconstruct-esp32) for embedded C++ and the [examples repo](https://github.com/SuperInstance/openconstruct-examples) for a C onboarding walkthrough.

## License

MIT
