/*H************************************************************************************************
 * FILENAME:        battery_hal.h
 *
 * DESCRIPTION:
 *      Battery Hardware Abstraction Layer (HAL), this header provides an abstraction over the
 *      battery status exposing hardware-independent functions.
 *
 * PUBLIC FUNCTIONS:
 *      void        BATTERY_HAL_init()
 *      uint16_t    BATTERY_HAL_getVoltage()
 *      uint8_t     BATTERY_HAL_getPercentage()
 *
 * NOTES:
 *
 * AUTHOR: Andrea Piccin    <andrea.piccin@studenti.unitn.it>
 *
 * START DATE: 31 Jan 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 * 04 Feb 2024  Andrea Piccin   Refactoring
 * 04 Feb 2024  Andrea Piccin   Definitions moved to the source file, now hided to the user
 */

#ifndef BATTERY_HAL_H
#define BATTERY_HAL_H

#include <stdint.h>

#include "driverlib/driverlib.h"

/*F************************************************************************************************
 * NAME: void BATTERY_HAL_init()
 *
 * DESCRIPTION:
 *      Initialises the hardware required for the voltage readings.
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
void BATTERY_HAL_init();

/*F************************************************************************************************
 * NAME: uint16_t BATTERY_HAL_getVoltage()
 *
 * DESCRIPTION:
 *      Reads the current voltage of the battery pack, the result is expressed in mV (10^-3 V).
 *
 * INPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
            None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *      RETURN:
 *          Type:   uint16_t    unsigned 16-bit integer
 *          Value:  current voltage in mV
 *
 *  NOTE:
 *      The voltage is expressed in mV in order to avoid unnecessary floating point operations.
 */
uint16_t BATTERY_HAL_getVoltage();

/*F************************************************************************************************
 * NAME: uint8_t BATTERY_HAL_getPercentage()
 *
 * DESCRIPTION:
 *      Reads the current percentage of the battery pack, the result is expressed % so with a
 *      three-digit integer.
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
 *      RETURN:
 *          Type:   uint8_t    unsigned 8-bit integer
 *          Value:  current battery percentage
 *
 *  NOTE:
 */
uint8_t BATTERY_HAL_getPercentage();

#endif // BATTERY_HAL_H