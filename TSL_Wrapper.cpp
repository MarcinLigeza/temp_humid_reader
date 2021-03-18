#include "TSL_Wrapper.hpp"

#include <pigpiod_if2.h>

#include "TSL2581_defs.hpp"

#define IIC_Addr 					0x39
#define IIC_Bus                     2

TSL_Wrapper::TSL_Wrapper(int pi_, QObject *parent) :
    QObject(parent),
    pi(pi_)
{
    handle = i2c_open(pi, IIC_Bus, IIC_Addr,0);
    init_TSL2581();
}

/**********************************************************************************************
* @brief  	init_TSL2581()
* @param   	power on ,set gain is 16,interrupt is 402ms
* @param
* @param    This field selects the integration time for each conversion.
**********************************************************************************************/
void TSL_Wrapper::init_TSL2581()
{
    /* write date from tsl2561 */
    i2c_write(COMMAND_CMD | CONTROL,CONTROL_POWERON);//power on
    time_sleep(2);//Wait 2 seconds for power on

    i2c_write(COMMAND_CMD | TIMING, INTEGRATIONTIME_400MS);  //400MS
    i2c_write(COMMAND_CMD | CONTROL, ADC_EN | CONTROL_POWERON); //Every ADC cycle generates interrupt
    i2c_write(COMMAND_CMD | INTERRUPT, INTR_INTER_MODE);	//TEST MODE
    i2c_write(COMMAND_CMD | ANALOG, GAIN_16X);				//GAIN = 16
}

/**********************************************************************************************
* @brief  	reload_register()
* @param   Interrupts need to be maintained for several cycles
* @param   When the interrupt bit is 0, reload the register
* @param   Configure the special registers, clear the interrupt bits, and then re-enable the ADC
***********************************************************************************************/
void TSL_Wrapper::reload_register()
{
    i2c_write(COMMAND_CMD | TRANSACTION_SPECIAL | SPECIAL_FUN_INTCLEAR, INTR_INTER_MODE);
    i2c_write(COMMAND_CMD | CONTROL, ADC_EN | CONTROL_POWERON); //Every ADC cycle generates interrupt
}

/**********************************************************************************************
* @brief  	set_interrupt_threshold(uint16_t min,uint16_t max)
* @param   	low and high max 2^16 = 65536
* @param
* @param    This field selects the integration time for each conversion.
**********************************************************************************************/
void TSL_Wrapper::set_interrupt_threshold(uint16_t min, uint16_t max)
{
    uint8_t DataLLow,DataLHigh,DataHLow,DataHHigh;
    DataLLow = min % 256;
    DataLHigh = min / 256;
    i2c_write(COMMAND_CMD | THLLOW, DataLLow);
    i2c_write(COMMAND_CMD | THLHIGH, DataLHigh);

    DataHLow = max % 256;
    DataHHigh = max / 256;
    i2c_write(COMMAND_CMD | THHLOW, DataHLow);
    i2c_write(COMMAND_CMD | THHHIGH, DataHHigh);
}

/**********************************************************************************************
* @brief  	Read_Channel()
* @param
* @param   	read two ADC data
* @param
**********************************************************************************************/
void TSL_Wrapper::read_channel()
{
    uint8_t DataLow,DataHigh;
    DataLow = i2c_read(COMMAND_CMD | TRANSACTION | DATA0LOW);
    DataHigh = i2c_read(COMMAND_CMD | TRANSACTION | DATA0HIGH);
    channel_0 = 256 * DataHigh + DataLow ;

    DataLow = i2c_read(COMMAND_CMD | TRANSACTION | DATA1LOW);
    DataHigh = i2c_read(COMMAND_CMD | TRANSACTION | DATA1HIGH);
    channel_1 = 256 * DataHigh + DataLow ;
}

uint32_t TSL_Wrapper::calculate_lux(uint16_t iGain, uint16_t tIntCycles)
{
    uint32_t chScale0;
    uint32_t chScale1;
    uint32_t channel1;
    uint32_t channel0;
    uint32_t temp;
    uint32_t ratio1 = 0;
    uint32_t ratio;
    uint32_t lux_temp;
    uint16_t b{}, m{};

    // No scaling if nominal integration (148 cycles or 400 ms) is used
    if (tIntCycles == NOM_INTEG_CYCLE)
    {
    //     chScale0 = 65536;
      chScale0 = (1 << (CH_SCALE));
    }
    else
    chScale0 = (NOM_INTEG_CYCLE << CH_SCALE) / tIntCycles;
    switch (iGain)
    {
    case 0: // 1x gain
      chScale1 = chScale0; // No scale. Nominal setting
      break;
    case 1: // 8x gain
      chScale0 = chScale0 >> 3; // Scale/multiply value by 1/8
      chScale1 = chScale0;
      break;
    case 2: // 16x gain
      chScale0 = chScale0 >> 4; // Scale/multiply value by 1/16
      chScale1 = chScale0;
      break;
    case 3: // 128x gain
      chScale1 = chScale0 / CH1GAIN128X;
      chScale0 = chScale0 / CH0GAIN128X;
      break;
    }
    // Read Channel for ADC
    read_channel();
    // scale the channel values
    channel0 = (channel_0 * chScale0) >>  CH_SCALE;
    channel1 = (channel_1 * chScale1) >>  CH_SCALE;
    //printf(" C10= %ld  C11 =%ld \n ",channel0,channel1);
    // find the ratio of the channel values (Channel1/Channel0)
    if (channel0 != 0)
    ratio1 = (channel1 << (RATIO_SCALE + 1)) / channel0;
    ratio = (ratio1 + 1) >> 1;	  									 // round the ratio value

    if (ratio <= K1C)
    {    b = B1C;    m = M1C;  }
    else if (ratio <= K2C)
    {    b = B2C;    m = M2C;  }
    else if (ratio <= K3C)
    {    b = B3C;    m = M3C;  }
    else if (ratio <= K4C)//276
    {    b = B4C;    m = M4C;  }
    else if (ratio > K5C)//276
    {    b = B5C;    m = M5C;  }

    temp = ((channel0 * b) - (channel1 * m));
    temp += (1 << (LUX_SCALE - 1));			// round lsb (2^(LUX_SCALE-1))
    //  temp = temp + 32768;
    lux_temp = temp >> LUX_SCALE;			// strip off fractional portion
    return (lux_temp);
}

void TSL_Wrapper::i2c_write(uint8_t address, uint8_t data)
{
    i2c_write_byte_data(pi, handle, address, data);
}

uint8_t TSL_Wrapper::i2c_read(uint8_t address)
{
    return i2c_read_byte_data(pi, handle, address);
}
