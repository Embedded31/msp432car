/*H************************************************************************************************
 * FILENAME:        infrared_hal.h
 *
 * DESCRIPTION:
 *      Infrared Receiver Hardware Abstraction Layer (HAL), this header provides an abstraction
 *      over the reception of infrared signals exposing hardware-independent functions.
 *
 * PUBLIC FUNCTIONS:
 *      void    IR_HAL_init()
 *      void    IR_HAL_registerMessageCallback(IRCallback callback)
 *      void    IR_HAL_triggerCommandReceived(const IRCommand command)
 *
 * NOTES:
 *      The infrared HAL contains the Interrupt Service Routine (ISR) associated with the signal
 *      pin of the IR receiver, every time that a message is received the ISR will execute a
 *      callback function that can be set using the registerMessageCallback() function.
 *
 * AUTHOR: Simone Rossi    <simone.rossi-2@studenti.unitn.it>
 *
 * START DATE: 5 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 * 07 Feb 2024  Andrea Piccin   Refactoring
 * 08 Feb 2024  Andrea Piccin   Introduced callback mechanism
 * 19 Feb 2024  Simone Rossi    Modified for testing
 */
#include <stdbool.h>
#include <stdint.h>

#ifndef INFRARED_HAL_H
#define INFRARED_HAL_H

/*T************************************************************************************************
 * NAME: IRCommand
 *
 * DESCRIPTION:
 *      Represent one of the possible commands that can be sent from the remote.
 *
 * SPECIFICATIONS:
 *      Type:   enum
 *      Values: the options are in the form IR_COMMAND_X where X indicates the button of the remote
 */
typedef enum {
    IR_COMMAND_UP = 70,
    IR_COMMAND_DOWN = 21,
    IR_COMMAND_LEFT = 68,
    IR_COMMAND_RIGHT = 67,
    IR_COMMAND_OK = 64,
    IR_COMMAND_0 = 82,
    IR_COMMAND_1 = 22,
    IR_COMMAND_2 = 25,
    IR_COMMAND_3 = 13,
    IR_COMMAND_4 = 12,
    IR_COMMAND_5 = 24,
    IR_COMMAND_6 = 94,
    IR_COMMAND_7 = 8,
    IR_COMMAND_8 = 28,
    IR_COMMAND_9 = 90,
    IR_COMMAND_ASTERISK = 66,
    IR_COMMAND_HASHTAG = 74
} IRCommand;

/*T************************************************************************************************
 * NAME: IRCallback
 *
 * DESCRIPTION:
 *      It's a pointer to a function that manages the IR command after its reception.
 *
 * SPECIFICATIONS:
 *      Type:   void*
 *      Args:   IRCommand   command     the read command
 *              bool        isValid     false if the message contains errors, true otherwise
 */
typedef void (*IRCallback)(IRCommand command, bool isValid);

/*F************************************************************************************************
 * NAME: void IR_HAL_init()
 *
 * DESCRIPTION:
 *      Initialises the hardware required for the reception of infrared messages
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
void IR_HAL_init();

/*F************************************************************************************************
 * NAME: void IR_HAL_registerMessageCallback(IRCallback callback)
 *
 * DESCRIPTION:
 *      Registers the given IRCallback as the function to call when a new message is read.
 *
 * INPUTS:
 *      PARAMETERS:
 *          IRCallback      callback        the function to register as callback
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
void IR_HAL_registerMessageCallback(IRCallback callback);

/*F************************************************************************************************
 * NAME: void IR_HAL_triggerCommandReceived(const IRCommand command);
 *
 * DESCRIPTION:
 *      Simulate the reception of the given message.
 *
 * INPUTS:
 *      PARAMETERS:
 *          const char* message              Received message.
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
void IR_HAL_triggerCommandReceived(const IRCommand command);

#endif // INFRARED_HAL_H