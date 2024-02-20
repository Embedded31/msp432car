/*H************************************************************************************************
 * FILENAME:        powertrain.c
 *
 * DESCRIPTION:
 *      This source file provides an abstraction over the management of the powertrain system
 *      providing basics functionality for managing robot movements.
 *
 * PUBLIC FUNCTIONS:
 *      void    Powertrain_Module_init()
 *      void    Powertrain_Module_stop()
 *      void    Powertrain_Module_moveForward()
 *      void    Powertrain_Module_moveBackward()
 *      void    Powertrain_Module_increaseSpeed()
 *      void    Powertrain_Module_decreaseSpeed()
 *      void    Powertrain_Module_turnLeft(uint8_t angle)
 *      void    Powertrain_Module_turnRight(uint8_t angle)
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
 * 19 Feb 2024  Andrea Piccin   Refactoring, removed busy waiting mechanisms, add speed management
 */
#include <stddef.h>

#include "../../inc/driverlib/driverlib.h"
#include "../../inc/powertrain_module.h"
#include "../../inc/timer_hal.h"
#include "../../inc/bluetooth_hal.h"

#ifdef TEST
#include "../../tests/motor_hal.h"
#else
#include "../../inc/motor_hal.h"
#endif

#define PI 3.14159265358979323846   /* PI value                                             */
#define POWERTRAIN_FWD_SPEED 40     /* Default speed for forward movements                  */
#define POWERTRAIN_REV_SPEED 20     /* Default speed for backward movements                 */
#define POWERTRAIN_TURN_SPEED 50    /* Default speed for turns                              */
#define WHEEL_DIAMETER 6.5          /* Wheel diameter in centimeters                        */
#define WHEEL_MAX_ANGULAR_SPEED 36  /* Wheel maximum angular speed in degrees per second    */

/*T************************************************************************************************
 * NAME: Powertrain
 *
 * DESCRIPTION:
 *      Represent the powertrain system
 *
 * SPECIFICATIONS:
 *      Type:   struct
 *      Vars:   Motor         left_motor        Represent the left pair of motors
 *              Motor         right_motor       Represent the right pair of motors
 */
typedef struct {
    Motor left_motor;
    Motor right_motor;
} Powertrain;

// Functions delcaration
void wait_milliseconds(uint32_t time);
uint32_t calculate_time_from_angle(uint8_t speedPercentage, uint8_t angle);
uint32_t calculate_time_from_distance(uint8_t speedPercentage, uint8_t distance);

// Static and global variables
volatile static Powertrain powertrain;        /* Store the powertrain struct            */
PowertrainCallback powertrainCallback = NULL; /* function to invoke on position reached */

/*F************************************************************************************************
 * NAME: void Powertrain_Module_init()
 *
 * DESCRIPTION:
 *      Initialises the motors.
 *      [1] Initialise motor hal system
 *      [2] Initialise the powertrain and its components
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
 *          Powertrain      powertrain      Initialised
 *
 *  NOTE:
 */
void Powertrain_Module_init() {
    Interrupt_disableMaster();

    // [1] Initialize motor hal system
    MOTOR_HAL_init();

    // [2] Initialize the powertrain and its components
    MOTOR_HAL_motorInit(&powertrain.left_motor, MOTOR_INIT_LEFT);
    MOTOR_HAL_motorInit(&powertrain.right_motor, MOTOR_INIT_RIGHT);

    Interrupt_enableMaster();
}

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
void Powertrain_Module_stop() {
    if (powertrain.left_motor.state.speed > 0)
        MOTOR_HAL_stop(&powertrain.left_motor);
    if (powertrain.right_motor.state.speed > 0)
        MOTOR_HAL_stop(&powertrain.right_motor);
}

/*F************************************************************************************************
 * NAME: void Powertrain_Module_moveForward()
 *
 * DESCRIPTION:
 *      Move the robot forward infinitely
 *      [1] Set motors direction to forward
 *      [2] Set motors speed to default speed
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
 *          Motor   powertrain.left_motor   Speed set to POWERTRAIN_FWD_SPEED
 *                                          Direction set to MOTOR_DIR_FORWARD
 *          Motor   powertrain.right_motor  Speed set to POWERTRAIN_FWD_SPEED
 *                                          Direction set to MOTOR_DIR_FORWARD
 *
 *  NOTE:
 */
