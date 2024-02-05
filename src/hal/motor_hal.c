/*H************************************************************************************************
 * FILENAME:        motor_hal.c
 *
 * DESCRIPTION:
 *      This source file provides an abstraction over the management of two separate direct current
 *      motors via the L298N motor driver.
 *
 * PUBLIC FUNCTIONS:
 *      void    MOTOR_HAL_init()
 *      void    MOTOR_HAL_motorInit(Motor *motor, MotorInitTemplate initTemplate)
 *      void    MOTOR_HAL_setSpeed(Motor *motor, uint8_t speed)
 *      void    MOTOR_HAL_setDirection(Motor *motor, MotorDirection direction)
 *      void    MOTOR_HAL_stop(Motor *motor)
 *
 * NOTES:
 *      In our implementation there are two motors attached to each of the L298N channels, so we
 *      have a total of four motors controlled as left and right pairs.
 *
 * AUTHOR: Simone Rossi    <simone.rossi-2@studenti.unitn.it>
 *
 * START DATE: 31 Jan 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 * 05 Feb 2024  Andrea Piccin   Refactoring
 */
#include "../../inc/motor_hal.h"

#define MOTOR_TIMER_PERIOD 5000        /* Max value of the counter           */
#define MOTOR_ENABLE_PORT GPIO_PORT_P2 /* Port for the PWM signals           */
#define MOTOR_R_PWM GPIO_PIN5          /* Pin for the right motor PWM signal */
#define MOTOR_L_PWM GPIO_PIN4          /* Pin fot the left motor PWM signal  */
#define MOTOR_INPUT_PORT GPIO_PORT_P4  /* Port of the direction pins         */
#define MOTOR_R_IN1 GPIO_PIN1          /* Right motor's direction pin 1      */
#define MOTOR_R_IN2 GPIO_PIN2          /* Right motor's direction pin 2      */
#define MOTOR_L_IN1 GPIO_PIN4          /* Left motor's direction pin 1       */
#define MOTOR_L_IN2 GPIO_PIN3          /* Left motor's direction pin 2       */

/*F************************************************************************************************
 * NAME: void MOTOR_HAL_init()
 *
 * DESCRIPTION:
 *      Initialises the hardware required for the motors:
 *      [1] Configure the base timer to output a 100Hz signal that will be used in the generation
 *          of the PWM signal
 *      [2] Start the timer
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
 *      The 100Hz values comes from the TTL standard adopted by the L298N that requires a 10ms
 *      period (freq = 1/period => freq = 1/0.010 = 100Hz)
 */
void MOTOR_HAL_init() {
    // [1] Configure the  base timer
    Timer_A_UpModeConfig upConfig = {
        TIMER_A_CLOCKSOURCE_SMCLK,           // SMCLK = 24MHz
        TIMER_A_CLOCKSOURCE_DIVIDER_48,      // SMCLK/24 = 500kHz
        MOTOR_TIMER_PERIOD,                  // SMCLK/24/5000 = 100Hz
        TIMER_A_TAIE_INTERRUPT_DISABLE,      // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_DISABLE, // Disable CCR0 interrupt
        TIMER_A_DO_CLEAR                     // Clear value
    };
    Timer_A_configureUpMode(TIMER_A0_BASE, &upConfig);

    // [2] start timer
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
}

/*F************************************************************************************************
 * NAME: void MOTOR_HAL_motorInit(Motor *motor, MotorInitTemplate initTemplate);
 *
 * DESCRIPTION:
 *      Initialises a Motor instance following a template that specifies if it has to be configured
 *      as right or left motor. The steps of the procedure are:
 *      [1] Configure motor's IN1 and IN2 pins
 *      [2] Configure the enable pin as PWM output
 *      [3] Initialise the motor values
 *      [4] Set up the Capture Compare Register (CCR) for the PWM signal generation
 *
 * INPUTS:
 *      PARAMETERS:
 *          Motor*              motor           Motor that has to be initialised
 *          MotorInitTemplate   initTemplate    Specifies if the motor configuration
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          Motor*              motor           All the fields of the struct are set
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 *      The CCR associated with the pin 2.4 (left PWM) is the CCR1 while the one regarding the pin
 *      2.5 (right PWM) is the CCR2.
 */
