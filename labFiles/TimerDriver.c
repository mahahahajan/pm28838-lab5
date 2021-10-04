/* Timer Driver Module */

#include "TimerDriver.h"


/* constants */

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
	

/* module direct imports */
extern uint8_t minuteSeconds;
extern uint8_t hourMinutes;
extern uint8_t oldHour;
extern uint8_t oldMinute;
extern uint8_t alarmHour;
extern uint8_t alarmMinute;
extern uint8_t alarmEnabled;
extern uint32_t updateIntervalSeconds;


/* module fields */

volatile uint32_t tone_id = 1;
volatile uint32_t note_counter = 0;


/* module internal functions */

void Timer_HandleAlarm(void){
	
	Audio_SetTone(tone_id);
	Audio_OutputTone();
	
	note_counter++;
	if (note_counter < 500) {
		tone_id = 1;
	} else if (note_counter < 1000) {
		tone_id = 2;
	} else if (note_counter < 1500) {
		tone_id = 3;
	} else if (note_counter < 2000) {
		tone_id = 4;
	} else if (note_counter < 2500) {
		tone_id = 5;
	} else if (note_counter < 3000) {
		tone_id = 6;
	} else if (note_counter < 3500) {
		tone_id = 7;
	} else if (note_counter < 4000) {
		tone_id = 8;
	} else note_counter = 0;
	
}

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
  TIMER0_TAILR_R = (80000/2) - 1;        
  TIMER0_IMR_R |= TIMER_IMR_TATOIM;// enable timeout (rollover) interrupt
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// clear timer0A timeout flag
	//TIMER0_TAPR_R = 0;
  TIMER0_CTL_R |= TIMER_CTL_TAEN;  // enable timer0A 32-b, periodic, interrupts
  // **** interrupt initialization ****
                                   // Timer0A=priority 2
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // top 3 bits
  NVIC_EN0_R = 1<<19;              // enable interrupt 19 in NVIC
}
void Timer1_Init(void){
  volatile uint32_t delay;
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  delay = SYSCTL_RCGCTIMER_R;   // allow time to finish activating
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, down-count 
  TIMER1_TAILR_R = 80000000;  // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|0x00008000; // ?? priority 4
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
	TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}
/*
void Timer2_Init(void){
	SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = 80000-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|(1<<29); // priority  
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
	
	Audio_SetTone(tone_id);
	
}
*/

void Timer0A_Handler(void){
	
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer0A timeout
	
  //PF2 ^= 0x04;                   // profile
	
	Switches_Loop();
	Timer_HandleAlarm();
  
	//uint32_t time_measurement = TIMER1_TAR_R;
	
}

uint32_t get_time_measurement(void) {
	uint32_t time_measurement = TIMER1_TAR_R;
	return time_measurement;
}

/*
void Timer2A_Handler(void){
	
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;    // acknowledge timer2 timeout
	//uint32_t xyz = 500/1234567+0x02;  // this line may cause jitter
	
	//Switches_Loop();
	
	//PF2 ^= 0x04;  
}
*/

uint32_t update_counter = 0;
uint8_t seconds = 0;
int firstTimerAround = 1;
void Timer1A_Handler(void){
	
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
	
	update_counter++;
	if (update_counter >= 2) {
		if (firstTimerAround == 1) {
			ESP_Request_Time();
			firstTimerAround = 0;
		}
	}
	if (update_counter >= updateIntervalSeconds) {
		ESP_Request_Time();
		update_counter = 0;
	}
	
	seconds += 1;
	if (seconds >= minuteSeconds) {
		uint8_t newMinute = oldMinute + 1;
		uint8_t newHour = oldHour;
		// we went up a minute
		seconds = 0;
		if (newMinute >= hourMinutes) {
			// we went up an hour
			newHour += 1;
			newMinute = 0;
		}
		
		DrawHands(newHour, newMinute); // time vars are updated in here
		
		if (alarmEnabled) {
			if (alarmHour == oldHour && alarmMinute == oldMinute) {
				Audio_TurnOn();
			}
		}
	}
}


/* module external methods */

void Timer_Init(void){
	update_counter = updateIntervalSeconds - 1;
	Timer0_Init();
	Timer1_Init();
	//Timer2_Init();
}

void Timer_InitTask2(void(*task2)(void)) {
	Timer2_Init(task2, 800000); 
}

void Timer_InitTask3(void(*task3)(void)) {
  Timer3_Init(task3, 40000000); 
}

