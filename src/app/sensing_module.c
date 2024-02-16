/*H************************************************************************************************
 * FILENAME:        sensing_module.c
 *
 * DESCRIPTION:
 *      This files contains high level function to control the Ultrasonic Sensor SR-HC04 mounted on the
 *      Servo motor SG90. These sensors are used to detect objects in front, on the left, or on the right 
 *      of the msp432car
 *
 * PUBLIC FUNCTIONS:
 *      void        Sensing_Module_init()
 *      void        Sensing_Module_US_callback()
 *      bool        Sensing_Module_checkClearance(uint8_t deg)
 *      bool        Sensing_Module_checkClearance_LEFT()
 *      bool        Sensing_Module_checkClearance_RIGHT()
 *      bool        Sensing_Module_checkClearance_FRONT()
 *
 * NOTES:
 *      !!! TODO the measurement does not work as intended: the program does not wait for the servo motor
 *      to finish rotating before making a US measurement.
 *      We must add a delay between the instruction to rotate the Servo and the measurement of distance with
 *      ultrasonic sensor. As this is hardware related i suggest to implement this method in the HAL.
 *      The servo rotation speed is 60 deg / 0.1 second, but we will mount a sensor on it so our rotation speed
 *      will be slower.
 *
 * AUTHOR: Matteo Frizzera    <matteo.frizzera@studenti.unitn.it>
 *
 * START DATE: 12 Feb 2024
 *
 * CHANGES:
 *      15 Feb 2024  Matteo Frizzera     Added polling and global variable to wait for servo to reach target direction
 * 
 */

#include "../../inc/sensing_module.h"
#include "../hal/ultrasonic_hal.c"
#include "../hal/servo_hal.c"

#define SERVO_POS_LEFT SERVO_MIN_POSITION   /* position left */
#define SERVO_POS_FRONT 0                   /* position front, meaning in between right and left */
#define SERVO_POS_RIGHT SERVO_MAX_POSITION  /* position right */

Servo servo; /* servo motor on wich the ultrasonic sensor is mounted */

volatile bool Sensing_Module_US_measurementFinished; /* flag is true if US sensor has finished measuring distance                       */
volatile bool Sensing_Module_SERVO_finishedMoving;   /* flag to keep track of when servo motor has reached position set with SERVO_HAL_setPosition(servo, pos)*/
volatile uint16_t Sensing_Module_US_distance;        /* distance of object detected, US_RESULT_NO_OBJECT if no object was detected */


/*F************************************************************************************************
 * NAME: void Sensing_Module_US_callback(uint16_t distance)()
 *
 * DESCRIPTION:
 *      Callback function to execute when US sensor has finished measurement
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
 *          bool Sensing_Module_US_measurementFinished      set to true
 *          uint16_t Sensing_Module_US_distance             set to distance measured 
 * 
 *
 *  NOTE:
 */
void Sensing_Module_US_callback(uint16_t distance){
    Sensing_Module_US_measurementFinished = true;
    Sensing_Module_US_distance = distance;
}


/*F************************************************************************************************
 * NAME: void Sensing_Module_SERVO_callback()
 *
 * DESCRIPTION:
 *      Callback function to execute when servo has reached target direction (after a direction change)
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
 *          bool Sensing_Module_US_measurementFinished      set to true
 *          uint16_t Sensing_Module_US_distance             set to distance measured 
 * 
 *
 *  NOTE:
 */
void Sensing_Module_SERVO_callback(){
    Sensing_Module_SERVO_finishedMoving = true;
}


/*F************************************************************************************************
 * NAME: void Sensing_Module_init()
 *
 * DESCRIPTION:
 *      Initializes Servo motor and US sensor
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
 *          bool Sensing_Module_US_measurementFinished      set to false
 *          bool Sensing_Module_SERVO_finishedMoving        set to false
 *          USCallback usCallback                           set to callback function
 *          ServoCallback servoCallback                     set to callback function
 *
 *  NOTE:
 */
void Sensing_Module_init(){
    US_HAL_init();
    usCallback = &Sensing_Module_US_callback;

    SERVO_HAL_init(&servo);
    servoCallback = &Sensing_Module_SERVO_callback;

    Sensing_Module_US_measurementFinished = false;
    Sensing_Module_SERVO_finishedMoving = false;
}


