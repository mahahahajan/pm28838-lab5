#ifndef _MUSIC_H_
#define _MUSIC_H_

#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#include "DAC.h"

#define C 		523
#define B 		494
#define Bb  	466
#define A			440
#define Ab 		415
#define G			392
#define Gb 		370
#define F			349
#define E			330
#define Eb		311
#define D			294
#define Db		277
#define LowC	262




typedef unsigned short Sound;


struct Note{
	Sound* currSound;
	uint16_t freq;
};
	
struct Song{
	//TODO: Create song struct here
	//uint16_t songVals[155] = {};
	struct Note notes[150];	
};

void Music_Play(int songNum);

void Music_Stop(void);

void Play(void);

void PlayFromBeginning(void); 



#endif