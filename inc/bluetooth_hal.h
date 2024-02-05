/*H************************************************************************************************
 * FILENAME:        bluetooth_hal.h
 *
 * DESCRIPTION:
 *      Bluetooth Hardware Abstraction Layer (HAL), this header provides an abstraction over the
 *      UART communications with the Bluetooth Low Energy (BLE) module (HC-08 v2.2).
 *
 * PUBLIC FUNCTIONS:
 *      void        BT_HAL_init()
 *      void        BT_HAL_sendMessage(const char* data)
 *      bool        BT_HAL_isMessageAvailable()
 *      void        BT_HAL_readMessage(char* buffer, size_t length)
 *
 * NOTES:
 *
 * AUTHOR: Andrea Piccin    <andrea.piccin@studenti.unitn.it>
 *
 * START DATE: 01 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 * 04 Feb 2024  Andrea Piccin   Refactoring
 */

#ifndef BLUETOOTH_HAL_H
#define BLUETOOTH_HAL_H

#include <stddef.h>
#include <stdint.h>

#include "driverlib/driverlib.h"

/*F************************************************************************************************
 * NAME: void BT_HAL_init()
 *
 * DESCRIPTION:
 *      Initialises the hardware required for the bluetooth communications.
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
void BT_HAL_init();

/*F************************************************************************************************
 * NAME: void BT_HAL_sendMessage(const char* data);
 *
 * DESCRIPTION:
 *      Send a string to the BLE module that will send it to every connected device.
 *
 * INPUTS:
 *      PARAMETERS:
 *          const char* data            Pointer to the string (ends with '\0')
 *      GLOBALS:
 *          uint32_t    BT_EUSCI_BASE   EUSCI module
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void BT_HAL_sendMessage(const char *data);

/*F************************************************************************************************
 * NAME: bool BT_HAL_isMessageAvailable();
 *
 * DESCRIPTION:
 *      Check if there is an unread message.
 *
 * INPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
            None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *      RETURN:
 *          Type:   bool
 *          Value:  true if an unread message exists, false otherwise
 *
 *  NOTE:
 */
bool BT_HAL_isMessageAvailable();

/*F************************************************************************************************
 * NAME: void BT_HAL_readMessage(char* buffer, size_t length);
 *
 * DESCRIPTION:
 *      Read the last received and unread message.s
 *
 * INPUTS:
 *      PARAMETERS:
 *          char*       buffer          Pointer to the location to write
 *          size_t      length          Length of the buffer
 *      GLOBALS:
 *          int         BT_BUFFER_SIZE  Max size of the message buffer
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          char*       buffer          Writes the string
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void BT_HAL_readMessage(char *buffer, size_t length);

#endif // BLUETOOTH_HAL_H