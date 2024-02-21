/*H************************************************************************************************
 * FILENAME:        telemetry_module.h
 *
 * DESCRIPTION:
 *      This header files contains the definitions of high level functions to send and receive
 *      messages using bluetooth. These messages contain information about the status of the
 *      msp432car
 *      Messages could be about speed and direction changes, acceleration, sudden stops, low
 *      battery status, object detection, etc.
 *
 * PUBLIC FUNCTIONS:
 *      void Telemetry_Module_init()
 *      void Telemetry_Module_notify(MessageType, MessageSeverity, const char *msg)
 *      void Telemetry_Module_notifyBatteryStatus()
 *      void Telemetry_Module_notifyLeftMotorSpeedChange(Motor *motor, uint8_t speed)
 *      void Telemetry_Module_notifyRightMotorSpeedChange(Motor *motor, uint8_t speed)
 *      void Telemetry_Module_notifyLeftMotorDirChange(Motor *motor, MotorDirection direction)
 *      void Telemetry_Module_notifyRightMotorDirChange(Motor *motor, MotorDirection direction)
 *      void Telemetry_Module_notifyObjectDetected(uint8_t servoDirection, uint16_t objectDistance)
 *
 * NOTES:
 *
 * AUTHOR: Matteo Frizzera    <matteo.frizzera@studenti.unitn.it>
 *
 * START DATE: 12 Feb 2024
 *
 * CHANGES:
 * DATE            AUTHOR              DETAIL
 * 20 Feb 2024     Matteo Frizzera     Add function to notify mode switch
 * 20 Feb 2024     Andrea Piccin       Refactor, removed utility functions from header
 *                                     Fixed structures declaration
 */
#include <stdbool.h>
#include <stdint.h>

#ifdef TEST
#include "../tests/motor_hal.h"
#else
#include "motor_hal.h"
#endif

#ifndef TELEMETRY_MODULE_H
#define TELEMETRY_MODULE_H

/*T************************************************************************************************
 * NAME: MessageSeverity
 *
 * DESCRIPTION:
 *      Represent the severity (ie importance) of the Message being sent
 *
 * SPECIFICATIONS:
 *      Type:   enum
 *      Values: MSG_LOW_SEVERITY        least important type of message
 *              MSG_MEDIUM_SEVERITY     message of medium or average importance
 *              MSG_HIGH_SEVERITY       most important message, relating to crucial and key parts of
 * the system
 */
typedef enum { MSG_LOW_SEVERITY, MSG_MEDIUM_SEVERITY, MSG_HIGH_SEVERITY } MessageSeverity;

/*T************************************************************************************************
 * NAME: MessageType
 *
 * DESCRIPTION:
 *      Represent the type of Message that is being notified
 *
 * SPECIFICATIONS:
 *      Type:   enum
 *      Values: MSG_OBJECT_DETECTED                 detected an object in the path of the msp432car
 *              MSG_BATTERY_STATUS_UPDATE           updates battery status
 *              MSG_SPEED_UPDATE                    speed of motor has changed
 *              MSG_MOTOR_DIR_UPDATE                direction of motor has changed
 *              MSG_MODE_SWITCH                     control mode becomes manual or auto
 *
 */
typedef enum {
    MSG_OBJECT_DETECTED,
    MSG_BATTERY_STATUS_UPDATE,
    MSG_L_MOTOR_SPEED_UPDATE,
    MSG_R_MOTOR_SPEED_UPDATE,
    MSG_L_MOTOR_DIR_UPDATE,
    MSG_R_MOTOR_DIR_UPDATE,
    MSG_MODE_SWITCH,
} MessageType;

/*F************************************************************************************************
 * NAME: Telemetry_Module_init()
 *
 * DESCRIPTION:
 *      Initializes components, registers callback regarding speed and direction changes for both
 *      left and right motors
 *
 * INPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          Powertrain      powertrain    struct containing both motors of the msp432car
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *
 *  NOTE:
 */
void Telemetry_Module_init();

