/*
 * aic23.h
 *
 *  Created on: 20.05.2022
 *      Author: Leon Farchau
 */

#include <stdint.h>

#ifndef AIC23_H_
#define AIC23_H_

void aic23_reset( void );
void aic23_config( uint16_t volume );
int aic23_send( uint16_t* data);
void aic23_setVolume( uint16_t volume );

#endif /* AIC23_H_ */
