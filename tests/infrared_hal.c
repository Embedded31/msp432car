/*H************************************************************************************************
 * FILENAME:        infrared_hal.c
 *
 * DESCRIPTION:
 *      Infrared Receiver Hardware Abstraction Layer (HAL), this source file provides an
 *      abstraction over the reception of infrared signals exposing hardware-independent functions.
 *
 * PUBLIC FUNCTIONS:
 *      void    IR_HAL_init()
 *      void    IR_HAL_registerMessageCallback(IRCallback callback);
 *
 * NOTES:
 *      Due to the nature of the sensor's output a falling edge on the pin corresponds to a rising
 *      edge of the remote signal.
 *      The signal follows the NEC standard that is composed of:
 *      - a start signal of 9ms
 *      - pause of 4.5ms
 *      - 32 pulses in which an intra-edge time of 1ms indicates a logical 0 and a time of 2ms a 1
 *      - final signals (these signals are ignored in this implementation)
 *      The 32 pulses are divided into 4 8-bit fields: address, negated address, command and negate
 *      command.
 *      Thanks to the negated fields is possible to implement an easy error checking.
 *      Signal schema here:
 *      https://techdocs.altium.com/sites/default/files/wiki_attachments/296329/NECMessageFrame.png
 *
 * AUTHOR: Simone Rossi    <simone.rossi-2@studenti.unitn.it>
 *
 * START DATE: 19 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 */
#include <stdio.h>

#include "infrared_hal.h"

IRCallback irCallback = NULL;      /* Function to call after the reception of a message */
volatile uint32_t message;         /* Entire 32 bit IR message                          */

void IR_HAL_init() {}

void IR_HAL_registerMessageCallback(IRCallback callback) { irCallback = callback; }

void IR_HAL_parseAndForward() {
    // check validity
    uint8_t address = (message & 0xFF000000) >> 24;
    uint8_t address_inv = (message & 0x00FF0000) >> 16;
    uint8_t command = (message & 0x0000FF00) >> 8;
    uint8_t command_inv = message;
    bool isValid = !(address & address_inv) && !(command & command_inv);

    // if there is a registered callback function call it
    if (irCallback != NULL) {
        irCallback((IRCommand)command, isValid);
    }
}

void IR_HAL_triggerCommandReceived(const IRCommand command) {
    message = command << 8;
    IR_HAL_parseAndForward();
}