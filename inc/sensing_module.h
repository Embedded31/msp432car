/*H************************************************************************************************
 * FILENAME:        sensing_module.h
 *
 * DESCRIPTION:
 *      This header files contains the definitions of high level functions to control the Ultrasonic Sensor SR-HC04
 *      mounted on the Servo motor SG90. These sensors are used to detect objects in front, on the left, or on the right 
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
 *      None
 *
 * AUTHOR: Matteo Frizzera    <matteo.frizzera@studenti.unitn.it>
 *
 * START DATE: 12 Feb 2024
 *
 * CHANGES:
 *      none
 * 
 */

#include <stdint.h>
#include <stdbool.h>

#ifndef SENSING_MODULE_H
#define SENSING_MODULE_H

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
void Sensing_Module_US_callback(uint16_t distance);


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
 *          USCallback usCallback                           set to callback function
 *
 *  NOTE:
 */
void Sensing_Module_init();


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
 *      None
 */
bool Sensing_Module_checkClearance(uint8_t deg);


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
bool Sensing_Module_checkClearance_LEFT();


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
bool Sensing_Module_checkClearance_RIGHT();


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
bool Sensing_Module_checkClearance_FRONT();

#endif // SENSING_MODULE_H