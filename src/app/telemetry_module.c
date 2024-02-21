/*H************************************************************************************************
 * FILENAME:        telemetry_module.c
 *
 * DESCRIPTION:
 *      This files contains high level functions to manage bluetooth communication from the
 *      msp432car. It provides methods to send data about the systems on the car. Both specific and
 *      more general functions are provided, allowing to send any type of information
 *
 *
 * PUBLIC FUNCTIONS:
 *      void Telemetry_Module_init()
 *      void Telemetry_Module_Notify(MessageType, MessageSeverity, const char *msg)
 *      void Telemetry_Module_NotifyBatteryStatus()
 *      void Telemetry_Module_NotifyLeftMotorSpeedChange(Motor *motor, uint8_t speed)
 *      void Telemetry_Module_NotifyRightMotorSpeedChange(Motor *motor, uint8_t speed)
 *      void Telemetry_Module_NotifyLeftMotorDirChange(Motor *motor, MotorDirection direction)
 *      void Telemetry_Module_NotifyRightMotorDirChange(Motor *motor, MotorDirection direction)
 *      void Telemetry_Module_NotifyObjectDetected(uint8_t servoDirection, uint16_t objectDistance)

 * NOTES:
 *      Every message contains key value pairs separated by the SEPARATOR defined below.
 *
 * AUTHOR: Matteo Frizzera    <matteo.frizzera@studenti.unitn.it>
 *
 * START DATE: 12 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 * 20 Feb 2024  Andrea Piccin   Refactor, removed utility functions from header file; test ready
 */
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../../inc/powertrain_module.h"
#include "../../inc/telemetry_module.h"

#ifdef TEST
#include "../../tests/battery_hal.h"
#include "../../tests/bluetooth_hal.h"
#include "../../tests/motor_hal.h"
#else
#include "../../inc/battery_hal.h"
#include "../../inc/bluetooth_hal.h"
#include "../../inc/motor_hal.h"
#endif

#define SEPARATOR ',' /*   the message will contain key - value pairs separated by commas      */

#define MAX_MSG_LEN                                                                                \
    18 /*  all messages should be 32 char long (including \r\n, so 30 total                        \
           characters of actual msg). The header "type:%d,sev:%d," already takes                   \
           up 13 chars, so max len of msg is 30-13=17 chars, plus one for the                      \
           string termination char \0                                           */

char buffer[MAX_MSG_LEN]; /* buffer to write msg content to before being sent out to notify      */

/*T************************************************************************************************
 * NAME: Message_Info
 *
 * DESCRIPTION:
 *      Represent basic information shared across all types of messages: severity and type
 *
 *  Type:   struct
 *  Vars:   MessageSeverity severity         how important the message is
 *          MessageType     type             what the message is about
 */
typedef struct {
    MessageSeverity severity;
    MessageType type;
} Message_Info;

/*T************************************************************************************************
 * NAME: Message_ObjectDetected
 *
 * DESCRIPTION:
 *      Contains information about the detection of an obstacle
 *
 *  Type:   struct
 *  Vars:   MessageSeverity     severity         how important the message is
 *          MessageType         type             what the message is about
 */
typedef struct {
    Message_Info messageInfo;
    uint8_t direction;
    uint16_t distance;
} Message_ObjectDetected;

/*T************************************************************************************************
 * NAME: Message_BatteryStatusUpdate
 *
 * DESCRIPTION:
 *      Represent information about the battery
 *
 *  Type:   struct
 *  Vars:   Message_Info    messageInfo       basic info about the message
            uint16_t        voltage           battery voltage
 */
typedef struct {
    Message_Info messageInfo;
    uint16_t voltage;
} Message_BatteryStatusUpdate;

/*T************************************************************************************************
 * NAME: Message_MotorSpeedUpdate
 *
 * DESCRIPTION:
 *      Represent information about a change in motor speed
 *
 *  Type:   struct
 *  Vars:   Message_Info    messageInfo       basic info about the message
            uint8_t         speed             new speed of the motor
 */
