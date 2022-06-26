/*
 * audio_creation.h
 *
 *  Created on: 20.05.2022
 *      Author: Leon Farchau
 */

#ifndef AUDIO_CREATION_H_
#define AUDIO_CREATION_H_

#include "stdint.h"

//data types

typedef struct ChannelHandler
{
    volatile uint16_t subStepsPerTick;//10:6
    volatile uint16_t subStepCnt;//10:6
    volatile uint16_t gain;//10:6
    volatile uint16_t maxGain;//10:6
    volatile int16_t gainIncrement; //10:6
}ChannelHandler;

// defines

#define     SIZE_OF_BUFFER      1024
#define     CHANNELS            16

//functions declarations

interrupt void aud_sampleISR(void);
void aud_init();
void aud_startTone(uint16_t tone,  uint16_t maxGain, uint16_t channelIndex);
void aud_stopChannel(uint16_t channelIndex);
void aud_setGain(uint16_t gain, uint16_t channelIndex);
void aud_reset();

#endif /* AUDIO_CREATION_H_ */
