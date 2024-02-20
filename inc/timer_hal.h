/*H************************************************************************************************
 * FILENAME:        timer_hal.h
 *
 * DESCRIPTION:
 *      Timer(s) Hardware Abstraction Layer (HAL), this header file provides an abstraction over
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
#include <stdint.h>

#ifndef TIMER_HAL_H
#define TIMER_HAL_H

/*T************************************************************************************************
 * NAME: TimerCallback
 *
 * DESCRIPTION:
 *      It's a pointer to a function that executes when the timer reaches zero.
 *
 * SPECIFICATIONS:
 *      Type:   void*
 *      Args:   None
 */
typedef void (*TimerCallback)(void);

/*F************************************************************************************************
 * NAME: void TIMER_HAL_init();
 *
 * DESCRIPTION:
 *      Initialises the timer's hardware.
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
void TIMER_HAL_init();

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
void TIMER_HAL_setupPeriodicTimer(uint32_t count);

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
void TIMER_HAL_registerPeriodicTimerCallback(TimerCallback callback);

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
void TIMER_HAL_acquireSharedTimer(uint32_t count, TimerCallback callback);

/*F************************************************************************************************
 * NAME: void TIMER_HAL_releaseSharedTimer()
 *
 * DESCRIPTION:
 *      Stops the shared timer and unregisters the callback.
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
void TIMER_HAL_releaseSharedTimer();

#endif // TIMER_HAL_H