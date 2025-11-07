/*
 * stm32_integration_example.c
 *
 * Example showing integration of timing system in a real STM32 application
 * Demonstrates common embedded patterns: ISRs, callbacks, and main loop
 */

#include "timing_wrapper_system.h"
#include "port.h"
#include <stdint.h>
#include <stddef.h>

/*
 * Example: Timing a signal processing function
 * Common in DSP applications on STM32
 */
static void process_adc_sample(uint16_t sample)
{
    uint32_t filtered_value;
    static uint32_t prev_sample = 0U;
    
    TIMED_FUNCTION_ENTER("process_adc_sample");
    
    /* Simple low-pass filter */
    filtered_value = (prev_sample + (uint32_t)sample) >> 1;
    prev_sample = filtered_value;
    
    TIMED_FUNCTION_EXIT();
}

/*
 * Example: Timing a communication protocol handler
 * Common in UART/SPI/I2C applications
 */
static uint8_t calculate_crc8(uint8_t const * const data, uint32_t length)
{
    uint8_t crc;
    uint32_t i;
    uint8_t byte;
    uint32_t bit;
    
    TIMED_FUNCTION_ENTER("calculate_crc8");
    
    crc = 0U;
    
    if (data != NULL)
    {
        for (i = 0U; i < length; i++)
        {
            byte = data[i];
            
            for (bit = 0U; bit < 8U; bit++)
            {
                if (((crc ^ byte) & 0x80U) != 0U)
                {
                    crc = (crc << 1) ^ 0x07U;
                }
                else
                {
                    crc = crc << 1;
                }
                
                byte = byte << 1;
            }
        }
    }
    
    TIMED_FUNCTION_EXIT();
    
    return crc;
}

/*
 * Example: Timing a state machine update
 * Common in control applications
 */
typedef enum
{
    STATE_IDLE = 0U,
    STATE_ACTIVE = 1U,
    STATE_ERROR = 2U
} system_state_t;

static system_state_t update_state_machine(system_state_t current_state, uint8_t input)
{
    system_state_t next_state;
    
    TIMED_FUNCTION_ENTER("update_state_machine");
    
    next_state = STATE_IDLE;  /* Default state */
    
    switch (current_state)
    {
        case STATE_IDLE:
            if (input == 1U)
            {
                next_state = STATE_ACTIVE;
            }
            else
            {
                next_state = STATE_IDLE;
            }
            break;
            
        case STATE_ACTIVE:
            if (input == 0U)
            {
                next_state = STATE_IDLE;
            }
            else if (input == 2U)
            {
                next_state = STATE_ERROR;
            }
            else
            {
                next_state = STATE_ACTIVE;
            }
            break;
            
        case STATE_ERROR:
            if (input == 3U)
            {
                next_state = STATE_IDLE;
            }
            else
            {
                next_state = STATE_ERROR;
            }
            break;
            
        default:
            next_state = STATE_IDLE;
            break;
    }
    
    TIMED_FUNCTION_EXIT();
    
    return next_state;
}

/*
 * Example: Retrieving and transmitting timing statistics
 * Useful for runtime performance monitoring
 */
static void transmit_timing_report_via_uart(void)
{
    uint32_t i;
    uint32_t func_count;
    timing_stats_t const * stats;
    char const * func_name;
    uint32_t avg_cycles;
    
    TIMED_FUNCTION_ENTER("transmit_timing_report_via_uart");
    
    func_count = timing_get_function_count();
    
    for (i = 0U; i < func_count; i++)
    {
        func_name = timing_get_function_name((int32_t)i);
        stats = timing_get_stats((int32_t)i);
        
        if ((func_name != NULL) && (stats != NULL) && (stats->call_count > 0U))
        {
            /* Calculate average cycles (call_count > 0 check prevents division by zero) */
            avg_cycles = stats->total_cycles / stats->call_count;
            
            /*
             * In real code, format and transmit these via UART/USB/CAN:
             * - Function name: func_name
             * - Call count: stats->call_count
             * - Average cycles: avg_cycles
             * - Min cycles: stats->min_cycles
             * - Max cycles: stats->max_cycles
             *
             * Example output format:
             * "process_adc_sample,1000,234,180,350\n"
             * name,calls,avg,min,max
             */
        }
    }
    
    TIMED_FUNCTION_EXIT();
}

