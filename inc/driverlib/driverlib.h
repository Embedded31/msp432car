/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
#ifndef __DRIVERLIB__H_
#define __DRIVERLIB__H_

/* Common Modules */
#include "adc14.h"
// #include <ti/devices/msp432p4xx/driverlib/aes256.h>
// #include <ti/devices/msp432p4xx/driverlib/comp_e.h>
#include "cpu.h"
// #include <ti/devices/msp432p4xx/driverlib/crc32.h>
#include "cs.h"
// #include <ti/devices/msp432p4xx/driverlib/dma.h>
#include "eusci.h"
// #include <ti/devices/msp432p4xx/driverlib/fpu.h>
#include "gpio.h"
// #include <ti/devices/msp432p4xx/driverlib/i2c.h>
#include "interrupt.h"
// #include <ti/devices/msp432p4xx/driverlib/mpu.h>
#include "pcm.h"
// #include <ti/devices/msp432p4xx/driverlib/pmap.h>
// #include <ti/devices/msp432p4xx/driverlib/pss.h>
// #include <ti/devices/msp432p4xx/driverlib/ref_a.h>
// #include <ti/devices/msp432p4xx/driverlib/reset.h>
// #include <ti/devices/msp432p4xx/driverlib/rom.h>
// #include <ti/devices/msp432p4xx/driverlib/rom_map.h>
#include "rtc_c.h"
// #include <ti/devices/msp432p4xx/driverlib/spi.h>
// #include <ti/devices/msp432p4xx/driverlib/systick.h>
#include "timer32.h"
#include "timer_a.h"
#include "uart.h"
#include "wdt_a.h"

/* Device specific modules */
#include "flash.h"
#include "sysctl.h"

/* Offset Definitions */
#define HWREG8(x) (*((volatile uint8_t *)(x)))
#define HWREG16(x) (*((volatile uint16_t *)(x)))
#define HWREG32(x) (*((volatile uint32_t *)(x)))
#define HWREG(x) (HWREG16(x))
#define HWREG8_L(x) (*((volatile uint8_t *)((uint8_t *)&x)))
#define HWREG8_H(x) (*((volatile uint8_t *)(((uint8_t *)&x) + 1)))
#define HWREG16_L(x) (*((volatile uint16_t *)((uint16_t *)&x)))
#define HWREG16_H(x) (*((volatile uint16_t *)(((uint16_t *)&x) + 1)))

#endif
