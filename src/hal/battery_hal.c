/*H************************************************************************************************
 * FILENAME:        battery_hal.c
 *
 * DESCRIPTION:
 *      Battery Hardware Abstraction Layer (HAL), this source file provides an abstraction over the
 *      battery status exposing hardware-independent functions.
 *
 * PUBLIC FUNCTIONS:
 *      void        BATTERY_HAL_init()
 *      uint16_t    BATTERY_HAL_getVoltage()
 *      uint8_t     BATTERY_HAL_getPercentage()
 *
 * NOTES:
 *      The battery pack outputs 8.4V at peak that cannot be handled by the MSP432P401R so a
 *      voltage divider (16kΩ,10kΩ) is introduced for rescaling the 8.4V to 3.23V;
 *      the BATTERY_DIVIDER value comes from the scaling ratio 1 + (16kΩ / 10kΩ) = 2.6.
 *
 * AUTHOR: Andrea Piccin    <andrea.piccin@studenti.unitn.it>
 *
 * START DATE: 31 Jan 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 * 04 Feb 2024  Andrea Piccin   Refactoring
 * 04 Feb 2024  Andrea Piccin   Removed unnecessary 1.0 multiplication in getPercentage()
 */

#include "../../inc/battery_hal.h"

#define BATTERY_ADC_PORT GPIO_PORT_P6   /* Battery input port                                */
#define BATTERY_ADC_PIN GPIO_PIN1       /* Battery input pin                                 */
#define BATTERY_ADC_INPUT ADC_INPUT_A14 /* ADC input of the battery (port,pin) pair          */
#define BATTERY_ADC_MEM ADC_MEM0        /* ADC memory register used for the battery readings */
#define BATTERY_MAX_VOLTAGE 8400        /* Fully charged battery voltage (mV)                */
#define BATTERY_MIN_VOLTAGE 6000        /* Discharged battery voltage (mV)                   */
#define BATTERY_DIVIDER 2.6             /* Fixed hardware-dependent value (see notes)        */

/*F************************************************************************************************
 * NAME: void BATTERY_HAL_init()
 *
 * DESCRIPTION:
 *      Initialises the ADC hardware needed for the voltage evaluation:
 *      [1] Configures the battery PIN as analog input
 *      [2] Enables and configures the ADC module
 *      [3] Configures the ADC memory register
 *      [4] Enable the conversion
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
void BATTERY_HAL_init() {
    /* [1] Input pin configuration */
    GPIO_setAsPeripheralModuleFunctionInputPin(BATTERY_ADC_PORT, BATTERY_ADC_PIN,
                                               GPIO_TERTIARY_MODULE_FUNCTION);

    /* [2] Enabling ADC hardware and configure it to use 1Mhz clock obtained from MCLK (24MHz) with
     * the addition of a 4 predivider and a 6 divider */
    ADC14_enableModule();
    ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_4, ADC_DIVIDER_6,
                     ADC_NONDIFFERENTIAL_INPUTS);

    /* [3] Configure the ADC memory register in Single Sample on the A14 */
    ADC14_configureSingleSampleMode(BATTERY_ADC_MEM, true);
    ADC14_configureConversionMemory(BATTERY_ADC_MEM, ADC_VREFPOS_AVCC_VREFNEG_VSS,
                                    BATTERY_ADC_INPUT, false);

    /* [4] Enabling manual-triggered conversion */
    ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);
    ADC14_enableConversion();
}

/*F************************************************************************************************
 * NAME: uint16_t BATTERY_HAL_getVoltage()
 *
 * DESCRIPTION:
 *      The function goes through the following steps:
 *      [1] Trigger an ADC conversion
 *      [2] Wait the completion of the conversion
 *      [3] Rescale the result in order to retrieve the real voltage that can be calculated by the
 *          formula:
 *              mV = ((res * 3.3) / 16384) * 1000 * BATTERY_DIVIDER
 *          where the 3.3 comes from the ref voltage of the conversion.
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
 *          Type:   uint16_t    unsigned 16-bit integer
 *          Value:  current voltage expressed in mV
 *
 *  NOTE:
 *      The voltage is expressed in mV in order to avoid unnecessary floating point operations.
 */
uint16_t BATTERY_HAL_getVoltage() {
    /* [1] Trigger the conversion */
    ADC14_toggleConversionTrigger();

    /* [2] Wait the conversion and read the result */
    uint64_t status;
    do {
        status = ADC14_getEnabledInterruptStatus();
    } while (status & BATTERY_ADC_MEM);
    uint16_t result = ADC14_getResult(BATTERY_ADC_MEM);

    /* [3] Rescale the result and return it */
    return (uint16_t)(((result * 3.3) / 16384) * BATTERY_DIVIDER * 1000);
}

/*F************************************************************************************************
 * NAME: uint8_t BATTERY_HAL_getPercentage()
 *
 * DESCRIPTION:
 *      Calls the BATTERY_HAL_getVoltage() and maps the result to a scale
 *      0-to-100 basing on BATTERY_MAX_VOLTAGE and BATTERY_MIN_VOLTAGE.
 *      The percentage is calculated as follows:
 *          % = (voltage - MIN_VOLTAGE) / (MAX_VOLTAGE - MIN_VOLTAGE) * 100
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
 *          Type:   uint8_t    unsigned 8-bit integer
 *          Value:  current battery percentage
 *
 *  NOTE:
 *      The conversion from uint16_t to float is necessary in order to enable
 *      the floating-point division in the percentage calculation.
 */
uint8_t BATTERY_HAL_getPercentage() {
    float voltage = BATTERY_HAL_getVoltage();
    uint8_t percentage =
        ((voltage - BATTERY_MIN_VOLTAGE) / (BATTERY_MAX_VOLTAGE - BATTERY_MIN_VOLTAGE)) * 100;
    return percentage;
}