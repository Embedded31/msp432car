/*H************************************************************************************************
 * FILENAME:        usb_serial_hal.h
 *
 * DESCRIPTION:
 *      Hardware Abstraction Layer (HAL) for serial communication via the USB port.
 *
 * PUBLIC FUNCTIONS:
 *      void        USB_SERIAL_HAL_init()
 *      void        USB_SERIAL_HAL_sendMessage(const char* format, ...)
 *      void        USB_SERIAL_HAL_registerMessageCallback(USBCallback callback)
 *
 * NOTES:
 *
 * AUTHOR: Andrea Piccin    <andrea.piccin@studenti.unitn.it>
 *
 * START DATE: 12 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 */

#ifndef USB_SERIAL_HAL_H
#define USB_SERIAL_HAL_H

#include <stddef.h>
#include <stdint.h>

#include "../inc/driverlib/driverlib.h"

/*T************************************************************************************************
 * NAME: USBCallback
 *
 * DESCRIPTION:
 *      It's a pointer to a function that manages the serial message after its reception.
 *
 * SPECIFICATIONS:
 *      Type:   void*
 *      Args:   const char*     message     text of the bluetooth incoming message
 */
typedef void (*USBCallback)(const char *message);

/*F************************************************************************************************
 * NAME: void USB_SERIAL_HAL_init()
 *
 * DESCRIPTION:
 *      Initialises the hardware required for the serial communications.
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
void USB_SERIAL_HAL_init();

/*F************************************************************************************************
 * NAME: void USB_SERIAL_HAL_sendMessage(const char* format, ...);
 *
 * DESCRIPTION:
 *      Send a string via the USB serial interface
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
void USB_SERIAL_HAL_sendMessage(const char *format, ...);

/*F************************************************************************************************
 * NAME:  void USB_SERIAL_HAL_registerMessageCallback(USBCallback callback)
 *
 * DESCRIPTION:
 *      Set the passed USBCallback as the function to call when a new message is ready
 *
 * INPUTS:
 *      PARAMETERS:
 *          USBCallback      callback        Function to register as callback
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
void USB_SERIAL_HAL_registerMessageCallback(USBCallback callback);

#endif // USB_SERIAL_HAL_H