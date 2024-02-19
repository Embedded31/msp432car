/*H************************************************************************************************
 * FILENAME:        telemetry_module.h
 *
 * DESCRIPTION:
 *      This header files contains the definitions of high level functions to send and receive messages
 *      using bluetooth. These messages contain information about the status of the msp432car
 *      Messages could be about speed and direction changes, accelleration, sudden stops, low battery
 *      status, object detection, ...
 *
 * PUBLIC FUNCTIONS:
 *      
 *
 * NOTES:
 *      None
 *
 * AUTHOR: Matteo Frizzera    <matteo.frizzera@studenti.unitn.it>
 *
 * START DATE: 12 Feb 2024
 *
 * CHANGES:
 *      none
 * 
 */

#include <stdint.h>
#include "../motor_hal.h"

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
 *              MSG_HIGH_SEVERITY       most important message, relating to crucial and key parts of the system
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
 *              MSG_BATTERY_STATUS_UPDATE           updates battery status, for example when battery is decreasing
 *              MSG_SPEED_UPDATE                    speed of motor has changed
 *              MSG_MOTOR_DIR_UPDATE                direction of motor has changed
 */
typedef enum { MSG_OBJECT_DETECTED, MSG_BATTERY_STATUS_UPDATE, MSG_MOTOR_SPEED_UPDATE, MSG_MOTOR_DIR_UPDATE } MessageType;

/*S************************************************************************************************
 * NAME: Message_InfoStruct
 *
 * DESCRIPTION:
 *      Represent basic information shared across all types of messages: severity and type
 *
 *  Type:   struct
 *  Vars:   MessageSeverity severity         how important the message is
 *          MessageType     type             what the message is about
 */
typedef struct Message_InfoStruct {
    MessageSeverity severity;
    MessageType type;
};
typedef struct Message_InfoStruct Message_Info;

/*S************************************************************************************************
 * NAME: Message_InfoStruct
 *
 * DESCRIPTION:
 *      Represent basic information shared across all types of messages: severity and type
 *
 *  Type:   struct
 *  Vars:   MessageSeverity     severity         how important the message is
 *          MessageType         type             what the message is about
 */
typedef struct Message_ObjectDetectedStruct {
    Message_Info messageInfo;
    uint8_t  direction;
    uint16_t distance;
};
typedef struct Message_ObjectDetectedStruct Message_ObjectDetected;


/*S************************************************************************************************
 * NAME: Message_BatteryStatusUpdateStruct
 *
 * DESCRIPTION:
 *      Represent information about the battery
 *
 *  Type:   struct
 *  Vars:   Message_Info    messageInfo       basic info about the message
            uint8_t         percentage        battery percentage
            uint16_t        voltage           battery voltage
 */
typedef struct Message_BatteryStatusUpdateStruct {
    Message_Info messageInfo;
    uint8_t percentage;
    uint16_t voltage;
};
typedef struct Message_BatteryStatusUpdateStruct Message_BatteryStatusUpdate;


/*S************************************************************************************************
 * NAME: Message_MotorSpeedUpdateStruct
 *
 * DESCRIPTION:
 *      Represent information about a change in motor speed
 *
 *  Type:   struct
 *  Vars:   Message_Info    messageInfo       basic info about the message
            uint8_t         speed             new speed of the motor
 */
typedef struct Message_MotorSpeedUpdateStruct {
    Message_Info messageInfo;
    uint8_t speed;
};
typedef struct Message_MotorSpeedUpdateStruct Message_MotorSpeedUpdate;


/*S************************************************************************************************
 * NAME: Message_MotorDirectionUpdateStruct
 *
 * DESCRIPTION:
 *      Represent information about a change in motor direction
 *
 *  Type:   struct
 *  Vars:   Message_Info     messageInfo       basic info about the message
            MotorDirection   direction         new direction of the motor
 */
typedef struct Message_MotorDirectionUpdateStruct {
    Message_Info messageInfo;
    MotorDirection direction;
};
typedef struct Message_MotorDirectionUpdateStruct Message_MotorDirectionUpdate;


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
 * NAME: void Telemetry_Module_Notify(MessageType, MessageSeverity, const char *msg)
 *
 * DESCRIPTION:
 *      This general function allows to send information about any event. The total message length
 *      must be less than 32 characters total. 
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
 *
 *  NOTE:
 */
void Telemetry_Module_Notify(MessageType messageType, MessageSeverity messageSeverity, const char *msg);


/*F************************************************************************************************
 * NAME: void Telemetry_Module_SendMsgBatteryStatus(Message_BatteryStatusUpdate *batteryUpdate)
 *
 * DESCRIPTION:
 *      Auxiliary function to Telemetry_Module_NotifyBatteryStatus
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
void Telemetry_Module_SendMsgBatteryStatus(Message_BatteryStatusUpdate *batteryUpdate);


/*F************************************************************************************************
 * NAME: void Telemetry_Module_NotifyBatteryStatus()
 *
 * DESCRIPTION:
 *      This functions send a bluetooth message with information on battery status, that is
 *      battery percentage and voltage
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
 *
 *  NOTE:
 */
void Telemetry_Module_NotifyBatteryStatus();


/*F************************************************************************************************
 * NAME: Telemetry_Module_SendMsgMotorSpeedChange(Message_MotorSpeedUpdate *speedUpdate)
 *
 * DESCRIPTION:
 *      Auxiliary function to Telemetry_Module_NotifyMotorSpeedChange
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
void Telemetry_Module_SendMsgMotorSpeedChange(Message_MotorSpeedUpdate *speedUpdate);


/*F************************************************************************************************
 * NAME: void Telemetry_Module_NotifyMotorSpeedChange(Motor *motor, uint8_t speed)
 *
 * DESCRIPTION:
 *      This functions send a bluetooth message about a motor speed change
 *      
 *
 * INPUTS:
 *      PARAMETERS:
 *          Motor       *motor      the motor in wich the speed change has occurred
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
void Telemetry_Module_NotifyMotorSpeedChange(Motor *motor, uint8_t speed);


/*F************************************************************************************************
 * NAME: void Telemetry_Module_SendMsgMotorDirChange(Message_MotorDirectionUpdate *dirUpdate)
 *
 * DESCRIPTION:
 *      Auxiliary function to Telemetry_Module_NotifyMotorDirChange
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
void Telemetry_Module_SendMsgMotorDirChange(Message_MotorDirectionUpdate *dirUpdate);


/*F************************************************************************************************
 * NAME: void Telemetry_Module_NotifyMotorDirChange(Motor *motor, MotorDirection direction)
 *
 * DESCRIPTION:
 *      This functions send a bluetooth message about a motor direction change
 *      
 * INPUTS:
 *      PARAMETERS:
*           Motor              *motor          the motor in wich the direction change has occurred
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
void Telemetry_Module_NotifyMotorDirChange(Motor *motor, MotorDirection direction);


/*F************************************************************************************************
 * NAME: Telemetry_Module_SendMsgObjectDetected(Message_ObjectDetected *objectDetected)
 *
 * DESCRIPTION:
 *      Auxiliary function to Telemetry_Module_NotifyObjectDetected
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
void Telemetry_Module_SendMsgObjectDetected(Message_ObjectDetected *objectDetected);


/*F************************************************************************************************
 * NAME: void Telemetry_Module_NotifyObjectDetected(uint8_t servoDirection, uint16_t objectDistance)
 *
 * DESCRIPTION:
 *      This functions send a bluetooth message with information on the object being detected
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
void Telemetry_Module_NotifyObjectDetected(uint8_t servoDirection, uint16_t objectDistance);


#endif // TELEMETRY_MODULE_H

