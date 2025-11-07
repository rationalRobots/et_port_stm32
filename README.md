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
- `example_decorator_usage.c` - Example using automatic decorator approach (recommended)
- `example_usage.c` - Example using manual instrumentation approach
- `stm32_integration_example.c` - Real-world STM32 usage patterns

## Quick Start

There are two ways to use the timing system:
1. **Automatic Decorator Approach** (Recommended) - No manual instrumentation needed
2. **Manual Instrumentation** - For more control or special cases

### Method 1: Automatic Decorator (Recommended)

#### 1. Initialize the Timing System

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

#### 2. Enable Profiling for Functions

Simply add `PROFILE_FUNC_ENABLE` before your function definition:

```c
/* For functions that return a value */
PROFILE_FUNC_ENABLE(my_function, uint32_t, (uint32_t param), (param))
{
    /* Your function implementation - no timing macros needed! */
    uint32_t result = param * 2U;
    return result;
}

/* For void functions */
PROFILE_FUNC_ENABLE_VOID(init_hardware, (void), ())
{
    /* Your initialization code */
}

/* For functions with multiple parameters */
PROFILE_FUNC_ENABLE(add_numbers, uint32_t, (uint32_t a, uint32_t b), (a, b))
{
    return a + b;
}
```

The macro parameters are:
- **func_name**: Name of your function
- **return_type**: Return type (omit for PROFILE_FUNC_ENABLE_VOID)
- **params**: Parameter list WITH types, e.g., `(uint32_t a, float b)`
- **args**: Argument list WITHOUT types, e.g., `(a, b)`

### Method 2: Manual Instrumentation

For cases where you need more control:

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

Both methods produce the same timing statistics:

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

## How the Decorator Approach Works

The `PROFILE_FUNC_ENABLE` macro uses preprocessor magic to automatically wrap your function:

**What you write:**
```c
PROFILE_FUNC_ENABLE(my_func, uint32_t, (uint32_t x), (x))
{
    return x * 2;
}
```

**What the preprocessor generates:**
```c
// Implementation function (hidden from callers)
static uint32_t my_func_impl(uint32_t x) {
    return x * 2;
}

// Wrapper function (this is what gets called)
uint32_t my_func(uint32_t x) {
    int32_t const timing_idx_ = timing_wrapper_enter("my_func");
    uint32_t const timing_start_ = port_get_cycle_count();
    uint32_t const result_ = my_func_impl(x);
    timing_wrapper_exit(timing_idx_, timing_start_);
    return result_;
}
```

Benefits:
- No manual instrumentation needed inside function body
- Original function logic remains clean and readable
- Easy to enable/disable profiling by removing one line
- Works with any return type and parameter list

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