void Powertrain_Module_moveForward() {
    // [1] Set motors direction to forward
    MOTOR_HAL_setDirection(&powertrain.left_motor, MOTOR_DIR_FORWARD);
    MOTOR_HAL_setDirection(&powertrain.right_motor, MOTOR_DIR_FORWARD);
    // [2] Set speed
    MOTOR_HAL_setSpeed(&powertrain.left_motor, POWERTRAIN_FWD_SPEED);
    MOTOR_HAL_setSpeed(&powertrain.right_motor, POWERTRAIN_FWD_SPEED);
}

/*F************************************************************************************************
 * NAME: void Powertrain_Module_moveBackward()
 *
 * DESCRIPTION:
 *      Move the robot backward infinitely
 *      [1] Set motors direction to reverse
 *      [2] Set motors speed to default speed
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
 *          Motor   powertrain.left_motor   Speed set to POWERTRAIN_REV_SPEED
 *                                          Direction set to MOTOR_DIR_REVERSE
 *          Motor   powertrain.right_motor  Speed set to POWERTRAIN_REV_SPEED
 *                                          Direction set to MOTOR_DIR_REVERSE
 *
 *  NOTE:
 */
void Powertrain_Module_moveBackward() {
    // [1] Set motors direction to reverse
    MOTOR_HAL_setDirection(&powertrain.left_motor, MOTOR_DIR_REVERSE);
    MOTOR_HAL_setDirection(&powertrain.right_motor, MOTOR_DIR_REVERSE);
    // [2] Set speed
    MOTOR_HAL_setSpeed(&powertrain.left_motor, POWERTRAIN_REV_SPEED);
    MOTOR_HAL_setSpeed(&powertrain.right_motor, POWERTRAIN_REV_SPEED);
}

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
 *          Powertrain      powertrain      Set of motors to manange
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          Powertrain      powertrain      Motor's speed increased by 10
 *
 *  NOTE:
 */
void Powertrain_Module_increaseSpeed() {
    MotorState leftState = powertrain.left_motor.state;
    MotorState rightState = powertrain.right_motor.state;

    if (leftState.direction != MOTOR_DIR_STOP && leftState.speed < 100)
        MOTOR_HAL_setSpeed(&powertrain.left_motor, leftState.speed + 10);
    if (rightState.direction != MOTOR_DIR_STOP && rightState.speed < 100)
        MOTOR_HAL_setSpeed(&powertrain.right_motor, rightState.speed + 10);
}

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
 *          Powertrain      powertrain      Set of motors to manange
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          Powertrain      powertrain      Motor's speed decreased by 10
 *
 *  NOTE:
 */
void Powertrain_Module_decreaseSpeed() {
    MotorState leftState = powertrain.left_motor.state;
    MotorState rightState = powertrain.right_motor.state;

    if (leftState.direction != MOTOR_DIR_STOP && leftState.speed > 20)
        MOTOR_HAL_setSpeed(&powertrain.left_motor, leftState.speed - 10);
    if (rightState.direction != MOTOR_DIR_STOP && rightState.speed > 20)
        MOTOR_HAL_setSpeed(&powertrain.right_motor, rightState.speed - 10);
}

/*F************************************************************************************************
 * NAME: void Powertrain_Module_turnLeft(uint8_t angle);
 *
 * DESCRIPTION:
 *      Make the robot turn left by the specified angle
 *      [1] Alternate the motors direction to turn left
 *      [2] Wait the required milliseconds to turn by the specified angle
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
 *          Motor   powertrain.left_motor   Speed set to POWERTRAIN_TURN_SPEED
 *                                          Direction set to MOTOR_DIR_REVERSE
 *          Motor   powertrain.right_motor  Speed set to POWERTRAIN_TURN_SPEED
 *                                          Direction set to MOTOR_DIR_FORWARD
 *
 *  NOTE:
 */
void Powertrain_Module_turnLeft(uint8_t angle) {
    // [1] Alternate the motors direction to turn left
    MOTOR_HAL_setDirection(&powertrain.left_motor, MOTOR_DIR_REVERSE);
    MOTOR_HAL_setDirection(&powertrain.right_motor, MOTOR_DIR_FORWARD);
    MOTOR_HAL_setSpeed(&powertrain.left_motor, POWERTRAIN_TURN_SPEED);
    MOTOR_HAL_setSpeed(&powertrain.right_motor, POWERTRAIN_TURN_SPEED);

    // [2] Wait the required milliseconds to turn by the specified angle
    uint32_t time = calculate_time_from_angle(POWERTRAIN_TURN_SPEED, angle);
    wait_milliseconds(time);
}

