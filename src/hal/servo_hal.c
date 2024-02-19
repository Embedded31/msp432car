/*H************************************************************************************************
 * FILENAME:        servo_hal.c
 *
 * DESCRIPTION:
 *      This source file provides the implementation of function that allows the abstraction of the
 *      management of a SG90 Servo motor.
 *
 * PUBLIC FUNCTIONS:
 *      void    SERVO_HAL_init(Servo* servo)
 *      void    SERVO_HAL_setPosition(Servo* servo, int8_t position)
 *      void    SERVO_HAL_registerPositionReachedCallback(ServoCallback callback)
 *
 * NOTES:
 *      The PWM timings are calculated basing on the SG90 datasheet:
 *      http://www.ee.ic.ac.uk/pcheung/teaching/DE1_EE/stores/sg90_datasheet.pdf
 *      and on experimental measurements.
 *
 * AUTHOR: Simone Rossi    <simone.rossi-2@studenti.unitn.it>
 *
 * START DATE: 08 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR              DETAIL
 * 13 Feb 2024  Andrea Piccin       Refactoring
 * 15 Feb 2024  Matteo Frizzera     Added callback mechanism to notify servo reached final position
 * 15 Feb 2024  Andrea Piccin       Refactoring, functions using timer A0 now use TIMER32_0
 */
#include <stdlib.h>

#include "../../inc/driverlib/driverlib.h"
#include "../../inc/servo_hal.h"

#define SERVO_PORT GPIO_PORT_P5    /* Port for the PWM signals                                */
#define SERVO_PIN GPIO_PIN6        /* Pin for the PWM signals                                 */
#define SERVO_TIMER_PERIOD 20000   /* PWM period of 20ms                                      */
#define SERVO_MIN_POS_TICKS 680    /* Time of the high PWM signal for -90 deg                 */
#define SERVO_MID_POS_TICKS 1400   /* Time of the high PWM signal for 0 deg                   */
#define SERVO_MAX_POS_TICKS 2300   /* Time of the high PWM signal for +90deg                  */
#define SERVO_LOAD_COEFFICIENT 2.2 /* Corrects the rotational delay due to attached weight    */
#define SERVO_180DEG_TICKS 28125   /* Timer32 ticks for a 180 deg turn according to datasheet */

/* In order to simplify the code the following definition introduces a new ticks count for
 * a full rotation in which the correction coefficient is already applied */
#define SERVO_ADJ_180DEG_TICKS (SERVO_180DEG_TICKS * SERVO_LOAD_COEFFICIENT)

ServoCallback servoCallback; /* function to execute when the servo reaches its final position */

/*F************************************************************************************************
 * NAME: void SERVO_HAL_init(Servo* servo);
 *
 * DESCRIPTION:
 *      Initialises a Servo motor instance setting the initial position to the value in between
 *      the min and max position values. The steps of the procedure are:
 *      [1] Initialise servo's values
 *      [2] Configure servo's pin
 *      [3] Configure the  base timer
 *      [4] Set up the Capture Compare Register (CCR) for the PWM signal generation
 *      [5] Init the timer32 module used for waiting for the servo to be in place
 *      [6] Wait for the servo to be at 0 deg position
 *      [7] Enable timer interrupts
 *
 * INPUTS:
 *      PARAMETERS:
 *          Servo*              servo           Motor that has to be initialised
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          Servo*              servo           All the fields of the struct are set
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void SERVO_HAL_init(Servo *servo) {
    Interrupt_disableMaster();
    // [1] Initialize servo's values
    servo->ccr = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    servoCallback = NULL;

    // [2] Configure servo's pin
    GPIO_setAsPeripheralModuleFunctionOutputPin(SERVO_PORT, SERVO_PIN,
                                                GPIO_PRIMARY_MODULE_FUNCTION);

    // [3] Configure the  base timer
    Timer_A_UpModeConfig upConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,           // SMCLK = 24MHz
        TIMER_A_CLOCKSOURCE_DIVIDER_24,      // SMCLK/24 = 1MHz -> 0.001ms period
        SERVO_TIMER_PERIOD,                  // 20ms / 0.001ms = 20000 ticks
        TIMER_A_TAIE_INTERRUPT_DISABLE,      // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE, // Disable CCR0 interrupt
        TIMER_A_DO_CLEAR,                    // Clear value
    };
    Timer_A_configureUpMode(TIMER_A2_BASE, &upConfig);
    Timer_A_clearTimer(TIMER_A2_BASE);
    Timer_A_startCounter(TIMER_A2_BASE, TIMER_A_UP_MODE);

    // [4] Set up the Capture Compare Register (CCR) for the PWM signal generation
    const Timer_A_CompareModeConfig compareConfig = {
        servo->ccr,
        TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,
        TIMER_A_OUTPUTMODE_TOGGLE_SET,
        SERVO_MID_POS_TICKS,
    };
    Timer_A_initCompare(TIMER_A2_BASE, &compareConfig);

    /* [5] Init timer32 module used for waiting for the servo to be in place
     * timer clock: MCLK / 256 = 24MHz / 256 = 93750 Hz => 0.01ms period */
    Timer32_initModule(TIMER32_0_BASE, TIMER32_PRESCALER_256, TIMER32_32BIT, TIMER32_PERIODIC_MODE);

    // [6] Wait for the servo to be at 0 deg position
    Timer32_setCount(TIMER32_0_BASE, SERVO_ADJ_180DEG_TICKS);
    Timer32_startTimer(TIMER32_0_BASE, true);
    while (Timer32_getValue(TIMER32_0_BASE) != 0)
        ;
    servo->state.position = 0;

    // [7] Enable timer interrupts
    Timer32_clearInterruptFlag(TIMER32_0_BASE);
    Timer32_enableInterrupt(TIMER32_0_INTERRUPT);
    Interrupt_enableInterrupt(INT_T32_INT1);

    Interrupt_enableMaster();
}

