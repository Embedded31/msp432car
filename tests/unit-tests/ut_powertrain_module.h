/*H************************************************************************************************
 * FILENAME:        ut_powertrain_module.h
 *
 * DESCRIPTION:
 *      This header file provides the test functions to verify the correct behavior of the
 *      powertrain module.
 *
 * PUBLIC FUNCTIONS:
 *      void    UT_Powertrain_Module_init();
 *      void    UT_Powertrain_Module_testSpeed
 *      void    UT_Powertrain_Module_testMovement()
 *
 * NOTES:
 *
 * AUTHOR: Simone Rossi    <simone.rossi-2@studenti.unitn.it>
 *
 * START DATE: 20 Feb 2024
 *
 * CHANGES:
 * DATE         AUTHOR          DETAIL
 */
#ifndef POWERTRAIN_MODULE_H_
#define POWERTRAIN_MODULE_H_

void UT_Powertrain_Module_init();
void UT_Powertrain_Module_testSpeed();
void UT_Powertrain_Module_testMovement();

#endif // POWERTRAIN_MODULE_H_