/*H************************************************************************************************
 * FILENAME:        telemetry_module.h
 *
 * DESCRIPTION:
 *      This header files contains the definitions of high level functions to send and receive messages
 *      using bluetooth. These messages contain information about the status of the msp432car
 *      Messages could be about speed and direction changes, accelleration, sudden stops, low battery status, object detection, ...
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
 *      Values: LOW_SEVERITY        least important type of message
 *              MEDIUM_SEVERITY     message of medium or average importance
 *              HIGH_SEVERITY       most important message, relating to crucial and key parts of the system
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
 *      Values: OBJECT_DETECTED                 detected an object in the path of the msp432car  
 *              BATTERY_STATUS_UPDATE           updates battery status, for example when battery is decreasing
 *              SPEED_UPDATE                    speed of motor has changed
 */
typedef enum { MSG_OBJECT_DETECTED, MSG_BATTERY_STATUS_UPDATE, MSG_MOTOR_SPEED_UPDATE, MSG_MOTOR_DIR_UPDATE } MessageType;


typedef struct Message_InfoStruct {
    MessageSeverity severity;
    MessageType type;
};
typedef struct Message_InfoStruct Message_Info;


typedef struct Message_ObjectDetectedStruct {
    Message_Info messageInfo;
    uint8_t  direction;
    uint16_t distance;
};
typedef struct Message_ObjectDetectedStruct Message_ObjectDetected;


typedef struct Message_BatteryStatusUpdateStruct {
    Message_Info messageInfo;
    uint8_t percentage;
    uint16_t voltage;
};
typedef struct Message_BatteryStatusUpdateStruct Message_BatteryStatusUpdate;


typedef struct Message_MotorSpeedUpdateStruct {
    Message_Info messageInfo;
    uint8_t speed;
};
typedef struct Message_MotorSpeedUpdateStruct Message_MotorSpeedUpdate;


typedef struct Message_MotorDirectionUpdateStruct {
    Message_Info messageInfo;
    MotorDirection direction;
};
typedef struct Message_MotorDirectionUpdateStruct Message_MotorDirectionUpdate;


#endif // TELEMETRY_MODULE_H

