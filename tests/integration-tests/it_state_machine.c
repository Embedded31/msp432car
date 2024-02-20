/*C************************************************************************************************
 * FILENAME:        it_state_machine.c
 *
 * DESCRIPTION:
 *      This source file provides the test functions to verify the correct behavior of the
 *      state machine.
 *
 * PUBLIC FUNCTIONS:
 *      void    IT_State_Machine_test()
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

#include "../../src/app/state_machine.c"
#include "../infrared_hal.h"
#include "../servo_hal.h"
#include "../ultrasonic_hal.h"

void IT_State_Machine_test() {
    // Execute the init state
    (*FSM_stateMachine[FSM_currentState].function)();
    assert(FSM_currentState == STATE_REMOTE && "Unexpected state");

    // Stay in the remote state until receiving the '*' command
    (*FSM_stateMachine[FSM_currentState].function)();
    assert(FSM_currentState == STATE_REMOTE && "Unexpected state");
    IR_HAL_triggerCommandReceived(IR_COMMAND_ASTERISK);
    assert(FSM_currentState == STATE_RUNNING && "Unexpected state");

    // Change current state on obstacle detection
    US_HAL_triggerMeasurement(50);
    assert(FSM_currentState == STATE_RUNNING && "Unexpected state");
    US_HAL_triggerMeasurement(10);
    assert(FSM_currentState == STATE_SENSING && "Unexpected state");

    // Turn after sensing the side's clearance
    US_HAL_triggerMeasurement(10);
    US_HAL_triggerMeasurement(30);
    assert(FSM_currentState == STATE_TURNING && "Unexpected state");

    // Trigger the position reached callback to return to the running state
    SERVO_HAL_triggerPositionReached();
    assert(FSM_currentState == STATE_RUNNING && "Unexpected state");

    // Enter the remote state from a random state
    US_HAL_triggerMeasurement(10);
    assert(FSM_currentState == STATE_SENSING && "Unexpected state");
    IR_HAL_triggerCommandReceived(IR_COMMAND_ASTERISK);
    assert(FSM_currentState == STATE_RUNNING && "Unexpected state");
}