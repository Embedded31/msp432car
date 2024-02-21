/*H************************************************************************************************
 * FILENAME:        sensing_module.h
 *
 * DESCRIPTION:
 *      This header files contains the definitions of high level functions to control the
 *      ultrasonic sensor SR-HC04 mounted on the servo motor SG90. These sensors are used to detect
 *      objects in front, on the left, or on the right of the msp432car.
 *
 * PUBLIC FUNCTIONS:
 *      void    Sensing_Module_init()
 *      void    Sensing_Module_checkSingleClearance(int8_t deg)
 *      void    Sensing_Module_checkDoubleClearance(int8_t deg1, int8t deg2)
 *      void    Sensing_Module_checkFrontClearance()
 *      void    Sensing_Module_checkLateralClearance()
 *      void    Sensing_Module_registerSingleMeasurementReadyCallback(SensingSingleCallback call)
 *      void    Sensing_Module_registerDoubleMeasurementReadyCallback(SensingDoubleCallback call)
 *
 * NOTES:
 *
 * AUTHOR: Matteo Frizzera    <matteo.frizzera@studenti.unitn.it>
 *
 * START DATE: 12 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR              DETAIL
 * 16 Feb 2024  Andrea Piccin       Refactoring
 * 19 Feb 2024  Andrea Piccin       Single (front) and Double (lateral) measurements callbacks
 */
#include <stdbool.h>
#include <stdint.h>

#ifndef SENSING_MODULE_H
#define SENSING_MODULE_H

/*T************************************************************************************************
 * NAME: SensingSingleCallback
 *
 * DESCRIPTION:
 *      It's a pointer to a function that executes when a single ultrasonic measurement is ready
 *
 * SPECIFICATIONS:
 *      Type:   void*
 *      Args:   bool    isFree      True if the path is clear, false otherwise
 */
typedef void (*SensingSingleCallback)(bool isFree);

/*T************************************************************************************************
 * NAME: SensingDoubleCallback
 *
 * DESCRIPTION:
 *      It's a pointer to a function that executes when a double ultrasonic measurement is ready
 *
 * SPECIFICATIONS:
 *      Type:   void*
 *      Args:   bool    isDir1Free      True if the path is clear, false otherwise
 *              bool    isDir2Free      True if the path is clear, false otherwise
 */
typedef void (*SensingDoubleCallback)(bool isDir1Free, bool isDir2Free);

/*F************************************************************************************************
 * NAME: void Sensing_Module_init()
 *
 * DESCRIPTION:
 *      Initializes the hardware needed to carry out distance measurements in the surroundings of
 *      the car.
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
 *          Nones
 *
 *  NOTE:
 */
void Sensing_Module_init();

/*F************************************************************************************************
 * NAME: void Sensing_Module_checkSingleClearance(int8_t deg)
 *
 * DESCRIPTION:
 *      Moves motor to specified direction (from left -90 deg to right 90 deg) then uses ultrasonic
 *      sensor to check if there is an object in that direction.
 *
 * INPUTS:
 *      PARAMETERS:
 *          uint8_t     deg    Direction to check
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
void Sensing_Module_checkSingleClearance(int8_t deg);

/*F************************************************************************************************
 * NAME: void Sensing_Module_checkDoubleClearance(int8_t deg1, int8 deg2)
 *
 * DESCRIPTION:
 *      Moves motor to specified direction (from left -90 deg to right 90 deg) then uses ultrasonic
 *      sensor to check if there is an object in that direction, then checks in the same way the
 *      second direction.
 *
 * INPUTS:
 *      PARAMETERS:
 *          int8_t      deg1     First direction to check
 *          int8_t      deg2     Second direction to check
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
void Sensing_Module_checkDoubleClearance(int8_t deg1, int8_t deg2);

/*F************************************************************************************************
 * NAME: void Sensing_Module_checkLateralClearance()
 *
 * DESCRIPTION:
 *      Checks if there is an object on the left or on the right of the robot.
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
void Sensing_Module_checkLateralClearance();

/*F************************************************************************************************
 * NAME: void Sensing_Module_checkFrontClearance()
 *
 * DESCRIPTION:
 *      Calls general function Sensing_Module_checkClearance to check if there is an object in
 *      front of the robot.
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
void Sensing_Module_checkFrontClearance();

/*F************************************************************************************************
 * NAME: void Sensing_Module_registerSingleMeasurementReadyCallback(SensingSingleCallback callback)
 *
 * DESCRIPTION:
 *      Registers the SensingSingleCallback as the function to call when the single measurement
 *      is ready.
 *
 * INPUTS:
 *      PARAMETERS:
 *          SensingSingleCallback       callback        The function to register as callback
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
void Sensing_Module_registerSingleMeasurementReadyCallback(SensingSingleCallback callback);

/*F************************************************************************************************
 * NAME: Sensing_Module_registerDoubleMeasurementReadyCallback(SensingDoubleCallback callback)
 *
 * DESCRIPTION:
 *      Registers the SensingDoubleCallback as the function to call when the double measurement
 *      is ready.
 *
 * INPUTS:
 *      PARAMETERS:
 *          SensingDoubleCallback       callback        The function to register as callback
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
void Sensing_Module_registerDoubleMeasurementReadyCallback(SensingDoubleCallback callback);

#endif // SENSING_MODULE_H