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
 *      void        FSM_turning()
 *      void        FSM_remote()
 *      void        obstacleCallback()
 *      void        rotateCallback()
 *      void        sensingCallback()
 *      void        switchModeCallback()
 *      void        timerCallback()
 *
 * NOTES:
 *
 * AUTHOR: Simone Rossi    <simone.rossi-2@studenti.unitn.it>
 *
 * START DATE: 19 Feb 2024
 *
 * CHANGES:
 * 19 Feb 2024  Simone Rossi    Updating and refactoring
 * 20 Feb 2024  Simone Rossi    Added periodic sensing of frontal obstacles
 */
#include <stdbool.h>

#include "../../inc/driverlib/driverlib.h"
#include "../../inc/powertrain_module.h"
#include "../../inc/remote_module.h"
#include "../../inc/sensing_module.h"
#include "../../inc/state_machine.h"

#ifndef TEST
    #include "../../inc/timer_hal.h"
#endif

#define SENSING_TIMER_COUNT 46875 // 24MHz / 256 / 46875 = 2Hz = 0.5s

void obstacleCallback(bool free);
void turnedCallback();
void sensingCallback(bool free_left, bool free_right);
void switchModeCallback();
void timerCallback();

FSM_State FSM_currentState = STATE_INIT; // Current FSM state
FSM_StateMachine FSM_stateMachine[] = {
    // FSM initialization
    {STATE_INIT, FSM_init},       {STATE_RUNNING, FSM_running}, {STATE_SENSING, FSM_sensing},
    {STATE_TURNING, FSM_turning}, {STATE_REMOTE, FSM_remote},
};

/*F************************************************************************************************
 * NAME: void FSM_init()
 *
 * DESCRIPTION:
 *      Initializes the FSM:
 *      [1] Register the required callbacks
 *      [2] Disable sleep on interrupt service routine exit
 *      [3] Initialize timer32 module used for periodically probing for obstacles
 *      [4] Register timer callback
 *      [5] Update current state
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
    Interrupt_disableMaster();

    // [1] Register the required callbacks
    Remote_Module_registerModeChangeRequestCallback(switchModeCallback);
    Sensing_Module_registerSingleMeasurementReadyCallback(obstacleCallback);
    Sensing_Module_registerDoubleMeasurementReadyCallback(sensingCallback);
    Powertrain_Module_registerTurnCompletedCallback(turnedCallback);

    // [2] Disable sleep on interrupt service routine exit
    Interrupt_disableSleepOnIsrExit();

    // [3] Initialize timer32 module used for periodically probing for obstacles
    TIMER_HAL_setupPeriodicTimer(SENSING_TIMER_COUNT);

    // [4] Register timer callback
    TIMER_HAL_registerPeriodicTimerCallback(timerCallback);

    // [5] Update current state
    FSM_currentState = STATE_REMOTE;

    Interrupt_enableMaster();
}

/*F************************************************************************************************
 * NAME: void FSM_running()
 *
 * DESCRIPTION:
 *      Handle the STATE_RUNNING state:
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
void FSM_running() {}

/*F************************************************************************************************
 * NAME: void FSM_sensing()
 *
 * DESCRIPTION:
 *      Handle the STATE_SENSING state:
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
void FSM_sensing() {}

/*F************************************************************************************************
 * NAME: void FSM_turning()
 *
 * DESCRIPTION:
 *      Handle the STATE_TURNING state:
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
void FSM_turning() {}

/*F************************************************************************************************
 * NAME: void FSM_remote()
 *
 * DESCRIPTION:
 *      Handle the STATE_REMOTE state:
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
void FSM_remote() {}

/*F************************************************************************************************
 * NAME: void obstacleCallback(bool free)
 *
 * DESCRIPTION:
 *      Callback to call when an obstacle is met:
 *      [1] If the path is not clear stop
 *      [2] Start sensing the surroundings
 *      [3] Update current state
 *
 * INPUTS:
 *      PARAMETERS:
 *          bool        free                Indicate whether the path is clear
 *          FSM_State   FSM_currentState    Current state of the FSM
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          FSM_State   FSM_currentState    Current state of the FSM
 *
 *  NOTE:
 */
