/*H************************************************************************************************
 * FILENAME:        ultrasonic_hal.h
 *
 * DESCRIPTION:
 *      This source file provides an abstraction over the measurement of the distance via an
 *      ultrasonic sensor, the SR-HC04.
 *
 * PUBLIC FUNCTIONS:
 *      void        US_HAL_init()
 *      void        US_HAL_triggerMeasurement()
 *      bool        US_HAL_isMeasurementReady()
 *      uint16_t    US_HAL_getMeasurement()
 *
 * NOTES:
 *
 * AUTHOR: Andrea Piccin    <andrea.piccin@studenti.unitn.it>
 *
 * START DATE: 06 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 */
#include <stdbool.h>

#include "../../inc/driverlib/driverlib.h"
#include "../../inc/ultrasonic_hal.h"

#define US_PORT GPIO_PORT_P1           /* Sensor's port                                     */
#define US_TRIGGER_PIN GPIO_PIN6       /* Trigger pin                                       */
#define US_ECHO_PIN GPIO_PIN7          /* Echo pin                                          */
#define US_TRIGGER_PERIOD 4            /* Ticks of the trigger signal, 4 at 375kHz = 10.6µs */
#define US_TICKS_TO_USEC_DIVIDER 0.375 /* Fixed value to convert the number of ticks in µs  */
#define US_TICKS_TO_CM_DIVIDER 21.866  /* Fixed value to convert the number of ticks in cm  */
#define US_OFFSET_FIX 12               /* Fixed value that fixes a sensor offset error      */

volatile bool isMeasurementReady; /* If the last triggered measurement is terminated       */
volatile uint16_t startTick;      /* Value of the counter at the rising edge on echo pin   */
volatile uint16_t endTick;        /* Value of the counter at the falling edge on echo pin  */

/*F************************************************************************************************
 * NAME: void US_HAL_init()
 *
 * DESCRIPTION:
 *      Initialises the hardware required for the distance sensing:
 *      [1] I/O pins configuration
 *      [2] Interrupt setup for catching rising and falling edge on echo pin
 *      [3] Timer configuration (continuous mode)
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
 *          bool    isMeasurementReady      Set to false
 *
 *  NOTE:
 */
void US_HAL_init() {
    // [1] I/O pins configuration
    GPIO_setAsOutputPin(US_PORT, US_TRIGGER_PIN);
    GPIO_setOutputLowOnPin(US_PORT, US_TRIGGER_PIN);
    GPIO_setAsInputPin(US_PORT, US_ECHO_PIN);

    // [2] Interrupt setup
    GPIO_clearInterruptFlag(US_PORT, US_ECHO_PIN);
    GPIO_enableInterrupt(US_PORT, US_ECHO_PIN);
    Interrupt_enableInterrupt(INT_PORT1);
    Interrupt_enableMaster();

    // [3] Timer configuration
    const Timer_A_ContinuousModeConfig contMode = {
        TIMER_A_CLOCKSOURCE_SMCLK,      // 24 MHz
        TIMER_A_CLOCKSOURCE_DIVIDER_64, // 24 MHz / 64 = 375 kHz
        TIMER_A_TAIE_INTERRUPT_DISABLE, // Disable overflow interrupt
        TIMER_A_DO_CLEAR                // Restart from zero after max count
    };
    Timer_A_configureContinuousMode(TIMER_A2_BASE, &contMode);
}

/*F************************************************************************************************
 * NAME: void US_HAL_triggerMeasurement()
 *
 * DESCRIPTION:
 *      This procedure start a new measurement setting the trigger pin on logic high, wait 10µs
 *      and setting the pin back to low logic level.
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
 *          bool    isMeasurementReady      Set to false
 *
 *  NOTE:
 */
void US_HAL_triggerMeasurement() {
    // stop and clear the timer
    Timer_A_stopTimer(TIMER_A2_BASE);
    Timer_A_clearTimer(TIMER_A2_BASE);

    // send a 10µs signal to the trigger pin
    GPIO_setOutputHighOnPin(US_PORT, US_TRIGGER_PIN);
    Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_CONTINUOUS_MODE);
    while (Timer_A_getCounterValue(TIMER_A2_BASE) < 4)
        ;
    GPIO_setOutputLowOnPin(US_PORT, US_TRIGGER_PIN);

    // reset the measurement ready flag
    isMeasurementReady = false;
}

/*F************************************************************************************************
 * NAME: bool US_HAL_isMeasurementReady()
 *
 * DESCRIPTION:
 *      Exposes the value of the isMeasurementReady global variable.
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
 *      RETURN:
 *          Type:   bool
 *          Value:  true if there is a valid measurement, false otherwise
 *
 *  NOTE:
 */
