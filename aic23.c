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
 * @fn              fn_AIC23_config( void )
 * @brief           Konfiguration des AIC23B Audio Codecs via I2C
 * @details         Funktion um das "Control Interface" des TLV320AIC23B Audio
 *                  Codes via I2C-Bus anzusprechen. Die gew�nschten
 *                  Einstellungen werden �bertragen und in die entsprechenden
 *                  Register geschrieben.
 *  @note           MODE = @b 0 - Das "Control Interface" ist �ber TWI bzw. I2C
 *                  zu erreichen\n
 *                  CS = @b 0 - Die Adresse ist auf 0b0011010 festgelegt\n
 */
void aic23_config( uint16_t volume ) {
    /*
     * Aufbau des I2C Datenrahmen
     *
     * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
     * | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | A | 15| 14| 13| 12| 11| 10| 9 | 8 | A | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | A |
     * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
     * |I2C-Address................|R/W|ACK|Register-Address...........|   |ACK|                               |ACK|
     * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+   +
     * |                           |   |   |                           |Register-Data..........................|   |
     * +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
     */

    uint16_t reset[2] = {0x1E,0x00};
    uint16_t power_down_control[2] = {0x0C,0x07};               //line, mic and adc -> off; dac, out, osc, clk, off -> on
    uint16_t analog_audio_path_control[2] = {0x08,0x10};        //mic muted, dac selected
    uint16_t digital_audio_path_control[2] = {0x0A,0x07};       //ADC high pass filter enabled
    uint16_t digital_audio_interface_format[2] = {0x0E,0x43};   //DSP Format, 16 Bit input length, dac right channel on (LRCIN high), no swap leftRight, master mode
    uint16_t sample_rate_control[2] = {0x10,0x0};               //sample rate adc, dac auf 48kHz
    uint16_t digital_interface_activation[2] = {0x12,0x01};     //USB activated
    uint16_t left_line_input_volume_control[2] = {0x01,0x97};   //muted, 0dB, left/right, sync disabled
    uint16_t right_line_input_volume_control[2] = {0x03,0x97};  //muted, 0dB, left/right, sync disabled

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

    aic23_setVolume(volume);
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
 *  @fn             fn_AIC23_send( uint16_t* data )
 *  @brief          Kommunikation mit dem "Control Interface" des AIC23
 *  @details        Funktion zur Kommunikation mit dem "Control Interface" des
 *                  TLV320AIC23B Audio Codecs. Der �bergebene, 16-Bit breite
 *                  Wert wird via I2C-Bus �bertragen.
 *  @note           7 Bit Register Adresse [15:9] + 9 Bit Register Daten [8:0]
 *
 *  @param[in]      data    16-Bit f�r die �bertragung via I2C (Adresse + Daten)
 *                  count   number of 16-Bit Data to send
 *
 *  @return         @b 0 - erfolgreiche �bertragung\n
 *                  @b 1 - Bus belegt, Startsequenz kann nicht generiert werden\n
 *                  @b 2 - Timeout (beim ersten Byte)\n
 *                  @b 3 - NACK (not Acknowledge) empfangen\n
 *                  @b 4 - Timeout (beim Warten auf XRDY)\n
 *                  @b 5 - NACK (not Acknowledge) empfangen (beim letztem Byte)\n
 */
int aic23_send( uint16_t* data)
{

    int S = I2C_write( data, 2, 1, 0x1Au, 1, 30000 ); //1x16Bit Data, Master mode, Slave Adress, Transfer mode (S-A-D...(n)..D-P)

    return S;
}

void aic23_mute()
{
    int n = 0;

    /* Anzahl der Wiederholungen, bevor die �bertragung abgebrochen wird      */
    int N = 5;

    /* Register Adresse + Daten um das "Reset Register" zu beschreiben        */
    uint16_t adressLeft = 0x460u;
    uint16_t adressRight = 0x660u;
    aic23_send(&adressLeft);
    aic23_send(&adressRight);
}

void aic23_unmute()
{
    /* Register Adresse + Daten um das "Reset Register" zu beschreiben        */
    uint16_t adressLeft = 0x47Fu;
    uint16_t adressRight = 0x67Fu;
    aic23_send(&adressLeft);
    aic23_send(&adressRight);
}

/**
 *  @fn             fn_AIC23_reset( void )
 *  @brief          Reset Register des AIC23 "Control Interface" auf 0 0000 0000 setzen
 *  @details        Via I2C Bus wird das "Reset Register" (Adresse: 0b0001111)
 *                  des TLV320AIC23B Audio Codecs gelscht, um so einen Reset
 *                  auszulsen.
 *
 *  @return         @b 0 - bei erfolgreicherbertragung des Reset Befehls\n
 *                  @b 1 - falls nach N Versuchen kein bertragungsversuch erfolgreich war
 */
void aic23_reset( void )
{
    int n = 0;

    /* Anzahl der Wiederholungen, bevor die bertragung abgebrochen wird      */
    int N = 5;

    /* Register Adresse + Daten um das "Reset Register" zu beschreiben        */
    uint16_t reset[] = { 0x1E00u };

    /* Ausfhren von N bertragungsversuchen                                  */
    while( n < N )
    {
        if( 0 == aic23_send( reset ) )
        {
            n = N;
        }

        n++;
    }

    /* Zeit fr den AIC23 um den Reset durchzufhren                          */
    aic23_delay( 50000 );
}

/**
 *  @fn             fn_delay( Uint16 rounds )
 *  @brief          Warte-/Verzgerungsfunktion
 *  @details        Funktion die eine Zeit lang "nichts tut" um so z.B. externen
 *                  Komponenten Zeit fr einen Reset einzurumen.
 *  @todo           fn_delay sollte in einem spteren Entwicklungsschritt durch
 *                  eine elegantere Lsung, z.B. mit einem Timer ersetzt werden,
 *                  um eine Nebenlufigkeit zu ermglichen!
 *
 *  @param[in]      rounds  Anzahl der Wiederholungen
 */
void aic23_delay( uint16_t rounds )
{
    uint16_t i, j;

    for( i = 0; i <= rounds; i++ )
    {
        for( j = 0; j <= 10; j++ )
        {
            asm( "\t nop" );
        }
    }
}
