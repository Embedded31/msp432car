/*H************************************************************************************************
 * FILENAME:        sensing_module.c
 *
 * DESCRIPTION:
 *      This source file contains high level function to control the Ultrasonic Sensor SR-HC04
 *      mounted on the Servo motor SG90. These sensors are used to detect objects in front, on the
 *      left, or on the right of the msp432car.
 *
 * PUBLIC FUNCTIONS:
 *      void    Sensing_Module_init()
 *      void    Sensing_Module_checkClearance(uint8_t deg)
 *      void    Sensing_Module_checkLateralClearance()
 *      void    Sensing_Module_checkFrontClearance()
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
 * 15 Feb 2024  Matteo Frizzera     Added polling to wait for servo to reach target direction
 * 16 Feb 2024  Andrea Piccin       Refactoring, removed busy waiting mechanism
 * 19 Feb 2024  Andrea Piccin       Single (front) and Double (lateral) measurements callbacks
 */
#include <stdbool.h>
#include <stddef.h>

#ifdef TEST
#include "../../test/servo_hal.h"
#include "../../test/ultrasonic_hal.h"
#else
#include "../../inc/servo_hal.h"
#include "../../inc/ultrasonic_hal.h"
#endif

#include "../../inc/sensing_module.h"

#define SERVO_POS_LEFT SERVO_MAX_POSITION  /* Position left                                     */
#define SERVO_POS_FRONT 0                  /* Position front, meaning in between right and left */
#define SERVO_POS_RIGHT SERVO_MIN_POSITION /* Position right                                    */
#define SENSING_FREE_THRESHOLD 20          /* Minimum allowed clearance                         */

/* Utility function declaration */
void Sensing_Module_onUSMeasurementReady(uint16_t distance);

/*T************************************************************************************************
 * NAME: SensingMode
 *
 * DESCRIPTION:
 *      Specifies if the next measurement is single (e.g. front) or double (e.g. lateral)
 *
 * SPECIFICATIONS:
 *      Type:   enum
 *      Values: SENSING_SINGLE_SAMPLE_MODE
 *              SENSING_DOUBLE_SAMPLE_MODE
 */
typedef enum {
    SENSING_SINGLE_SAMPLE_MODE,
    SENSING_DOUBLE_SAMPLE_MODE,
} SensingMode;

Servo servo;                          /* Servo motor on which the ultrasonic sensor is mounted  */
SensingMode currentSensingMode;       /* Represent the current operating mode                   */
SensingSingleCallback singleCallback; /* Callback function for single measurement               */
SensingDoubleCallback doubleCallback; /* Callback function for double measurements              */
volatile uint16_t previousSample;     /* Value of the last measurement (for double samples)     */
volatile int8_t nextDirection;        /* Direction of the next measurement (for double samples) */
volatile uint8_t sampleCount;         /* Count of the taken samples (for double samples)        */

/*F************************************************************************************************
 * NAME: void Sensing_Module_init()
 *
 * DESCRIPTION:
 *      Initializes Servo motor and ultrasonic sensor, set the servo motor callback (notification
 *      of servo in position) to trigger a new ultrasonic measurement.
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
void Sensing_Module_init() {
    US_HAL_init();
    SERVO_HAL_init(&servo);
    SERVO_HAL_registerPositionReachedCallback(US_HAL_triggerMeasurement);
    US_HAL_registerMeasurementCallback(Sensing_Module_onUSMeasurementReady);
    currentSensingMode = SENSING_SINGLE_SAMPLE_MODE;
    singleCallback = NULL;
    doubleCallback = NULL;
    sampleCount = 0;
}

/*F************************************************************************************************
 * NAME: void Sensing_Module_checkSingleClearance(int8_t deg)
 *
 * DESCRIPTION:
 *      Moves motor to specified direction (from left -90 deg to right 90 deg) then uses ultrasonic
 *      sensor to check if there is an object in that direction.
 *      When the servo reaches its final position it will automatically trigger a new ultrasonic
 *      measurement.
 *
 * INPUTS:
 *      PARAMETERS:
 *          uint8_t deg     direction to check
 *      GLOBALS:
 *          Servo   servo   servo motor to rotate
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void Sensing_Module_checkSingleClearance(int8_t deg) {
    currentSensingMode = SENSING_SINGLE_SAMPLE_MODE;
    SERVO_HAL_setPosition(&servo, deg);
}

/*F************************************************************************************************
 * NAME: void Sensing_Module_checkDoubleClearance(uint8_t deg)
 *
 * DESCRIPTION:
 *      Moves motor to the specified direction (from left -90 deg to right 90 deg) then uses
 *      ultrasonic sensor to check if there is an object in that direction, repeats the measurement
 *      on the second target direction.
 *      When the servo reaches its final position it will automatically trigger a new ultrasonic
 *      measurement.
 *
 * INPUTS:
 *      PARAMETERS:
 *          uint8_t deg     first direction to check
 *          uint8_t deg     second direction to check
 *      GLOBALS:
 *          Servo   servo   servo motor to rotate
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void Sensing_Module_checkDoubleClearance(int8_t deg1, int8_t deg2) {
    currentSensingMode = SENSING_DOUBLE_SAMPLE_MODE;
    nextDirection = deg2;
    SERVO_HAL_setPosition(&servo, deg1);
}

/*F************************************************************************************************
 * NAME: void Sensing_Module_checkLateralClearance
 *
 * DESCRIPTION:
 *      Checks if there is an object on the left or on the right of the robot.
 *
 * INPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          Servo servo                                      servo motor to rotate
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
            None
 *
 *  NOTE:
 */
