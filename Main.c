#include "Main.h"

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))

// constants
//use const here

// direct imports from modules
//use extern here

void LogicAnalyzerTask(void){
  UART0_DR_R = 0x80|GPIO_PORTF_DATA_R; // sends at 10kHz
}
// measures analog voltage on PD3
void ScopeTask(void){  // called 10k/sec
  UART0_DR_R = (ADC1_SSFIFO3_R>>4); // send ADC to TExaSdisplay
}
// measures analog voltage on PE4
void ScopeTask2(void){  // called 10k/sec
  UART0_DR_R = (ADC0_SSFIFO3_R>>5); // send ADC to TExaSdisplay
}

int main(void) {
	
	// pick one of the following three lines, all three set PLL to 80 MHz
  PLL_Init(Bus80MHz);              // 1) call to have no TExaS debugging								
  //TExaS_SetTask(&LogicAnalyzerTask); // 2) call to activate logic analyzer
  //TExaS_SetTask(&ScopeTask);       // or 3) call to activate analog scope PD3
	  
  LaunchPad_Init();                  // activate port F
  PF1 = 0;
  PF2 = 0;                           // turn off LED
	PF3 = 0;
	
	// init all drivers
  DisableInterrupts();
	Timer_Init();
	Switches_Init();
	Music_Init();
  EnableInterrupts();
	
	while(1){
    PF1 ^= 0x02;
  }
	
}

