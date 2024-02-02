/*H****************************************************************************
 * FILENAME:        bz_hal.h
 *
 * DESCRIPTION:
 *      Battery Hardware Abstraction Layer (HAL), this header provides an
 *      abstraction over the battery status exposing hardware-independent
 *      functions.
 *
 * PUBLIC FUNCTIONS:
 *      void        BATTERY_HAL_init()
 *      uint16_t    BATTERY_HAL_getVoltage()
 *      uint8_t     BATTERY_HAL_getPercentage()
 *
 * GLOBAL DATA:
 *      int         BATTERY_ADC_PORT    Port on which perform readings
 *      int         BATTERY_ADC_PIN     Pin on which perform readings
 *      int         BATTERY_ADC_INPUT   ADC input for (port,pin) pair
 *      int         BATTERY_ADC_MEM     ADC register for results
 *      int         BATTERY_MAX_VOLTAGE Fully charged battery voltage
 *      int         BATTERY_MIN_VOLTAGE Fully discharged battery voltage
 *      float       BATTERY_DIVIDER     Fixed hardware-dependent value
 *
 * NOTES:
 *      The BATTERY_DIVIDER value is used to compute the real voltage
 *      due to the use of a voltage divider for scaling the voltage to
 *      a MSP432P401R tolerable one.
 *
 * AUTHOR: Andrea Piccin    <andrea.piccin@studenti.unitn.it>
 *
 * START DATE: 31 Jan 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 */

#ifndef BATTERY_HAL_H
#define BATTERY_HAL_H

#include <stdint.h>

#include "driverlib/driverlib.h"

#define BATTERY_ADC_PORT 6
#define BATTERY_ADC_PIN 1
#define BATTERY_ADC_INPUT ADC_INPUT_A14
#define BATTERY_ADC_MEM ADC_MEM0
#define BATTERY_MAX_VOLTAGE 8400
#define BATTERY_MIN_VOLTAGE 6000
#define BATTERY_DIVIDER 2.6

/*F****************************************************************************
 * NAME: void BATTERY_HAL_init()
 *
 * DESCRIPTION:
 *      Initialises the hardware required for the voltage readings
 *
 * INPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          int     BATTERY_ADC_PORT
 *          int     BATTERY_ADC_PIN
 *          int     BATTERY_ADC_INPUT
 *          int     BATTERY_ADC_MEM
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

/*F****************************************************************************
 * NAME: uint16_t BATTERY_HAL_getVoltage()
 *
 * DESCRIPTION:
 *      Reads the current voltage of the battery pack, the result is
 *      expressed in mV (10^-3 V)
 *
 * INPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          float   BATTERY_DIVIDER    Fixed hardware-dependent value
 *          int     BATTERY_ADC_MEM    ADC register for results
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
 *      The voltage is expressed in mV in order to avoid unnecessary
 *      floating point operations.
 */
uint16_t BATTERY_HAL_getVoltage();

/*F****************************************************************************
 * NAME: uint8_t BATTERY_HAL_getPercentage()
 *
 * DESCRIPTION:
 *      Reads the current percentage of the battery pack, the result is
 *      expressed % so with a three-digit integer
 *
 * INPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          int         BATTERY_MAX_VOLTAGE Fully charged battery voltage
 *          int         BATTERY_MIN_VOLTAGE Fully discharged battery voltage
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