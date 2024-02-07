/*H************************************************************************************************
 * FILENAME:        motor_hal.h
 *
 * DESCRIPTION:
 *      This header provides an abstraction over the management of two separate direct current
 *      motors via the L298N motor driver.
 *
 * PUBLIC FUNCTIONS:
 *      void    MOTOR_HAL_init()
 *      void    MOTOR_HAL_motorInit(Motor *motor, MotorInitTemplate initTemplate)
 *      void    MOTOR_HAL_setSpeed(Motor *motor, uint8_t speed)
 *      void    MOTOR_HAL_setDirection(Motor *motor, MotorDirection direction)
 *      void    MOTOR_HAL_stop(Motor *motor)
 *
 * NOTES:
 *      In our implementation there are two motors attached to each of the L298N channels, so we
 *      have a total of four motors controlled as left and right pairs.
 *
 * AUTHOR: Simone Rossi    <simone.rossi-2@studenti.unitn.it>
 *
 * START DATE: 31 Jan 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 * 04 Feb 2024  Andrea Piccin   Refactoring
 * 04 Feb 2024  Andrea Piccin   Definitions moved to the source file, hidden to the user
 */
#include <stdint.h>

#include "driverlib/driverlib.h"

#ifndef MOTOR_HAL_H
#define MOTOR_HAL_H

/*T************************************************************************************************
 * NAME: MotorDirection
 *
 * DESCRIPTION:
 *      Represent the direction of a motor.
 *
 * SPECIFICATIONS:
 *      Type:   enum
 *      Values: MOTOR_DIR_FORWARD   Clockwise (CW) rotation, propels the car forward
 *              MOTOR_DIR_REVERSE   Counterclockwise (CCW) rotation, propels the car backward
 *              MOTOR_DIR_STOP      No rotation, the car stops
 */
typedef enum {
    MOTOR_DIR_FORWARD,
    MOTOR_DIR_REVERSE,
    MOTOR_DIR_STOP
} MotorDirection;

/*T************************************************************************************************
 * NAME: MotorInitTemplate
 *
 * DESCRIPTION:
 *      Represent the way the motor has to be initialised, the L298N is capable of managing two
 *      separate channels that are used to manage left and right motors independently.
 *
 * SPECIFICATIONS:
 *      Type:   enum
 *      Values: MOTOR_INIT_LEFT    Indicates the configuration for the left motor
 *              MOTOR_INIT_RIGHT   Indicates the configuration for the right motor
 */
typedef enum { MOTOR_INIT_LEFT, MOTOR_INIT_RIGHT } MotorInitTemplate;

/*T************************************************************************************************
 * NAME: MotorState
 *
 * DESCRIPTION:
 *      Represent current state of the motor
 *
 * SPECIFICATIONS:
 *      Type:   struct
 *      Vars:   uint8_t         speed       Speed of the motor in percentage
 *              MotorDirection  direction   Direction of the motor
 */
typedef struct {
    uint8_t speed;
    MotorDirection direction;
} MotorState;

/*T************************************************************************************************
 * NAME: Motor
 *
 * DESCRIPTION:
 *      Represent a channel of the L298N motor driver
 *
 * SPECIFICATIONS:
 *      Type:   struct
 *      Vars:   uint8_t         in1_pin     Pin for clockwise (CW) rotation
 *              uint8_t         in2_pin     Pin for counterclockwise (CCW) rotation
 *              uint8_t         ccr         Capture Compare Register with the target value for PWM
 *              MotorState      state       Current state of the motor
 */
typedef struct {
    uint8_t in1_pin;
    uint8_t in2_pin;
    uint8_t ccr;
    MotorState state;
} Motor;

/*F************************************************************************************************
 * NAME: void MOTOR_HAL_init()
 *
 * DESCRIPTION:
 *      Initialises the hardware required for the motors
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
void MOTOR_HAL_init();

/*F************************************************************************************************
 * NAME: void MOTOR_HAL_motorInit(Motor *motor, MotorInitTemplate initTemplate);
 *
 * DESCRIPTION:
 *      Initialises a Motor instance following a template that specifies if it has to be configured
 *      as right or left motor.
 *
 * INPUTS:
 *      PARAMETERS:
 *          Motor*              motor           Motor that has to be initialised
 *          MotorInitTemplate   initTemplate    Specifies if the motor configuration
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          Motor*              motor           All the fields of the struct are set
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void MOTOR_HAL_motorInit(Motor *motor, MotorInitTemplate initTemplate);

/*F************************************************************************************************
 * NAME: void MOTOR_HAL_setSpeed(Motor *motor, uint8_t speed);
 *
 * DESCRIPTION:
 *      Set the speed of a motor
 *
 * INPUTS:
 *      PARAMETERS:
 *          Motor*      motor       Target motor
 *          uint8_t     speed       Specifies the wanted speed in percentage 0 to 100.
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          uint8_t*    motor->state.speed     Set on the current speed
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void MOTOR_HAL_setSpeed(Motor *motor, uint8_t speed);

/*F************************************************************************************************
 * NAME: void MOTOR_HAL_setDirection(Motor *motor, MotorDirection direction);
 *
 * DESCRIPTION:
 *      Set the direction of a motor that can be forward, reverse or stop.
 *
 * INPUTS:
 *      PARAMETERS:
 *          Motor*              motor           Target motor
 *          MotorDirection      direction       Specifies the wanted directions
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          MotorDirection*     motor->state.direction     Set on the current direction
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void MOTOR_HAL_setDirection(Motor *motor, MotorDirection direction);

/*F************************************************************************************************
 * NAME: void MOTOR_HAL_stop(Motor *motor);
 *
 * DESCRIPTION:
 *      Stops the motor
 *
 * INPUTS:
 *      PARAMETERS:
 *          Motor*              motor           Target motor
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          uint8_t*            motor->state.speed         Set to zero
 *          MotorDirection*     motor->state.direction     Set to MOTOR_DIR_STOP
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void MOTOR_HAL_stop(Motor *motor);

#endif // MOTOR_HAL_H
