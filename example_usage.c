/*
 * example_usage.c
 *
 * Demonstrates how to use the timing wrapper system
 * with BARR-C compliant function wrapping
 */

#include "timing_wrapper_system.h"
#include "port.h"
#include <stdint.h>

/* Example function that performs some work */
static uint32_t calculate_fibonacci(uint32_t n)
{
    uint32_t result;
    uint32_t a;
    uint32_t b;
    uint32_t temp;
    uint32_t i;
    
    /* Timing wrapper entry - registers function and captures start time */
    TIMED_FUNCTION_ENTER("calculate_fibonacci");
    
    /* Function implementation */
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
    
    /* Timing wrapper exit - records elapsed time and updates statistics */
    TIMED_FUNCTION_EXIT();
    
    return result;
}

/* Example function that performs array processing */
static uint32_t sum_array(uint32_t const * const array, uint32_t length)
{
    uint32_t result;
    uint32_t i;
    
    /* Timing wrapper entry */
    TIMED_FUNCTION_ENTER("sum_array");
    
    result = 0U;
    
    if (array != NULL)
    {
        for (i = 0U; i < length; i++)
        {
            result += array[i];
        }
    }
    
    /* Timing wrapper exit */
    TIMED_FUNCTION_EXIT();
    
    return result;
}

/* Example function showing nested timing (outer function) */
static void process_data(void)
{
    uint32_t test_data[10] = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U, 9U, 10U};
    uint32_t sum;
    uint32_t fib;
    
    /* Timing wrapper entry for outer function */
    TIMED_FUNCTION_ENTER("process_data");
    
    /* Call timed functions (each tracks its own timing) */
    sum = sum_array(test_data, 10U);
    fib = calculate_fibonacci(10U);
    
    /* Prevent compiler optimization */
    (void)sum;
    (void)fib;
    
    /* Timing wrapper exit */
    TIMED_FUNCTION_EXIT();
}

/*
 * Example main function demonstrating the timing system usage
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
    
    /* Run some timed functions multiple times */
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
    
    /* Example: Reset statistics for a specific function */
    timing_reset_stats(0);
    
    /* Example: Reset all statistics */
    timing_reset_all_stats();
    
    retval = 0;
    return retval;
}
