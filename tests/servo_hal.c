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
 *      void    SERVO_HAL_triggerPositionReached()
 *
 * NOTES:
 *      The PWM timings are calculated basing on the SG90 datasheet:
 *      http://www.ee.ic.ac.uk/pcheung/teaching/DE1_EE/stores/sg90_datasheet.pdf
 *      and on experimental measurements.
 *
 * AUTHOR: Simone Rossi    <simone.rossi-2@studenti.unitn.it>
 *
 * START DATE: 19 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR              DETAIL
 */
#include <stdlib.h>

#include "servo_hal.h"

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

void SERVO_HAL_init(Servo *servo) {
    servo->ccr = 1;
    servoCallback = NULL;
    servo->state.position = 0;
}

uint16_t SERVO_HAL_positionToTicks(int8_t position) {
    uint16_t deltaTicks;
    if (position < 0) {
        deltaTicks = SERVO_MID_POS_TICKS - SERVO_MIN_POS_TICKS;
    } else {
        deltaTicks = SERVO_MAX_POS_TICKS - SERVO_MID_POS_TICKS;
    }
    return SERVO_MID_POS_TICKS + (position * (deltaTicks * 1.0) / 90);
}

void SERVO_HAL_setPosition(Servo *servo, int8_t position) {
    if (position < SERVO_MIN_POSITION)
        position = SERVO_MIN_POSITION;

    if (position > SERVO_MAX_POSITION)
        position = SERVO_MAX_POSITION;

    servo->state.position = position;
    servoCallback();
}

void SERVO_HAL_registerPositionReachedCallback(ServoCallback callback) { servoCallback = callback; }

void SERVO_HAL_triggerPositionReached() {
    if (servoCallback != NULL)
        servoCallback();
}