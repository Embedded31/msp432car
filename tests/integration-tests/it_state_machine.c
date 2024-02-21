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

#include "../../inc/state_machine.h"
#include "../../inc/sensing_module.h"
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
    Sensing_Module_checkFrontClearance();
    US_HAL_triggerNextAction(50);
    assert(FSM_currentState == STATE_RUNNING && "Unexpected state");
    Sensing_Module_checkFrontClearance();
    US_HAL_triggerNextAction(10);
    assert(FSM_currentState == STATE_SENSING && "Unexpected state");

    // Turn and restart after sensing the side's clearance
    US_HAL_triggerNextAction(10);
    US_HAL_triggerNextAction(30);
    assert(FSM_currentState == STATE_RUNNING && "Unexpected state");

    // Enter the remote state from a random state
    Sensing_Module_checkFrontClearance();
    US_HAL_triggerNextAction(10);
    assert(FSM_currentState == STATE_SENSING && "Unexpected state");
    IR_HAL_triggerCommandReceived(IR_COMMAND_ASTERISK);
    assert(FSM_currentState == STATE_REMOTE && "Unexpected state");
}