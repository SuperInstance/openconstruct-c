/**
 * @file test.c
 * @brief Basic tests for the OpenConstruct C API
 *
 * Tests API correctness:
 * - Module count verification
 * - Module iteration
 * - Phase constants validation
 * - Phase transitions
 * - Null pointer safety
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "openconstruct.h"

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) \
    do { \
        printf("TEST: %s... ", name); \
        fflush(stdout); \
    } while (0)

#define PASS() \
    do { \
        printf("PASS\n"); \
        tests_passed++; \
    } while (0)

#define FAIL(msg) \
    do { \
        printf("FAIL: %s\n", msg); \
        tests_failed++; \
    } while (0)

#define ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            FAIL(msg); \
            return; \
        } \
    } while (0)

void test_registry_create_free(void) {
    TEST("registry_create_free");

    OcRegistry *reg = oc_registry_create();
    ASSERT(reg != NULL, "Failed to create registry");

    oc_registry_free(reg);
    PASS();
}

void test_registry_count(void) {
    TEST("registry_count");

    OcRegistry *reg = oc_registry_create();
    ASSERT(reg != NULL, "Failed to create registry");

    size_t count = oc_registry_count(reg);
    ASSERT(count > 0, "Registry has no modules");

    oc_registry_free(reg);
    PASS();
}

void test_registry_get_module(void) {
    TEST("registry_get_module");

    OcRegistry *reg = oc_registry_create();
    ASSERT(reg != NULL, "Failed to create registry");

    const OcModule *module = oc_registry_get(reg, 0);
    ASSERT(module != NULL, "Failed to get module at index 0");
    ASSERT(module->id != NULL, "Module ID is NULL");

    // Verify first module is spectral-graph-core
    ASSERT(strcmp(module->id, "spectral-graph-core") == 0,
           "First module is not spectral-graph-core");

    oc_registry_free(reg);
    PASS();
}

void test_module_iteration(void) {
    TEST("module_iteration");

    OcRegistry *reg = oc_registry_create();
    ASSERT(reg != NULL, "Failed to create registry");

    size_t count = oc_registry_count(reg);

    // Verify all modules have non-NULL fields
    for (size_t i = 0; i < count; i++) {
        const OcModule *module = oc_registry_get(reg, i);
        ASSERT(module != NULL, "Module pointer is NULL");
        ASSERT(module->id != NULL, "Module ID is NULL");
        ASSERT(module->domain != NULL, "Module domain is NULL");
        ASSERT(module->name != NULL, "Module name is NULL");
        ASSERT(module->one_line != NULL, "Module one_line is NULL");
    }

    oc_registry_free(reg);
    PASS();
}

void test_null_safety(void) {
    TEST("null_safety");

    ASSERT(oc_registry_count(NULL) == 0, "NULL registry should return 0");
    ASSERT(oc_registry_get(NULL, 0) == NULL, "NULL registry get should return NULL");
    ASSERT(oc_session_phase(NULL) == 0, "NULL session phase should return 0");

    PASS();
}

void test_session_lifecycle(void) {
    TEST("session_lifecycle");

    OcSession *session = oc_session_start();
    ASSERT(session != NULL, "Failed to start session");
    ASSERT(oc_session_phase(session) == OC_PHASE_SELF_DECLARATION,
           "Initial phase should be SELF_DECLARATION");

    // Phase 1: Declare agent
    OcResult result = oc_declare_agent(session, "TestAgent", "gpt-4");
    ASSERT(result.success, "Failed to declare agent");
    ASSERT(result.phase == OC_PHASE_MODULE_SELECTION,
           "Phase should be MODULE_SELECTION after declaring agent");

    oc_string_free(result.message);
    oc_string_free(result.data_json);

    // Verify phase transition
    ASSERT(oc_session_phase(session) == OC_PHASE_MODULE_SELECTION,
           "Session phase not updated correctly");

    // Phase 2: Select modules
    result = oc_select_modules(session, "[\"spectral-graph-core\"]");
    ASSERT(result.success, "Failed to select modules");
    ASSERT(result.phase == OC_PHASE_INTERFACE_SELECTION,
           "Phase should be INTERFACE_SELECTION after selecting modules");

    oc_string_free(result.message);
    oc_string_free(result.data_json);

    oc_session_free(session);
    PASS();
}

void test_config_generation(void) {
    TEST("config_generation");

    OcSession *session = oc_session_start();
    ASSERT(session != NULL, "Failed to start session");

    // Set up agent
    OcResult result = oc_declare_agent(session, "ConfigTest", "claude-3");
    ASSERT(result.success, "Failed to declare agent");
    oc_string_free(result.message);
    oc_string_free(result.data_json);

    result = oc_select_modules(session, "[\"plato-room\"]");
    ASSERT(result.success, "Failed to select modules");
    oc_string_free(result.message);
    oc_string_free(result.data_json);

    // Generate config
    char *config = oc_generate_config(session);
    ASSERT(config != NULL, "Failed to generate config");

    // Verify config contains expected fields
    ASSERT(strstr(config, "ConfigTest") != NULL, "Config missing agent name");
    ASSERT(strstr(config, "plato-room") != NULL, "Config missing selected module");
    ASSERT(strstr(config, "session_id") != NULL, "Config missing session_id");

    oc_string_free(config);
    oc_session_free(session);
    PASS();
}

void test_phase_constants(void) {
    TEST("phase_constants");

    ASSERT(OC_PHASE_SELF_DECLARATION == 1, "Wrong phase constant for SELF_DECLARATION");
    ASSERT(OC_PHASE_MODULE_SELECTION == 2, "Wrong phase constant for MODULE_SELECTION");
    ASSERT(OC_PHASE_INTERFACE_SELECTION == 3, "Wrong phase constant for INTERFACE_SELECTION");
    ASSERT(OC_PHASE_CONNECTION_SETUP == 4, "Wrong phase constant for CONNECTION_SETUP");
    ASSERT(OC_PHASE_ENVIRONMENT_GEN == 5, "Wrong phase constant for ENVIRONMENT_GEN");

    PASS();
}

void test_domain_categories(void) {
    TEST("domain_categories");

    OcRegistry *reg = oc_registry_create();
    ASSERT(reg != NULL, "Failed to create registry");

    size_t count = oc_registry_count(reg);
    int has_math = 0, has_plato = 0, has_agents = 0;

    for (size_t i = 0; i < count; i++) {
        const OcModule *module = oc_registry_get(reg, i);
        if (strcmp(module->domain, "math") == 0) has_math = 1;
        if (strcmp(module->domain, "plato") == 0) has_plato = 1;
        if (strcmp(module->domain, "agents") == 0) has_agents = 1;
    }

    ASSERT(has_math, "Registry missing math domain");
    ASSERT(has_plato, "Registry missing plato domain");
    ASSERT(has_agents, "Registry missing agents domain");

    oc_registry_free(reg);
    PASS();
}

int main(void) {
    printf("=== OpenConstruct C API Tests ===\n\n");

    test_registry_create_free();
    test_registry_count();
    test_registry_get_module();
    test_module_iteration();
    test_null_safety();
    test_session_lifecycle();
    test_config_generation();
    test_phase_constants();
    test_domain_categories();

    printf("\n=== Test Results ===\n");
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);

    if (tests_failed == 0) {
        printf("\nAll tests passed!\n");
        return 0;
    } else {
        printf("\nSome tests failed.\n");
        return 1;
    }
}