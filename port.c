/*
 * port.c
 *
 * STM32/ARM Cortex-M implementation of port layer for timing system
 * Uses DWT (Data Watchpoint and Trace) cycle counter
 */

#include "port.h"

/* ARM Cortex-M DWT (Data Watchpoint and Trace) Register Addresses */
#define DWT_CTRL_REG        (*((volatile uint32_t *)0xE0001000U))
#define DWT_CYCCNT_REG      (*((volatile uint32_t *)0xE0001004U))
#define DEM_CR_REG          (*((volatile uint32_t *)0xE000EDFCU))

/* DWT Control Register bit definitions */
#define DWT_CTRL_CYCCNTENA  (1U << 0)   /* Enable CYCCNT */

/* Debug Exception and Monitor Control Register bit definitions */
#define DEM_CR_TRCENA       (1U << 24)  /* Enable trace and debug blocks */

/*
 * Initialize the cycle counter hardware
 * Enables the DWT cycle counter for ARM Cortex-M
 */
void port_cycle_counter_init(void)
{
    /* Enable trace and debug blocks (required for DWT) */
    DEM_CR_REG |= DEM_CR_TRCENA;
    
    /* Reset the cycle counter */
    DWT_CYCCNT_REG = 0U;
    
    /* Enable the cycle counter */
    DWT_CTRL_REG |= DWT_CTRL_CYCCNTENA;
}

/*
 * Get the current CPU cycle count
 * Returns the value of DWT->CYCCNT register
 */
uint32_t port_get_cycle_count(void)
{
    return DWT_CYCCNT_REG;
}
