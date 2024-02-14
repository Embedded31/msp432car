/*H************************************************************************************************
 * FILENAME:        bluetooth_module.h
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
typedef enum { LOW_SEVERITY, MEDIUM_SEVERITY, HIGH_SEVERITY } eventSeverity;


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
typedef enum { OBJECT_DETECTED, BATTERY_PERCENTAGE_UPDATE, SPEED_UPDATE } eventType;


struct MessageObjectDetected {
    uint8_t  direction;
    uint16_t distance;
};

struct MessageBatteryPercentageUpdate {
    uint8_t percentage;
};

struct MessageMotorSpeedUpdate {
    uint8_t speed;
};

struct MessageMotorDirectionUpdate {
    MotorDirection direction;
};



