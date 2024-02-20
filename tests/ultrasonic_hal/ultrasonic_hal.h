/*H************************************************************************************************
 * FILENAME:        ultrasonic_hal.h
 *
 * DESCRIPTION:
 *      This header provides an abstraction over the measurement of the distance via an ultrasonic
 *      sensor, the SR-HC04.
 *
 * PUBLIC FUNCTIONS:
 *      void        US_HAL_init()
 *      void        US_HAL_triggerMeasurement(uint16_t distance)
 *      void        US_HAL_registerMeasurementCallback(USCallback callback);
 *
 * NOTES:
 *
 * AUTHOR: Andrea Piccin    <andrea.piccin@studenti.unitn.it>
 *
 * START DATE: 06 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 * 10 Feb 2024  Andrea Piccin   Introduced callback mechanism
 * 19 Feb 2024  Simone Rossi
 */
#include <stdint.h>

#ifndef ULTRASONIC_HAL_H
#define ULTRASONIC_HAL_H

#define US_RESULT_NO_OBJECT UINT16_MAX /* Value to be returned if no object is detected     */

/*T************************************************************************************************
 * NAME: USCallback
 *
 * DESCRIPTION:
 *      It's a pointer to a function that manages the ultrasonic distance measurement when ready
 *
 * SPECIFICATIONS:
 *      Type:   void*
 *      Args:   uint16_t    distance     value of the measurement
 */
typedef void (*USCallback)(uint16_t distance);

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
 * NAME: void US_HAL_triggerMeasurement(uint16_t distance)
 *
 * DESCRIPTION:
 *      Sends a signal to the sensor in order to start a new measurement.
 *
 * INPUTS:
 *      PARAMETERS:
 *          uint16_t    distance    Specifies the distance to measure
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
void US_HAL_triggerMeasurement(uint16_t distance);

/*F************************************************************************************************
 * NAME: void US_HAL_registerMeasurementCallback(USCallback callback)
 *
 * DESCRIPTION:
 *      Set the passed USCallback as the function to call when a new measurement is ready
 *
 * INPUTS:
 *      PARAMETERS:
 *          USCallback  callback     Function to register as callback
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
void US_HAL_registerMeasurementCallback(USCallback callback);

#endif // ULTRASONIC_HAL_H