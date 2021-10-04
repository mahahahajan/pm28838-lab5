#ifndef _ESPDRIVER_H_
#define _ESPDRIVER_H_

#include <stdio.h>
#include <stdint.h>
#include "UART.h"
#include "esp8266.h"
#include "../inc/tm4c123gh6pm.h"
#include "../inc/ADCSWTrigger.h"

#include "TimerDriver.h"

void ESP_Init(void);

void ESP_Request_Time(void);

#endif