/*F************************************************************************************************
 * NAME: void Powertrain_Module_turnRight(uint8_t angle);
 *
 * DESCRIPTION:
 *      Make the robot turn right by the specified angle
 *      [1] Alternate the motors direction to turn right
 *      [2] Wait the required milliseconds to turn by the specified angle
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
 *          Motor   powertrain.left_motor   Speed set to POWERTRAIN_TURN_SPEED
 *                                          Direction set to MOTOR_DIR_FORWARD
 *          Motor   powertrain.right_motor  Speed set to POWERTRAIN_TURN_SPEED
 *                                          Direction set to MOTOR_DIR_REVERSE
 *
 *  NOTE:
 */
void Powertrain_Module_turnRight(uint8_t angle) {
    // [1] Alternate the motors direction to turn right
    MOTOR_HAL_setDirection(&powertrain.left_motor, MOTOR_DIR_FORWARD);
    MOTOR_HAL_setDirection(&powertrain.right_motor, MOTOR_DIR_REVERSE);
    MOTOR_HAL_setSpeed(&powertrain.left_motor, POWERTRAIN_TURN_SPEED);
    MOTOR_HAL_setSpeed(&powertrain.right_motor, POWERTRAIN_TURN_SPEED);

    // [2] Wait the required milliseconds to turn by the specified angle
    uint32_t time = calculate_time_from_angle(POWERTRAIN_TURN_SPEED, angle);
    wait_milliseconds(time);
}

/*F************************************************************************************************
 * NAME: void Powertrain_Module_onTimerEnded()
 *
 * DESCRIPTION:
 *      This function is registered as callback for the shared timer when acquired by the
 *      powertrain module, when it is called the rotation is ended and the motors have to be
 *      halted.
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
void Powertrain_Module_onTimerEnded() {
    // [1] Set motors direction to stop
    MOTOR_HAL_setDirection(&powertrain.left_motor, MOTOR_DIR_STOP);
    MOTOR_HAL_setDirection(&powertrain.right_motor, MOTOR_DIR_STOP);

    // [2] Clear interrupt flags
    Timer32_clearInterruptFlag(TIMER32_0_BASE);
    TIMER_HAL_releaseSharedTimer();
}

/*F************************************************************************************************
 * NAME: void wait_milliseconds(uint32_t time);
 *
 * DESCRIPTION:
 *      Wait the specified amount of milliseconds.
 *      [1] acquire the shared timer
 *
 * INPUTS:
 *      PARAMETERS:
 *          uint32_t        time       Specifies the ticks (1 tick = 0.01ms) to wait
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
void wait_milliseconds(uint32_t time) {
    // [1] Acquire the timer
    TIMER_HAL_acquireSharedTimer(time, Powertrain_Module_onTimerEnded);
}

/*F************************************************************************************************
 * NAME: void calculate_time_from_angle(uint8_t speedPercentage, uint8_t angle);
 *
 * DESCRIPTION:
 *      Calculate the time required to turn by the specified angle.
 *
 * INPUTS:
 *      PARAMETERS:
 *          uint8_t      speedPercentage    Speed of the motor in percentage
 *          uint8_t      angle              Specifies the relative angle to reach in degrees
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
uint32_t calculate_time_from_angle(uint8_t speedPercentage, uint8_t angle) {
    float speed = WHEEL_MAX_ANGULAR_SPEED * (speedPercentage * 1.0 / 100);
    uint32_t time = angle * 10000 / speed;
    return time;
}

/*F************************************************************************************************
 * NAME: void Powertrain_Module_registerTurnCompletedCallback(PowertrainCallback callback);
 *
 * DESCRIPTION:
 *      Registers the PowertrainCallback as the function to call when the robot reaches the
 *      desired position
 *
 * INPUTS:
 *      PARAMETERS:
 *          PowertrainCallback callback                 The function to register as callback
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          PowertrainCallback  powertrainCallback      Set to the given callback
 *
 *  NOTE:
 */
void Powertrain_Module_registerTurnCompletedCallback(PowertrainCallback callback) {
    powertrainCallback = callback;
}
