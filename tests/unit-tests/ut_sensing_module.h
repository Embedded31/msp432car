/*H************************************************************************************************
 * FILENAME:        ut_sensing_module.h
 *
 * DESCRIPTION:
 *      This test file contains testing functions for the sensing module, specifically we:
 *      [1] Register a function as single measurement callback
 *      [2] Set the return value of the ultrasonic measurement to a safe / unsafe distance
 *      [3] Trigger the measurement and check the validity of the parameters in the callback.
 *      The process is repeated for the double check function.
 *
 * PUBLIC FUNCTIONS:
 *      void    UT_Sensing_Module_init()
 *      void    UT_Sensing_Module_checkSingleClearance()
 *      void    UT_Sensing_Module_checkDoubleClearance()
 *
 * NOTES:
 *
 * AUTHOR: Andrea Piccin    <andrea.piccin@studenti.unitn.it>
 *
 * START DATE: 20 Feb 2024
 *
 * CHANGES:
 */
#include <stdbool.h>
#include <assert.h>

#include "../ultrasonic_hal.h"
#include "../../inc/sensing_module.h"

#define UT_SENSING_SAFE_DISTANCE    30
#define UT_SENSING_UNSAFE_DISTANCE  10

void UT_Sensing_Module_init();
void UT_Sensing_Module_checkSingleClearance();
void UT_Sensing_Module_checkDoubleClearance();