typedef struct {
    Message_Info messageInfo;
    uint8_t speed;
} Message_MotorSpeedUpdate;

/*T************************************************************************************************
 * NAME: Message_MotorDirectionUpdate
 *
 * DESCRIPTION:
 *      Represent information about a change in motor direction
 *
 *  Type:   struct
 *  Vars:   Message_Info     messageInfo       basic info about the message
            MotorDirection   direction         new direction of the motor
 */
typedef struct {
    Message_Info messageInfo;
    MotorDirection direction;
} Message_MotorDirectionUpdate;

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
void Telemetry_Module_init() {
    BATTERY_HAL_init();
}

/*F************************************************************************************************
 * NAME: void Telemetry_Module_notify(MessageType messageType, MessageSeverity messageSeverity,
 * const char *msg)
 *
 * DESCRIPTION:
 *      Sends a message using bluetooth HAL. A message is about an Event, that must have a type and
 * severity
 *
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
 *      RETURN:
 *          None
 *
 *  NOTE:
 */
void Telemetry_Module_notify(MessageType messageType, MessageSeverity messageSeverity,
                             const char *msg) {
    BT_HAL_sendMessage("type:%d%csev:%d%c%s", messageType, SEPARATOR, messageSeverity, SEPARATOR,
                       msg);
}

/*F************************************************************************************************
 * NAME: void Telemetry_Module_sendMsgBatteryStatus(Message_BatteryStatusUpdate *batteryUpdate)
 *
 * DESCRIPTION:
 *      This functions reads data from a battery message struct, formats it and puts it into a
 *      string buffer. It then calls Telemetry_Module_Notify to actually send the data over BT
 *
 * INPUTS:
 *      PARAMETERS:
 *          Message_BatteryStatusUpdate     *batteryUpdate       message struct containing the data
 *                                                               to send
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
void Telemetry_Module_sendMsgBatteryStatus(const Message_BatteryStatusUpdate *batteryUpdate) {

    // prints to buffer the battery information
    sprintf(buffer, "v:%d", batteryUpdate->voltage);

    Telemetry_Module_notify(batteryUpdate->messageInfo.type, batteryUpdate->messageInfo.severity,
                            buffer);
}

/*F************************************************************************************************
 * NAME: void Telemetry_Module_notifyBatteryStatus()
 *
 * DESCRIPTION:
 *      This functions fills a message struct with information about the battery status, then calls
 *      a function to send the message
 *      [1] creates message struct containing battery information
 *      [2] gets battery percentage value
 *      [3] gets battery voltage value
 *      [4] sets meta information about the message, namely type and severity depending on battery
 * percentage [5] calls function to actually send the message
 *
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
 *          None
 *
 *  NOTE:
 */
void Telemetry_Module_notifyBatteryStatus() {
    // [1] creates message struct containing battery information
    Message_BatteryStatusUpdate batteryUpdate;

    // [2] gets battery percentage value
    uint8_t batteryPercentage = BATTERY_HAL_getPercentage();

    // [3] gets battery voltage value
    uint16_t batteryVoltage = BATTERY_HAL_getVoltage();
    batteryUpdate.voltage = batteryVoltage;

    // [4] sets meta information about the message, namely type and severity depending on battery
    // percentage
    if (batteryPercentage >= 80) {
        batteryUpdate.messageInfo.severity = MSG_LOW_SEVERITY;
    } else if (batteryPercentage <= 20) {
        batteryUpdate.messageInfo.severity = MSG_HIGH_SEVERITY;
    } else {
        batteryUpdate.messageInfo.severity = MSG_MEDIUM_SEVERITY;
    }

    batteryUpdate.messageInfo.type = MSG_BATTERY_STATUS_UPDATE;

    // [5] calls function to actually send the message
    Telemetry_Module_sendMsgBatteryStatus(&batteryUpdate);
}

