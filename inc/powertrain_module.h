/*H************************************************************************************************
 * FILENAME:        powertrain.h
 *
 * DESCRIPTION:
 *      This header provide an abstraction over the management of the powertrain system providing
 *      basics functionality for managing robot movements.
 *
 * PUBLIC FUNCTIONS:
 *      void    Powertrain_Module_init()
 *      void    Powertrain_Module_stop()
 *      void    Powertrain_Module_moveForward()
 *      void    Powertrain_Module_moveBackward()
 *      void    Powertrain_Module_increaseSpeed()
 *      void    Powertrain_Module_decreaseSpeed()
 *      void    Powertrain_Module_turnLeft(int8_t angle)
 *      void    Powertrain_Module_turnRight(int8_t angle)
 *      void    Powertrain_Module_registerTurnCompletedCallback(PowertrainCallback callback)
 *
 * NOTES:
 *
 * AUTHOR: Simone Rossi    <simone.rossi-2@studenti.unitn.it>
 *
 * START DATE: 9 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 * 19 Feb 2024  Andrea Piccin   Refactoring, removed move by distance, add speed management
 */
#include <stdint.h>

#include "motor_hal.h"

#ifndef POWERTRAIN_MODULE_H_
#define POWERTRAIN_MODULE_H_

/*T************************************************************************************************
 * NAME: PowertrainCallback
 *
 * DESCRIPTION:
 *      It's a pointer to a function that executes when the robot reaches the desired position
 *
 * SPECIFICATIONS:
 *      Type:   void*
 *      Args:   None
 */
typedef void (*PowertrainCallback)();

/*F************************************************************************************************
 * NAME: void Powertrain_Module_init()
 *
 * DESCRIPTION:
 *      Initialises the motors.
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
void Powertrain_Module_init();

/*F************************************************************************************************
 * NAME: void Powertrain_Module_stop()
 *
 * DESCRIPTION:
 *      Stop the robot if currently in motion.
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
void Powertrain_Module_stop();

/*F************************************************************************************************
 * NAME: void Powertrain_Module_moveForward()
 *
 * DESCRIPTION:
 *      Move the robot forward infinitely
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
 *      The execution will be halted until the stop function is called.
 */
void Powertrain_Module_moveForward();

/*F************************************************************************************************
 * NAME: void Powertrain_Module_moveBackward()
 *
 * DESCRIPTION:
 *      Move the robot backward infinitely
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
 *      The execution will be halted until the stop function is called.
 */
void Powertrain_Module_moveBackward();

/*F************************************************************************************************
 * NAME: void Powertrain_Module_increaseSpeed();
 *
 * DESCRIPTION:
 *      Increases the speed of the motors by 10% (max 100%);
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
void Powertrain_Module_increaseSpeed();

/*F************************************************************************************************
 * NAME: void Powertrain_Module_decreaseSpeed();
 *
 * DESCRIPTION:
 *      Decreases the speed of the motors by 10% (min 20%);
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
void Powertrain_Module_decreaseSpeed();

/*F************************************************************************************************
 * NAME: void Powertrain_Module_turnLeft(uint8_t angle);
 *
 * DESCRIPTION:
 *      Make the robot turn left by the specified angle
 *
 * INPUTS:
 *      PARAMETERS:
 *          uint8_t     angle       Specifies the angle to turn.
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
void Powertrain_Module_turnLeft(uint8_t angle);

/*F************************************************************************************************
 * NAME: void Powertrain_Module_turnRight(uint8_t angle);
 *
 * DESCRIPTION:
 *      Make the robot turn right by the specified angle
 *
 * INPUTS:
 *      PARAMETERS:
 *          uint8_t     angle       Specifies the angle to turn.
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
void Powertrain_Module_turnRight(uint8_t angle);

/*F************************************************************************************************
 * NAME: void Powertrain_Module_registerTurnCompletedCallback(PowertrainCallback callback);
 *
 * DESCRIPTION:
 *      Registers the PowertrainCallback as the function to call when the robot reaches the
 *      desired position
 *
 * INPUTS:
 *      PARAMETERS:
 *          PowertrainCallback callback         The function to register as callback
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
void Powertrain_Module_registerTurnCompletedCallback(PowertrainCallback callback);

#endif /* POWERTRAIN_MODULE_H_ */
