/*C************************************************************************************************
 * FILENAME:        main.c
 *
 * DESCRIPTION:
 *      This source file contains the main function that initialize the system
 *      and start the finite state machine
 *
 * PUBLIC FUNCTIONS:
 *      void        main()
 *
 * NOTES:
 *
 * AUTHOR: Simone Rossi    <simone.rossi-2@studenti.unitn.it>
 *
 * START DATE: 19 Feb 2024
 *
 * CHANGES:
 */
#include <stdbool.h>

#include "../../inc/state_machine.h"
#include "../../inc/system.h"

/*F************************************************************************************************
 * NAME: void main()
 *
 * DESCRIPTION:
 *      [1] Initialize the system
 *      [2] Start the finite state machine
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
int main(void) {
    // [1] Initialize the system
    System_init();

    // [2] Start the finite state machine
    while(true) {
       if(FSM_currentState < NUM_STATES){
           (*FSM_stateMachine[FSM_currentState].function)();
       } else {
           while(true) {} // Error: unknown state, halting the system
       }
   }
}
