/* Timer Driver Module */

#include "TimerDriver.h"
#include "Music.h"


/* constants */

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
	

/* module direct imports */



/* module fields */

volatile uint32_t tone_id = 1;
volatile uint32_t note_counter = 0;


/* module internal functions */
//TODO: Check if this is right -- this is wrong im 99% sure 
const uint32_t audio_master_freq = sxth;

void Timer0_Init(void){
  volatile uint32_t delay;
  // **** general initialization ****
  SYSCTL_RCGCTIMER_R |= 0x01;      // activate timer0
  delay = SYSCTL_RCGCTIMER_R;      // allow time to finish activating
  TIMER0_CTL_R &= ~TIMER_CTL_TAEN; // disable timer0A during setup
  TIMER0_CFG_R = 0;                // configure for 32-bit timer mode
  // **** timer0A initialization ****
                                   // configure for periodic mode
  TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;
  //TIMER0_TAILR_R = audio_master_freq - 1;        
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
	//TIMER0_TAPR_R = 0;
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
}

void Timer0A_SetPeriod(uint32_t period) {
	TIMER0_TAILR_R = period - 1;     
}
void Timer0A_Enable(){
	  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
}

void Timer0A_Disable() {
	TIMER0_IMR_R &= ~TIMER_IMR_TATOIM;    // 7) arm timeout interrupt -- last bit needs to be 0 to be "disarmed"
}


void Timer1_Init(void){
  volatile uint32_t delay;
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  delay = SYSCTL_RCGCTIMER_R;   // allow time to finish activating
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, down-count 
  TIMER1_TAILR_R = (80000/2) - 1;  // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // ?? priority 4
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
	TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}

/*
void Timer0A_Handler(void){
	
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
	
  //PF2 ^= 0x04;                   // profile
	
}
*/

void Timer1A_Handler(void){
	
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
	
	Switches_Loop();
	
}


/* module external methods */

void Timer_Init(void){
	Timer0_Init();
	Timer1_Init();
}

void Timer_InitTask2(void(*task2)(void)) {
	Timer2_Init(task2, 800000); 
}

void Timer_InitTask3(void(*task3)(void)) {
  Timer3_Init(task3, 40000000); 
}

