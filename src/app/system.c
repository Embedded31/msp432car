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
 */
#include "../../inc/system.h"
#include "../../inc/driverlib/driverlib.h"
#include "../../inc/powertrain_module.h"
#include "../../inc/remote_module.h"
#include "../../inc/sensing_module.h"
#include "../../inc/telemetry_module.h"
#include "../../inc/timer_hal.h"

#define DCO_FREQUENCY CS_DCO_FREQUENCY_24 // 24MHz

/*F************************************************************************************************
 * NAME: void system_init()
 *
 * DESCRIPTION:
 *      Initializes the system:
 *      [1] Stop the watchdog timer
 *      [2] Configure wait states and voltage level
 *      [3] Set the centered frequency of the Digitally Controlled Oscillator (DCO)
 *      [4] Init all modules
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

    // [4] Init all modules
    Powertrain_Module_init();
    Remote_Module_init();
    Sensing_Module_init();
    Telemetry_Module_init();
    TIMER_HAL_init();
}