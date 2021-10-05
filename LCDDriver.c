/* LCD Driver Module */

#include "LCDDriver.h"


/* constants */

#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))


/* direct module imports */


/* module fields */



/* module methods/functions */

void Display_Init(void) {
	ST7735_InitR(INITR_REDTAB);
	ST7735_SetTextColor(ST7735_WHITE);
	ST7735_DrawString(7, 8, "LAB 5", ST7735_WHITE);
}

/*
void DrawTopMenu() {
	Output_Clear();
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_DrawString(2, 1, "Menu:", clockColor);
	ST7735_DrawString(3, 2, "Set Time", clockColor);
	ST7735_DrawString(3, 3, "Set Alarm", clockColor);
	ST7735_DrawString(3, 4, "Toggle Colors", clockColor);
	if (currentClockType == GRAPHICAL) {
		ST7735_DrawString(3, 5, "Show Numeric", clockColor);
	} else {
		ST7735_DrawString(3, 5, "Show Graphical", clockColor);
	}
}
int8_t currOption = -1;

void InitTopMenu() {
	currOption = 0;
	ST7735_DrawString(2, 2, ">", clockColor); // set time ?
}

void nextMenuOption() {
	switch(currOption) {
		case 0: // was on set time, now to set alarm
			currOption = 1;
			ST7735_DrawString(2, 2, ">", backgroundColor); // deselect option 1
			ST7735_DrawString(2, 3, ">", clockColor); // select option 2
			break;
		case 1: //was on set alarm, now to set color
			currOption = 2;
			ST7735_DrawString(2, 3, ">", backgroundColor); // deselect option 2
			ST7735_DrawString(2, 4, ">", clockColor); // select option 3
			break;
		case 2: //was on set color, now on set military
			currOption = 3;
			ST7735_DrawString(2, 4, ">", backgroundColor); // deselect option 2
			ST7735_DrawString(2, 5, ">", clockColor); // select option 3
			break;
		case 3:
			currOption = 0;
			ST7735_DrawString(2, 5, ">", backgroundColor); // deselect option 2
			ST7735_DrawString(2, 2, ">", clockColor); // select option 3
		default: 
			break;
	}
}

uint32_t getMenuOption(void) {
	return currOption;
}
*/
	