void Sensing_Module_checkLateralClearance() {
    Sensing_Module_checkDoubleClearance(SERVO_POS_LEFT, SERVO_POS_RIGHT);
}

/*F************************************************************************************************
 * NAME: void Sensing_Module_checkFrontClearance()
 *
 * DESCRIPTION:
 *      Calls general function Sensing_Module_checkClearance to check wether there is an object in
 *      front.
 *
 * INPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          Servo servo                                      servo motor to rotate
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void Sensing_Module_checkFrontClearance() { Sensing_Module_checkSingleClearance(SERVO_POS_FRONT); }

/*F************************************************************************************************
 * NAME: void Sensing_Module_registerSingleMeasurementReadyCallback(SensingSingleCallback callback)
 *
 * DESCRIPTION:
 *      Registers the SensingSingleCallback as the function to call when a single measurement
 *      is ready.
 *
 * INPUTS:
 *      PARAMETERS:
 *          SensingSingleCallback    callback           The function to register as callback
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          SensingSingleCallback    singleCallback      Set to the given function
 *
 *  NOTE:
 */
void Sensing_Module_registerSingleMeasurementReadyCallback(SensingSingleCallback callback) {
    singleCallback = callback;
}

/*F************************************************************************************************
 * NAME: Sensing_Module_registerDoubleMeasurementReadyCallback(SensingDoubleCallback callback)
 *
 * DESCRIPTION:
 *      Registers the SensingDoubleCallback as the function to call when the double measurement
 *      is ready.
 *
 * INPUTS:
 *      PARAMETERS:
 *          SensingDoubleCallback  callback           The function to register as callback
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          SensingDoubleCallback  doubleCallback     Set to the given function
 *
 *  NOTE:
 */
void Sensing_Module_registerDoubleMeasurementReadyCallback(SensingDoubleCallback callback) {
    doubleCallback = callback;
}

/*F************************************************************************************************
 * NAME: void Sensing_Module_onUSMeasurementReady(uint16_t distance)
 *
 * DESCRIPTION:
 *      This function acts as callback function when a measurement of the ultrasonic sensor ends,
 *      there are two possible function modes:
 *      - SINGLE_SAMPLE_MODE: the request is for a single measurement (e.g. for front measurements)
 *                            so when the result is ready we'll forward it to the callback function
 *       - DOUBLE_SAMPLE_MODE: the request is for a double measurement (e.g. left & right) so
 *                            when the first measurement is ready we'll store it and when also
 *                            the second is ready we'll call the double callback function.
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
void Sensing_Module_onUSMeasurementReady(uint16_t distance) {
    if (currentSensingMode == SENSING_SINGLE_SAMPLE_MODE) {
        if (singleCallback != NULL)
            singleCallback(distance > SENSING_FREE_THRESHOLD);
    } else { // double sensing mode
        sampleCount++;
        if (sampleCount < 2) {
            previousSample = distance;
            SERVO_HAL_setPosition(&servo, nextDirection);
        } else {
            sampleCount = 0;
            bool isDir1Free = previousSample > SENSING_FREE_THRESHOLD;
            bool isDir2Free = distance > SENSING_FREE_THRESHOLD;
            if (doubleCallback != NULL)
                doubleCallback(isDir1Free, isDir2Free);
        }
    }
}
