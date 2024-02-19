/*H************************************************************************************************
 * FILENAME:        sensing_module.h
 *
 * DESCRIPTION:
 *      This header files contains the definitions of high level functions to control the
 *      ultrasonic sensor SR-HC04 mounted on the servo motor SG90. These sensors are used to detect
 *      objects in front, on the left, or on the right of the msp432car.
 *
 * PUBLIC FUNCTIONS:
 *      void        Sensing_Module_init()
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
 * 16 Feb 2024  Andrea Piccin       Refactoring
 * 19 Feb 2024  Andrea Piccin       Ultrasonic measurement callback moved to state_machine.c
 */
#include <stdint.h>

#ifndef SENSING_MODULE_H
#define SENSING_MODULE_H

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
 * NAME: void Sensing_Module_checkClearance(uint8_t deg)
 *
 * DESCRIPTION:
 *      Moves motor to specified direction (from left -90 deg to right 90 deg) then uses ultrasonic
 *      sensor to check wether there is an object in that direction.
 *
 * INPUTS:
 *      PARAMETERS:
 *          uint8_t deg     checks wether there is an object in this direction
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
void Sensing_Module_checkClearance(uint8_t deg);

/*F************************************************************************************************
 * NAME: void Sensing_Module_checkLeftClearance()
 *
 * DESCRIPTION:
 *      Calls general function Sensing_Module_checkClearance to check wether there is an object on
 *      the left.
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
void Sensing_Module_checkLeftClearance();

/*F************************************************************************************************
 * NAME: void Sensing_Module_checkRightClearance();
 *
 * DESCRIPTION:
 *      Calls general function Sensing_Module_checkClearance to check wether there is an object on
 *      the right.
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
void Sensing_Module_checkRightClearance();

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

#endif // SENSING_MODULE_H