bool US_HAL_isMeasurementReady() { return isMeasurementReady; }

/*F************************************************************************************************
 * NAME: uint16_t US_HAL_getMeasurement()
 *
 * DESCRIPTION:
 *      This function converts the duration of the last echo signal (in number of ticks of the
 *      375kHz timer) to the corresponding distance in centimeters and returns the result.
 *
 * INPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          bool        isMeasurementReady     If the last triggered measurement is terminated
 *          uint16_t    startTick              Value of the counter at the rising edge on echo pin
 *          uint16_t    endTick                Value of the counter at the falling edge on echo pin
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          bool        isMeasurementReady     Set to false
 *      RETURN:
 *          Type:   uint16_t        unsigned 16-bit integer
 *          Value:  the distance from the ahead object in cm, US_RESULT_INVALID if the
 *                  measurement wasn't ready and US_RESULT_NO_OBJ if nothing is detected.
 *
 *  NOTE:
 *      [1] The TICKS_TO_CM_DIVIDER conversion divider comes from the following calculation:
 *          - For converting the travelling time of the sound waves (in µs) to the distance (in cm)
 *            we use the formula distance (m) = time (s) * sound speed (340m/s) / 2 that applied to
 *            our case became:
 *                      distance (cm) = [time (µs) *10^-6 * 343m/s / 2] / 10^2
 *            to, in 1µs the travelled distance is 0.01715cm
 *          - For translating ticks to microseconds we have to consider the frequency of the timer
 *            375kHz, that means that between every tick there is a period of 1/375*10^3 seconds
 *            approx 2.667µs, the formula is ticks * 2.667µs.
 *          The final formula (ticks to cm) is obtained by the previous two and says that one tick
 *          corresponds to (0.01715 * 2.666)cm = 0,04573cm (approx 1/21.866).
 *      [2] Following the sensor's datasheet the valid result range is between 4cm and 200cm.
 *      [3] Following the sensor's datasheet if after 36ms from the echo rising edge there isn't a
 *          falling edge the result has to be interpreted as nothing is in front of the sensor.
 */
uint16_t US_HAL_getMeasurement() {
    // if there isn't a result return the invalid result values
    if (!isMeasurementReady)
        return US_RESULT_INVALID;

    /* calculate the delta between the end and start tick and the corresponding time in µs,
     * if the time exceed 36ms return the no object detected value */
    uint16_t delta = endTick - startTick;
    uint16_t usec = delta / US_TICKS_TO_USEC_DIVIDER;
    if (usec > 36000)
        return US_RESULT_NO_OBJECT;

    // convert the delta in cm, subtract the sensor's error and return
    return (delta / US_TICKS_TO_CM_DIVIDER) - US_OFFSET_FIX;
}

/*ISR**********************************************************************************************
 * NAME: void PORT1_IRQHandler()
 *
 * DESCRIPTION:
 *      This function is called every time that an interrupt regarding the port no.1 occurs, in
 *      this function we're interested only in the pin 1.7 interrupts.
 *      We can have an interrupt for two reasons:
 *      Rising edge:    the echo signal made a logic low-to-high transition, we have to catch the
 *                      start tick of the echo signal.
 *      Falling edge:   the echo signal made a logic high-to-low transition, we have to catch the
 *                      end tick of the echo signal and set the isMeasurementReady flag.
 *
 * INPUTS:
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      GLOBALS:
 *          bool        isMeasurementReady     Set to true in the falling edge procedure
 *          uint16_t    startTick              Set to the TIMER_A2 counter value at the time of
 *                                             the rising edge
 *          uint16_t    endTick                Set to the TIMER_A2 counter value at the time of
 *                                             the falling edge
 *
 *  NOTE:
 */
// cppcheck-suppress unusedFunction
void PORT1_IRQHandler() {
    uint32_t status = GPIO_getEnabledInterruptStatus(US_PORT);
    GPIO_clearInterruptFlag(US_PORT, US_ECHO_PIN);

    if (status & US_ECHO_PIN) {
        // rising edge
        if (GPIO_getInputPinValue(US_PORT, US_ECHO_PIN)) {
            startTick = Timer_A_getCounterValue(TIMER_A2_BASE);
        }
        // falling edge
        else {
            endTick = Timer_A_getCounterValue(TIMER_A2_BASE);
            isMeasurementReady = true;
            Timer_A_stopTimer(TIMER_A2_BASE);
        }
    }
}