/*H************************************************************************************************
 * FILENAME:        bluetooth_hal.h
 *
 * DESCRIPTION:
 *      Bluetooth Hardware Abstraction Layer (HAL), this header provides an abstraction over the
 *      UART communications with the Bluetooth Low Energy (BLE) module (HC-08 v2.2).
 *
 * PUBLIC FUNCTIONS:
 *      void        BT_HAL_init()
 *      void        BT_HAL_sendMessage(const char* format, ...)
 *      void        BT_HAL_registerMessageCallback(BTCallback callback)
 *      void        BT_HAL_triggerMessageReceived(const char* message)
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
 * 09 Feb 2024  Andrea Piccin   Introduced callback mechanism
 * 19 Feb 2024  Simone Rossi    Modified for testing
 */

#ifndef BLUETOOTH_HAL_H
#define BLUETOOTH_HAL_H

#include <stddef.h>
#include <stdint.h>

#include "driverlib/driverlib.h"

/*T************************************************************************************************
 * NAME: BTCallback
 *
 * DESCRIPTION:
 *      It's a pointer to a function that manages the Bluetooth message after its reception.
 *
 * SPECIFICATIONS:
 *      Type:   void*
 *      Args:   const char*     message     text of the bluetooth incoming message
 */
typedef void (*BTCallback)(const char *message);

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
 *          const char* format              Format of the string in printf style
 *          ...         args                Like in printf
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
void BT_HAL_sendMessage(const char *format, ...);

/*F************************************************************************************************
 * NAME:  void BT_HAL_registerMessageCallback(BTCallback callback)
 *
 * DESCRIPTION:
 *      Set the passed BTCallback as the function to call when a new message is ready
 *
 * INPUTS:
 *      PARAMETERS:
 *          BTCallback      callback        Function to register as callback
 *      GLOBALS:
            None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void BT_HAL_registerMessageCallback(BTCallback callback);

/*F************************************************************************************************
 * NAME: void BT_HAL_BT_HAL_triggerMessageReceived(const char* message);
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
void BT_HAL_triggerMessageReceived(const char* message);

#endif // BLUETOOTH_HAL_H