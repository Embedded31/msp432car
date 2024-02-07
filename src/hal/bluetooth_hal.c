/*H************************************************************************************************
 * FILENAME:        bluetooth_hal.c
 *
 * DESCRIPTION:
 *      Bluetooth Hardware Abstraction Layer (HAL), this source file provides an abstraction over
 *      the UART communications with the Bluetooth Low Energy (BLE) module (HC-08 v2.2).
 *
 * PUBLIC FUNCTIONS:
 *      void    BT_HAL_init()
 *      void    BT_HAL_sendMessage(const char* data)
 *      bool    BT_HAL_isMessageAvailable()
 *      void    BT_HAL_readMessage(char* buffer, size_t length)
 *
 * NOTES:
 *      [1] Every time that a reception interrupt is generated by the eUSCI module  related to the
 *          Bluetooth the IRQHandler provided in this file will read the incoming message and store
 *          it in a global variable, then it will set the messageReady global variable to 1.
 *      [2] The global variables are declared as static volatile due to the interaction with the
 *          IRQHandler to avoid issues.
 *
 * AUTHOR: Andrea Piccin    <andrea.piccin@studenti.unitn.it>
 *
 * START DATE: 01 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 * 04 Feb 2024  Andrea Piccin   Refactoring
 */

#include <string.h>

#include "../../inc/bluetooth_hal.h"

#define BT_PORT GPIO_PORT_P3        /* Bluetooth I/O port                          */
#define BT_RX_PIN GPIO_PIN2         /* Bluetooth RX pin                            */
#define BT_TX_PIN GPIO_PIN3         /* Bluetooth TX pin                            */
#define BT_EUSCI_BASE EUSCI_A2_BASE /* eUSCI module used for UART communications   */
#define BT_EUSCI_INT INT_EUSCIA2    /* eUSCI interrupt related to the eUSCI module */
#define BT_BUFFER_SIZE 256          /* Max size of the unread message              */

static volatile char message[BT_BUFFER_SIZE]; /* Contains the unread message        */
static volatile int currentRxChar = 0;        /* Index of the current char to read  */
static volatile bool isMessageReady = false;  /* If there is a pending message      */
static volatile const char *currentTxChar;    /* Indicates the current char to send */
static volatile bool isMessageSent = true;    /* If the transmission is terminated  */

/*F************************************************************************************************
 * NAME: void BT_HAL_init()
 *
 * DESCRIPTION:
 *      Initialises the hardware required for the bluetooth communications:
 *      [1] Configure the RX and TX pins to be used for UART
 *      [2] Configure and enable the UART module
 *      [3] Enable interrupts
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
 *
 *  NOTE:
 */
