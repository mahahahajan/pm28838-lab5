#ifndef _TIMERDRIVER_H_
#define _TIMERDRIVER_H_

#include <stdio.h>
#include <stdint.h>
#include "ST7735.h"
#include "../inc/tm4c123gh6pm.h"

#include "Timer2.h"
#include "Timer3.h"

#include "AudioDriver.h"
#include "SwitchDriver.h"

void Timer_Init(void);

void Timer_HandleAlarm(void);

uint32_t get_time_measurement(void);

void Timer_InitTask2(void(*task2)(void));

void Timer_InitTask3(void(*task3)(void));

void Timer0A_Enable();
void Timer0A_Disable();
void Timer0A_SetPeriod(uint32_t period);


#endif
