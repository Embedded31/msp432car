/*H************************************************************************************************
 * FILENAME:        servo_hal.h
 *
 * DESCRIPTION:
 *      This header provides an abstraction over the management of a SG-90 servo motor
 *
 * PUBLIC FUNCTIONS:
 *      void    SERVO_HAL_init(Servo* servo);
 *      void    SERVO_HAL_setPosition(Servo* servo, int8_t position);
 *      void    SERVO_HAL_registerPositionReachedCallback(ServoCallback callback);
 *
 * NOTES:
 *
 * AUTHOR: Simone Rossi    <simone.rossi-2@studenti.unitn.it>
 *
 * START DATE: 08 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR              DETAIL
 * 15 Feb 2024  Matteo Frizzera     Added functions to wait for servo to finish moving
 * 16 Feb 2024  Andrea Piccin       Refactoring
 */
#include <stdint.h>

#ifndef SERVO_HAL_H
#define SERVO_HAL_H

#define SERVO_MIN_POSITION -90   /* Minimum position in deg */
#define SERVO_MAX_POSITION 90    /* Maximum position in deg */

/*T************************************************************************************************
 * NAME: ServoCallback
 *
 * DESCRIPTION:
 *      It's a pointer to a function that executes when servo has finished rotating to new position
 *
 * SPECIFICATIONS:
 *      Type:   void*
 *      Args:   None
 */
typedef void (*ServoCallback)();

/*T************************************************************************************************
 * NAME: ServoState
 *
 * DESCRIPTION:
 *      Represent current state of the servo
 *
 * SPECIFICATIONS:
 *      Type:   struct
 *      Vars:   uint8_t         position    Current position of the servo
 */
typedef struct {
    int8_t position;
} ServoState;

/*T************************************************************************************************
 * NAME: Servo
 *
 * DESCRIPTION:
 *      Represent a SG90 servo motor
 *
 * SPECIFICATIONS:
 *      Type:   struct
 *      Vars:   uint8_t         ccr         Capture Compare Register with the target value for PWM
 *              ServoState      state       Current state of the servo
 */
typedef struct {
    uint8_t ccr;
    ServoState state;
} Servo;

/*F************************************************************************************************
 * NAME: void SERVO_HAL_init(Servo* servo);
 *
 * DESCRIPTION:
 *      Initialises a Servo motor instance setting the initial position to the value in between
 *      the min and max position values.
 *
 * INPUTS:
 *      PARAMETERS:
 *          Servo*              servo           Motor that has to be initialised
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          Servo*              servo           All the fields of the struct are set
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void SERVO_HAL_init(Servo *servo);

/*F************************************************************************************************
 * NAME: void SERVO_HAL_setPosition(Servo* servo, int8_t position);
 *
 * DESCRIPTION:
 *      Set the position of a servo
 *
 * INPUTS:
 *      PARAMETERS:
 *          Servo*      servo                   Target servo
 *          int8_t      position                Specifies the position to which rotate.
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          int8_t      servo->state.position     Set on the current position
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void SERVO_HAL_setPosition(Servo *servo, int8_t position);

/*F************************************************************************************************
 * NAME: void SERVO_HAL_registerPositionReachedCallback(ServoCallback callback)
 *
 * DESCRIPTION:
 *      Registers the ServoCallback as the function to call when servo motor reaches destination
 *
 * INPUTS:
 *      PARAMETERS:
 *          ServoCallback callback                  The function to register as callback         
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          ServoCallback servoCallback             Set to the given callback
 *
 *  NOTE:
 */
void SERVO_HAL_registerPositionReachedCallback(ServoCallback callback);

#endif // SERVO_HAL_H