/*F************************************************************************************************
 * NAME: Sensing_Module_checkClearance(uint8_t deg)
 *
 * DESCRIPTION:
 *      Moves motor to specified direction (from left -90 deg to right 90 deg) then uses ultrasonic
 *      sensor to check wether there is an object in that direction.
 *      The callback function sets to true the flag Sensing_Module_US_measurementFinished to notify it has finished,
 *      then this flag is again set to false.
 *      [1] makes SERVO motor rotate to desired direction
 *      [2] starts US distance measurement
 *      [3] Polling to wait for measurement result
 *      [4] Returns true if path is clear, false if it detected an object
 *
 * INPUTS:
 *      PARAMETERS:
 *          uint8_t deg     checks wether there is an object in this direction
 *      GLOBALS:
 *          Servo servo                                      servo motor to rotate
 *          bool Sensing_Module_US_measurementFinished       US measurement is finished flag
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          bool      Sensing_Module_US_measurementFinished      set to false
 *          uint16_t  Sensing_Module_US_distance                 set to measured distance
 *
 *  NOTE:
 *      TODO this dosen't work right now, it needs to wait for servo motor to finish rotating
 *      before triggering US measurement 
 */
bool Sensing_Module_checkClearance(uint8_t deg){
    // [1] makes SERVO motor rotate to desired direction
    SERVO_HAL_setPosition(&servo, deg);

    // waits for SERVO to reach target direction
    while(!Sensing_Module_SERVO_finishedMoving);

    Sensing_Module_SERVO_finishedMoving = false;

    /* TODO here it should wait for the servo motor to finish moving to the desired direction
        We could implement a callback in the Servo HAL to notify when moving is done or just wait 
        a fixed amount of time
    */

    // [2] starts US distance measurement
    US_HAL_triggerMeasurement();


    // [3] polling to wait for measurement results
    while(!Sensing_Module_US_measurementFinished);

    Sensing_Module_US_measurementFinished = false;

    // [4] Returns true if path is clear, false if it detected an object
    if(Sensing_Module_US_distance == US_RESULT_NO_OBJECT){
        return true; 
    }
    else{
        return false;
    }
}


/*F************************************************************************************************
 * NAME: Sensing_Module_checkClearance_LEFT
 *
 * DESCRIPTION:
 *      Calls general function Sensing_Module_checkClearance to check wether there is an object on the left
 *      Returns true if path is clear, false if it detected an object
 * 
 * INPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          Servo servo                                      servo motor to rotate
 *          bool Sensing_Module_US_measurementFinished       US measurement is finished flag
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          bool      Sensing_Module_US_measurementFinished      set to false
 *          uint16_t  Sensing_Module_US_distance                 set to measured distance
 *
 *  NOTE:
 *  
 */
bool Sensing_Module_checkClearance_LEFT(){
    return Sensing_Module_checkClearance(SERVO_POS_LEFT);
}


/*F************************************************************************************************
 * NAME: Sensing_Module_checkClearance_RIGHT
 *
 * DESCRIPTION:
 *      Calls general function Sensing_Module_checkClearance to check wether there is an object on the right
 *      Returns true if path is clear, false if it detected an object
 * 
 * INPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          Servo servo                                      servo motor to rotate
 *          bool Sensing_Module_US_measurementFinished       US measurement is finished flag
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          bool      Sensing_Module_US_measurementFinished      set to false
 *          uint16_t  Sensing_Module_US_distance                 set to measured distance
 *
 *  NOTE:
 *  
 */
bool Sensing_Module_checkClearance_RIGHT(){
    return Sensing_Module_checkClearance(SERVO_POS_RIGHT);
}


/*F************************************************************************************************
 * NAME: Sensing_Module_checkClearance_FRONT
 *
 * DESCRIPTION:
 *      Calls general function Sensing_Module_checkClearance to check wether there is an object in front
 *      Returns true if path is clear, false if it detected an object
 * 
 * INPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          Servo servo                                      servo motor to rotate
 *          bool Sensing_Module_US_measurementFinished       US measurement is finished flag
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          bool      Sensing_Module_US_measurementFinished      set to false
 *          uint16_t  Sensing_Module_US_distance                 set to measured distance
 *
 *  NOTE:
 *  
 */
bool Sensing_Module_checkClearance_FRONT(){
    return Sensing_Module_checkClearance(SERVO_POS_FRONT);
}