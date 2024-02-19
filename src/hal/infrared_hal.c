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
 * START DATE: 5 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 * 07 Feb 2024  Andrea Piccin   Refactoring
 * 08 Feb 2024  Andrea Piccin   Introduced callback mechanism
 */
#include <stdio.h>

#include "../../inc/driverlib/driverlib.h"
#include "../../inc/infrared_hal.h"

#define IR_PORT GPIO_PORT_P2    /* Port of the infrared signal                    */
#define IR_PIN GPIO_PIN7        /* Pin of the infrared signal                     */
#define IR_INVALID_THRESHOLD 50 /* Time (ms) after which the reception is aborted */

IRCallback irCallback = NULL;      /* Function to call after the reception of a message */
volatile uint16_t lastFallingEdge; /* Tick of the last signal rising edge               */
volatile uint32_t message;         /* Entire 32 bit IR message                          */
volatile uint8_t bitIndex;         /* Index of the next bit to write                    */
volatile int8_t byteIndex;         /* Index of the next byte to write                   */

/*F************************************************************************************************
 * NAME: void IR_HAL_init()
 *
 * DESCRIPTION:
 *      Initialises the hardware required for the reception of infrared messages:
 *      [1] Input pin and GPIO interrupt setup
 *      [2] Timer configuration
 *      [3] Global variables initialisation
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
 *      The sensor normally outputs a high signal that goes low during the reception of a message,
 *      we use a pull-down resistor to have the signal in a well-known state when the sensor's
 *      output is low.
 */
void IR_HAL_init() {
    Interrupt_disableMaster();

    // [1] Input pin and GPIO interrupt setup
    GPIO_setAsInputPinWithPullDownResistor(IR_PORT, IR_PIN);
    GPIO_enableInterrupt(IR_PORT, IR_PIN);
    Interrupt_enableInterrupt(INT_PORT2);

    // [2] Timer configuration
    const Timer_A_ContinuousModeConfig contConfig = {
        TIMER_A_CLOCKSOURCE_ACLK,       // 32768 Hz
        TIMER_A_CLOCKSOURCE_DIVIDER_4,  // 32768 / 4 = 8192 Hz
        TIMER_A_TAIE_INTERRUPT_DISABLE, // disable overflow interrupt
        TIMER_A_DO_CLEAR,               // clear the counter on overflow
    };
    Timer_A_configureContinuousMode(TIMER_A3_BASE, &contConfig);
    Timer_A_startCounter(TIMER_A3_BASE, TIMER_A_CONTINUOUS_MODE);

    // [3] Global variables initialisation
    bitIndex = 0;
    byteIndex = 3;
    lastFallingEdge = 0;

    Interrupt_enableMaster();
}

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
 *          IRCallback      irCallback      Points to the given callback function
 *
 *  NOTE:
 */
void IR_HAL_registerMessageCallback(IRCallback callback) { irCallback = callback; }

/*F************************************************************************************************
 * NAME: void IR_HAL_parseAndForward()
 *
 * DESCRIPTION:
 *      This function is called by the ISR and is in charge of disassemble the message in its
 *      parts, checking the message validity and calling the callback function.
 *
 * INPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          uint32_t    message     The 32-bit infrared message
 *          IRCallback  irCallback  The function to execute on command reception
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void IR_HAL_parseAndForward() {
    GPIO_disableInterrupt(IR_PORT, IR_PIN);
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

    // reset counters
    bitIndex = 0;
    byteIndex = 3;
    lastFallingEdge = 0;

    GPIO_enableInterrupt(IR_PORT, IR_PIN);
}

/*ISR**********************************************************************************************
 * NAME: void PORT2_IRQHandler()
 *
 * DESCRIPTION:
 *      This function is called every time that an interrupt regarding the IR_PORT occurs, the
 *      procedure filters the interrupt regarding a falling edge on the IR_PIN and follows these
 *      steps:
 *      [1] Checks if the current one is the first falling edge, in this case we need only to
 *          update the lastFallingEdge variable.
 *      [2] Else we need to calculate the time difference between this and the last falling edge
 *          [2.a] If the time delta is grater than the IR_INVALID_THRESHOLD discard every reading
 *          [2.b] If the time delta corresponds to 13ms we expect a new message, discard every
 *                unfinished reading and reset the timer (timer reset prevents the timer from
 *                overflowing during the IR message reading that will cause time misreadings)
 *          [2.c] If the time delta is 1ms or 2ms set the corresponding bit to its value and update
 *                the counters
 *          If none of the above conditions are matched we're probably receiving a termination
 *          signal, and we can ignore it.
 *      [3] If the byteIndex goes to -1 we have read all the 32 bits, we can forward the message to
 *          the parsing function.
 *
 * INPUTS:
 *      GLOBALS:
 *          uint16_t        lastFallingEdge     Tick of the last signal rising edge
 *
 *  OUTPUTS:
 *      GLOBALS:
 *          uint16_t        lastFallingEdge     Updated with the tick of the last interrupt
 *          uint32_t        message             With an updated bit
 *          uint8_t         bitIndex            Increased by one or reset to 0 if it has reached 8
 *          uint8_t         byteIndex           Decreased by one if all its bits are set
 *
 *  NOTE:
 */
// cppcheck-suppress unusedFunction
void PORT2_IRQHandler() {
    uint32_t status = GPIO_getEnabledInterruptStatus(IR_PORT);
    GPIO_clearInterruptFlag(IR_PORT, IR_PIN);

    if (status & IR_PIN) {
        // catch the falling edge
        if (!GPIO_getInputPinValue(IR_PORT, IR_PIN)) {
            uint16_t now = Timer_A_getCounterValue(TIMER_A3_BASE);

            // [1] if it's the first edge just update the global variable
            if (lastFallingEdge == 0) {
                lastFallingEdge = now;
                return;
            }

            // [2] else we need to calculate the time difference from the last edge
            uint8_t deltaMs = (now - lastFallingEdge) * 0.122;
            lastFallingEdge = now;

            // [2.a] (invalid message) or [2.b] (new message incoming), reset the counters
            if (deltaMs >= IR_INVALID_THRESHOLD || deltaMs == 13) {
                bitIndex = 0;
                byteIndex = 3;
                lastFallingEdge = 0;
                Timer_A_clearTimer(TIMER_A3_BASE);
                return;
            }

            // [2.c] if the delta is 1ms or 2ms, update the message and the counters.
            // A 1ms delta corresponds to a 0 and a 2ms delta to a 1.
            if (deltaMs == 1 || deltaMs == 2) {

                if (deltaMs == 1)
                    message &= ~(1 << ((byteIndex * 8) + bitIndex));
                else
                    message |= 1 << ((byteIndex * 8) + bitIndex);

                bitIndex++;
                if (bitIndex == 8) {
                    bitIndex = 0;
                    byteIndex--;
                }

                // [3] The message is ready
                if (byteIndex == -1)
                    IR_HAL_parseAndForward();
            }
        }
    }
}