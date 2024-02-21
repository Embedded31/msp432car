#include <stdio.h>

//#include "unit_test/ut_powertrain_module.h"
#include "integration-tests/it_state_machine.h"
#include "unit-tests/ut_sensing_module.h"
#include "unit-tests/ut_powertrain_module.h"
#include "../src/app/system.c"

void main() {
    System_init();

    // Starting powertrain module test
    printf("Starting powertrain module test ...\n");
    UT_Powertrain_Module_init();
    UT_Powertrain_Module_testSpeed();
    UT_Powertrain_Module_testMovement();
    printf("Powertrain module test PASSED\n");

    // Starting sensing module test
    printf("Starting sensing module test ...\n");
    UT_Sensing_Module_init();
    UT_Sensing_Module_checkSingleClearance();
    UT_Sensing_Module_checkDoubleClearance();
    printf("Sensing module test PASSED\n");

    // Starting state machine test
    printf("Starting state machine test ...\n");
    IT_State_Machine_test();
    printf("State machine test PASSED\n");
}