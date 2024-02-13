/*H************************************************************************************************
 * FILENAME:        usb_serial_hal.c
 *
 * DESCRIPTION:
 *      USB serial communication Hardware Abstraction Layer (HAL).
 *
 * PUBLIC FUNCTIONS:
 *      void        USB_SERIAL_HAL_init()
 *      void        USB_SERIAL_HAL_sendMessage(const char* format, ...)
 *      void        USB_SERIAL_HAL_registerMessageCallback(USBCallback callback)
 *
 * NOTES:
 *      Every time that a reception interrupt is generated by the eUSCI module related to the
 *      USB serial, the RQHandler provided in this file will read the incoming message and invoke
 *      the callback function.
 *
 * AUTHOR: Andrea Piccin    <andrea.piccin@studenti.unitn.it>
 *
 * START DATE: 12 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 */
#include <stdarg.h>
#include <stdio.h>

#include "usb_serial_hal.h"
#include "../inc/queue.h"

#define USB_PORT GPIO_PORT_P1        /* USB I/O port                          */
#define USB_RX_PIN GPIO_PIN2         /* USB RX pin                            */
#define USB_TX_PIN GPIO_PIN3         /* USB TX pin                            */
#define USB_BUFFER_SIZE 100          /* Max size of the unread message        */

/*T************************************************************************************************
 * NAME: TxState
 *
 * DESCRIPTION:
 *      Represent the state of the transmission routine.
 *
 * SPECIFICATIONS:
 *      Type:       enum
 *      Values:     TX_IDLE         When no message is being transmitted
 *                  TX_MESSAGE      When the body of the message is being transmitted
 *                  TX_CR           When the carriage return '\r' char have to be sent
 *                  TX_LF           When the line feed '\n' char have to be sent
 */
typedef enum { TX_IDLE, TX_MESSAGE, TX_CR, TX_LF } TxState;

USBCallback usbCallback;                                 /* To call when a new message is ready */
volatile char usbIncomingMessageBuffer[USB_BUFFER_SIZE]; /* Contains the incoming message       */
volatile uint16_t usbCurrentRxIndex;                     /* Index of the current char to read   */
volatile StringQueue usbOutgoingMessagesQueue;           /* Queue of the messages to send       */
volatile char *usbCurrentTxPointer;                      /* Pointer to the string to send       */
volatile TxState usbCurrentTxState;                      /* State the transmission              */

/*F************************************************************************************************
 * NAME: void USB_SERIAL_HAL_init()
 *
 * DESCRIPTION:
 *      Initialises the hardware required for the usb serial communications:
 *      [1] Configure the RX and TX pins to be used for UART
 *      [2] Configure and enable the UART module
 *      [3] Initialise global variables
 *      [4] Enable interrupts
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
 *          uint16_t        usbCurrentRxIndex           Set to 0
 *          StringQueue     usbOutgoingMessagesQueue    Initialised
 *          char*           usbCurrentTxPointer         Set to NULL
 *          TxState         usbCurrentTxState           Set to TX_IDLE
 *          USBCallback     usbCallback                 Set to NULL
 *
 *  NOTE:
 */
