/*H************************************************************************************************
 * FILENAME:        system.c
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
 * 20 Feb 2024  Andrea Piccin   Introduced shared 32-bit timer
 */
#include <stddef.h>

#include "../../inc/system.h"
#include "../../inc/driverlib/driverlib.h"
#include "../../inc/powertrain_module.h"
#include "../../inc/remote_module.h"
#include "../../inc/sensing_module.h"
//#include "../../docs/telemetry_module.h"

#define DCO_FREQUENCY CS_DCO_FREQUENCY_24 // 24MHz

SharedTimerCallback currentCallback;    /* function to call on shared timer expiration */

/*F************************************************************************************************
 * NAME: void system_init()
 *
 * DESCRIPTION:
 *      Initializes the system:
 *      [1] Stop the watchdog timer
 *      [2] Configure wait states and voltage level
 *      [3] Set the centered frequency of the Digitally Controlled Oscillator (DCO)
 *      [4] Init shared 32-bit timer
 *      [5] Init all modules
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
void System_init() {
    // [1] Stop the watchdog timer
    WDT_A_holdTimer();

    // [2] Configure wait states and voltage level
    FlashCtl_setWaitState(FLASH_BANK0, 1);
    FlashCtl_setWaitState(FLASH_BANK1, 1);
    // Sets the core voltage level for higher processing capabilities
    PCM_setCoreVoltageLevel(PCM_VCORE1);

    // [3] Set the centered frequency of the Digitally Controlled Oscillator (DCO)
    CS_setDCOCenteredFrequency(DCO_FREQUENCY);

    // [4] Init a shared 32-bit timer
    Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_256, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    Timer32_enableInterrupt(TIMER32_0_BASE);
    Timer32_clearInterruptFlag(TIMER32_0_BASE);
    Interrupt_enableInterrupt(INT_T32_INT1);
    currentCallback = NULL;

    // [4] Init all modules
    Powertrain_Module_init();
    Remote_Module_init();
    Sensing_Module_init();
    //Telemetry_Module_init();
}

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
void System_acquireSharedTimer(uint32_t count, SharedTimerCallback callback){
    Timer32_setCount(TIMER32_0_BASE, count);
    Interrupt_registerInterrupt(INT_T32_INT1, callback);
    Timer32_startTimer(TIMER32_0_BASE, true);
}

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
 *          SharedTimerCallback     currentCallback     Set to NULL
 *
 *  NOTE:
 */
void System_releaseSharedTimer(){
    Timer32_haltTimer(TIMER32_0_BASE);
    currentCallback = NULL;
}
