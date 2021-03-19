/*
 * NAU7802_ADC.c
 *
 *  Created on: Mar 16, 2021
 *      Author: Josh Penner
 */

#include <NAU7802_ADC.h>

// adc status variables
bool adcStPwrUpReady;
bool adcStDigPower;
bool adcStAnaPower;
bool adcStCycleReady;

// bytes sent/received variables
int bytesSent = 0;
int bytesReceived = 0;

bool AdcPowerOnSequence(void)
{
    // set RR bit to 1 to reset register values
    AdcWrite(NAU7802_REG_PU_CTRL, PU_CTRL_RR);
    // set RR bit to 0 and PUD bit 1 in r0X00 to enter normal operation
    AdcWrite(NAU7802_REG_PU_CTRL, PU_CTRL_PUD);
    // after 200 us, check device is ready
    DELAY_US(200);
    AdcGetStatus();
    if(adcStPwrUpReady)
    {
        //device ready

        // configure device registers?

        // power up analog and digital circuits PUA and PUD
        AdcWrite(NAU7802_REG_PU_CTRL, (PU_CTRL_PUA | PU_CTRL_PUD));

        return SUCCESS;
    }
    else return FAIL;
}

void AdcGetStatus(void)
{
    uint16_t adcResponse = AdcRead(NAU7802_REG_PU_CTRL);
    adcStPwrUpReady = adcResponse & PU_CTRL_PUR;
    adcStDigPower = adcResponse & PU_CTRL_PUD;
    adcStAnaPower = adcResponse & PU_CTRL_PUA;
    adcStCycleReady = adcResponse & PU_CTRL_CR;
}

float AdcInitConversion(void)
{
    // initiate a conversion on the ADC

    // set Cycle Start bit
    AdcWrite(NAU7802_REG_PU_CTRL, PU_CTRL_PUA | PU_CTRL_PUD | PU_CTRL_CS);

    // check the Cycle Ready bit
    AdcGetStatus();
    if(adcStCycleReady)
    {
        // read ADC conversion result
            uint32_t adcConvResultB2 = AdcRead(NAU7802_REG_ADCO_B2);
            uint32_t adcConvResultB1 = AdcRead(NAU7802_REG_ADCO_B1);
            uint32_t adcConvResultB0 = AdcRead(NAU7802_REG_ADCO_B0);

            uint32_t adcConvResultConcat = (adcConvResultB2 << 16) + (adcConvResultB1 << 8) + adcConvResultB0;

            float adcConvResultVolts = adcConvResultConcat * AdcMaxVolts / AdcMaxVal;

            return adcConvResultVolts;
    }
    return 0;
}

void AdcWrite(uint16_t adcRegAddr, uint16_t dataByte)
{
    // Transmit data on the I2C bus
    I2caRegs.I2CSAR = NAU7802_I2C_SLAVE_ADDR;
    I2caRegs.I2CCNT = 2;        // number of data bytes
    I2caRegs.I2CDXR = adcRegAddr;     // load data into the transmit register
    I2caRegs.I2CMDR.all = I2C_START_STOP_TRANSMIT;    // start transmitting
    while (I2caRegs.I2CSTR.bit.XRDY != 1);  // wait for transmit register available
    bytesSent++;

    I2caRegs.I2CDXR = dataByte;     // load data into the transmit register
    while (I2caRegs.I2CSTR.bit.XRDY != 1);  // wait for transmit register available
    bytesSent++;

}

uint16_t AdcRead(uint16_t RegAddr)
{
    // Read data on the I2C bus

    // transmit register address to read
    I2caRegs.I2CSAR = NAU7802_I2C_SLAVE_ADDR;
    I2caRegs.I2CCNT = 1;        // number of data bytes to transmit
    I2caRegs.I2CDXR = RegAddr;     // load data into the transmit register
    I2caRegs.I2CMDR.all = I2C_START_STOP_TRANSMIT;    // start transmitting
    while (I2caRegs.I2CSTR.bit.XRDY != 1);  // wait for transmit register available
    bytesSent++;

    // change to receive mode

    I2caRegs.I2CCNT = 1;        // number of data bytes to receive
    I2caRegs.I2CMDR.bit.TRX = 0;    // receive mode (R/W = 1)
    I2caRegs.I2CMDR.all = I2C_START_NOSTOP_RECEIVE;    // start receive mode
    while (I2caRegs.I2CSTR.bit.RRDY != 1);        // wait until the data receive register is ready to be read
    bytesReceived++;
    uint16_t data = I2caRegs.I2CDRR;

    return data;
}

void ConfigI2C(void)
{
    // set up I2C register values

    CLK_enableI2cClock(myClk);

    ConfigI2cClock(); // Set I2C clock to 10 Khz
}

void ConfigI2cClock(void)
{
    // sets the I2C master clock frequency on SCL pin

    I2caRegs.I2CMDR.bit.IRS = 0; // set I2C module into reset state

    // I2CCLK(Fmod) = SYSCLK/(I2CPSC+1) = 10 MHz
    #if (CPU_FRQ_40MHZ||CPU_FRQ_50MHZ)
      I2caRegs.I2CPSC.all = 4;       // Prescaler - need 7-12 Mhz on module clk
    #endif
    #if (CPU_FRQ_60MHZ)
      I2caRegs.I2CPSC.all = 5;       // Prescaler - need 7-12 Mhz on module clk
    #endif
    // MSTCLK(Fmst) = Fmod/[(ICCH+d)+(ICCL+d)] = 10 Mhz / 1000 = 10 kHz
    I2caRegs.I2CCLKL = 495;           // NOTE: must be non zero
    I2caRegs.I2CCLKH = 495;            // NOTE: must be non zero
    // note: d = 5

    I2caRegs.I2CMDR.bit.IRS = 1; // set I2C module out of reset state
}
