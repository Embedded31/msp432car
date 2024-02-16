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
 * NAME: eventSeverity
 *
 * DESCRIPTION:
 *      Represent the severity (ie importance) of the event being sent
 *
 * SPECIFICATIONS:
 *      Type:   enum
 *      Values: LOW_SEVERITY        least important type of event
 *              MEDIUM_SEVERITY     event of medium or average importance
 *              HIGH_SEVERITY       most important event, relating to crucial and key parts of the system
 */
typedef enum { LOW_SEVERITY, MEDIUM_SEVERITY, HIGH_SEVERITY } EventSeverity;


/*T************************************************************************************************
 * NAME: eventSeverity
 *
 * DESCRIPTION:
 *      Represent the severity (ie importance) of the event being sent
 *
 * SPECIFICATIONS:
 *      Type:   enum
 *      Values: OBJECT_DETECTED                 detected an object in the path of the msp432car  
 *              BATTERY_PERCENTAGE_UPDATE       updates battery status, for example when battery is decreasing
 *              SPEED_UPDATE                    speed of motor has changed
 */
typedef enum { OBJECT_DETECTED, BATTERY_PERCENTAGE_UPDATE, SPEED_UPDATE } EventType;


typedef struct Message_InfoStruct {
    EventSeverity severity;
    EventType type;
};
typedef struct Message_InfoStruct Message_Info;


typedef struct Message_ObjectDetectedStruct {
    Message_Info messageInfo;
    uint8_t  direction;
    uint16_t distance;
};
typedef struct Message_ObjectDetectedStruct Message_ObjectDetected;


typedef struct Message_BatteryPercentageUpdateStruct {
    Message_Info messageInfo;
    uint8_t percentage;
};
typedef struct Message_BatteryPercentageUpdateStruct Message_BatteryPercentageUpdate;


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

