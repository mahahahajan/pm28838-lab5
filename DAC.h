#ifndef _DAC_H_
#define _DAC_H_

#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#include "../inc/tm4c123gh6pm.h"
#include "../inc/CortexM.h"

#include "Music.h"




void DAC_Init(uint16_t data);

void DAC_Out(uint16_t num); // don't return, just output next value to DAC


#endif





