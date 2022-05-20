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
    volatile uint16_t subStepsPerTick;
    volatile uint16_t subStepCnt;
    volatile uint16_t gain;
    volatile uint16_t gainDecrement;
}ChannelHandler;

// defines

#define     SIZE_OF_BUFFER      1024
#define     CHANNELS            16

//functions declarations

interrupt void aud_sampleISR(void);
void aud_init();
void aud_setTone(uint32_t tone, uint16_t channelIndex);
void aud_setGain(uint16_t gain, uint16_t channelIndex);

#endif /* AUDIO_CREATION_H_ */
