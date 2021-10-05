#ifndef _AUDIODRIVER_H_
#define _AUDIODRIVER_H_

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/CortexM.h"


void Audio_Init(void);

void Audio_TurnOn(void);
void Audio_TurnOff(void);

void Audio_SetTone(uint32_t newToneId);
void Audio_OutputTone(void);

#endif
