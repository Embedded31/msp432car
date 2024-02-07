/*H************************************************************************************************
 * FILENAME:        ultrasonic_hal.h
 *
 * DESCRIPTION:
 *      This header provides an abstraction over the measurement of the distance via an ultrasonic
 *      sensor, the SR-HC04.
 *
 * PUBLIC FUNCTIONS:
 *      void        US_HAL_init()
 *      void        US_HAL_triggerMeasurement()
 *      bool        US_HAL_isMeasurementReady()
 *      uint16_t    US_HAL_getMeasurement()
 *
 * NOTES:
 *
 * AUTHOR: Andrea Piccin    <andrea.piccin@studenti.unitn.it>
 *
 * START DATE: 06 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 */
#include <stdint.h>

#ifndef ULTRASONIC_HAL_H
#define ULTRASONIC_HAL_H

#define US_RESULT_INVALID 0            /* Value to be returned if the result isn't ready    */
#define US_RESULT_NO_OBJECT UINT16_MAX /* Value to be returned if no object is detected     */

/*F************************************************************************************************
 * NAME: void US_HAL_init()
 *
 * DESCRIPTION:
 *      Initialises the hardware required for the distance sensing.
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
void US_HAL_init();

/*F************************************************************************************************
 * NAME: void US_HAL_triggerMeasurement()
 *
 * DESCRIPTION:
 *      Sends a signal to the sensor in order to start a new measurement.
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
void US_HAL_triggerMeasurement();

/*F************************************************************************************************
 * NAME: bool US_HAL_isMeasurementReady()
 *
 * DESCRIPTION:
 *      Check if there is a terminated measurement.
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
 *          Type:   bool
 *          Value:  true if there is a valid measurement, false otherwise
 *
 *  NOTE:
 */
bool US_HAL_isMeasurementReady();

/*F************************************************************************************************
 * NAME: uint16_t US_HAL_getMeasurement()
 *
 * DESCRIPTION:
 *      Retrieves the last measurement.
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
 *          Type:   uint16_t        unsigned 16-bit integer
 *          Value:  the distance from the ahead object in cm, US_RESULT_INVALID if the
 *                  measurement wasn't ready and US_RESULT_NO_OBJ if nothing is detected.
 *
 *  NOTE:
 *      The suggested range is between 4cm and 200cm.
 */
uint16_t US_HAL_getMeasurement();

#endif // ULTRASONIC_HAL_H