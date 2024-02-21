/*H************************************************************************************************
 * FILENAME:        remote_module.c
 *
 * DESCRIPTION:
 *      This source file contains the implementations of high level functions to manually control
 *      the motors using IR commands.
 *
 * PUBLIC FUNCTIONS:
 *      void        Remote_Module_init()
 *      void        Remote_Module_registerModeChangeRequestCallback(RemoteCallback callback);
 *
 * NOTES:
 *
 * AUTHOR: Andrea Piccin    <andrea.piccin@studenti.unitn.it>
 *
 * START DATE: 19 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 * 21 Feb 2024  Andrea Piccin   Refactoring, added test support
 */
#include <stdbool.h>
#include <string.h>

#include "../../inc/remote_module.h"
#include "../../inc/powertrain_module.h"
#include "../../inc/state_machine.h"

#ifdef TEST
#include "../../tests/bluetooth_hal.h"
#include "../../tests/infrared_hal.h"
#else
#include "../../inc/bluetooth_hal.h"
#include "../../inc/infrared_hal.h"
#endif

RemoteCallback remoteCallback;

void Remote_Module_onIRMessageReceived(IRCommand command, bool isValid) {
    if (FSM_currentState != STATE_REMOTE && command != IR_COMMAND_ASTERISK)
        return;

    if (isValid) {
        switch (command) {
        case IR_COMMAND_UP: /* Start motors forward at default speed  */
            Powertrain_Module_moveForward();
            break;
        case IR_COMMAND_DOWN: /* Start motors backward ad default speed */
            Powertrain_Module_moveBackward();
            break;
        case IR_COMMAND_LEFT: /* Rotate 45 deg CCW                      */
            Powertrain_Module_turnLeft(45);
            break;
        case IR_COMMAND_RIGHT: /* Rotate 45 deg CW                       */
            Powertrain_Module_turnRight(45);
            break;
        case IR_COMMAND_OK: /* Stop the motors                        */
            Powertrain_Module_stop();
            break;
        case IR_COMMAND_2: /* Increase speed                         */
            Powertrain_Module_increaseSpeed();
            break;
        case IR_COMMAND_8: /* Decrease speed                         */
            Powertrain_Module_decreaseSpeed();
            break;
        case IR_COMMAND_ASTERISK: /* Switch to autonomous mode              */
            if (remoteCallback != NULL)
                remoteCallback();
            break;
        default: /* Nothing to do                          */
            break;
        }
    }
}

void Remote_Module_onBTMessageReceived(const char *message) {
    char command[4];
    command[3] = '\0';
    strncpy(command, message, 3);

    if (FSM_currentState != STATE_REMOTE && strcmp(command, "MAN") != 0)
        return;

    if (strcmp(command, "FWD") == 0) { /* Start motors forward at default speed  */
        Powertrain_Module_moveForward();
    } else if (strcmp(command, "REV") == 0) { /* Start motors backward at default speed */
        Powertrain_Module_moveBackward();
    } else if (strcmp(command, "LFT") == 0) { /* Rotate 45 deg CCW                      */
        Powertrain_Module_turnLeft(45);
    } else if (strcmp(command, "RGT") == 0) { /* Rotate 45 deg CW                       */
        Powertrain_Module_turnRight(45);
    } else if (strcmp(command, "STP") == 0) { /* Stop the motors                        */
        Powertrain_Module_stop();
    } else if (strcmp(command, "AUT") == 0) { /* Switch to autonomous mode              */
        if (remoteCallback != NULL)
            remoteCallback();
    } else if (strcmp(command, "MAN") == 0) { /* Switch to manual mode                  */
        if (remoteCallback != NULL)
            remoteCallback();
    }
}

void Remote_Module_init() {
    BT_HAL_init();
    IR_HAL_init();
    IR_HAL_registerMessageCallback(Remote_Module_onIRMessageReceived);
    BT_HAL_registerMessageCallback(Remote_Module_onBTMessageReceived);
    remoteCallback = NULL;
}

void Remote_Module_registerModeChangeRequestCallback(RemoteCallback callback) {
    remoteCallback = callback;
}
