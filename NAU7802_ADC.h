/*
 * NAU7802_ADC.h
 *
 *  Created on: Mar 10, 2021
 *      Author: Josh Penner
 */

#ifndef NAU7802_ADC_H_
#define NAU7802_ADC_H_

#include "DSP28x_Project.h"
#include "clk.h"
#include "cpu.h"
#include "flash.h"
#include "pie.h"
#include "pll.h"
#include "wdog.h"
#include "gpio.h"
#include "adc.h"
#include "timer.h"
#include "i2c.h"

extern CLK_Handle myClk;
extern FLASH_Handle myFlash;
extern GPIO_Handle myGpio;
extern PIE_Handle myPie;
extern CPU_Handle myCpu;
extern PLL_Handle myPll;
extern WDOG_Handle myWDog;

// define constants
#define NAU7802_I2C_SLAVE_ADDR        0x2A
#define AdcMaxVolts  4.83
#define AdcMaxVal   16777215
#define SUCCESS 1
#define FAIL    0

// function declarations
bool AdcPowerOnSequence(void);
void AdcGetStatus(void);
void AdcWrite(uint16_t adcRegAddr, uint16_t dataByte);
uint16_t AdcRead(uint16_t adcRegAddr);
float AdcInitConversion(void);
void ConfigI2C(void);
void ConfigI2cClock(void);

// **define on-board I2C mode control register (MDR) states**
#define I2C_START_STOP_TRANSMIT     0x2E20
#define I2C_START_NOSTOP_TRANSMIT   0x2620
#define I2C_START_STOP_RECEIVE      0x2C20
#define I2C_START_NOSTOP_RECEIVE    0x2420

// **define ADC registers**
#define NAU7802_REG_PU_CTRL         0x00    // Power up and control
#define NAU7802_REG_CTRL1           0x01    // Control 1
#define NAU7802_REG_CTRL2           0x02    // Control 2

#define NAU7802_REG_OCAL1_B2        0x03    // channel 1 offset calibration bytes
#define NAU7802_REG_OCAL1_B1        0x04
#define NAU7802_REG_OCAL1_B0        0x05

#define NAU7802_REG_GCAL1_B3        0x06    // channel 1 gain calibration bytes
#define NAU7802_REG_GCAL1_B2        0x07
#define NAU7802_REG_GCAL1_B1        0x08
#define NAU7802_REG_GCAL1_B0        0x09

#define NAU7802_REG_OCAL2_B2        0x0A    // channel 2 offset calibration bytes
#define NAU7802_REG_OCAL2_B1        0x0B
#define NAU7802_REG_OCAL2_B0        0x0C

#define NAU7802_REG_GCAL2_B3        0x0D    // channel 2 gain calibration bytes
#define NAU7802_REG_GCAL2_B2        0x0E
#define NAU7802_REG_GCAL2_B1        0x0F
#define NAU7802_REG_GCAL2_B0        0x10

#define NAU7802_REG_I2C_CTRL        0x11    // I2C control

#define NAU7802_REG_ADCO_B2         0X12    // ADC conversion result bytes
#define NAU7802_REG_ADCO_B1         0X13
#define NAU7802_REG_ADCO_B0         0X14

#define NAU7802_REG_OTP_B1          0x15    //  OTP[15:8]
#define NAU7802_REG_OTP_B0          0x16    //  OTP[7:0]

// **define messages to send / receive**

// PU_CTRL register
#define PU_CTRL_RR      0x01    // Register reset
#define PU_CTRL_PUD     0x02    // Power up digital circuit
#define PU_CTRL_PUA     0x04    // Power up analog circuit
#define PU_CTRL_PUR     0x08    // Power up ready (read only)
#define PU_CTRL_CS      0x10    // Cycle start
#define PU_CTRL_CR      0x20    // Cycle ready (read only)
#define PU_CTRL_OSCS    0x40    // System clock source select
#define PU_AVDDS        0x80    // AVDD source select

#endif /* NAU7802_ADC_H_ */