/*F************************************************************************************************
 * NAME: void Telemetry_Module_notify(MessageType, MessageSeverity, const char *msg)
 *
 * DESCRIPTION:
 *      This general function allows to send information about any event. The total message length
 *      must be less than 32 characters total.
 *
 * INPUTS:
 *      PARAMETERS:
 *          MessageType       messageType       type of message
 *          MessageSeverity   messageSeverity   severity of message
 *          const char *      msg               content of message, could be any string
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
void Telemetry_Module_notify(MessageType messageType, MessageSeverity messageSeverity,
                             const char *msg);

/*F************************************************************************************************
 * NAME: void Telemetry_Module_notifyBatteryStatus()
 *
 * DESCRIPTION:
 *      This functions send a bluetooth message with information on battery status, that is
 *      battery percentage and voltage
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
void Telemetry_Module_notifyBatteryStatus();

/*F************************************************************************************************
 * NAME: void Telemetry_Module_notifyMotorSpeedChange(Motor *motor, uint8_t speed)
 *
 * DESCRIPTION:
 *      This functions send a bluetooth message about a left motor speed change
 *
 *
 * INPUTS:
 *      PARAMETERS:
 *          Motor       *motor      the motor in which the speed change has occurred
 *          uint8_t      speed      new speed of the motor
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *      RETURN:
 *          None
 *
 *  NOTE:
 */
void Telemetry_Module_notifyLeftMotorSpeedChange(Motor *motor, uint8_t speed);

/*F************************************************************************************************
 * NAME: void Telemetry_Module_notifyRightMotorSpeedChange(Motor *motor, uint8_t speed)
 *
 * DESCRIPTION:
 *      This functions send a bluetooth message about a right motor speed change
 *
 *
 * INPUTS:
 *      PARAMETERS:
 *          Motor       *motor      the motor in which the speed change has occurred
 *          uint8_t      speed      new speed of the motor
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *      RETURN:
 *          None
 *
 *  NOTE:
 */
void Telemetry_Module_notifyRightMotorSpeedChange(Motor *motor, uint8_t speed);

/*F************************************************************************************************
 * NAME: void Telemetry_Module_notifyLeftMotorDirChange(Motor *motor, MotorDirection direction)
 *
 * DESCRIPTION:
 *      This functions send a bluetooth message about a left motor direction change
 *
 * INPUTS:
 *      PARAMETERS:
 *          Motor*              motor          the motor which direction has changed
 *          MotorDirection      direction      new direction of the motor
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *      RETURN:
 *          None
 *
 *  NOTE:
 */
void Telemetry_Module_notifyLeftMotorDirChange(Motor *motor, MotorDirection direction);

/*F************************************************************************************************
 * NAME: void Telemetry_Module_notifyRightMotorDirChange(Motor *motor, MotorDirection direction)
 *
 * DESCRIPTION:
 *      This functions send a bluetooth message about a right motor direction change
 *
 * INPUTS:
 *      PARAMETERS:
 *          Motor*              motor          the motor which direction has changed
 *          MotorDirection      direction      new direction of the motor
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *      RETURN:
 *          None
 *
 *  NOTE:
 */
void Telemetry_Module_notifyRightMotorDirChange(Motor *motor, MotorDirection direction);

/*F************************************************************************************************
 * NAME: void Telemetry_Module_notifyObjectDetected(uint8_t servoDirection, uint16_t objectDistance)
 *
 * DESCRIPTION:
 *      This functions send a bluetooth message with information on the object being detected
 *
 * INPUTS:
 *      PARAMETERS:
 *          uint8_t     servoDirection    direction of the object being detected
 *          uint16_t    objectDistance    distance of the obj from the US sensor (in centimeters)
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *      RETURN:
 *          None
 *
 *  NOTE:
 *      this function is called in an interrupt when a US measurement is ready.
 */
void Telemetry_Module_notifyObjectDetected(uint8_t servoDirection, uint16_t objectDistance);

/*F************************************************************************************************
 * NAME: Telemetry_Module_notifyModeSwitch(bool controlled)
 *
 * DESCRIPTION:
 *      This functions send a bluetooth message with information about the driving mode the robot
 *      is currently in.
 *
 * INPUTS:
 *      PARAMETERS:
 *          bool    controlled     Whether the robot is being remotely controlled
 *      GLOBALS:
 *          None
 *
 *  OUTPUTS:
 *      PARAMETERS:
 *          None
 *      GLOBALS:
 *          None
 *      RETURN:
 *          None
 *
 *  NOTE:
 */
void Telemetry_Module_notifyModeSwitch(bool controlled);

#endif // TELEMETRY_MODULE_H