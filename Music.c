#include "Music.h"

extern uint8_t isPlaying;

const unsigned short SinWave[64] = {  
  2048,2409,2756,3076,3356,3586,3757,3862,3898,3862,3757,
  3586,3356,3076,2756,2409,2048,1687,1340,1020,740,510,
  339,234,198,234,339,510,740,1020,1340,1687,
  2048,2409,2756,3076,3356,3586,3757,3862,3898,3862,
  3757,3586,3356,3076,2756,2409,2048,1687,1340,1020,740,510,339,234,198,
  234,339,510,740,1020,1340,1687
};  

struct Note notes[] = {
	{A, quarter}, {B, quarter}, {C, quarter}, {D, quarter}, {E, quarter}, {F, quarter}, {G, quarter}
};


const Sound Wave[64] = {  
  1024,1122,1215,1302,1378,1440,1486,1514,1524,1514,1486,
  1440,1378,1302,1215,1122,1024,926,833,746,670,608,
  562,534,524,534,562,608,670,746,833,926
	, // repeat for 64 entries
  1024,1122,1215,1302,1378,1440,1486,1514,1524,1514,1486,
  1440,1378,1302,1215,1122,1024,926,833,746,670,608,
  562,534,524,534,562,608,670,746,833,926
};

const Sound Flute[32];

struct MySong song = {100, notes, SinWave, 8, 0, 0};
static int noteDuration = 0;

void Music_Init(void) {
	DAC_Init(0);
	isPlaying = 1;
	Timer0A_SetPeriod(song.notes[0].currSound);
	noteDuration = song.notes[0].length;
}

// timer handler
volatile long currDuration = 0;
volatile uint16_t I = 0;

void Timer0A_Handler(void){
  TIMER0_ICR_R = TIMER_ICR_TATOCINT; // acknowledge
  //currDuration ++; // note has been played for another duration?
	I = song.instrumentPointer;
  DAC_Out(SinWave[I]);
	song.instrumentPointer++;
	TIMER0_TAILR_R = song.notes[song.notePointer].currSound;
}

void Play(void) { //basically reenable the interrupt and tell it to output to dac 
	Timer0A_Enable();
}

void Rewind(void) { // disable interrupt, reset indexes to beginning, enable
	Stop();
	Timer0A_SetPeriod(song.notes[0].currSound);
	noteDuration = song.notes[0].length;
	Play();
}

void Stop(void) { //turn off the interrupt
	Timer0A_Disable();
}
