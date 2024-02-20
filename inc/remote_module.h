/*H************************************************************************************************
 * FILENAME:        remote_module.h
 *
 * DESCRIPTION:
 *      This header file contains the definitions of high level functions to manually control the
 *      motors using IR commands.
 *
 * PUBLIC FUNCTIONS:
 *      void        Remote_Module_init()
 *      void        Remote_Module_registerAutoModeRequestCallback(RemoteCallback callback);
 *
 * NOTES:
 *
 * AUTHOR: Andrea Piccin    <andrea.piccin@studenti.unitn.it>
 *
 * START DATE: 19 Feb 2024
 *
 * CHANGES:
 */
#ifndef REMOTE_MODULE_H
#define REMOTE_MODULE_H

/*T************************************************************************************************
 * NAME: RemoteCallback
 *
 * DESCRIPTION:
 *      It's a pointer to a function that executes when a mode change is requested.
 *
 * SPECIFICATIONS:
 *      Type:   void*
 *      Args:   None
 */
typedef void (*RemoteCallback)();

/*F************************************************************************************************
 * NAME: void Remote_Module_init()
 *
 * DESCRIPTION:
 *      Initialises the hardware and variables needed for the IR remote control.
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
void Remote_Module_init();

/*F************************************************************************************************
 * NAME: void Remote_Module_registerAutoModeRequestCallback(RemoteCallback callback)
 *
 * DESCRIPTION:
 *      Initialises the hardware and variables needed for the IR remote control.
 *
 * INPUTS:
 *      PARAMETERS:
 *          RemoteCallback      callback    Function to register as callback
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
void Remote_Module_registerModeChangeRequestCallback(RemoteCallback callback);

#endif //REMOTE_MODULE_H
