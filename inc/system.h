/*H************************************************************************************************
 * FILENAME:        system.h
 *
 * DESCRIPTION:
 *      This header provides an abstraction over the initialization of the system.
 *
 * PUBLIC FUNCTIONS:
 *      void    system_init()
 *
 * NOTES:
 *
 * AUTHOR: Simone Rossi    <simone.rossi-2@studenti.unitn.it>
 *
 * START DATE: 13 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 */
#include <stdint.h>

#ifndef SYSTEM_H_
#define SYSTEM_H_

/*T************************************************************************************************
 * NAME: SharedTimerCallback
 *
 * DESCRIPTION:
 *      It's a pointer to a function that executes when the shared timer reaches zero.
 *
 * SPECIFICATIONS:
 *      Type:   void*
 *      Args:   None
 */
typedef void (*SharedTimerCallback)(void);

/*F************************************************************************************************
 * NAME: void system_init()
 *
 * DESCRIPTION:
 *      Initializes the system.
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
void System_init();

/*F************************************************************************************************
 * NAME: void System_acquireSharedTimer(uint32_t count, SharedTimerCallback callback)
 *
 * DESCRIPTION:
 *      Set up the shared 32-bit timer in order to perform a countdown from the give count to
 *      zero and then call the given callback function.
 *
 * INPUTS:
 *      PARAMETERS:
 *          uint32_t                count           Number of ticks to wait (1 tick = 0.01ms)
 *          SharedTimerCallback     callback        Function to call on timer expiration
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
void System_acquireSharedTimer(uint32_t count, SharedTimerCallback callback);

/*F************************************************************************************************
 * NAME: void System_releaseSharedTimer()
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
void System_releaseSharedTimer();

#endif /* SYSTEM_H_ */
