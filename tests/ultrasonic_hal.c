/*H************************************************************************************************
 * FILENAME:        ultrasonic_hal.h
 *
 * DESCRIPTION:
 *      This source file provides an abstraction over the measurement of the distance via an
 *      ultrasonic sensor, the SR-HC04.
 *
 * PUBLIC FUNCTIONS:
 *      void        US_HAL_init()
 *      void        US_HAL_triggerMeasurement()
 *      void        US_HAL_registerMeasurementCallback(USCallback callback);
 *      void        US_HAL_setDefaultDistance(uint16_t distance);
 *
 * NOTES:
 *
 * AUTHOR: Simone Rossi    <simone.rossi-2@studenti.unitn.it>
 *
 * START DATE: 06 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 */
#include <stdio.h>
#include "ultrasonic_hal.h"

USCallback usCallback;       /* Function to call when a new measurement is ready      */

void US_HAL_init() {
    usCallback = NULL;
}

void US_HAL_triggerMeasurement() {
}

void US_HAL_registerMeasurementCallback(USCallback callback) { usCallback = callback; }

void US_HAL_triggerNextAction(uint16_t distance){
    if(usCallback!=NULL)
        usCallback(distance);
}