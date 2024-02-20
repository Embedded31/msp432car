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

#include "../../inc/state_machine.h"
#include "../../inc/driverlib/driverlib.h"

#include "../../inc/powertrain_module.h"
#include "../../inc/sensing_module.h"
#include "../../inc/remote_module.h"

#define SENSING_TIMER_COUNT 46875       // 24MHz / 256 / 46875 = 2Hz = 0.5s

void obstacleCallback();
void turnedCallback();
void sensingCallback(bool free_left, bool free_right);
void switchModeCallback();

volatile bool controlled = true;    // Indicate whether the robot is being remotely controlled
volatile bool obstructed = false;   // Indicate whether the path is obstructed by an obstacle
volatile bool sensing = false;      // Indicate whether the sensing procedure has been completed
volatile bool turned = false;       // Indicate whether a turn has been completed

/*F************************************************************************************************
 * NAME: void FSM_init()
 *
 * DESCRIPTION:
 *      Initializes the FSM:
 *      [1] Register the required callbacks
 *      [2] Disable sleep on interrupt service routine exit
 *      [3] Initialize timer32 module used for periodically probing for obstacles
 *      [4] Enable timer interrupts
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
    Povertrain_Module_registerTurnCompletedCallback(turnedCallback);

    // [2] Disable sleep on interrupt service routine exit
    Interrupt_disableSleepOnIsrExit();

    // [3] Initialize timer32 module used for periodically probing for obstacles
    Timer32_initModule(TIMER32_1_BASE, TIMER32_PRESCALER_256, TIMER32_32BIT, TIMER32_PERIODIC_MODE);
    Timer32_setCount(TIMER32_1_BASE, SENSING_TIMER_COUNT);
    Timer32_startTimer(TIMER32_1_BASE, true);

    // [4] Enable timer interrupts
    Timer32_clearInterruptFlag(TIMER32_1_BASE);
    Timer32_enableInterrupt(TIMER32_1_INTERRUPT);
    Interrupt_enableInterrupt(INT_T32_INT2);

    // [5] Update current state
    FSM_currentState = STATE_REMOTE;

    Interrupt_enableMaster();
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
 *      [2] Sleep until the sensing procedure has been completed or remotely controlled
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
    Sensing_Module_checkLateralClearance();
    sensing = true;

    // [2] Sleep until the sensing procedure has been completed or remotely controlled
    while (sensing && !controlled) {
        PCM_gotoLPM0InterruptSafe();
    }

    // [3] Update current state
    FSM_currentState = controlled ? STATE_REMOTE : STATE_TURNING;
}

/*F************************************************************************************************
 * NAME: void FSM_turning()
 *
 * DESCRIPTION:
 *      Handle the STATE_TURNING state:
 *      [1] Sleep until the robot has turned or remotely controlled
 *      [2] Update current state
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
void FSM_turning() {
    turned = false;

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
 * NAME: void turnedCallback()
 *
 * DESCRIPTION:
 *      Callback to call when the robot has turned:
 *      [1] If the state is STATE_TURNING set turned variable
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
void turnedCallback() {
    // [1] If the state is STATE_TURNING set turned variable
    if (FSM_currentState == STATE_TURNING) {
        turned = true;
    }
}

/*F************************************************************************************************
 * NAME: void sensingCallback()
 *
 * DESCRIPTION:
 *      Callback to call when the sensing procedure has finished:
 *      [1] If the state is STATE_SENSING set sensing variable
 *      [2] Turn around based on the directions not obstructed
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
 *          bool    sensing     Indicate whether the sensing procedure has been completed.
 *
 *  NOTE:
 */
void sensingCallback(bool free_left, bool free_right) {
    // [1] If the state is STATE_SENSING set sensing variable
    if (FSM_currentState == STATE_SENSING) {
        sensing = false;
    }

    // [2] Turn around based on the directions not obstructed
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

/*ISR**********************************************************************************************
 * NAME: void T32_INT2_IRQHandler()
 *
 * DESCRIPTION:
 *      This function is called every time that the TIMER32_1 counter reaches zero, this means
 *      that a new sensing procedure for detecting frontal obstacles has to be started.
 *
 * INPUTS:
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
// cppcheck-suppress unusedFunction
void T32_INT2_IRQHandler() {
    Timer32_clearInterruptFlag(TIMER32_1_BASE);
    Sensing_Module_checkFrontClearance();
}
