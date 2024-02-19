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

#define DCO_FREQUENCY CS_DCO_FREQUENCY_24 // 24MHz

/*F************************************************************************************************
 * NAME: void system_init()
 *
 * DESCRIPTION:
 *      Initializes the system:
 *      [1] Stop the watchdog timer
 *      [2] Configure wait states and voltage level
 *      [3] Set the centered frequency of the Digitally Controlled Oscillator (DCO)
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
void system_init() {
    // [1] Stop the watchdog timer
    WDT_A_holdTimer();

    // [2] Configure wait states and voltage level
    FlashCtl_setWaitState(FLASH_BANK0, 1);
    FlashCtl_setWaitState(FLASH_BANK1, 1);
    // Sets the core voltage level for higher processing capabilities
    PCM_setCoreVoltageLevel(PCM_VCORE1);

    // [3] Set the centered frequency of the Digitally Controlled Oscillator (DCO)
    CS_setDCOCenteredFrequency(DCO_FREQUENCY);
}