void MOTOR_HAL_motorInit(Motor *motor, MotorInitTemplate initTemplate) {
    // [1] Configure motor's IN1 and IN2 pins
    uint8_t inPins;
    if (initTemplate == MOTOR_INIT_LEFT) {
        inPins = MOTOR_L_IN1 | MOTOR_L_IN2;
    } else {
        inPins = MOTOR_R_IN1 | MOTOR_L_IN2;
    }
    GPIO_setAsOutputPin(MOTOR_INPUT_PORT, inPins);
    GPIO_setOutputLowOnPin(MOTOR_INPUT_PORT, inPins);

    // [2] Configure the enable pin as PWM output
    if (initTemplate == MOTOR_INIT_LEFT) {
        GPIO_setAsPeripheralModuleFunctionOutputPin(MOTOR_ENABLE_PORT, MOTOR_L_PWM,
                                                    GPIO_PRIMARY_MODULE_FUNCTION);
    } else {
        GPIO_setAsPeripheralModuleFunctionOutputPin(MOTOR_ENABLE_PORT, MOTOR_R_PWM,
                                                    GPIO_PRIMARY_MODULE_FUNCTION);
    }

    // [3] Initialise the motor values
    if (initTemplate == MOTOR_INIT_LEFT) {
        motor->in1_pin = MOTOR_L_IN1;
        motor->in2_pin = MOTOR_L_IN2;
        motor->ccr = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    } else {
        motor->in1_pin = MOTOR_R_IN1;
        motor->in2_pin = MOTOR_R_IN2;
        motor->ccr = TIMER_A_CAPTURECOMPARE_REGISTER_2;
    }
    motor->state.speed = 0;
    motor->state.direction = MOTOR_DIR_FORWARD;

    // [4] Set up the Capture Compare Register (CCR) for the PWM signal generation
    const Timer_A_CompareModeConfig config = {motor->ccr, TIMER_A_CAPTURECOMPARE_INTERRUPT_DISABLE,
                                              TIMER_A_OUTPUTMODE_TOGGLE_SET, 0};
    Timer_A_initCompare(TIMER_A0_BASE, &config);
}

/*F************************************************************************************************
 * NAME: void MOTOR_HAL_setSpeed(Motor *motor, uint8_t speed);
 *
 * DESCRIPTION:
 *      Set the speed of a motor and updates the motor state
 *
 * INPUTS:
 *      PARAMETERS:
 *          Motor*      motor       Target motor
 *          uint8_t     speed       Specifies the wanted speed in percentage 0 to 100.
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          uint8_t*    motor->state.speed     Set on the current speed
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void MOTOR_HAL_setSpeed(Motor *motor, uint8_t speed) {
    // Update PWM signal
    uint16_t dutyCycle = speed * MOTOR_TIMER_PERIOD / 100;
    Timer_A_setCompareValue(TIMER_A0_BASE, motor->ccr, dutyCycle);

    // Update motor info
    motor->state.speed = speed;
}

/*F************************************************************************************************
 * NAME: void MOTOR_HAL_setDirection(Motor *motor, MotorDirection direction);
 *
 * DESCRIPTION:
 *      Stop the motor by setting its IN1 and IN2 pins to zero, if the direction is different from
 *      MOTOR_DIR_STOP set the correct pin to HIGH output.
 *      Finally, update the motor state.
 *
 * INPUTS:
 *      PARAMETERS:
 *          Motor*              motor           Target motor
 *          MotorDirection      direction       Specifies the wanted directions
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          MotorDirection*     motor->state.direction     Set on the current direction
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void MOTOR_HAL_setDirection(Motor *motor, MotorDirection direction) {
    // Stop the car by clearing the current configuration
    GPIO_setOutputLowOnPin(MOTOR_INPUT_PORT, motor->in1_pin | motor->in2_pin);

    // Set the new pins
    if (direction == MOTOR_DIR_FORWARD)
        GPIO_setOutputHighOnPin(MOTOR_INPUT_PORT, motor->in1_pin);
    else if (direction == MOTOR_DIR_REVERSE)
        GPIO_setOutputHighOnPin(MOTOR_INPUT_PORT, motor->in2_pin);

    // Update direction and speed
    motor->state.direction = direction;
    if (direction == MOTOR_DIR_STOP)
        motor->state.speed = 0;
}

/*F************************************************************************************************
 * NAME: void MOTOR_HAL_stop(Motor *motor);
 *
 * DESCRIPTION:
 *      Stops the motor by setting its IN1 and IN2 pin to zero then update the motor state.
 *
 * INPUTS:
 *      PARAMETERS:
 *          Motor*              motor           Target motor
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          uint8_t*            motor->state.speed         Set to zero
 *          MotorDirection*     motor->state.direction     Set to MOTOR_DIR_STOP
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void MOTOR_HAL_stop(Motor *motor) {
    // Stop the car by clearing the current configuration
    GPIO_setOutputLowOnPin(MOTOR_INPUT_PORT, motor->in1_pin | motor->in2_pin);
    motor->state.direction = MOTOR_DIR_STOP;
    motor->state.speed = 0;
}
