/*
 * test_timing_system.c
 *
 * Unit tests for the timing wrapper system
 * Can be compiled with standard gcc for testing on host
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

/* Test initialization */
static void test_initialization(void)
{
    printf("Testing initialization...\n");
    
    timing_system_init();
    
    assert(timing_get_function_count() == 0U);
    
    printf("  PASS: System initializes with zero functions\n");
}

/* Test function registration */
static void test_function_registration(void)
{
    int32_t idx1;
    int32_t idx2;
    int32_t idx3;
    char const * name;
    
    printf("Testing function registration...\n");
    
    timing_system_init();
    
    /* Register first function */
    idx1 = timing_register_function("func1");
    assert(idx1 == 0);
    assert(timing_get_function_count() == 1U);
    
    /* Register second function */
    idx2 = timing_register_function("func2");
    assert(idx2 == 1);
    assert(timing_get_function_count() == 2U);
    
    /* Re-register first function (should return same index) */
    idx3 = timing_register_function("func1");
    assert(idx3 == 0);
    assert(timing_get_function_count() == 2U);
    
    /* Verify names */
    name = timing_get_function_name(0);
    assert(name != NULL);
    
    name = timing_get_function_name(1);
    assert(name != NULL);
    
    printf("  PASS: Functions register correctly\n");
}

/* Test basic timing */
static void test_basic_timing(void)
{
    int32_t func_idx;
    timing_stats_t const * stats;
    
    printf("Testing basic timing...\n");
    
    timing_system_init();
    mock_cycle_count = 1000U;
    
    /* Simulate function call with 100 cycles */
    func_idx = timing_wrapper_enter("test_func");
    uint32_t start = port_get_cycle_count();
    advance_cycles(100U);
    timing_wrapper_exit(func_idx, start);
    
    /* Verify statistics */
    stats = timing_get_stats(func_idx);
    assert(stats != NULL);
    assert(stats->call_count == 1U);
    assert(stats->last_cycles == 100U);
    assert(stats->total_cycles == 100U);
    assert(stats->min_cycles == 100U);
    assert(stats->max_cycles == 100U);
    
    printf("  PASS: Basic timing works correctly\n");
}

/* Test multiple calls */
static void test_multiple_calls(void)
{
    int32_t func_idx;
    timing_stats_t const * stats;
    uint32_t start;
    
    printf("Testing multiple calls...\n");
    
    timing_system_init();
    mock_cycle_count = 0U;
    
    /* First call: 100 cycles */
    func_idx = timing_wrapper_enter("multi_func");
    start = port_get_cycle_count();
    advance_cycles(100U);
    timing_wrapper_exit(func_idx, start);
    
    /* Second call: 200 cycles */
    start = port_get_cycle_count();
    advance_cycles(200U);
    timing_wrapper_exit(func_idx, start);
    
    /* Third call: 150 cycles */
    start = port_get_cycle_count();
    advance_cycles(150U);
    timing_wrapper_exit(func_idx, start);
    
    /* Verify statistics */
    stats = timing_get_stats(func_idx);
    assert(stats != NULL);
    assert(stats->call_count == 3U);
    assert(stats->total_cycles == 450U);
    assert(stats->min_cycles == 100U);
    assert(stats->max_cycles == 200U);
    assert(stats->last_cycles == 150U);
    
    printf("  PASS: Multiple calls tracked correctly\n");
}

/* Test statistics reset */
static void test_reset_stats(void)
{
    int32_t func_idx;
    timing_stats_t const * stats;
    uint32_t start;
    
    printf("Testing statistics reset...\n");
    
    timing_system_init();
    mock_cycle_count = 0U;
    
    /* Make a call */
    func_idx = timing_wrapper_enter("reset_func");
    start = port_get_cycle_count();
    advance_cycles(100U);
    timing_wrapper_exit(func_idx, start);
    
    /* Verify stats exist */
    stats = timing_get_stats(func_idx);
    assert(stats != NULL);
    assert(stats->call_count == 1U);
    
    /* Reset stats */
    timing_reset_stats(func_idx);
    
    /* Verify stats are cleared */
    stats = timing_get_stats(func_idx);
    assert(stats != NULL);
    assert(stats->call_count == 0U);
    assert(stats->total_cycles == 0U);
    assert(stats->min_cycles == UINT32_MAX);
    assert(stats->max_cycles == 0U);
    
    printf("  PASS: Statistics reset works\n");
}

/* Test wraparound handling */
static void test_wraparound(void)
{
    int32_t func_idx;
    timing_stats_t const * stats;
    uint32_t start;
    
    printf("Testing wraparound handling...\n");
    
    timing_system_init();
    mock_cycle_count = UINT32_MAX - 50U;
    
    /* Call that wraps around */
    func_idx = timing_wrapper_enter("wrap_func");
    start = port_get_cycle_count();
    advance_cycles(100U);  /* Will wrap to 49 */
    timing_wrapper_exit(func_idx, start);
    
    /* Verify wraparound is handled correctly */
    stats = timing_get_stats(func_idx);
    assert(stats != NULL);
    assert(stats->call_count == 1U);
    assert(stats->last_cycles == 100U);
    
    printf("  PASS: Wraparound handled correctly\n");
}

/* Test invalid parameters */
static void test_invalid_parameters(void)
{
    timing_stats_t const * stats;
    char const * name;
    
    printf("Testing invalid parameters...\n");
    
    timing_system_init();
    
    /* Invalid function index */
    stats = timing_get_stats(-1);
    assert(stats == NULL);
    
    stats = timing_get_stats(100);
    assert(stats == NULL);
    
    name = timing_get_function_name(-1);
    assert(name == NULL);
    
    name = timing_get_function_name(100);
    assert(name == NULL);
    
    /* Reset invalid index (should not crash) */
    timing_reset_stats(-1);
    timing_reset_stats(100);
    
    printf("  PASS: Invalid parameters handled safely\n");
}

/* Main test runner */
int main(void)
{
    printf("=== Running Timing System Tests ===\n\n");
    
    test_initialization();
    test_function_registration();
    test_basic_timing();
    test_multiple_calls();
    test_reset_stats();
    test_wraparound();
    test_invalid_parameters();
    
    printf("\n=== All Tests Passed ===\n");
    
    return 0;
}