void BT_HAL_init() {
    /* [1] Configure the RX and TX pins to be used for UART */
    GPIO_setAsPeripheralModuleFunctionInputPin(BT_PORT, BT_TX_PIN | BT_RX_PIN,
                                               GPIO_PRIMARY_MODULE_FUNCTION);

    /* [2] Configure and enable the UART module */
    const eUSCI_UART_ConfigV1 BT_uartConfig = {
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
    UART_initModule(BT_EUSCI_BASE, &BT_uartConfig);
    UART_enableModule(BT_EUSCI_BASE);

    /* [3] Enable interrupts */
    UART_enableInterrupt(BT_EUSCI_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(BT_EUSCI_INT);
    Interrupt_enableMaster();
}

/*F************************************************************************************************
 * NAME: void BT_HAL_sendMessage(const char* data)
 *
 * DESCRIPTION:
 *      Sends a message to the BLE module via the UART communication, the bluetooth module will
 *      forward it and every connected device will receive it, the procedure goes through the
 *      following steps:
 *      [1] Wait if the transmission of a previous message is not over yet
 *      [2] Sets the global variable currentChar to the data pointer
 *      [3] Enables the transmit interrupt that signals if the buffer is ready
 *      [4] The ISR will send the message
 *
 * INPUTS:
 *      PARAMETERS:
 *          const char* data            Pointer to the string (ends with '\0')
 *      GLOBALS:
 *          bool        isMessageSent   If the transmission is terminated
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          bool        isMessageSent   Set to false
 *          char*       currentTxChar   Equals data
 *
 *  NOTE:
 */
void BT_HAL_sendMessage(const char *data) {
    while (!isMessageSent)
        ;
    isMessageSent = false;
    currentTxChar = data;
    UART_enableInterrupt(BT_EUSCI_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT);
}

/*F************************************************************************************************
 * NAME: bool BT_HAL_isMessageAvailable()
 *
 * DESCRIPTION:
 *      Checks if there is an unread message in the message buffer.
 *
 * INPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          bool        isMessageReady      If there is a pending message
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *      RETURN:
 *          Type: bool
 *          Value: true if there is an unread message, false otherwise
 *
 *  NOTE:
 */
bool BT_HAL_isMessageAvailable() { return isMessageReady; }

/*F************************************************************************************************
 * NAME: void BT_HAL_readMessage(char* buffer, size_t length)
 *
 * DESCRIPTION:
 *      Copies the unread message to the buffer passed as parameter, if there isn't an unread
 *      message the empty string "\0" is copied.
 *      In the end reset the message buffer.
 *
 * INPUTS:
 *      PARAMETERS:
 *          char*       buffer              Pointer to the location to write
 *          size_t      length              Length of the destination
 *      GLOBALS:
 *          bool        isMessageReady      If there is a pending message
 *          char*       message             Contains the unread message
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          char*       buffer              Writes the string
 *      GLOBALS:
 *          bool        isMessageReady      Set to false
 *          int         currentRxChar       Index of the current char to read
 *
 *  NOTE:
 *      Only the first min(BT_BUFFER_SIZE, length) characters will be copied into the destination
 *      buffer.
 */
void BT_HAL_readMessage(char *buffer, size_t length) {
    /* if no messages available return "" */
    if (!isMessageReady)
        strcpy(buffer, "");

    /* copy the string into the buffer */
    size_t len = BT_BUFFER_SIZE < length ? BT_BUFFER_SIZE : length;
    strncpy(buffer, message, len);

    /* reset the message buffer */
    isMessageReady = false;
    currentRxChar = 0;
}

/*ISR**********************************************************************************************
 * NAME: void EUSCIA2_IRQHandler()
 *
 * DESCRIPTION:
 *      This function is called every time that an interrupt regarding the Bluetooth EUSCI module
 *      raises, two procedures can be executed:
 *      RECEIVE_INTERRUPT:  the interrupt signals that there is a character in the RX buffer, read
 *                          it, store it in the message buffer and update the currentRxChar; if the
 *                          end of string char is read ('\n', '\r' or '\0') set the isMessageReady
 *                          variable to true.
 *      TRANSMIT_INTERRUPT: the interrupt signals that the TX buffer is ready, the currentTxChar is
 *                          sent and the pointer is updated to the next char; if the currentChar is
 *                          equal to '\0' then set the isMessageSent to true and disable the
 *                          Transmit interrupts.
 *
 * INPUTS:
 *      GLOBALS:
 *          int             currentRxChar       Current char to read
 *          bool            isMessageReady      If there is a pending message
 *          char*           currentTxChar       Current char to send
 *
 *  OUTPUTS:
 *      GLOBALS:
 *          int             currentRxChar       Incremented by one after a RX
 *          bool            isMessageReady      Set true when RX is terminated
 *          char*           currentTxChar       Incremented by one after a TX
 *          bool            isMessageSent       Set true when TX is terminated
 *          char*           message             Contains the unread message
 *
 *  NOTE:
 *      If the reception interrupt is triggered while there is an unread message this one will be
 *      lost and overwrite with the new one.
 */
// cppcheck-suppress unusedFunction
void EUSCIA2_IRQHandler(void) {
    uint32_t status = UART_getEnabledInterruptStatus(BT_EUSCI_BASE);

    /* Receive routine */
    if (status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG) {
        /* read the char */
        char r = UART_receiveData(BT_EUSCI_BASE);

        /* if there is an unread message and we have other terminators on the buffer just skip
         * these characters, this happens when the strings are terminated with "\r\n" */
        if (isMessageReady && (r == '\n' || r == '\r' || r == '\0')) {
            return;
        }

        /* if new characters are on the buffer overwrite the current stored message */
        if (isMessageReady) {
            isMessageReady = false;
            currentRxChar = 0;
        }

        /* check array boundaries */
        if (0 <= currentRxChar && currentRxChar < BT_BUFFER_SIZE) {
            if (r == '\n' || r == '\r' || r == '\0') {
                message[currentRxChar] = '\0';
                isMessageReady = true;
            } else {
                message[currentRxChar] = r;
                currentRxChar++;
            }
        }
    }

    /* Transmit routine */
    if (status & EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG) {
        UART_transmitData(BT_EUSCI_BASE, *currentTxChar);
        if (*currentTxChar) {
            currentTxChar++;
        } else {
            UART_disableInterrupt(BT_EUSCI_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT);
            UART_clearInterruptFlag(BT_EUSCI_BASE, EUSCI_A_UART_TRANSMIT_INTERRUPT_FLAG);
            isMessageSent = true;
        }
    }
}