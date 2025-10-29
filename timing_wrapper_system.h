/*
 * timing_wrapper_system.h
 *
 * Drop this into your embedded project to automatically
 * wrap listed functions with cycle-based timing and metrics.
 *
 * BARR-C compliant:
 *  - Single return per function
 *  - Explicit linkage and types
 *  - No runtime I/O inside timing wrapper
 *  - Deterministic and reentrant-safe (per-function statics)
 */

#ifndef TIMING_WRAPPER_SYSTEM_H
#define TIMING_WRAPPER_SYSTEM_H

#include <stdint.h>

/* Maximum number of functions that can be registered for timing */
#define MAX_TIMED_FUNCTIONS 32U

/* Timing statistics structure for each function */
typedef struct
{
    uint32_t call_count;      /* Number of times function was called */
    uint32_t total_cycles;    /* Total cycles spent in function */
    uint32_t min_cycles;      /* Minimum cycles for a single call */
    uint32_t max_cycles;      /* Maximum cycles for a single call */
    uint32_t last_cycles;     /* Cycles from the last call */
} timing_stats_t;

/* Function metadata for the registry */
typedef struct
{
    char const * name;        /* Function name (string literal) */
    timing_stats_t stats;     /* Timing statistics */
} function_timing_entry_t;

/* Public API */

/*
 * Initialize the timing system
 * Must be called before any timed functions are invoked
 */
extern void timing_system_init(void);

/*
 * Register a function for timing
 * Returns: index in the timing table, or -1 on error
 */
extern int32_t timing_register_function(char const * const func_name);

/*
 * Get timing statistics for a registered function
 * Returns: pointer to stats, or NULL if index is invalid
 */
extern timing_stats_t const * timing_get_stats(int32_t func_index);

/*
 * Reset statistics for a specific function
 */
extern void timing_reset_stats(int32_t func_index);

/*
 * Reset statistics for all functions
 */
extern void timing_reset_all_stats(void);

/*
 * Get the number of registered functions
 */
extern uint32_t timing_get_function_count(void);

/*
 * Get function name by index
 * Returns: pointer to function name string, or NULL if invalid
 */
extern char const * timing_get_function_name(int32_t func_index);

/*
 * Internal wrapper function - records entry timestamp
 * Returns: function index for use in exit wrapper
 */
extern int32_t timing_wrapper_enter(char const * const func_name);

/*
 * Internal wrapper function - records exit timestamp and updates stats
 */
extern void timing_wrapper_exit(int32_t func_index, uint32_t entry_timestamp);

/* Macro to simplify function wrapping */
#define TIMED_FUNCTION_ENTER(name) \
    int32_t const timing_idx_ = timing_wrapper_enter(name); \
    uint32_t const timing_start_ = port_get_cycle_count()

#define TIMED_FUNCTION_EXIT() \
    timing_wrapper_exit(timing_idx_, timing_start_)

#endif /* TIMING_WRAPPER_SYSTEM_H */
