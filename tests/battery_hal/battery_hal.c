/*H************************************************************************************************
 * FILENAME:        battery_hal.c
 *
 * DESCRIPTION:
 *      Battery Hardware Abstraction Layer (HAL), this source file provides an abstraction over the
 *      battery status exposing hardware-independent functions.
 *
 * PUBLIC FUNCTIONS:
 *      void        BATTERY_HAL_init()
 *      uint16_t    BATTERY_HAL_getVoltage()
 *      uint8_t     BATTERY_HAL_getPercentage()
 *
 * NOTES:
 *      The battery pack outputs 8.4V at peak that cannot be handled by the MSP432P401R so a
 *      voltage divider (16kΩ,10kΩ) is introduced for rescaling the 8.4V to 3.23V;
 *      the BATTERY_DIVIDER value comes from the scaling ratio 1 + (16kΩ / 10kΩ) = 2.6.
 *
 * AUTHOR: Simone Rossi    <simone.rossi-2@studenti.unitn.it>
 *
 * START DATE: 19 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 */

#include <stdlib.h>

#include "battery_hal.h"

#define BATTERY_MAX_VOLTAGE 8400        /* Fully charged battery voltage (mV)                */
#define BATTERY_MIN_VOLTAGE 6000        /* Discharged battery voltage (mV)                   */

void BATTERY_HAL_init() {
}

uint16_t BATTERY_HAL_getVoltage() {
    return BATTERY_MIN_VOLTAGE + rand() % (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE);
}

uint8_t BATTERY_HAL_getPercentage() {
    float voltage = BATTERY_HAL_getVoltage();
    uint8_t percentage =
        ((voltage - BATTERY_MIN_VOLTAGE) / (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE)) * 100;
    return percentage;
}