void USB_SERIAL_HAL_init() {
    Interrupt_disableMaster();
    /* [1] Configure the RX and TX pins to be used for UART */
    GPIO_setAsPeripheralModuleFunctionInputPin(USB_PORT, USB_TX_PIN | USB_RX_PIN,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    /* [2] Configure and enable the UART module */
    const eUSCI_UART_ConfigV1 USB_uartConfig = {
            EUSCI_A_UART_CLOCKSOURCE_SMCLK,               /* use SMCLK as clock source             */
            156,                                          /* scale SMCLK for a 9600 Baud Rate      */
            4,                                            /* set first modulation stage            */
            0,                                            /* set second modulation stage           */
            EUSCI_A_UART_NO_PARITY,                       /* disable parity error check            */
            EUSCI_A_UART_LSB_FIRST,                       /* least significant bit first           */
            EUSCI_A_UART_ONE_STOP_BIT,                    /* transmission pause between two bytes  */
            EUSCI_A_UART_MODE,                            /* use standard UART mode                */
            EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION /* oversampling for baud rate generation */
    };
    UART_initModule(EUSCI_A0_BASE, &USB_uartConfig);
    UART_enableModule(EUSCI_A0_BASE);

    /* [3] Initialise the global variables */
    usbCurrentRxIndex = 0;
    queue_init(&usbOutgoingMessagesQueue);
    usbCurrentTxPointer = NULL;
    usbCurrentTxState = TX_IDLE;
    usbCallback = NULL;

    /* [4] Enable interrupts */
    UART_clearInterruptFlag(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);
    UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA0);
    Interrupt_enableMaster();
}

/*F************************************************************************************************
 * NAME: void USB_SERIAL_HAL_sendMessage(const char* format, ...)
 *
 * DESCRIPTION:
 *      Sends a message via the usb serial interface, the procedure goes through the following
 *      steps:
 *      [1] Creates the message using the sprintf
 *      [2] Adds the message to the outgoing messages queue
 *      [3] Enables the transmit interrupt that signals if the transmission buffer is ready
 *      [4] The ISR will send the message
 *
 * INPUTS:
 *      PARAMETERS:
 *          const char* format              Format of the string in printf style
 *          ...         args                Like in printf
 *      GLOBALS:
 *          StringQueue usbOutgoingMessagesQueue    Queue of the messages to send
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          StringQueue usbOutgoingMessagesQueue    A new string is enqueued
 *
 *  NOTE:
 *      The queue has a fixed size of 10 elements, every exceeding message will be lost
 */
void USB_SERIAL_HAL_sendMessage(const char *format, ...) {
    if (queue_isFull(&usbOutgoingMessagesQueue))
        return;

    // [1] Creates the message using the sprintf
    char msg[QUEUE_ELEMENT_SIZE];
    va_list args;
    va_start(args, format);
    vsnprintf(msg, sizeof(msg), format, args);
    va_end(args);

    // [2] Adds the message to the outgoing messages queue
    queue_enqueue(&usbOutgoingMessagesQueue, msg);

    // [3] Enables the transmit interrupt
    UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT);
    // [4] The ISR will send the message
}

/*F************************************************************************************************
 * NAME: void USB_SERIAL_HAL_registerMessageCallback(USBCallback callback)
 *
 * DESCRIPTION:
 *      Registers the given USBCallback as the function to call when a new message is ready.
 *
 * INPUTS:
 *      PARAMETERS:
 *          USBCallback      callback        The function to register as callback
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          USBCallback      usbCallback      Set to callback
 *
 *  NOTE:
 */
void USB_SERIAL_HAL_registerMessageCallback(USBCallback callback) { usbCallback = callback; }

/*F************************************************************************************************
 * NAME: void USB_SERIAL_HAL_forwardAndReset()
 *
 * DESCRIPTION:
 *      This function is called by the ISR and is in charge of invoking the callback function
 *      passing the new message to it, then prepares the global variables for a new reading.
 *
 * INPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          char*           usbIncomingMessageBuffer    The read string
 *          USBCallback     usbCallback                 The function to execute
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          uint16_t        usbCurrentRxIndex            Set to zero
 *
 *  NOTE:
 */
void USB_SERIAL_HAL_forwardAndReset() {
    UART_disableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    if (usbCallback != NULL)
        usbCallback(usbIncomingMessageBuffer);
    usbCurrentRxIndex = 0;
    UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
}

