/*H************************************************************************************************
 * FILENAME:        timer_hal.h
 *
 * DESCRIPTION:
 *      Timer(s) Hardware Abstraction Layer (HAL), this source file provides an abstraction over
 *      the usage of two 32-bit timers:
 *      - Periodic Timer: a timer continuously running at 93750Hz.
 *      - Shared Timer: a timer working in one shot mode that can be activated on request
 *
 * PUBLIC FUNCTIONS:
 *      void    TIMER_HAL_init()
 *      void    TIMER_HAL_setupPeriodicTimer(uint32_t count);
 *      void    TIMER_HAL_registerPeriodicTimerCallback(TimerCallback callback);
 *      void    TIMER_HAL_acquireSharedTimer(uint32_t count, TimerCallback callback);
 *      void    TIMER_HAL_releaseSharedTimer();
 *
 * NOTES:
 *
 * AUTHOR: Andrea Piccin    <andrea.piccin@studenti.unitn.it>
 *
 * START DATE: 20 Feb 2024
 *
 * CHANGES:
 */
#include <stdbool.h>

#include "../../inc/driverlib/driverlib.h"
#include "../../inc/timer_hal.h"

/*F************************************************************************************************
 * NAME: void TIMER_HAL_init();
 *
 * DESCRIPTION:
 *      Initialises the timer's hardware with the following steps:
 *      [1] Timer32_0 initialization (periodic) at MCLK/256 Hz with 32-bit resolution
 *      [2] Timer32_1 initialization (shared) at MCLK/256 Hz with 32-bit resolution
 *
 * INPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void TIMER_HAL_init() {
    // [1] Timer 0 initialization
    Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_256, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    Timer32_clearInterruptFlag(TIMER32_0_BASE);
    Timer32_enableInterrupt(TIMER32_0_BASE);
    Interrupt_enableInterrupt(INT_T32_INT1);

    // [2] Timer 1 initialisation
    Timer32_initModule(TIMER32_1_BASE, TIMER32_PRESCALER_256, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    Timer32_clearInterruptFlag(TIMER32_1_BASE);
    Timer32_enableInterrupt(TIMER32_1_BASE);
    Interrupt_enableInterrupt(INT_T32_INT2);
}

/*F************************************************************************************************
 * NAME: void TIMER_HAL_setupPeriodicTimer(uint32_t count, TimerCallback callback)
 *
 * DESCRIPTION:
 *      Set up a 32-bit timer in order to perform a countdown from the given count to zero, trigger
 *      an interrupt and restart.
 *
 * INPUTS:
 *      PARAMETERS:
 *          uint32_t          count           Number of ticks of the period (1 tick = 0.01ms)
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 *      The callback function must clear the TIMER32_1_INTERRUPT flag.
 */
void TIMER_HAL_setupPeriodicTimer(uint32_t count) {
    Timer32_setCount(TIMER32_1_BASE, count);
    Timer32_startTimer(TIMER32_1_BASE, false);
}

/*F************************************************************************************************
 * NAME: void TTIMER_HAL_registerPeriodicTimerCallback(TimerCallback callback);
 *
 * DESCRIPTION:
 *      Registers the given function as ISR for the TIMER32_1_INTERRUPT interrupt.
 *
 * INPUTS:
 *      PARAMETERS:
 *          TimerCallback     callback        Function to call on timer expiration
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 *      The callback function must clear the TIMER32_1_INTERRUPT flag.
 */
void TIMER_HAL_registerPeriodicTimerCallback(TimerCallback callback) {
    Interrupt_registerInterrupt(INT_T32_INT2, callback);
}

/*F************************************************************************************************
 * NAME: void TIMER_HAL_acquireSharedTimer(uint32_t count, TimerCallback callback)
 *
 * DESCRIPTION:
 *      Set up the shared 32-bit timer in order to perform a countdown from the given count to
 *      zero and then call the given callback function.
 *
 * INPUTS:
 *      PARAMETERS:
 *          uint32_t          count           Number of ticks to wait (1 tick = 0.01ms)
 *          TimerCallback     callback        Function to call on timer expiration
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 *      The callback function must clear the TIMER32_0_INTERRUPT flag.
 */
void TIMER_HAL_acquireSharedTimer(uint32_t count, TimerCallback callback) {
    Timer32_setCount(TIMER32_0_BASE, count);
    Interrupt_registerInterrupt(INT_T32_INT1, callback);
    Timer32_startTimer(TIMER32_0_BASE, true);
}

/*F************************************************************************************************
 * NAME: void TIMER_HAL_releaseSharedTimer()
 *
 * DESCRIPTION:
 *      Stops the shared timer.
 *
 * INPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void TIMER_HAL_releaseSharedTimer() { Timer32_haltTimer(TIMER32_0_BASE); }