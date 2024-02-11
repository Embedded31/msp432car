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
 *      void        BT_HAL_registerMessageCallback(BTCallback callback)
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
 *          const char* data            Pointer to the string (ends with '\0')
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
void BT_HAL_sendMessage(const char *data);

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

#endif // BLUETOOTH_HAL_H