/*ISR**********************************************************************************************
 * NAME: void EUSCIA0_IRQHandler()
 *
 * DESCRIPTION:
 *      This function is called every time that an interrupt regarding the usb serial module
 *      raises, two procedures can be executed:
 *      RECEIVE_INTERRUPT:  the interrupt signals that there is a character in the RX buffer, read
 *                          it, store it in the message buffer and update the usbCurrentRxIndex; if
 *                          the end of string is read ('\n', '\r' or '\0') forward the message to
 *                          the callback function.
 *      TRANSMIT_INTERRUPT: the interrupt signals that the TX buffer is ready, the first message on
 *                          the outgoing queue is dequeued and sent followed by \r\n.
 *                          When all the messages are sent disable the transmission interrupt.
 *
 * INPUTS:
 *      GLOBALS:
 *          int             usbCurrentRxIndex          Index of the current char to read
 *          char*           usbCurrentTxPointer        Current string to send
 *          StringQueue     usbOutgoingMessagesQueue   Queue of the messages to send
 *          TxState         usbCurrentTxState          State the transmission
 *
 *  OUTPUTS:
 *      GLOBALS:
 *          int             usbCurrentRxIndex          Incremented by one after a RX
 *          char*           usbIncomingMessageBuffer   Updated with a new character after one RX
 *          char*           usbCurrentTxPointer        Incremented by one after a TX
 *          TxState         usbCurrentTxState          Updated
 *
 *  NOTE:
 */
// cppcheck-suppress unusedFunction
void EUSCIA0_IRQHandler(void) {
    uint32_t status = UART_getEnabledInterruptStatus(EUSCI_A0_BASE);

    /* Receive routine */
    if (status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) {
        /* read the char */
        char r = UART_receiveData(EUSCI_A0_BASE);

        /* In case a message is terminated by \r\n we ignore all the extra
         * terminating characters */
        if (usbCurrentRxIndex == 0 && (r == '\n' || r == '\r' || r == '\0')) {
            return;
        }

        // if buffer overflows send the partial string
        if (usbCurrentRxIndex == USB_BUFFER_SIZE) {
            usbIncomingMessageBuffer[USB_BUFFER_SIZE - 1] = '\0';
            USB_SERIAL_HAL_forwardAndReset();
            return;
        }

        // if r is a termination char invoke the callback function and prepare for the next message
        if (r == '\n' || r == '\r' || r == '\0') {
            usbIncomingMessageBuffer[usbCurrentRxIndex] = '\0';
            USB_SERIAL_HAL_forwardAndReset();
        } else {
            usbIncomingMessageBuffer[usbCurrentRxIndex] = r;
            usbCurrentRxIndex++;
        }
    }

    /* Transmit routine */
    if (status & EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) {

        /* if the state is TX_IDLE there is no transmission, if there is a message in the queue
         * load it, otherwise disable the interrupts */
        if (usbCurrentTxState == TX_IDLE) {
            if (queue_isEmpty(&usbOutgoingMessagesQueue)) {
                UART_disableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT);
            } else {
                usbCurrentTxPointer = queue_front(&usbOutgoingMessagesQueue);
                usbCurrentTxState = TX_MESSAGE;
            }
        }

        /* if the state is TX_MESSAGE there is a message to send, proceed with the next character,
         * if the string is terminated, switch to the TX_EOL state */
        else if (usbCurrentTxState == TX_MESSAGE) {
            if (*usbCurrentTxPointer != '\0') {
                UART_transmitData(EUSCI_A0_BASE, *usbCurrentTxPointer);
                usbCurrentTxPointer++;
            } else {
                queue_dequeue(&usbOutgoingMessagesQueue);
                usbCurrentTxState = TX_CR;
            }
        }

            /* if the state is TX_CR or TX_LF the two termination characters (\r\n) have to be sent */
        else if (usbCurrentTxState == TX_CR) {
            UART_transmitData(EUSCI_A0_BASE, '\r');
            usbCurrentTxState = TX_LF;
        } else if (usbCurrentTxState == TX_LF) {
            UART_transmitData(EUSCI_A0_BASE, '\n');
            usbCurrentTxState = TX_IDLE;
        }
    }
}