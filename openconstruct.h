/**
 * @file openconstruct.h
 * @brief OpenConstruct C ABI Header
 *
 * OpenConstruct C ABI — the keystone for polyglot bindings.
 * Any language that can call C functions can use OpenConstruct.
 *
 * This header provides the complete C interface to the OpenConstruct library.
 */

#ifndef OPENCONSTRUCT_H
#define OPENCONSTRUCT_H

#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Phase constants for onboarding workflow
 */
#define OC_PHASE_SELF_DECLARATION 1
#define OC_PHASE_MODULE_SELECTION 2
#define OC_PHASE_INTERFACE_SELECTION 3
#define OC_PHASE_CONNECTION_SETUP 4
#define OC_PHASE_ENVIRONMENT_GEN 5

/**
 * @brief Opaque session handle
 *
 * Represents an active OpenConstruct onboarding session.
 * Created with oc_session_start() and freed with oc_session_free().
 */
typedef struct OcSession OcSession;

/**
 * @brief Opaque registry handle
 *
 * Represents the module registry containing available modules.
 * Created with oc_registry_create() and freed with oc_registry_free().
 */
typedef struct OcRegistry OcRegistry;

/**
 * @brief Module descriptor
 *
 * Contains metadata about a single module in the registry.
 * Returned pointers are owned by the registry and should not be freed.
 */
typedef struct OcModule {
    char *id;         /**< Module identifier */
    char *domain;     /**< Module domain (e.g., "math", "plato", "agents") */
    char *name;       /**< Human-readable module name */
    char *one_line;   /**< Short description of the module */
} OcModule;

/**
 * @brief Onboarding result
 *
 * Returned by operations that have success/failure states.
 * String fields must be freed with oc_string_free().
 */
typedef struct OcResult {
    bool success;     /**< Whether the operation succeeded */
    unsigned char phase; /**< Current phase after the operation */
    char *message;    /**< Human-readable result message */
    char *data_json;  /**< JSON data related to the operation */
} OcResult;

// ===== Registry Functions =====

/**
 * @brief Create a new OpenConstruct registry with default SuperInstance modules
 *
 * The registry contains pre-loaded modules for various domains including
 * mathematics, PLATO, and agent protocols.
 *
 * @return Pointer to a new OcRegistry, or NULL on failure
 */
OcRegistry *oc_registry_create(void);

/**
 * @brief Free a registry and all its resources
 *
 * After calling this function, the registry pointer becomes invalid.
 *
 * @param reg Pointer to the registry to free (NULL is safe)
 */
void oc_registry_free(OcRegistry *reg);

/**
 * @brief Get the number of modules in the registry
 *
 * @param reg Pointer to the registry (NULL returns 0)
 * @return Number of modules in the registry
 */
size_t oc_registry_count(const OcRegistry *reg);

/**
 * @brief Get a module by index
 *
 * The returned pointer is owned by the registry and must not be freed.
 * Valid indices are 0 to oc_registry_count() - 1.
 *
 * @param reg Pointer to the registry (NULL returns NULL)
 * @param index Zero-based index of the module
 * @return Pointer to the module, or NULL if index is out of range
 */
const OcModule *oc_registry_get(const OcRegistry *reg, size_t index);

// ===== Session Functions =====

/**
 * @brief Start a new onboarding session
 *
 * Creates a new session initialized to OC_PHASE_SELF_DECLARATION.
 *
 * @return Pointer to a new OcSession, or NULL on failure
 */
OcSession *oc_session_start(void);

/**
 * @brief Free a session and all its resources
 *
 * After calling this function, the session pointer becomes invalid.
 *
 * @param session Pointer to the session to free (NULL is safe)
 */
void oc_session_free(OcSession *session);

/**
 * @brief Get the current phase of a session
 *
 * @param session Pointer to the session (NULL returns 0)
 * @return Current phase constant (OC_PHASE_*), or 0 if session is NULL
 */
unsigned char oc_session_phase(const OcSession *session);

// ===== Onboarding Functions =====

/**
 * @brief Phase 1: Declare agent identity
 *
 * Moves the session from OC_PHASE_SELF_DECLARATION to OC_PHASE_MODULE_SELECTION.
 *
 * @param session Pointer to the session (NULL returns error result)
 * @param name Agent name (UTF-8 string)
 * @param model Agent model identifier (UTF-8 string)
 * @return OcResult with success status and next phase
 */
OcResult oc_declare_agent(OcSession *session, const char *name, const char *model);

/**
 * @brief Phase 2: Select modules for the agent
 *
 * Accepts a JSON array of module IDs (e.g., "[\"spectral-graph-core\",\"plato-room\"]").
 * Moves the session from OC_PHASE_MODULE_SELECTION to OC_PHASE_INTERFACE_SELECTION.
 *
 * @param session Pointer to the session (NULL returns error result)
 * @param module_ids_json JSON array string of module IDs to select
 * @return OcResult with success status and next phase
 */
OcResult oc_select_modules(OcSession *session, const char *module_ids_json);

/**
 * @brief Generate final configuration (Phase 5)
 *
 * Generates a JSON configuration string containing session ID, agent info,
 * selected modules, interfaces, and current phase.
 *
 * The returned string must be freed with oc_string_free().
 *
 * @param session Pointer to the session (NULL returns error message)
 * @return JSON configuration string, or error message on failure
 */
char *oc_generate_config(const OcSession *session);

// ===== Utility Functions =====

/**
 * @brief Free a string returned by the API
 *
 * Use this to free any char* pointers returned by OpenConstruct functions.
 * NULL is safe.
 *
 * @param s String pointer to free (NULL is safe)
 */
void oc_string_free(char *s);

/**
 * @brief Free strings in an OcResult
 *
 * Frees the message and data_json fields of an OcResult.
 * The result struct itself is stack-allocated and should not be freed.
 *
 * @param result Pointer to the OcResult to clean up
 */
void oc_result_free(OcResult *result);

#ifdef __cplusplus
}
#endif

#endif // OPENCONSTRUCT_H