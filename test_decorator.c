/*
 * test_decorator.c
 *
 * Test the automatic decorator-based profiling
 */

#include "timing_wrapper_system.h"
#include <stdio.h>
#include <assert.h>

/* Mock implementation of port functions for testing on host */
static uint32_t mock_cycle_count = 0U;

uint32_t port_get_cycle_count(void)
{
    return mock_cycle_count;
}

void port_cycle_counter_init(void)
{
    mock_cycle_count = 0U;
}

/* Helper to advance mock cycle counter */
static void advance_cycles(uint32_t cycles)
{
    mock_cycle_count += cycles;
}

/* Test function with decorator - simple return value */
PROFILE_FUNC_ENABLE(add_numbers, uint32_t, (uint32_t a, uint32_t b), (a, b))
{
    advance_cycles(50U);  /* Simulate 50 cycles of work */
    return a + b;
}

/* Test function with decorator - void function */
PROFILE_FUNC_ENABLE_VOID(do_work, (void), ())
{
    advance_cycles(100U);  /* Simulate 100 cycles of work */
}

/* Test function with decorator - multiple parameters */
PROFILE_FUNC_ENABLE(multiply_three, uint32_t, (uint32_t a, uint32_t b, uint32_t c), (a, b, c))
{
    advance_cycles(75U);  /* Simulate 75 cycles of work */
    return a * b * c;
}

/* Test basic decorator functionality */
static void test_decorator_basic(void)
{
    uint32_t result;
    timing_stats_t const * stats;
    int32_t func_idx;
    
    printf("Testing basic decorator functionality...\n");
    
    timing_system_init();
    mock_cycle_count = 0U;
    
    /* Call decorated function */
    result = add_numbers(5U, 3U);
    assert(result == 8U);
    
    /* Find the function in the registry */
    func_idx = timing_register_function("add_numbers");
    stats = timing_get_stats(func_idx);
    
    assert(stats != NULL);
    assert(stats->call_count == 1U);
    assert(stats->last_cycles == 50U);
    
    printf("  PASS: Basic decorator works\n");
}

/* Test void function decorator */
static void test_decorator_void(void)
{
    timing_stats_t const * stats;
    int32_t func_idx;
    
    printf("Testing void function decorator...\n");
    
    timing_system_init();
    mock_cycle_count = 0U;
    
    /* Call decorated void function */
    do_work();
    
    /* Find the function in the registry */
    func_idx = timing_register_function("do_work");
    stats = timing_get_stats(func_idx);
    
    assert(stats != NULL);
    assert(stats->call_count == 1U);
    assert(stats->last_cycles == 100U);
    
    printf("  PASS: Void function decorator works\n");
}

/* Test multiple parameter decorator */
static void test_decorator_multiple_params(void)
{
    uint32_t result;
    timing_stats_t const * stats;
    int32_t func_idx;
    
    printf("Testing multiple parameter decorator...\n");
    
    timing_system_init();
    mock_cycle_count = 0U;
    
    /* Call decorated function with multiple parameters */
    result = multiply_three(2U, 3U, 4U);
    assert(result == 24U);
    
    /* Find the function in the registry */
    func_idx = timing_register_function("multiply_three");
    stats = timing_get_stats(func_idx);
    
    assert(stats != NULL);
    assert(stats->call_count == 1U);
    assert(stats->last_cycles == 75U);
    
    printf("  PASS: Multiple parameter decorator works\n");
}

/* Test multiple calls to decorated function */
static void test_decorator_multiple_calls(void)
{
    uint32_t i;
    timing_stats_t const * stats;
    int32_t func_idx;
    
    printf("Testing multiple calls to decorated function...\n");
    
    timing_system_init();
    mock_cycle_count = 0U;
    
    /* Call decorated function multiple times */
    for (i = 0U; i < 10U; i++)
    {
        (void)add_numbers(i, i);
    }
    
    /* Find the function in the registry */
    func_idx = timing_register_function("add_numbers");
    stats = timing_get_stats(func_idx);
    
    assert(stats != NULL);
    assert(stats->call_count == 10U);
    assert(stats->total_cycles == 500U);  /* 10 calls * 50 cycles each */
    assert(stats->min_cycles == 50U);
    assert(stats->max_cycles == 50U);
    
    printf("  PASS: Multiple calls tracked correctly\n");
}

/* Main test runner */
int main(void)
{
    printf("=== Running Decorator Tests ===\n\n");
    
    test_decorator_basic();
    test_decorator_void();
    test_decorator_multiple_params();
    test_decorator_multiple_calls();
    
    printf("\n=== All Decorator Tests Passed ===\n");
    
    return 0;
}
