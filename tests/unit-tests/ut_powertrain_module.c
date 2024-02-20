/*C************************************************************************************************
 * FILENAME:        ut_powertrain_module.c
 *
 * DESCRIPTION:
 *      This header provide the test functions to verify the correct behavior of the
 *      powertrain module.
 *
 * PUBLIC FUNCTIONS:
 *      void    UT_Powertrain_Module_init();
 *      void    UT_Powertrain_Module_testSpeed
 *      void    UT_Powertrain_Module_testMovement()
 *
 * NOTES:
 *
 * AUTHOR: Simone Rossi    <simone.rossi-2@studenti.unitn.it>
 *
 * START DATE: 20 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 */
#include <assert.h>

#include "ut_powertrain_module.h"
#include "../../src/app/powertrain_module.c"

void UT_Powertrain_Module_init() {
    Powertrain_Module_init();
}

void UT_Powertrain_Module_testSpeed() {
    assert(powertrain.left_motor.state.speed == powertrain.right_motor.state.speed);
    uint8_t initial_speed = powertrain.left_motor.state.speed;
    Powertrain_Module_increaseSpeed();
    assert(powertrain.left_motor.state.speed == powertrain.right_motor.state.speed);
    uint8_t current_speed = powertrain.left_motor.state.speed;

    assert(current_speed == initial_speed * 1.1);

    Powertrain_Module_decreaseSpeed();
    assert(powertrain.left_motor.state.speed == powertrain.right_motor.state.speed);
    current_speed = powertrain.left_motor.state.speed;

    assert(current_speed == initial_speed);
}

void UT_Powertrain_Module_testMovement() {
    Powertrain_Module_stop();
    assert(powertrain.left_motor.state.direction == MOTOR_DIR_STOP
           && powertrain.right_motor.state.direction == MOTOR_DIR_STOP);

    Powertrain_Module_moveForward();
    assert(powertrain.left_motor.state.direction == MOTOR_DIR_FORWARD
        && powertrain.right_motor.state.direction == MOTOR_DIR_FORWARD);


    Powertrain_Module_moveBackward();
    assert(powertrain.left_motor.state.direction == MOTOR_DIR_REVERSE
           && powertrain.right_motor.state.direction == MOTOR_DIR_REVERSE);
}

