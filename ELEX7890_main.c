//#############################################################################
//
//  File:   F28027_LCD_I2C_main.c
//
//  Title:  F28027 connect to LCD via I2C main file
//
//  Created on: 2021Äê03ÔÂ17ÈÕ
//
//  Author: Jie Kang
//
//#############################################################################

#define _FLASH

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
#include <math.h>
#include <NAU7802_ADC.h>
#include <F28027_I2C_LCD.h>

// Create handles for drivers
CLK_Handle myClk;
FLASH_Handle myFlash;
GPIO_Handle myGpio;
PIE_Handle myPie;
CPU_Handle myCpu;
PLL_Handle myPll;
WDOG_Handle myWDog;

void Setup_handles();
void Init_system();

void main(void)
{
    Setup_handles();
    Init_system();
    init_I2C(0x27,20,4);
    init_LCD();
    backlight();
    AdcPowerOnSequence();
    setCursor (0,0);  // go to the top left corner
    writeStr("ELEX7890 NIBSMD");
    for(;;)
    {
        int i;
        float adcConvResult[100];
        float aveResult, finResult;
        if(GPIO_getData(myGpio, GPIO_Number_12) == 1)
        {
            for(i = 0; i < 100; i++)
            {
                AdcGetStatus();
                adcConvResult[i] = AdcInitConversion();
            }
            aveResult = 0;
            for(i = 0; i < 100; i++)
            {
                aveResult = aveResult + adcConvResult[i];
            }
            finResult = aveResult/100;
        }
        else
        {
            setCursor (0,1);
            writeStr("Result:");
            writeNum(finResult*100,3);
            writeStr(" mV   ");
        }
    }
}

void Setup_handles(void)
{
    // Initialize all the handles needed for this application
    myClk = CLK_init((void *)CLK_BASE_ADDR, sizeof(CLK_Obj));
    myCpu = CPU_init((void *)NULL, sizeof(CPU_Obj));
    myFlash = FLASH_init((void *)FLASH_BASE_ADDR, sizeof(FLASH_Obj));
    myGpio = GPIO_init((void *)GPIO_BASE_ADDR, sizeof(GPIO_Obj));
    myPie = PIE_init((void *)PIE_BASE_ADDR, sizeof(PIE_Obj));
    myPll = PLL_init((void *)PLL_BASE_ADDR, sizeof(PLL_Obj));
    myWDog = WDOG_init((void *)WDOG_BASE_ADDR, sizeof(WDOG_Obj));
}

void Init_system(void)
{
    // If running from flash copy RAM only functions to RAM
#ifdef _FLASH
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
#endif
    //disable watch dog
    WDOG_disable(myWDog);
    // Perform factory calibration
    CLK_enableAdcClock(myClk);
    (*Device_cal)();
    CLK_disableAdcClock(myClk);
    //Select the internal oscillator 1 as the clock source
    CLK_setOscSrc(myClk, CLK_OscSrc_Internal);
    // Setup the PLL for x10 /2 which will yield 50Mhz = 10Mhz * 10 / 2
    PLL_setup(myPll, PLL_Multiplier_10, PLL_DivideSelect_ClkIn_by_2);
    // Disable the PIE and all interrupts
    PIE_disable(myPie);
    PIE_disableAllInts(myPie);
    CPU_disableGlobalInts(myCpu);
    CPU_clearIntFlags(myCpu);
}