void obstacleCallback(bool free) {
    // [1] If the path is not clear stop
    if (FSM_currentState == STATE_RUNNING && !free) {
        // [2] Start sensing the surroundings
        Powertrain_Module_stop();
        Sensing_Module_checkLateralClearance();

        // [3] Update current state
        FSM_currentState == STATE_SENSING;
    }
}

/*F************************************************************************************************
 * NAME: void turnedCallback()
 *
 * DESCRIPTION:
 *      Callback to call when the robot has turned:
 *      [2] Update current state
 *
 * INPUTS:
 *      PARAMETERS:
 *          FSM_State   FSM_currentState    Current state of the FSM
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          FSM_State   FSM_currentState    Current state of the FSM
 *
 *  NOTE:
 */
void turnedCallback() {
    // [2] Update current state
    if (FSM_currentState == STATE_TURNING) {
        Powertrain_Module_moveForward();
        FSM_currentState = STATE_RUNNING;
    }
}

/*F************************************************************************************************
 * NAME: void sensingCallback()
 *
 * DESCRIPTION:
 *      Callback to call when the sensing procedure has finished:
 *      [1] Turn around based on the directions not obstructed
 *      [2] Update current state
 *
 * INPUTS:
 *      PARAMETERS:
 *          bool    free_left       Indicate whethere the left side is not obstructed
 *          bool    free_right      Indicate whethere the right side is not obstructed
 *          FSM_State   FSM_currentState    Current state of the FSM
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          FSM_State   FSM_currentState    Current state of the FSM
 *
 *  NOTE:
 */
void sensingCallback(bool free_left, bool free_right) {
    // [1] Turn around based on the directions not obstructed
    if (FSM_currentState == STATE_SENSING) {
        if (free_left) {
            Powertrain_Module_turnLeft(90);
        } else if (free_right) {
            Powertrain_Module_turnRight(90);
        } else {
            Powertrain_Module_turnRight(180);
        }

        // [2] Update current state
        FSM_current_State = STATE_TURNING;
    }
}

/*F************************************************************************************************
 * NAME: void switchModeCallback()
 *
 * DESCRIPTION:
 *      Callback to call when the drive mode changes (remotely controlled or autonomous drive):
 *      [1] Switch to the correct state based on the current state
 *      [2] Update current state
 *
 * INPUTS:
 *      PARAMETERS:
 *          FSM_State   FSM_currentState    Current state of the FSM
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          FSM_State   FSM_currentState    Current state of the FSM
 *
 *  NOTE:
 */
void switchModeCallback() {
    // [1] Switch to the correct state based on the current state
    switch (FSM_currentState) {
    // [2] Update current state
    case STATE_REMOTE:
        Powertrain_Module_moveForward();
        FSM_currentState = STATE_RUNNING;
        break;
    case STATE_RUNNING:
        FSM_currentState = STATE_REMOTE;
        break;
    case STATE_SENSING:
        FSM_currentState = STATE_REMOTE;
        break;
    case STATE_TURNING:
        FSM_currentState = STATE_REMOTE;
        break;
    default:
        FSM_currentState = STATE_INIT;
    }
}

/*F************************************************************************************************
 * NAME: void timerCallback()
 *
 * DESCRIPTION:
 *      Callback called periodically by the Timer32 to check for obstacles
 *      [1] Check for frontal obstacles
 *
 * INPUTS:
 *      PARAMETERS:
 *          FSM_State   FSM_currentState    Current state of the FSM
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
void timerCallback() {
    // [1] Check for frontal obstacles
    if (FSM_currentState == STATE_RUNNING) {
        Sensing_Module_checkFrontClearance();
    }
}
