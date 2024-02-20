/*H************************************************************************************************
 * FILENAME:        ut_sensing_module.c
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

static bool expectedDir1Result, expectedDir2Result;

void UT_Sensing_Module_onSingleMeasurement(bool isFree){
    assert(isFree == expectedDir1Result);
}

void UT_Sensing_Module_onDoubleMeasurement(bool isFreeDir1, bool isFreeDir2){
    assert(isFreeDir1 == expectedDir1Result);
    assert(isFreeDir2 == expectedDir2Result);
}

void UT_Sensing_Module_init(){
    Sensing_Module_init();
    Sensing_Module_registerSingleMeasurementReadyCallback(UT_Sensing_Module_onSingleMeasurement);
    Sensing_Module_registerDoubleMeasurementReadyCallback(UT_Sensing_Module_onDoubleMeasurement);
}

void UT_Sensing_Module_checkSingleClearance(){
    // safe distance test
    US_HAL_setDefaultDistance(UT_SENSING_SAFE_DISTANCE);
    expectedDir1Result = true;
    Sensing_Module_checkSingleClearance(30);

    // unsafe distance test
    US_HAL_setDefaultDistance(UT_SENSING_UNSAFE_DISTANCE);
    expectedDir1Result = false;
    Sensing_Module_checkSingleClearance(30);
}

void UT_Sensing_Module_checkDoubleClearance(){
    // safe distance test
    US_HAL_setDefaultDistance(UT_SENSING_SAFE_DISTANCE);
    expectedDir1Result = true;
    expectedDir2Result = true;
    Sensing_Module_checkSingleClearance(-20, 30);

    // unsafe distance test
    US_HAL_setDefaultDistance(UT_SENSING_UNSAFE_DISTANCE);
    expectedDir1Result = false;
    expectedDir2Result = false;
    Sensing_Module_checkSingleClearance(-20, 30);
}