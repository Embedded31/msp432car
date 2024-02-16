/*H************************************************************************************************
 * FILENAME:        telemetry_module.c
 *
 * DESCRIPTION:
 *      This files contains high level functions to manage bluetooth communication from the msp432car.
 *      It provides methods to send data about the systems on the car eg. send speed and battery information
 *      Both specific and more general functions are provided, allowing to send any type of information
 *      
 *
 * PUBLIC FUNCTIONS:
 *      void        Telemetry_Module_init()
 *      void        Telemetry_Module_NotifyBatteryPercentage()
 *      void        Telemetry_Module_NotifyBatteryPercentage(Message_BatteryPercentageUpdate batteryUpdate)
 * 
 *
 * NOTES:
 *      Every message contains key value pairs, for exaple speed: 18 or battery: 87
 *      These fields are separated by the SEPARATOR defined below
 *
 * AUTHOR: Matteo Frizzera    <matteo.frizzera@studenti.unitn.it>
 *
 * START DATE: 12 Feb 2024
 *
 * CHANGES:
 *      none
 * 
 */


#include "../../inc/telemetry_module.h"
#include "../hal/bluetooth_hal.c"
#include "../hal/battery_hal.c"
#include "../hal/motor_hal.c"

#define SEPARATOR "," // the message will contain key - value pairs separated by commas


/*F************************************************************************************************
 * NAME: Telemetry_Module_init()
 *
 * DESCRIPTION:
 *      Initializes variables and components
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
void Telemetry_Module_init(){

    // i think inits should all be done in main.c at startup, so this and other inits may need to be removed
    BATTERY_HAL_init();

}


/*F************************************************************************************************
 * NAME: void Telemetry_Module_Notify(EventType eventType, EventSeverity eventSeverity, const char *msg)
 *
 * DESCRIPTION:
 *      Sends a message over bluetooth. A message is about an Event, that must have a type and severity
 *      
 *
 * INPUTS:
 *      PARAMETERS:
 *          EventType       eventType       type of event
 *          EventSeverity   eventSeverity   severity of event
 *          const char *    msg             content of message, could be any string
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
void Telemetry_Module_Notify(EventType eventType, EventSeverity eventSeverity, const char *msg){
    BT_HAL_sendMessage("type: %d" SEPARATOR "sev: %d"  SEPARATOR "%s", eventType, eventSeverity, msg);           
}


/*F************************************************************************************************
 * NAME: void Telemetry_Module_NotifyBatteryPercentage()
 *
 * DESCRIPTION:
 *      This functions fills a message struct with information about the battery status, then calls a function to send
 *      the message
 *      [1] creates message struct containing battery information
 *      [2] gets battery percentage value
 *      [3] sets meta information about the message, namely type and severity depending on battery percentage
 *      [4] calls function to actually send data
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
void Telemetry_Module_NotifyBatteryPercentage(){
    // [1] creates message struct containing battery information
    Message_BatteryPercentageUpdate batteryUpdate;

    // [2] gets battery percentage value
    uint8_t batteryPercetage = BATTERY_HAL_getPercentage();
    batteryUpdate.percentage = batteryPercetage;

    

    // [3] sets meta information about the message, namely type and severity depending on battery percentage
    if(batteryPercetage >= 80){
        batteryUpdate.messageInfo.severity = LOW_SEVERITY;
    }
    else if(batteryPercetage <= 20){
        batteryUpdate.messageInfo.severity = HIGH_SEVERITY;
    }
    else{
        batteryUpdate.messageInfo.severity = MEDIUM_SEVERITY;
    }

    batteryUpdate.messageInfo.type = BATTERY_PERCENTAGE_UPDATE;

    // [4] calls function to actually send data
    Telemetry_Module_NotifyBatteryPercentage(batteryUpdate);
   
}


/*F************************************************************************************************
 * NAME: void Telemetry_Module_NotifyBatteryPercentage(Message_BatteryPercentageUpdate batteryUpdate)
 *
 * DESCRIPTION:
 *      This functions reads data from a battery message struct and sends it over bluetooth
 *
 * INPUTS:
 *      PARAMETERS:
 *          Message_BatteryPercentageUpdate     batteryUpdate       message struct containing the data to send
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
void Telemetry_Module_NotifyBatteryPercentage(Message_BatteryPercentageUpdate batteryUpdate){
    // field of the message containing battery percentage
    char batteryUpdateField[13];

    sprintf(batteryUpdateField, "battery: %d", batteryUpdate.percentage);

    Telemetry_Module_Notify(
        batteryUpdate.messageInfo.type,
        batteryUpdate.messageInfo.severity,
        batteryUpdateField
    )
}

