/**
 * @file example.c
 * @brief Complete example program demonstrating the OpenConstruct C API
 *
 * This program shows a full workflow:
 * 1. Create a registry and list available modules
 * 2. Start a new onboarding session
 * 3. Declare agent identity (Phase 1)
 * 4. Select modules (Phase 2)
 * 5. Generate final configuration (Phase 5)
 * 6. Clean up all resources
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "openconstruct.h"

static void print_module(const OcModule *module) {
    if (!module) return;
    printf("  ID:       %s\n", module->id);
    printf("  Domain:   %s\n", module->domain);
    printf("  Name:     %s\n", module->name);
    printf("  Summary:  %s\n", module->one_line);
    printf("\n");
}

static const char *phase_to_string(unsigned char phase) {
    switch (phase) {
        case OC_PHASE_SELF_DECLARATION:  return "Self Declaration";
        case OC_PHASE_MODULE_SELECTION:  return "Module Selection";
        case OC_PHASE_INTERFACE_SELECTION: return "Interface Selection";
        case OC_PHASE_CONNECTION_SETUP:  return "Connection Setup";
        case OC_PHASE_ENVIRONMENT_GEN:   return "Environment Generation";
        default: return "Unknown Phase";
    }
}

int main(void) {
    printf("=== OpenConstruct C API Example ===\n\n");

    // Step 1: Create a registry and list all modules
    printf("Step 1: Creating module registry...\n");
    OcRegistry *registry = oc_registry_create();
    if (!registry) {
        fprintf(stderr, "Failed to create registry\n");
        return 1;
    }

    size_t count = oc_registry_count(registry);
    printf("Registry created with %zu modules:\n\n", count);

    for (size_t i = 0; i < count; i++) {
        const OcModule *module = oc_registry_get(registry, i);
        if (module) {
            printf("[%zu]\n", i);
            print_module(module);
        }
    }

    // Step 2: Start a new session
    printf("Step 2: Starting new onboarding session...\n");
    OcSession *session = oc_session_start();
    if (!session) {
        fprintf(stderr, "Failed to start session\n");
        oc_registry_free(registry);
        return 1;
    }

    printf("Session started\n");
    printf("Current phase: %d (%s)\n\n",
           oc_session_phase(session),
           phase_to_string(oc_session_phase(session)));

    // Step 3: Declare agent identity (Phase 1)
    printf("Step 3: Declaring agent identity (Phase 1)...\n");
    OcResult result = oc_declare_agent(session, "ExampleAgent", "gpt-4");
    if (!result.success) {
        fprintf(stderr, "Failed to declare agent: %s\n", result.message);
        oc_string_free(result.message);
        oc_string_free(result.data_json);
        oc_session_free(session);
        oc_registry_free(registry);
        return 1;
    }

    printf("Agent declared successfully\n");
    printf("Result message: %s\n", result.message);
    printf("Current phase: %d (%s)\n\n",
           result.phase,
           phase_to_string(result.phase));

    oc_string_free(result.message);
    oc_string_free(result.data_json);

    // Step 4: Select modules (Phase 2)
    printf("Step 4: Selecting modules (Phase 2)...\n");
    const char *selected_modules_json = "[\"spectral-graph-core\",\"plato-room\",\"plato-puppeteer\"]";
    printf("Selecting modules: %s\n", selected_modules_json);

    result = oc_select_modules(session, selected_modules_json);
    if (!result.success) {
        fprintf(stderr, "Failed to select modules: %s\n", result.message);
        oc_string_free(result.message);
        oc_string_free(result.data_json);
        oc_session_free(session);
        oc_registry_free(registry);
        return 1;
    }

    printf("Modules selected successfully\n");
    printf("Result message: %s\n", result.message);
    printf("Current phase: %d (%s)\n\n",
           result.phase,
           phase_to_string(result.phase));

    oc_string_free(result.message);
    oc_string_free(result.data_json);

    // Step 5: Generate final configuration (Phase 5)
    printf("Step 5: Generating final configuration...\n");
    char *config_json = oc_generate_config(session);
    if (!config_json) {
        fprintf(stderr, "Failed to generate config\n");
        oc_session_free(session);
        oc_registry_free(registry);
        return 1;
    }

    printf("Configuration JSON:\n");
    printf("%s\n\n", config_json);
    oc_string_free(config_json);

    // Step 6: Clean up
    printf("Step 6: Cleaning up...\n");
    oc_session_free(session);
    oc_registry_free(registry);
    printf("All resources freed successfully\n");

    printf("\n=== Example completed successfully ===\n");
    return 0;
}