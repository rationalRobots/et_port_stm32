/*
 * port.h
 *
 * MCU-specific port layer for timing system
 * Provides low-level cycle counter access
 */

#ifndef PORT_H
#define PORT_H

#include <stdint.h>

/*
 * Get the current CPU cycle count
 *
 * For ARM Cortex-M with DWT (Data Watchpoint and Trace):
 *   Returns the value of DWT->CYCCNT register
 *
 * For other MCUs:
 *   Implement using available hardware timer/counter
 *
 * Returns: Current cycle count (32-bit, wraps around)
 */
extern uint32_t port_get_cycle_count(void);

/*
 * Initialize the cycle counter hardware
 * Must be called once at system startup before using port_get_cycle_count()
 *
 * For ARM Cortex-M with DWT:
 *   Enables the DWT cycle counter (DWT->CYCCNT)
 */
extern void port_cycle_counter_init(void);

#endif /* PORT_H */
