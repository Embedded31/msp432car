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
 *      void    MOTOR_HAL_registerSpeedChangeCallback(Motor* motor, MotorSpeedCallback callback)
 *      void    MOTOR_HAL_registerDirectionChangeCallback(Motor* motor, MotorDirCallback callback)
 *
 * NOTES:
 *
 * AUTHOR: Simone Rossi    <simone.rossi-2@studenti.unitn.it>
 *
 * START DATE: 19 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 */
#include <stdio.h>

#include "motor_hal.h"

#define MOTOR_TIMER_PERIOD 5000         /* Max value of the counter           */
#define MOTOR_ENABLE_PORT 1             /* Port for the PWM signals           */
#define MOTOR_R_PWM 5                   /* Pin for the right motor PWM signal */
#define MOTOR_L_PWM 6                   /* Pin for the left motor PWM signal  */
#define MOTOR_INPUT_PORT 1              /* Port of the direction pins         */
#define MOTOR_R_IN1 1                   /* Right motor's direction pin 1      */
#define MOTOR_R_IN2 2                   /* Right motor's direction pin 2      */
#define MOTOR_L_IN1 3                   /* Left motor's direction pin 1       */
#define MOTOR_L_IN2 4                   /* Left motor's direction pin 2       */

void MOTOR_HAL_init() {
}

void MOTOR_HAL_motorInit(Motor *motor, MotorInitTemplate initTemplate) {
    if (initTemplate == MOTOR_INIT_LEFT) {
        motor->in1_pin = MOTOR_L_IN1;
        motor->in2_pin = MOTOR_L_IN2;
        motor->ccr = 1;
    } else {
        motor->in1_pin = MOTOR_R_IN1;
        motor->in2_pin = MOTOR_R_IN2;
        motor->ccr = 2;
    }
    motor->state.speed = 0;
    motor->state.direction = MOTOR_DIR_STOP;
    motor->speedCallback = NULL;
    motor->dirCallback = NULL;
}

void MOTOR_HAL_setSpeed(Motor *motor, uint8_t speed) {
    if (motor->state.speed == speed)
        return;

    // Update motor info
    motor->state.speed = speed;

    // Notify the state change
    if (motor->speedCallback != NULL)
        motor->speedCallback(motor, motor->state.speed);
}

void MOTOR_HAL_setDirection(Motor *motor, MotorDirection direction) {
    if (motor->state.direction == direction)
        return;

    // Update direction and speed
    motor->state.direction = direction;
    if (direction == MOTOR_DIR_STOP)
        motor->state.speed = 0;

    // Notify the state change
    if (motor->dirCallback != NULL)
        motor->dirCallback(motor, motor->state.direction);
}

void MOTOR_HAL_stop(Motor *motor) { MOTOR_HAL_setDirection(motor, MOTOR_DIR_STOP); }

void MOTOR_HAL_registerSpeedChangeCallback(Motor *motor, MotorSpeedCallback callback) {
    motor->speedCallback = callback;
}

void MOTOR_HAL_registerDirectionChangeCallback(Motor *motor, MotorDirCallback callback) {
    motor->dirCallback = callback;
}