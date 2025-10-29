# et_port_stm32

BARR-C Compliant Function Timing Decorator System for STM32/ARM Cortex-M

## Overview

This repository provides a lightweight, BARR-C compliant timing wrapper system for embedded C projects running on STM32 and other ARM Cortex-M microcontrollers. The system automatically tracks function execution time using hardware cycle counters and maintains per-function performance metrics.

## Features

- **BARR-C Compliant**: Single return per function, explicit types, deterministic behavior
- **Hardware-Based Timing**: Uses ARM Cortex-M DWT (Data Watchpoint and Trace) cycle counter
- **Zero Runtime Overhead for I/O**: No printf or dynamic allocation in timing critical paths
- **Reentrant-Safe**: Per-function static statistics with deterministic behavior
- **Automatic Metrics**: Tracks call count, min/max/average/total cycles per function
- **Simple API**: Easy-to-use macros for wrapping functions
- **Nested Function Support**: Each function tracks its own timing independently

## Files

- `timing_wrapper_system.h` - Public API and data structures
- `timing_wrapper_system.c` - Core timing system implementation
- `port.h` - MCU-specific port layer interface
- `port.c` - STM32/ARM Cortex-M DWT implementation
- `example_usage.c` - Example code demonstrating usage

## Quick Start

### 1. Initialize the Timing System

```c
#include "timing_wrapper_system.h"

int main(void)
{
    /* Initialize timing system (must be called before any timed functions) */
    timing_system_init();
    
    /* Your application code */
    
    return 0;
}
```

### 2. Wrap Functions with Timing

```c
static uint32_t my_function(uint32_t param)
{
    uint32_t result;
    
    /* Add these two lines to enable timing */
    TIMED_FUNCTION_ENTER("my_function");
    
    /* Your function implementation */
    result = param * 2U;
    
    /* Add this before return */
    TIMED_FUNCTION_EXIT();
    
    return result;
}
```

### 3. Retrieve Timing Statistics

```c
void print_timing_stats(void)
{
    uint32_t i;
    uint32_t func_count;
    timing_stats_t const * stats;
    char const * func_name;
    
    func_count = timing_get_function_count();
    
    for (i = 0U; i < func_count; i++)
    {
        func_name = timing_get_function_name((int32_t)i);
        stats = timing_get_stats((int32_t)i);
        
        if ((func_name != NULL) && (stats != NULL))
        {
            /* Access statistics:
             * stats->call_count    - Number of times called
             * stats->total_cycles  - Total cycles across all calls
             * stats->min_cycles    - Minimum cycles in a single call
             * stats->max_cycles    - Maximum cycles in a single call
             * stats->last_cycles   - Cycles from most recent call
             * Average = stats->total_cycles / stats->call_count
             */
        }
    }
}
```

## API Reference

### Initialization

- `void timing_system_init(void)` - Initialize the timing system. Must be called once at startup.

### Statistics Access

- `uint32_t timing_get_function_count(void)` - Get number of registered functions
- `char const * timing_get_function_name(int32_t func_index)` - Get function name by index
- `timing_stats_t const * timing_get_stats(int32_t func_index)` - Get timing statistics for a function
- `void timing_reset_stats(int32_t func_index)` - Reset statistics for one function
- `void timing_reset_all_stats(void)` - Reset statistics for all functions

### Timing Macros

- `TIMED_FUNCTION_ENTER(name)` - Place at start of function (after variable declarations in C90)
- `TIMED_FUNCTION_EXIT()` - Place before each return statement

## Statistics Structure

```c
typedef struct
{
    uint32_t call_count;      /* Number of times function was called */
    uint32_t total_cycles;    /* Total cycles spent in function */
    uint32_t min_cycles;      /* Minimum cycles for a single call */
    uint32_t max_cycles;      /* Maximum cycles for a single call */
    uint32_t last_cycles;     /* Cycles from the last call */
} timing_stats_t;
```

## Configuration

- `MAX_TIMED_FUNCTIONS` - Maximum number of functions that can be timed (default: 32)
  - Defined in `timing_wrapper_system.h`
  - Adjust based on your application needs

## Platform Requirements

### ARM Cortex-M
- Requires DWT (Data Watchpoint and Trace) unit
- Available on most ARM Cortex-M3, M4, M7 processors
- DWT cycle counter provides cycle-accurate timing

### Other Platforms
To port to other MCUs, implement the following functions in `port.c`:
- `void port_cycle_counter_init(void)` - Initialize hardware cycle counter
- `uint32_t port_get_cycle_count(void)` - Read current cycle count

## BARR-C Compliance

This implementation follows BARR-C embedded coding standards:
- Single return point per function (enforced through careful control flow)
- Explicit types (uint32_t, int32_t instead of int)
- No dynamic memory allocation
- Deterministic execution time
- Reentrant-safe (no global state mutations during timing measurement)
- No runtime I/O in critical timing paths

## Example Output

When running the example code, timing statistics are collected for all wrapped functions:

```
Function: calculate_fibonacci
  Call count: 100
  Total cycles: 45,230
  Min cycles: 420
  Max cycles: 485
  Avg cycles: 452

Function: sum_array
  Call count: 100
  Total cycles: 12,340
  Min cycles: 115
  Max cycles: 130
  Avg cycles: 123
```

## Notes

- The 32-bit cycle counter wraps around approximately every 59.6 seconds at 72 MHz
- Wraparound is handled correctly for individual function calls
- For accurate timing, keep individual function execution under wraparound period
- Nested function calls: each function tracks its own total time including called functions

## License

This code is provided as-is for embedded systems development.
