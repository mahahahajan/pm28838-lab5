/* Switch Driver Module */

#include "SwitchDriver.h"
#include "Music.h"

/* constants */

#define NVIC_EN0_INT2           0x00000004  // Interrupt 2 enable
#define NVIC_EN0_INT4           0x00000010  // Interrupt 4 enable
#define PF3                     (*((volatile uint32_t *)0x40025020))
#define PF2                     (*((volatile uint32_t *)0x40025010))
#define PF1                     (*((volatile uint32_t *)0x40025008))
#define Pin0Mask 0x01
#define Pin1Mask 0x02
#define Pin2Mask 0x04
#define Pin3Mask 0x08
#define Pin4Mask 0x10
#define Pin5Mask 0x20
#define Pin6Mask 0x40
#define Pin7Mask 0x80
	
extern uint8_t oldHour;
extern uint8_t oldMinute;
extern uint8_t alarmHour;
extern uint8_t alarmMinute;
extern uint8_t alarmEnabled;
extern ClockType currentClockType;

/* module fields */

volatile uint32_t SW1;
volatile uint32_t SW2;
volatile uint32_t SW3;
volatile uint32_t SW4;
volatile uint32_t SW5;
volatile uint16_t SW_Delay1;
volatile uint16_t SW_Delay2;
volatile uint16_t SW_Delay3;
volatile uint16_t SW_Delay4;
volatile uint16_t SW_Delay5;
volatile uint32_t FallingEdges = 0;

uint8_t isPlaying = 0; // 0 is paused, 1 is playing
uint8_t mode = 0; // TBD
/* module internal functions */
void TogglePlayPause(void){
	if(isPlaying == 0){
		Play();
	} else {
		//Pause
		Stop();
	}
}


void PlayFromBeginning(void) {
	//Play from beginning
	if(isPlaying == 1) {
		Stop();
	}
	PlayFromBeginning();
	isPlaying = 1;
}

void Mode(void) { //cycle through modes
	if(mode == 2) mode = 0;
	else{
		mode++;
	}
}

void PortC_Init(void) {
	
	// activate port C and port E
  SYSCTL_RCGCGPIO_R |= 0x14;  // enable ports C and E
  while((SYSCTL_PRGPIO_R&0x14)==0); // allow time for clock to start
  GPIO_PORTC_PCTL_R &= ~0xFFFF0000;// configure PC4 as GPIO (default setting)
  GPIO_PORTC_AMSEL_R &= ~0xF0;// disable analog functionality on PC4-7 (default setting)
	//GPIO_PORTC_PUR_R |= 0x10;     //     no pullup for external switches (enable weak pull-up on PC4)
  GPIO_PORTC_DIR_R &= ~0xF0;  // make PC4-7 in (PC4-7 buttons) (default setting)
  GPIO_PORTC_AFSEL_R &= ~0xF0;// disable alt funct on PC4-7 (default setting)
  GPIO_PORTC_DEN_R |= 0xF0;   // enable digital I/O on PC4-7 
	
}

void EdgeCounterPortF_Init(void){                          
  SYSCTL_RCGCGPIO_R |= 0x00000020; // (a) activate clock for port F
  FallingEdges = 0;             // (b) initialize counter
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  GPIO_PORTF_DIR_R |=  0x0E;    // output on PF3,2,1 
  GPIO_PORTF_DIR_R &= ~0x11;    // (c) make PF4,0 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x1F;  //     disable alt funct on PF4,0
  GPIO_PORTF_DEN_R |= 0x1F;     //     enable digital I/O on PF4   
  GPIO_PORTF_PCTL_R &= ~0x000FFFFF; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R = 0;       //     disable analog functionality on PF
  GPIO_PORTF_PUR_R |= 0x11;     //     enable weak pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x10;     // (d) PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10;    //     PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x10;    //     PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x10;      // (e) clear flag4
  GPIO_PORTF_IM_R |= 0x10;      // (f) arm interrupt on PF4 *** No IME bit as mentioned in Book ***
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // (g) priority 5
  NVIC_EN0_R = 0x40000000;      // (h) enable interrupt 30 in NVIC
}
void GPIOPortF_Handler(void){
  GPIO_PORTF_ICR_R = 0x10;      // acknowledge flag4
  FallingEdges = FallingEdges + 1;
  //PF3 ^= 0x06; // profile
	if (SW5) {
		SW5 = 0;
	} else SW5 = 1;
}


/* module external functions */

void Switches_Init(void) {
	SW1 = 0;
	SW2 = 0;
	SW3 = 0;
	SW4 = 0;
	SW5 = 0;
	SW_Delay1 = 0;
	SW_Delay2 = 0;
	SW_Delay3 = 0;
	SW_Delay4 = 0;
	SW_Delay5 = 0;
	EdgeCounterPortF_Init();
	PortC_Init();
}

void Switches_Loop(void) {
	//Debounce stuff
	uint32_t switch_bitmap = GPIO_PORTC_DATA_R;
	uint8_t loop_delay = 4;
	if((switch_bitmap & Pin4Mask) != 0) {
		SW_Delay1 = loop_delay * ((switch_bitmap & Pin4Mask) >> 4);
	}
	if((switch_bitmap & Pin5Mask) != 0) {
		SW_Delay2 = loop_delay * ((switch_bitmap & Pin5Mask) >> 5);
	}
	if((switch_bitmap & Pin6Mask) != 0) {
		SW_Delay3 = loop_delay * ((switch_bitmap & Pin6Mask) >> 6);
	}
	if((switch_bitmap & Pin7Mask) != 0) {
		SW_Delay4 = loop_delay * ((switch_bitmap & Pin7Mask) >> 7);
	}
	//If the operator presses the play/pause button once the music should pause. 
	//Hitting the play/pause again causes music to continue. The play/pause button
	//does not restart from the beginning, rather it continues from the position it
	//was paused. If the rewind button is pressed, the music stops and the next play 
	//operation will start from the beginning. There is a mode switch that allows 
	//the operator to control some aspect of the player
	if (SW_Delay1 > 0) {// switch one 
		SW_Delay1 -= 1;
		if (SW_Delay1 == 0) {
			TogglePlayPause();
		}
	}
	if (SW_Delay2 > 0) { // switch two
		SW_Delay2 -= 1;
		if (SW_Delay2 == 0) {
			PlayFromBeginning();
		}
	}
	if (SW_Delay3 > 0) {
		SW_Delay3 -= 1;
		if (SW_Delay3 == 0) { // switch 3
			Mode();
		}
	}
	
}
