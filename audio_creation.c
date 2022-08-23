/*
 * audio_creation.c
 *
 *  Created on: 20.05.2022
 *      Author: Leon Farchau
 */

//includes

#include "audio_creation.h"

// CSL
#include    <csl.h>
#include    <csl_irq.h>
#include    <csl_mcbsp.h>
#include    <csl_timer.h>
#include    <csl_i2c.h>

// external variables

extern const int sin_LT[SIZE_OF_BUFFER];
extern MCBSP_Handle h_McBSP;
extern TIMER_Handle mhTimer1;

// global variables

uint16_t g_timPrecounter = 0;
volatile uint32_t g_ms = 0;
ChannelHandler ga_channel [CHANNELS];


//private prototypes

uint16_t aud_fToSubStepsPerTick(uint32_t f);


//public functions


/**
 * @brief Initializes the audio creation module
 * 
 */
void aud_init()
{

    int index = 0;
    ChannelHandler emptyCh = {0,0,0};
    for (index = 0; index < CHANNELS; ++index) {
        ga_channel[index] = emptyCh;
    }
}

/**
 * @brief Starts the tone /tone/ with a volume of /maxGain/ on the channel /channelIndex/
 * 
 * @param tone          Tone to be played
 * @param maxGain       MaxGain (Volume) for this tone
 * @param channelIndex  Channel to play this tone on
 */
void aud_startTone(uint16_t tone, uint16_t maxGain, uint16_t channelIndex)
{

    if(channelIndex >= CHANNELS)
    {
        return;
    }

    ga_channel[channelIndex].gainIncrement = 20;
    ga_channel[channelIndex].subStepsPerTick = aud_fToSubStepsPerTick(tone);
    ga_channel[channelIndex].maxGain = maxGain<<6;
}

/**
 * @brief Stops/mutes the tone on the channel /channelIndex
 * 
 * @param channelIndex  Channel to stop tone on
 */
void aud_stopChannel(uint16_t channelIndex)
{
    ga_channel[channelIndex].gainIncrement = -20;
}


/**
 * @brief Resets the audio creation module
 * 
 */
void aud_reset()
{
    int index = 0;
    for (index = 0; index < CHANNELS; ++index) {
        aud_setGain(0, index);
        aud_stopChannel(index);
    }

}


/**
 * @brief Sets the gain for the channel with the index /channelIndex/ to /gain/
 * 
 * @param gain          Gain to whoch the channel should be set to
 * @param channelIndex  Index of the Channel
 */
void aud_setGain(uint16_t gain, uint16_t channelIndex)
{
    if(channelIndex < CHANNELS)
    {
        ga_channel[channelIndex].gain = gain;
    }
}

/**
 * @brief ISR that is used for creation of the audio signals, should be called at a frequency of 48 kHz
 * 
 * @return interrupt 
 */
interrupt void aud_sampleISR(void)
{
    g_timPrecounter++;
    if(g_timPrecounter >= 48)
    {
        g_ms++;
        g_timPrecounter = 0;
    }

    //generate output
    uint32_t outputValue = 0;
    uint16_t index = 0;
    for (index = 0; index < CHANNELS; ++index) {
        if(ga_channel[index].gainIncrement > 0 || ga_channel[index].gain >= (-ga_channel[index].gainIncrement) )
        {
            ga_channel[index].subStepCnt += ga_channel[index].subStepsPerTick;

            if (ga_channel[index].gain < (ga_channel[index].maxGain - ga_channel[index].gainIncrement))
                ga_channel[index].gain += ga_channel[index].gainIncrement;
        }
        else
        {
            ga_channel[index].subStepCnt = 0;
        }
        //There should be no overflow, as max value is 2^16*1024 => 2^26
        outputValue += ((int32_t) sin_LT[ga_channel[index].subStepCnt >> 6] * (ga_channel[index].gain >> 6));
    }

    //Here an overflow could occur -> sum of all gains must not exceed 1024
    uint16_t output16 =  (unsigned) (outputValue >> 10);

    MCBSP_write32( h_McBSP, (uint16_t) output16 | (uint32_t) output16 << 16);
}


//private functions

/**
 * @brief Transform the frequency /f/ to Steps to be taken in 64 fractions
 * 
 * @param f             frequency in Hertz
 * @return uint16_t     Requeired substeps in fixed point arithmic with 10bits int and 6 bits fraction
 */
uint16_t aud_fToSubStepsPerTick(uint32_t f)
{
   return (uint32_t)65536 * f / ((uint32_t)48000);
}
