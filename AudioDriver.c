/* Audio Driver Module */

#include "AudioDriver.h"


/* constants */

#define PF2       (*((volatile uint32_t *)0x40025010))
#define PB1_OFF		0x00
#define PB1_ON		0x02


/* module fields */

volatile uint8_t speakerPower;
volatile uint32_t pulse;
volatile uint32_t toneId;


/* module internal functions */

void PortD_Init() {
	 SYSCTL_RCGCGPIO_R |= 0x08;        // 1) activate port D
   while((SYSCTL_PRGPIO_R&0x08)==0){};   // allow time for clock to stabilize
   // 2) no need to unlock PD3-0
   GPIO_PORTD_AMSEL_R &= ~0x0F;      // 3) disable analog functionality on PD3-0
   GPIO_PORTD_PCTL_R &= ~0x0000FFFF; // 4) GPIO
   GPIO_PORTD_DIR_R |= 0x0F;         // 5) make PD0 out
   GPIO_PORTD_AFSEL_R &= ~0x0F;      // 6) regular port function
   GPIO_PORTD_DEN_R |= 0x0F;
}

void PortB_Init(void) {
  SYSCTL_RCGCGPIO_R |= 0x02;      // activate port B,F
  while((SYSCTL_PRGPIO_R&0x02) != 0x02){};
  GPIO_PORTB_DIR_R |=  0xFF;   // output on PB1
  GPIO_PORTB_DEN_R |=  0xFF;   // enable on PB1
	GPIO_PORTB_DATA_R = 0x00;
}

void PulseOff(void) {
	GPIO_PORTB_DATA_R = PB1_OFF;
	//PF2 = 0x00;
}
void PulseOn(void) {
	GPIO_PORTB_DATA_R = PB1_ON;
	//PF2 = 0xFF;
}

void OutputTone_0(void) {
	PulseOff();
}
void OutputTone_1(void) {
	if (pulse) {
		PulseOff();
		pulse = 0;
	} else {
		PulseOn();
		pulse = 1;
	}
}
void OutputTone_N(uint32_t n) {
	uint32_t m = n / 2;
	if (pulse < m) PulseOff();
	else if (pulse < n) PulseOn();
	pulse++;
	if (pulse >= n) pulse = 0;
}


/* module external methods */

void Audio_Init(void) {
	PortB_Init();
	speakerPower = 0;
	toneId = 0;
	pulse = 0;
}

void Audio_TurnOn(void) {
	speakerPower = 1;
}
void Audio_TurnOff(void) {
	speakerPower = 0;
}

void Audio_SetTone(uint32_t newToneId) {
	toneId = newToneId;
}
void Audio_OutputTone(void){
	//GPIO_PORTB_DATA_R ^= 0xFD;
	if (speakerPower == 0) {
		OutputTone_0();
		return;
	}
	if (toneId == 1) {
		OutputTone_1();
	} else {
		OutputTone_N(toneId);
	}
}
