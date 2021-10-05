#include "Music.h"


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

void Music_Init(void) {
	DAC_Init(0);
}

void Music_Play(int songNum) {

}

void Music_Stop(void) {

}

void Play(void) {

}

void PlayFromBeginning(void) {

}

