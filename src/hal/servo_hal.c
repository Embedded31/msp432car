/*H************************************************************************************************
 * FILENAME:        servo_hal.c
 *
 * DESCRIPTION:
 *      This source file provides the implementation of function that allows the abstraction of the
 *      management of a SG90 Servo motor.
 *
 * PUBLIC FUNCTIONS:
 *      void    SERVO_HAL_init(Servo* servo);
 *      void    SERVO_HAL_setPosition(Servo* servo, int8_t position);
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
 * 15 Feb 2024  Matteo Frizzera     Added callback mechanism to notify when servo has reached target direction
 */
#include "../../inc/servo_hal.h"
#include "../../inc/driverlib/driverlib.h"

#define SERVO_PORT GPIO_PORT_P5  /* Port for the PWM signals                */
#define SERVO_PIN GPIO_PIN6      /* Pin for the PWM signals                 */
#define SERVO_TIMER_PERIOD 20000 /* PWM period of 20ms                      */
#define SERVO_MIN_POSITION -90   /* Minimum position in deg                 */
#define SERVO_MAX_POSITION 90    /* Maximum position in deg                 */
#define SERVO_MIN_POS_TICKS 660  /* Time of the high PWM signal for -90 deg */
#define SERVO_MID_POS_TICKS 1460 /* Time of the high PWM signal for 0 deg   */
#define SERVO_MAX_POS_TICKS 2350 /* Time of the high PWM signal for +90deg  */

// TODO need to take some experimental measurements to know how long it takes for the servo to rotate
// from the data sheet: 0.1 seconds / 60 degress when there is no load

#define SERVO_LOAD_COEFF 0.25                        /* Rotates slower as there is a load on the motor (weight of US sensor)
                                                        TODO some experimenal measurements need to be done                   */
#define SERVO_TIME_FULL_ROT (0.3 / SERVO_LOAD_COEFF) /* Seconds to make full rotation, from -90 to 90                        */   

// executes when servo has reached target direction, after a SERVO_HAL_setPosition(...)
ServoCallback servoCallback;

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
 *      [5] Set initial position
 *      [6] configure timer clock used to wait for servo motor to finish rotating
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
        0,
    };
    Timer_A_initCompare(TIMER_A2_BASE, &compareConfig);

    // [5] Set initial position
    SERVO_HAL_setPosition(servo, 0);

    // [6] configure timer clock used to wait for servo motor to finish rotating

    /* sets ACLK to LFXT (onboard Low Frequency oscillator, 32kHz freq.)
       ACLK will have a frequency of 32kHz                            */
    CS_initClockSignal(CS_ACLK, CS_LFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);

    servoCallback = NULL;

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
 *      Will generate a Capture Compare Interrupt on Timer A0 when servo has finished moving
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

    // starts timer to notify when servo has reached position
    // it calculates how much time to wait based on the change in position
    uint16_t period = (abs(position - servo->state.position) / 180) * SERVO_TIME_FULL_ROT;
    Timer_A_UpModeConfig upConfigServo = {
        TIMER_A_CLOCKSOURCE_ACLK,            // ACLK = 32kHz
        TIMER_A_CLOCKSOURCE_DIVIDER_32,      // ACLK/32 = 1kHz -> 0.001s period
        period / 0.001,                      // rotation time (s) / 0.001s
        TIMER_A_TAIE_INTERRUPT_DISABLE,      // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,  // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR,                    // Clear value
    };
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfigServo);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
    // enables Capture Compare interrupt on Timer A0
    Interrupt_enableInterrupt(INT_TA0_0);

    // Update servo info
    servo->state.position = position;
}


/*F************************************************************************************************
 * NAME: void SERVO_HAL_registerFinishedMovingCallback(ServoCallback callback)
 *
 * DESCRIPTION:
 *      Registers the ServoCallback as the function to call when servo motor reaches destination
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
void SERVO_HAL_registerFinishedMovingCallback(ServoCallback callback){
    servoCallback = callback;
}

/*ISR**********************************************************************************************
 * NAME: void PORT2_IRQHandler()
 *
 * DESCRIPTION:
 *      This function is called when a Capture Compare interrupt happens on Timer A0
 *
 * INPUTS:
 *      GLOBALS:
 *          ServoCallback   servoCallback       callback function to execute
 *
 *  OUTPUTS:
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void TA0_0_IRQHandler(void)
{
    // clears CC flag
    Timer_A_clearCaptureCompareInterrupt(TIMER_A0_BASE,
            TIMER_A_CAPTURECOMPARE_REGISTER_0);

    // stops timer
    Timer_A_stopTimer(TIMER_A0_BASE);

    // executes callback function
    servoCallback();
}