/*
 * Example: Periodic timing statistics collection
 * Called from a timer interrupt or main loop
 */
static void periodic_timing_check(void)
{
    static uint32_t check_counter = 0U;
    
    check_counter++;
    
    /* Every 1000 iterations, generate a timing report */
    if (check_counter >= 1000U)
    {
        transmit_timing_report_via_uart();
        
        /* Optionally reset statistics after reporting */
        /* timing_reset_all_stats(); */
        
        check_counter = 0U;
    }
}

/*
 * Example: Main application initialization and loop
 */
void application_main(void)
{
    system_state_t current_state;
    uint8_t test_data[16];
    uint8_t crc;
    uint32_t i;
    uint32_t loop_counter;
    
    /* Initialize hardware and timing system */
    /* HAL_Init(); */  /* STM32 HAL initialization */
    /* SystemClock_Config(); */  /* Configure system clocks */
    
    timing_system_init();  /* Initialize timing wrapper system */
    
    /* Application state */
    current_state = STATE_IDLE;
    loop_counter = 0U;
    
    /* Main application loop */
    while (1)
    {
        /* Process ADC samples */
        process_adc_sample((uint16_t)(loop_counter & 0xFFFU));
        
        /* Update state machine */
        current_state = update_state_machine(current_state, (uint8_t)(loop_counter % 4U));
        
        /* Calculate CRC for test data */
        for (i = 0U; i < 16U; i++)
        {
            test_data[i] = (uint8_t)(i + loop_counter);
        }
        crc = calculate_crc8(test_data, 16U);
        (void)crc;  /* Prevent unused variable warning */
        
        /* Periodic timing report */
        periodic_timing_check();
        
        loop_counter++;
        
        /* In a real application, you might use WFI (Wait For Interrupt) here */
        /* __WFI(); */
    }
}

/*
 * Example: Timing functions called from interrupts
 * Note: Keep ISR timing functions short and deterministic
 */
void TIM2_IRQHandler(void)
{
    TIMED_FUNCTION_ENTER("TIM2_IRQHandler");
    
    /* Handle timer interrupt */
    /* Process time-critical tasks */
    
    TIMED_FUNCTION_EXIT();
    
    /* Clear interrupt flag in real code */
    /* __HAL_TIM_CLEAR_IT(&htim2, TIM_IT_UPDATE); */
}

/*
 * Best Practices for Using Timing System in STM32:
 *
 * 1. Initialize Early:
 *    - Call timing_system_init() right after SystemClock_Config()
 *    - Ensures DWT cycle counter is ready before any timed functions
 *
 * 2. Function Selection:
 *    - Time performance-critical functions
 *    - Time functions suspected of causing delays
 *    - Time ISRs to ensure they meet real-time constraints
 *
 * 3. Statistics Collection:
 *    - Collect statistics periodically (not continuously)
 *    - Transmit via low-priority communication channel
 *    - Consider storing to flash for post-analysis
 *
 * 4. Memory Considerations:
 *    - MAX_TIMED_FUNCTIONS uses ~100 bytes per function
 *    - Adjust based on available RAM
 *    - 32 functions = ~3.2KB of RAM (32 * 100 bytes)
 *
 * 5. Overhead:
 *    - Entry/exit wrappers add ~20-50 cycles overhead
 *    - Negligible for functions > 100 cycles
 *    - Disable timing in production by removing macros
 *
 * 6. Cycle Counter Wraparound:
 *    - At 168MHz (STM32F4): wraps every ~25 seconds
 *    - At 72MHz (STM32F1): wraps every ~59 seconds
 *    - Individual function timing handles wraparound correctly
 *    - Keep function execution time < wraparound period
 */
