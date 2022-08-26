/*
 * aic23.c
 *
 *  Created on: 20.05.2022
 *      Author: Leon Farchau
 */

#include "aic23.h"

//--------------------------
uint16_t leftVolumeMute[2] = {0x05,0x184};
uint16_t leftVolumeUnmute[2] = {0x05,0x1F0};
uint16_t rightVolumeMute[2] = {0x07,0x180}; //180
uint16_t rightVolumeUnmute[2] = {0x07,0x1F0};

//private prototypes
void aic23_delay( uint16_t rounds );

/**
 * @brief Configuration of AIC23B audio chip via I2C interface
 * 
 */
void aic23_config( void ) {

    uint16_t reset[2] = {0x1E,0x00};
    uint16_t power_down_control[2] = {0x0C,0x07};                // line, mic and adc -> off; dac, out, osc, clk, off -> on
    uint16_t analog_audio_path_control[2] = {0x08,0x10};         // mic muted, dac selected
    uint16_t digital_audio_path_control[2] = {0x0A,0x07};        // ADC high pass filter enabled
    uint16_t digital_audio_interface_format[2] = {0x0E,0x43};    // DSP Format, 16 Bit input length, dac right channel on (LRCIN high), no swap leftRight, master mode
    uint16_t sample_rate_control[2] = {0x10,0x0};                // sample rate adc, dac auf 48kHz
    uint16_t digital_interface_activation[2] = {0x12,0x01};      // USB activated
    uint16_t left_line_input_volume_control[2] = {0x01,0x1F8};   // 0dB, left/right sync disabled
    uint16_t right_line_input_volume_control[2] = {0x03,0x1F8};  // 0dB, left/right sync disabled

    /* configure reset register */
    aic23_send(reset);
    /* configure power down control register */
    aic23_send(power_down_control);
    /* configure analog audio path register */
    aic23_send(analog_audio_path_control);
    /* configure digital audio path register */
    aic23_send(digital_audio_path_control);
    /* configure digital audio interface register */
    aic23_send(digital_audio_interface_format);
    /* configure sample rate control register */
    aic23_send(sample_rate_control);
    /* configure digital interface activation register */
    aic23_send(digital_interface_activation);
    /* configure left line input register */
    aic23_send(left_line_input_volume_control);
    /* configure right line input register */
    aic23_send(right_line_input_volume_control);

}

/**
 * Sets Volume for both channels from range -73db - +6db with 7 bit wide value
 */
void aic23_setVolume(uint16_t volume)
{
    volume &= 0x7F;

    uint16_t left_headphone_volume_control[2] = {0x05,0};    //maximum Volume, zero crossing enabled, left/right sync disabled
    uint16_t right_headphone_volume_control[2] = {0x07,0};   //maximum Volume, zero crossing enabled, left/right sync disabled 79 max volume

    left_headphone_volume_control[1] = 0x0180 + volume;
    right_headphone_volume_control[1] = 0x0180 + volume;

    /* configure left headphone register */
    aic23_send(left_headphone_volume_control);
    /* configure right headphone register */
    aic23_send(right_headphone_volume_control);
}


/**
 * @brief Send /data/ to the AIC23B audio chip using I2C
 * 
 * @param data  data to send
 * @return int  status if transmission was successfull
 */
int aic23_send( uint16_t* data)
{
    return I2C_write( data, 2, 1, 0x1Au, 1, 30000 );
}
