/*C************************************************************************************************
 * FILENAME:        state_machine.c
 *
 * DESCRIPTION:
 *      This source file contains the functions associated to every state of the FSM.
 *
 * PUBLIC FUNCTIONS:
 *      void        FSM_init()
 *      void        FSM_running()
 *      void        FSM_sensing()
 *      void        FSM_remote()
 *      void        obstacleCallback()
 *      void        rotateCallback()
 *      void        sensingCallback()
 *      void        switchModeCallback()
 *
 * NOTES:
 *
 * AUTHOR: Simone Rossi    <simone.rossi-2@studenti.unitn.it>
 *
 * START DATE: 19 Feb 2024
 *
 * CHANGES:
 * 19 Feb 2024  Simone Rossi    Updating and refactoring
 */
#include <stdbool.h>

#include "../../inc/state_machine.h"
#include "../../inc/driverlib/driverlib.h"

#include "../../inc/powertrain_module.h"
#include "../../inc/sensing_module.h"
#include "../../inc/remote_module.h"
#include "../../inc/servo_hal.h"

void obstacleCallback();
void rotateCallback();
void sensingCallback(bool free_left, bool free_right);
void switchModeCallback();

volatile bool controlled = true;    // Indicate whether the robot is being remotely controlled
volatile bool obstructed = false;   // Indicate whether the path is obstructed by an obstacle
volatile bool turned = false;       // Indicate whether a turn has been completed

/*F************************************************************************************************
 * NAME: void FSM_init()
 *
 * DESCRIPTION:
 *      Initializes the FSM:
 *      [1] Register the required callbacks
 *      [2] Disable sleep on interrupt service routine exit
 *      [3] Update current state
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
 *          FSM_State       FSM_currentState    Update current state
 *
 *  NOTE:
 */
void FSM_init() {
    // TODO: Move to system init
    Powertrain_Module_init();
    Sensing_Module_init();
    // TODO: Init remote module

    // [1] Register the required callbacks
    Remote_Module_registerModeChangeRequestCallback(switchModeCallback);
    SERVO_HAL_registerPositionReachedCallback(rotateCallback);
    // TODO: Register sensing callbacks (obstacle and sensing)

    // [2] Disable sleep on interrupt service routine exit
    Interrupt_disableSleepOnIsrExit();

    // [3] Update current state
    FSM_currentState = controlled ? STATE_REMOTE : STATE_RUNNING;
}

/*F************************************************************************************************
 * NAME: void FSM_running()
 *
 * DESCRIPTION:
 *      Handle the STATE_RUNNING state:
 *      [1] Start moving forward
 *      [2] Sleep until an obstacle is met or remotely controlled
 *      [3] Update current state
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
 *          FSM_State       FSM_currentState    Update current state
 *
 *  NOTE:
 */
void FSM_running() {
    // [1] Start moving forward
    obstructed = false;
    POWERTRAIN_move_forward();

    // [2] Sleep until an obstacle is met or remotely controlled
    while (!obstructed && !controlled) {
        PCM_gotoLPM0InterruptSafe();
    }

    // [3] Update current state
    FSM_currentState = controlled ? STATE_REMOTE : STATE_SENSING;
}

/*F************************************************************************************************
 * NAME: void FSM_sensing()
 *
 * DESCRIPTION:
 *      Handle the STATE_SENSING state:
 *      [1] Start sensing the surroundings
 *      [2] Sleep until the robot has turned or remotely controlled
 *      [3] Update current state
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
 *          FSM_State       FSM_currentState    Update current state
 *
 *  NOTE:
 */
void FSM_sensing() {
    // [1] Start sensing the surroundings
    turned = false;
    // TODO: Sensing_Module_checkSideClearance();

    // [2] Sleep until the robot has turned or remotely controlled
    while (!turned && !controlled) {
        PCM_gotoLPM0InterruptSafe();
    }

    // [3] Update current state
    FSM_currentState = controlled ? STATE_REMOTE : STATE_RUNNING;
}

/*F************************************************************************************************
 * NAME: void FSM_remote()
 *
 * DESCRIPTION:
 *      Handle the STATE_REMOTE state:
 *      [1] Enable sleep on interrupt service routine exit
 *      [2] Sleep until the robot mode is toggled to autonomous drive
 *      [3] Update current state
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
 *          FSM_State       FSM_currentState    Update current state
 *
 *  NOTE:
 */
void FSM_remote() {
    // [1] Enable sleep on interrupt service routine exit
    Interrupt_enableSleepOnIsrExit();

    // [2] Sleep until the robot mode is toggled to autonomous drive
    PCM_gotoLPM0InterruptSafe();

    // [3] Update current state
    FSM_currentState = controlled ? STATE_REMOTE : STATE_RUNNING;
}

/*F************************************************************************************************
 * NAME: void obstacleCallback()
 *
 * DESCRIPTION:
 *      Callback to call when an obstacle is met:
 *      [1] If the state is STATE_RUNNING set obstacle variable
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
 *          bool      obstructed    Indicate whether the path is obstructed
 *
 *  NOTE:
 */
void obstacleCallback() {
    // [1] If the state is STATE_RUNNING set obstacle variable
    if (FSM_currentState == STATE_RUNNING) {
        obstructed = true;
    }
}

/*F************************************************************************************************
 * NAME: void rotateCallback()
 *
 * DESCRIPTION:
 *      Callback to call when the robot has turned:
 *      [1] If the state is STATE_SENSING set turned variable
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
 *          bool      turned    Indicate whether a turn has been completed
 *
 *  NOTE:
 */
void rotateCallback() {
    // [1] If the state is STATE_SENSING set turned variable
    if (FSM_currentState == STATE_SENSING) {
        turned = true;
    }
}

/*F************************************************************************************************
 * NAME: void sensingCallback()
 *
 * DESCRIPTION:
 *      Callback to call when the sensing procedure has finished:
 *      [1] Turn around based on the directions not obstructed
 *
 * INPUTS:
 *      PARAMETERS:
 *          bool    free_left       Indicate whethere the left side is not obstructed
 *          bool    free_right      Indicate whethere the right side is not obstructed
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
void sensingCallback(bool free_left, bool free_right) {
    // [1] Turn around based on the directions not obstructed
    if (free_left) {
        Powertrain_Module_turnLeft(90);
    } else if (free_right) {
        Powertrain_Module_turnRight(90);
    } else {
        Powertrain_Module_turnRight(180);
    }
}

/*F************************************************************************************************
 * NAME: void switchModeCallback()
 *
 * DESCRIPTION:
 *      Callback to call when the drive mode changes (remotely controlled or autonomous drive):
 *      [1] Update the controlled variable
 *      [2] Disable sleep on interrupt service routine exit
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
 *          bool      controlled    Indicate whether the robot is being remotely controlled
 *
 *  NOTE:
 */
void switchModeCallback() {
    // [1] Update the controlled variable
    controlled != controlled;

    // [2] Disable sleep on interrupt service routine exit
    Interrupt_disableSleepOnIsrExit();
}
