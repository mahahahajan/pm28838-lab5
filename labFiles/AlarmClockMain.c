#include "AlarmClockMain.h"

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))

// constants
uint8_t minuteSeconds = 60; // 60
uint8_t hourMinutes = 60; // 60
uint32_t updateIntervalSeconds = 15; // 30

// direct imports from modules
extern uint8_t oldHour;
extern uint8_t oldMinute;
extern uint8_t alarmHour;
extern uint8_t alarmMinute;
extern uint8_t alarmEnabled;
extern int onMainMenu;
extern int onAlarmMenu;
extern int onTimeMenu;


void LogicAnalyzerTask(void){
  UART0_DR_R = 0x80|GPIO_PORTF_DATA_R; // sends at 10kHz
}
// measures analog voltage on PD3
void ScopeTask(void){  // called 10k/sec
  UART0_DR_R = (ADC1_SSFIFO3_R>>4); // send ADC to TExaSdisplay
}


int main(void) {
	
	Switches_Init();
	
	while(1){
    PF1 ^= 0x02;
  }
	
}

