/*
 * example_decorator_usage.c
 *
 * Demonstrates the automatic decorator-based profiling approach
 * using PROFILE_FUNC_ENABLE macro
 */

#include "timing_wrapper_system.h"
#include "port.h"
#include <stdint.h>
#include <stddef.h>

/*
 * Example 1: Simple function with return value
 * Just add PROFILE_FUNC_ENABLE before the function body
 */
PROFILE_FUNC_ENABLE(calculate_fibonacci, uint32_t, (uint32_t n), (n))
{
    uint32_t result;
    uint32_t a;
    uint32_t b;
    uint32_t temp;
    uint32_t i;
    
    /* Function implementation - no timing macros needed! */
    if (n <= 1U)
    {
        result = n;
    }
    else
    {
        a = 0U;
        b = 1U;
        
        for (i = 2U; i <= n; i++)
        {
            temp = a + b;
            a = b;
            b = temp;
        }
        
        result = b;
    }
    
    return result;
}

/*
 * Example 2: Function with multiple parameters
 */
PROFILE_FUNC_ENABLE(sum_array, uint32_t, (uint32_t const * const array, uint32_t length), (array, length))
{
    uint32_t result;
    uint32_t i;
    
    result = 0U;
    
    if (array != NULL)
    {
        for (i = 0U; i < length; i++)
        {
            result += array[i];
        }
    }
    
    return result;
}

/*
 * Example 3: Void function
 * Use PROFILE_FUNC_ENABLE_VOID for functions that don't return a value
 */
PROFILE_FUNC_ENABLE_VOID(process_data, (void), ())
{
    uint32_t test_data[10] = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U};
    uint32_t sum;
    uint32_t fib;
    
    /* Call other profiled functions */
    sum = sum_array(test_data, 10U);
    fib = calculate_fibonacci(10U);
    
    /* Prevent compiler optimization */
    (void)sum;
    (void)fib;
}

/*
 * Example 4: Function with complex return type
 */
typedef struct
{
    uint32_t min;
    uint32_t max;
    uint32_t sum;
} array_stats_t;

PROFILE_FUNC_ENABLE(analyze_array, array_stats_t, (uint32_t const * const array, uint32_t length), (array, length))
{
    array_stats_t stats;
    uint32_t i;
    
    stats.min = UINT32_MAX;
    stats.max = 0U;
    stats.sum = 0U;
    
    if ((array != NULL) && (length > 0U))
    {
        for (i = 0U; i < length; i++)
        {
            if (array[i] < stats.min)
            {
                stats.min = array[i];
            }
            if (array[i] > stats.max)
            {
                stats.max = array[i];
            }
            stats.sum += array[i];
        }
    }
    
    return stats;
}

/*
 * Example main function demonstrating the decorator-based timing system
 */
int main(void)
{
    uint32_t i;
    timing_stats_t const * stats;
    uint32_t func_count;
    char const * func_name;
    int32_t retval;
    
    /* Initialize the timing system (must be called first) */
    timing_system_init();
    
    /* Call profiled functions - timing happens automatically! */
    for (i = 0U; i < 100U; i++)
    {
        process_data();
    }
    
    /* Retrieve and process timing statistics */
    func_count = timing_get_function_count();
    
    for (i = 0U; i < func_count; i++)
    {
        func_name = timing_get_function_name((int32_t)i);
        stats = timing_get_stats((int32_t)i);
        
        if ((func_name != NULL) && (stats != NULL))
        {
            /*
             * In a real embedded system, you would log these statistics
             * to a debug interface, store them in non-volatile memory,
             * or transmit them over a communication interface.
             *
             * Statistics available:
             * - stats->call_count: Number of times function was called
             * - stats->total_cycles: Total cycles spent in function
             * - stats->min_cycles: Minimum cycles for a single call
             * - stats->max_cycles: Maximum cycles for a single call
             * - stats->last_cycles: Cycles from the last call
             *
             * Calculate average: stats->total_cycles / stats->call_count
             */
        }
    }
    
    /* Example: Test analyze_array */
    uint32_t test_array[5] = {10U, 5U, 20U, 3U, 15U};
    array_stats_t array_result = analyze_array(test_array, 5U);
    (void)array_result;  /* Prevent unused warning */
    
    /* Example: Reset statistics for a specific function */
    timing_reset_stats(0);
    
    /* Example: Reset all statistics */
    timing_reset_all_stats();
    
    retval = 0;
    return retval;
}
