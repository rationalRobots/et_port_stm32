/*
 * timing_wrapper_system.c
 *
 * Implementation of the BARR-C compliant timing wrapper system
 */

#include "timing_wrapper_system.h"
#include "port.h"
#include <stddef.h>
#include <string.h>

/* Global registry of timed functions */
static function_timing_entry_t timing_registry[MAX_TIMED_FUNCTIONS];
static uint32_t function_count = 0U;
static uint8_t is_initialized = 0U;

/*
 * Initialize the timing system
 */
void timing_system_init(void)
{
    uint32_t i;
    
    /* Initialize port-specific cycle counter */
    port_cycle_counter_init();
    
    /* Clear all entries in the registry */
    for (i = 0U; i < MAX_TIMED_FUNCTIONS; i++)
    {
        timing_registry[i].name = NULL;
        timing_registry[i].stats.call_count = 0U;
        timing_registry[i].stats.total_cycles = 0U;
        timing_registry[i].stats.min_cycles = UINT32_MAX;
        timing_registry[i].stats.max_cycles = 0U;
        timing_registry[i].stats.last_cycles = 0U;
    }
    
    function_count = 0U;
    is_initialized = 1U;
}

/*
 * Register a function for timing
 * Returns: index in the timing table, or -1 on error
 */
int32_t timing_register_function(char const * const func_name)
{
    int32_t result;
    uint32_t i;
    int32_t found_index;
    
    result = -1;
    
    /* Check if system is initialized */
    if (is_initialized == 0U)
    {
        /* Return error - not initialized */
        result = -1;
    }
    else if (func_name == NULL)
    {
        /* Return error - invalid parameter */
        result = -1;
    }
    else
    {
        /* Check if function is already registered */
        found_index = -1;
        for (i = 0U; i < function_count; i++)
        {
            if (timing_registry[i].name == func_name)
            {
                found_index = (int32_t)i;
                break;
            }
        }
        
        if (found_index >= 0)
        {
            /* Already registered, return existing index */
            result = found_index;
        }
        else if (function_count >= MAX_TIMED_FUNCTIONS)
        {
            /* Registry full */
            result = -1;
        }
        else
        {
            /* Register new function */
            timing_registry[function_count].name = func_name;
            timing_registry[function_count].stats.call_count = 0U;
            timing_registry[function_count].stats.total_cycles = 0U;
            timing_registry[function_count].stats.min_cycles = UINT32_MAX;
            timing_registry[function_count].stats.max_cycles = 0U;
            timing_registry[function_count].stats.last_cycles = 0U;
            
            result = (int32_t)function_count;
            function_count++;
        }
    }
    
    return result;
}

/*
 * Get timing statistics for a registered function
 * Returns: pointer to stats, or NULL if index is invalid
 */
timing_stats_t const * timing_get_stats(int32_t func_index)
{
    timing_stats_t const * result;
    
    if ((func_index >= 0) && ((uint32_t)func_index < function_count))
    {
        result = &timing_registry[func_index].stats;
    }
    else
    {
        result = NULL;
    }
    
    return result;
}

/*
 * Reset statistics for a specific function
 */
void timing_reset_stats(int32_t func_index)
{
    if ((func_index >= 0) && ((uint32_t)func_index < function_count))
    {
        timing_registry[func_index].stats.call_count = 0U;
        timing_registry[func_index].stats.total_cycles = 0U;
        timing_registry[func_index].stats.min_cycles = UINT32_MAX;
        timing_registry[func_index].stats.max_cycles = 0U;
        timing_registry[func_index].stats.last_cycles = 0U;
    }
}

/*
 * Reset statistics for all functions
 */
void timing_reset_all_stats(void)
{
    uint32_t i;
    
    for (i = 0U; i < function_count; i++)
    {
        timing_registry[i].stats.call_count = 0U;
        timing_registry[i].stats.total_cycles = 0U;
        timing_registry[i].stats.min_cycles = UINT32_MAX;
        timing_registry[i].stats.max_cycles = 0U;
        timing_registry[i].stats.last_cycles = 0U;
    }
}

/*
 * Get the number of registered functions
 */
uint32_t timing_get_function_count(void)
{
    return function_count;
}

/*
 * Get function name by index
 * Returns: pointer to function name string, or NULL if invalid
 */
char const * timing_get_function_name(int32_t func_index)
{
    char const * result;
    
    if ((func_index >= 0) && ((uint32_t)func_index < function_count))
    {
        result = timing_registry[func_index].name;
    }
    else
    {
        result = NULL;
    }
    
    return result;
}

/*
 * Internal wrapper function - records entry timestamp
 * Returns: function index for use in exit wrapper
 */
int32_t timing_wrapper_enter(char const * const func_name)
{
    int32_t func_index;
    
    /* Register or lookup function */
    func_index = timing_register_function(func_name);
    
    return func_index;
}

/*
 * Internal wrapper function - records exit timestamp and updates stats
 */
void timing_wrapper_exit(int32_t func_index, uint32_t entry_timestamp)
{
    uint32_t exit_timestamp;
    uint32_t elapsed_cycles;
    
    if ((func_index >= 0) && ((uint32_t)func_index < function_count))
    {
        exit_timestamp = port_get_cycle_count();
        
        /* Calculate elapsed cycles (handles 32-bit wraparound) */
        elapsed_cycles = exit_timestamp - entry_timestamp;
        
        /* Update statistics */
        timing_registry[func_index].stats.call_count++;
        timing_registry[func_index].stats.total_cycles += elapsed_cycles;
        timing_registry[func_index].stats.last_cycles = elapsed_cycles;
        
        /* Update min cycles */
        if (elapsed_cycles < timing_registry[func_index].stats.min_cycles)
        {
            timing_registry[func_index].stats.min_cycles = elapsed_cycles;
        }
        
        /* Update max cycles */
        if (elapsed_cycles > timing_registry[func_index].stats.max_cycles)
        {
            timing_registry[func_index].stats.max_cycles = elapsed_cycles;
        }
    }
}
