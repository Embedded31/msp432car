/*H************************************************************************************************
 * FILENAME:        state_machine.h
 *
 * DESCRIPTION:
 *      This header file contains the data structures and functions that defines the finite state
 *      machine.
 *
 * PUBLIC FUNCTIONS:
 *      void        FSM_init()
 *      void        FSM_running()
 *      void        FSM_sensing()
 *      void        FSM_turning()
 *      void        FSM_remote()
 *
 * NOTES:
 *
 * AUTHOR: Simone Rossi    <simone.rossi-2@studenti.unitn.it>
 *
 * START DATE: 19 Feb 2024
 *
 * CHANGES:
 */
#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

/*T************************************************************************************************
 * NAME: FSM_State
 *
 * DESCRIPTION:
 *      Represent the possible states of the FSM
 *
 * SPECIFICATIONS:
 *      Type:       enum
 *      Values:     STATE_INIT          Indicates the initialization state
 *                  STATE_RUNNING       Indicates the running state
 *                  STATE_SENSING       Indicates the sensing state
 *                  STATE TURNING       Indicates the turning state
 *                  STATE_REMOTE        Indicates the remote state
 *                  NUM_STATES          Indicates the number of states
 */
typedef enum {
    STATE_INIT,
    STATE_RUNNING,
    STATE_SENSING,
    STATE_TURNING,
    STATE_REMOTE,
    NUM_STATES,
} FSM_State;

/*T************************************************************************************************
 * NAME: FSM_StateMachine
 *
 * DESCRIPTION:
 *      Represent the finite state machine
 *
 * SPECIFICATIONS:
 *      Type:   struct
 *      Vars:   FSM_State           state       Current state of the FSM
 *              void (*void)(void)  function    Function to execute associated to the current state
 */
typedef struct{
    FSM_State state;
    void (*function)(void);
} FSM_StateMachine;

// Functions declaration
void FSM_init();        // Handle the STATE_INIT state
void FSM_running();     // Handle the STATE_RUNNING state
void FSM_sensing();     // Handle the STATE_SENSING state
void FSM_turning();     // Handle the STATE_TURNING state
void FSM_remote();      // Handle the STATE_REMOTE state

// Global variables definition
extern FSM_State FSM_currentState;    // Current FSM state
extern FSM_StateMachine FSM_stateMachine[];

#endif /* STATE_MACHINE_H_ */