/*F************************************************************************************************
 * NAME: void SERVO_HAL_positionToTicks(int8_t position);
 *
 * DESCRIPTION:
 *      Convert a position into the number of ticks for the PWM duty cycle
 *
 * INPUTS:
 *      PARAMETERS:
 *          int8_t          position        Target position, between -90 and 90
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *      RETURN:
 *          type:   uint16_t
 *          Value:  The number of ticks in which the signal should be high
 *
 *  NOTE:
 */
uint16_t SERVO_HAL_positionToTicks(int8_t position) {
    uint16_t deltaTicks;
    if (position < 0) {
        deltaTicks = SERVO_MID_POS_TICKS - SERVO_MIN_POS_TICKS;
    } else {
        deltaTicks = SERVO_MAX_POS_TICKS - SERVO_MID_POS_TICKS;
    }
    return SERVO_MID_POS_TICKS + (position * (deltaTicks * 1.0) / 90);
}

/*F************************************************************************************************
 * NAME: void SERVO_HAL_setPosition(Servo* servo, int8_t position);
 *
 * DESCRIPTION:
 *      Set the position of a servo
 *
 * INPUTS:
 *      PARAMETERS:
 *          Servo*      servo                      Target servo
 *          int8_t      position                   Specifies the position to which rotate.
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          int8_t      servo->state.position      Set on the current position
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 *      Calculates the number to insert in the timer32 countdown using this formula:
 *          ticks = (abs(targetPos - currentPos) / 180) * SERVO_ADJ_180DEG_TICKS
 *      where the number of ticks is the result of the proportion between the angle that the servo
 *      has to traver and the time to travel 180 deg.
 *      The servoCallback is registered as interrupt handler for the Timer32 interrupt.
 */
void SERVO_HAL_setPosition(Servo *servo, int8_t position) {
    if (position < SERVO_MIN_POSITION)
        position = SERVO_MIN_POSITION;

    if (position > SERVO_MAX_POSITION)
        position = SERVO_MAX_POSITION;

    // calculate duty time ticks
    uint16_t dutyCycleTicks = SERVO_HAL_positionToTicks(position);

    // Update compare value
    Timer_A_setCompareValue(TIMER_A2_BASE, servo->ccr, dutyCycleTicks);
    Timer_A_clearTimer(TIMER_A2_BASE);

    // starts timer to notify when servo has reached position (one shot mode)
    if (servo->state.position == position && servoCallback != NULL) {
        servoCallback();
    } else {
        uint32_t ticks =
            (abs(position - servo->state.position) * 1.0 / 180) * SERVO_ADJ_180DEG_TICKS;
        Timer32_setCount(TIMER32_0_BASE, ticks);
        Timer32_startTimer(TIMER32_0_BASE, true);
        servo->state.position = position;
    }
}

/*F************************************************************************************************
 * NAME: void SERVO_HAL_registerPositionReachedCallback(ServoCallback callback)
 *
 * DESCRIPTION:
 *      Registers the ServoCallback as the function to call when servo motor reaches destination.
 *
 * INPUTS:
 *      PARAMETERS:
 *          ServoCallback callback                  The function to register as callback
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          ServoCallback servoCallback             Set to the given callback
 *
 *  NOTE:
 */
void SERVO_HAL_registerPositionReachedCallback(ServoCallback callback) { servoCallback = callback; }

/*ISR**********************************************************************************************
 * NAME: void T32_INT1_IRQHandler()
 *
 * DESCRIPTION:
 *      This function is called every time that the TIMER32_0 counter reaches zero, this means
 *      that the servo has reached its final position.
 *      This procedure calls the registered callback (if existing).
 *
 * INPUTS:
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
// cppcheck-suppress unusedFunction
void T32_INT1_IRQHandler() {
    Timer32_clearInterruptFlag(TIMER32_0_BASE);
    if (servoCallback != NULL)
        servoCallback();
}