/*F************************************************************************************************
 * NAME: Telemetry_Module_sendMsgMotorSpeedChange(Message_MotorSpeedUpdate *speedUpdate)
 *
 * DESCRIPTION:
 *      This functions reads data from a motor speed message struct, formats it and puts it into a
 *      string buffer. It then calls Telemetry_Module_Notify to actually send the data over BT
 *
 * INPUTS:
 *      PARAMETERS:
 *          Message_MotorSpeedUpdate     *speedUpdate       message struct containing the speed
 *                                                          data send
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
void Telemetry_Module_sendMsgMotorSpeedChange(const Message_MotorSpeedUpdate *speedUpdate) {
    sprintf(buffer, "sp:%d", speedUpdate->speed);

    Telemetry_Module_notify(speedUpdate->messageInfo.type, speedUpdate->messageInfo.severity,
                            buffer);
}

/*F************************************************************************************************
 * NAME: void Telemetry_Module_notifyLeftMotorSpeedChange(Motor *motor, uint8_t speed)
 *
 * DESCRIPTION:
 *      This functions creates and fills a message motor speed struct with information about the
 *      left motor speed changes, then calls the sendMsg function to send the message
 *
 *
 * INPUTS:
 *      PARAMETERS:
 *          Motor       *motor      the left motor in wich the speed change has occurred
 *          uint8_t      speed      new speed of the motor
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
void Telemetry_Module_notifyLeftMotorSpeedChange(Motor *motor, uint8_t speed) {
    Message_MotorSpeedUpdate motorSpeedUpdate;

    motorSpeedUpdate.messageInfo.severity = MSG_LOW_SEVERITY;
    motorSpeedUpdate.messageInfo.type = MSG_L_MOTOR_SPEED_UPDATE;
    motorSpeedUpdate.speed = speed;

    Telemetry_Module_sendMsgMotorSpeedChange(&motorSpeedUpdate);
}

/*F************************************************************************************************
 * NAME: void Telemetry_Module_notifyRightMotorSpeedChange(Motor *motor, uint8_t speed)
 *
 * DESCRIPTION:
 *      This functions creates and fills a message motor speed struct with information about the
 *      right motor speed changes, then calls the sendMsg function to send the message
 *
 *
 * INPUTS:
 *      PARAMETERS:
 *          Motor       *motor      the right motor in wich the speed change has occurred
 *          uint8_t      speed      new speed of the motor
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
void Telemetry_Module_notifyRightMotorSpeedChange(Motor *motor, uint8_t speed) {
    Message_MotorSpeedUpdate motorSpeedUpdate;

    motorSpeedUpdate.messageInfo.severity = MSG_LOW_SEVERITY;
    motorSpeedUpdate.messageInfo.type = MSG_R_MOTOR_SPEED_UPDATE;
    motorSpeedUpdate.speed = speed;

    Telemetry_Module_sendMsgMotorSpeedChange(&motorSpeedUpdate);
}

/*F************************************************************************************************
 * NAME: void Telemetry_Module_sendMsgMotorDirChange(Message_MotorDirectionUpdate *dirUpdate)
 *
 * DESCRIPTION:
 *      This functions reads data from a motor direction message struct, formats it and puts it into
 * a string buffer. It then calls Telemetry_Module_Notify to actually send the data over BT
 *
 * INPUTS:
 *      PARAMETERS:
 *          Message_MotorDirectionUpdate     *dirUpdate      message struct containing the
 *                                                           direction data to send
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
void Telemetry_Module_sendMsgMotorDirChange(const Message_MotorDirectionUpdate *dirUpdate) {
    sprintf(buffer, "dir:%d", dirUpdate->direction);

    Telemetry_Module_notify(dirUpdate->messageInfo.type, dirUpdate->messageInfo.severity, buffer);
}

/*F************************************************************************************************
 * NAME: void Telemetry_Module_NotifyLeftMotorDirChange(Motor *motor, MotorDirection direction)
 *
 * DESCRIPTION:
 *      This functions creates and fills a message motor direction struct with information about
 *      the left motor direction changes, then calls a function to actually send the message
 *
 * INPUTS:
 *      PARAMETERS:
 *          Motor               *motor          left motor in wich the dir change has occurred
 *          MotorDirection       direction      new direction of the motor
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
void Telemetry_Module_notifyLeftMotorDirChange(Motor *motor, MotorDirection direction) {
    Message_MotorDirectionUpdate motorDirUpdate;

    motorDirUpdate.messageInfo.severity = MSG_LOW_SEVERITY;
    motorDirUpdate.messageInfo.type = MSG_L_MOTOR_DIR_UPDATE;
    motorDirUpdate.direction = direction;

    Telemetry_Module_sendMsgMotorDirChange(&motorDirUpdate);
}

/*F************************************************************************************************
 * NAME: void Telemetry_Module_notifyRightMotorDirChange(Motor *motor, MotorDirection direction)
 *
 * DESCRIPTION:
 *      This functions creates and fills a message motor direction struct with information about
 *      the right motor direction changes, then calls a function to actually send the message
 *
 * INPUTS:
 *      PARAMETERS:
 *          Motor              *motor          right motor in wich the dir change has occurred
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
void Telemetry_Module_notifyRightMotorDirChange(Motor *motor, MotorDirection direction) {
    Message_MotorDirectionUpdate motorDirUpdate;

    motorDirUpdate.messageInfo.severity = MSG_LOW_SEVERITY;
    motorDirUpdate.messageInfo.type = MSG_R_MOTOR_DIR_UPDATE;
    motorDirUpdate.direction = direction;

    Telemetry_Module_sendMsgMotorDirChange(&motorDirUpdate);
}

/*F************************************************************************************************
 * NAME: Telemetry_Module_sendMsgObjectDetected(Message_ObjectDetected *objectDetected)
 *
 * DESCRIPTION:
 *      This functions reads data from a object detected message struct, formats it and puts it into
 * a string buffer. It then calls Telemetry_Module_Notify to actually send the data over BT
 *
 * INPUTS:
 *      PARAMETERS:
 *          Message_ObjectDetected     *objectDetected      message struct containing the
 *                                                          direction and distance of the object
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
void Telemetry_Module_sendMsgObjectDetected(const Message_ObjectDetected *objectDetected) {
    sprintf(buffer, "dst:%d", objectDetected->distance);

    Telemetry_Module_notify(objectDetected->messageInfo.type, objectDetected->messageInfo.severity,
                            buffer);
}

/*F************************************************************************************************
 * NAME: void Telemetry_Module_notifyObjectDetected(uint8_t servoDirection, uint16_t objectDistance)
 *
 * DESCRIPTION:
 *      This functions creates and fills a message object detected struct with information about
 *      the detected object, then calls a function to actually send the information
 *
 * INPUTS:
 *      PARAMETERS:
 *           uint8_t     servoDirection    direction of the object being detected
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
 *      this function is called in an interrupt when a US measurement is ready, when it
 *      detected an object close enough
 */
void Telemetry_Module_notifyObjectDetected(uint8_t servoDirection, uint16_t objectDistance) {
    Message_ObjectDetected objectDetected;
    objectDetected.messageInfo.severity = MSG_HIGH_SEVERITY;
    objectDetected.messageInfo.type = MSG_OBJECT_DETECTED;
    objectDetected.distance = objectDistance;
    objectDetected.direction = servoDirection;

    Telemetry_Module_sendMsgObjectDetected(&objectDetected);
}

/*F************************************************************************************************
 * NAME: Telemetry_Module_notifyModeSwitch(bool controlled)
 *
 * DESCRIPTION:
 *      This functions notifies that the robot switched from manual remote controlled mode
 *      to automatic driving mode, or viceversa. Remote controlled mode is called manual
 *      and automatic driving mode is called auto
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
void Telemetry_Module_notifyModeSwitch(bool controlled) {
    sprintf(buffer, "mode:%s", controlled ? "0" : "1");
    Telemetry_Module_notify(MSG_MODE_SWITCH, MSG_HIGH_SEVERITY, buffer);
}
