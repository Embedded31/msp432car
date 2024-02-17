/*H************************************************************************************************
 * FILENAME:        sensing_module.c
 *
 * DESCRIPTION:
 *      This source file contains high level function to control the Ultrasonic Sensor SR-HC04
 *      mounted on the Servo motor SG90. These sensors are used to detect objects in front, on the
 *      left, or on the right of the msp432car.
 *
 * PUBLIC FUNCTIONS:
 *      void        Sensing_Module_init()
 *      void        Sensing_Module_ultrasonicMeasurementReady(uint16_t distance)
 *      void        Sensing_Module_checkClearance(uint8_t deg)
 *      void        Sensing_Module_checkLeftClearance()
 *      void        Sensing_Module_checkRightClearance()
 *      void        Sensing_Module_checkFrontClearance()
 *
 * NOTES:
 *
 * AUTHOR: Matteo Frizzera    <matteo.frizzera@studenti.unitn.it>
 *
 * START DATE: 12 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR              DETAIL
 * 15 Feb 2024  Matteo Frizzera     Added polling and global variable to wait for servo to reach target direction
 * 16 Feb 2024  Andrea Piccin       Refactoring, removed busy waiting mechanism
 */
#include "../../inc/sensing_module.h"
#include "../../inc/ultrasonic_hal.h"
#include "../../inc/servo_hal.h"

#define SERVO_POS_LEFT SERVO_MAX_POSITION   /* Position left                                     */
#define SERVO_POS_FRONT 0                   /* Position front, meaning in between right and left */
#define SERVO_POS_RIGHT SERVO_MIN_POSITION  /* Position right                                    */
#define SENSING_MIN_DISTANCE_THRESHOLD 15   /* Minimum allowed distance before stopping the car  */

Servo servo; /* servo motor on which the ultrasonic sensor is mounted */

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
void Sensing_Module_init(){
    US_HAL_init();
    SERVO_HAL_init(&servo);
    US_HAL_registerMeasurementCallback(Sensing_Module_ultrasonicMeasurementReady);
    SERVO_HAL_registerPositionReachedCallback(US_HAL_triggerMeasurement);
}

/*F************************************************************************************************
 * NAME: void Sensing_Module_ultrasonicMeasurementReady(uint16_t distance)
 *
 * DESCRIPTION:
 *      Callback function to execute when the ultrasonic sensor measurement is ready
 *
 * INPUTS:
 *      PARAMETERS:
 *          uint16_t distance       distance measured
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
void Sensing_Module_ultrasonicMeasurementReady(uint16_t distance){
    if(distance < SENSING_MIN_DISTANCE_THRESHOLD){
        //TODO: stop motors and other actions
    }
}

/*F************************************************************************************************
 * NAME: void Sensing_Module_checkClearance(uint8_t deg)
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
void Sensing_Module_checkClearance(uint8_t deg){
    SERVO_HAL_setPosition(&servo, deg);
}


/*F************************************************************************************************
 * NAME: void Sensing_Module_checkLeftClearance
 *
 * DESCRIPTION:
 *      Calls general function Sensing_Module_checkClearance to check wether there is an object on
 *      the left.
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
void Sensing_Module_checkLeftClearance(){
    Sensing_Module_checkClearance(SERVO_POS_LEFT);
}

/*F************************************************************************************************
 * NAME: void Sensing_Module_checkRightClearance
 *
 * DESCRIPTION:
 *      Calls general function Sensing_Module_checkClearance to check wether there is an object on
 *      the right.
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
void Sensing_Module_checkRightClearance(){
    Sensing_Module_checkClearance(SERVO_POS_RIGHT);
}


/*F************************************************************************************************
 * NAME: void Sensing_Module_checkClearance_FRONT
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
void Sensing_Module_checkFrontClearance(){
    Sensing_Module_checkClearance(SERVO_POS_FRONT);
}