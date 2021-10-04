/* LCD Driver Module */

#include "LCDDriver.h"


/* constants */

#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))


/* direct module imports */

extern int onMainMenu;
extern uint8_t esp_ready;


/* module fields */

ClockType currentClockType;
uint16_t backgroundColor;
uint16_t clockColor;
uint16_t hourHandColor;
uint16_t minuteHandColor;
double PI = 3.14;
//char* numbers[13] = {"12", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11"};
char* numbers2[4] = {"12", "3", "6", "9"};
char* milNumbers[4] = {"12", "15", "18", "21"};
uint8_t xBuf[4] = {10, 17, 10, 3};
uint8_t yBuff[4] = {5, 9, 13, 9};	
uint8_t oldHour, oldMinute, second;
uint8_t alarmHour, alarmMinute, alarmEnabled;
uint8_t colorScheme = 0;
//// 180 points on a circle of radius 2.000
//const int32_t CircleXbuf[180] = {2000, 1999, 1995, 1989, 1981, 1970, 1956, 1941, 1923, 1902, 1879, 1854, 1827, 1798, 1766, 1732, 1696, 1658, 1618, 1576, 1532, 1486, 1439, 1389, 1338, 1286, 1231, 1176, 1118, 1060, 1000, 939, 877, 813, 749, 684, 618, 551, 484, 416, 347, 278, 209, 140, 70, 0, -70, -140, -209, -278, -347, -416, -484, -551, -618, -684, -749, -813, -877, -939, -1000, -1060, -1118, -1176, -1231, -1286, -1338, -1389, -1439, -1486, -1532, -1576, -1618, -1658, -1696, -1732, -1766, -1798, -1827, -1854, -1879, -1902, -1923, -1941, -1956, -1970, -1981, -1989, -1995, -1999, -2000, -1999, -1995, -1989, -1981, -1970, -1956, -1941, -1923, -1902, -1879, -1854, -1827, -1798, -1766, -1732, -1696, -1658, -1618, -1576, -1532, -1486, -1439, -1389, -1338, -1286, -1231, -1176, -1118, -1060, -1000, -939, -877, -813, -749, -684, -618, -551, -484, -416, -347, -278, -209, -140, -70, 0, 70, 140, 209, 278, 347, 416, 484, 551, 618, 684, 749, 813, 877, 939, 1000, 1060, 1118, 1176, 1231, 1286, 1338, 1389, 1439, 1486, 1532, 1576, 1618, 1658, 1696, 1732, 1766, 1798, 1827, 1854, 1879, 1902, 1923, 1941, 1956, 1970, 1981, 1989, 1995, 1999
//};
//const int32_t CircleYbuf[180] = {0, 70, 140, 209, 278, 347, 416, 484, 551, 618, 684, 749, 813, 877, 939, 1000, 1060, 1118, 1176, 1231, 1286, 1338, 1389, 1439, 1486, 1532, 1576, 1618, 1658, 1696, 1732, 1766, 1798, 1827, 1854, 1879, 1902, 1923, 1941, 1956, 1970, 1981, 1989, 1995, 1999, 2000, 1999, 1995, 1989, 1981, 1970, 1956, 1941, 1923, 1902, 1879, 1854, 1827, 1798, 1766, 1732, 1696, 1658, 1618, 1576, 1532, 1486, 1439, 1389, 1338, 1286, 1231, 1176, 1118, 1060, 1000, 939, 877, 813, 749, 684, 618, 551, 484, 416, 347, 278, 209, 140, 70, 0, -70, -140, -209, -278, -347, -416, -484, -551, -618, -684, -749, -813, -877, -939, -1000, -1060, -1118, -1176, -1231, -1286, -1338, -1389, -1439, -1486, -1532, -1576, -1618, -1658, -1696, -1732, -1766, -1798, -1827, -1854, -1879, -1902, -1923, -1941, -1956, -1970, -1981, -1989, -1995, -1999, -2000, -1999, -1995, -1989, -1981, -1970, -1956, -1941, -1923, -1902, -1879, -1854, -1827, -1798, -1766, -1732, -1696, -1658, -1618, -1576, -1532, -1486, -1439, -1389, -1338, -1286, -1231, -1176, -1118, -1060, -1000, -939, -877, -813, -749, -684, -618, -551, -484, -416, -347, -278, -209, -140, -70
//};

/* module methods/functions */

void Display_Init(void) {
	ST7735_InitR(INITR_REDTAB);
	currentClockType = GRAPHICAL;
	backgroundColor = ST7735_BLACK;
	clockColor = ST7735_WHITE;
	hourHandColor = ST7735_GREEN;
	minuteHandColor = ST7735_BLUE;
	ST7735_SetTextColor(clockColor);
	alarmHour = 0;
	alarmMinute = 0;
	alarmEnabled = 0;
	colorScheme = 1;
	
	ST7735_DrawString(7, 8, "LOADING", ST7735_WHITE);
}

void SetupClock(uint8_t hour, uint8_t minute){
	oldHour = hour, oldMinute = minute;
	Output_Clear();
	ST7735_OutString("Clock");
	DrawClock();	
}

void DrawClock(){
	
	Output_Clear();
	//Draw bitmap
	if(currentClockType == GRAPHICAL){
		//Render graphic
		DrawGraphicalClock();
		DrawHands(oldHour, oldMinute);
	} else{
		//render numerical
		DrawNumericalClock(oldHour, oldMinute);
	}
}

void DrawGraphicalClock(){
	//ST7735_PlotClear(-2500, 2500, ST7735_BLACK );
	//ST7735_DrawCircle(64, 96, ST7735_WHITE);
	ST7735_XYplotInit("  Clock (Graphical) ",-2500, 2500, -2500, 2500, backgroundColor);
	
	// Input: x         columns from the left edge (0 to 20)
  //        y         rows from the top edge (0 to 15)
	// after plot clear, we have y from 0 - 12
	
	uint16_t r = 54;
	for(int i = 0; i < 360; i++){
		double x = r * cos(i*PI/180.00);
		double y = r * sin(i*PI/180.00);
		ST7735_DrawPixel(64 + x, 32+64+ y, clockColor);
		ST7735_DrawPixel(64 + x+1, 32+64+ y, clockColor);
		ST7735_DrawPixel(64 + x-1, 32+64+ y, clockColor);
		ST7735_DrawPixel(64 + x, 32+64+ y+1, clockColor);
		ST7735_DrawPixel(64 + x, 32+64+ y-1, clockColor);
	}
	uint16_t degrees = 450;
	r-=5;
	for(int i = 0; i < 12; i++) {
		double x = r * cos(degrees*PI/180.00);
		double y = r * sin(degrees*PI/180.00);
		ST7735_DrawPixel(64 + x, 32+64+ y, clockColor);
		degrees -= 30;
	}
	for(int i = 0; i < 4; i++) {
		if(oldHour >= 12) {
			ST7735_DrawString(xBuf[i], yBuff[i], milNumbers[i], clockColor);
		} else{
			ST7735_DrawString(xBuf[i], yBuff[i], numbers2[i], clockColor);
		}
			
		
	}
	//ST7735_XYplot(180,(int32_t *)CircleXbuf,(int32_t *)CircleYbuf, ST7735_WHITE);
}


void DrawHands(uint8_t newHour, uint8_t newMinute) {
	//PF2 ^= 0x04;
	//uint32_t time_measurementA = get_time_measurement();
	uint16_t degrees = 450;
	if (!onMainMenu && esp_ready) {
		//Hour Hand
		//	for(int i = 0; i < 25; i++) {
		//		if( i > 11) {
		//			hourHandColor = ST7735_BLUE;
		//		}
		//		
		//		
		//		hour++;
		//	}
		if (currentClockType == GRAPHICAL) {
			//old stuff needs to be cleared
			double xHour = 30 * cos( (degrees - (30*oldHour)) * PI/180.00); //get hour, use that to find out how many degrees to subtract, convert those degrees into a x offset, add to riginal
			double yHour = 30 * sin( (degrees - (30*oldHour)) * PI/180.00); //get hour, use that to find out how many degrees to subtract, convert those degrees into a x offset, add to riginal
			ST7735_Line(64, 32+64, 64 + xHour, 32+64-yHour, backgroundColor);
			
			double xMin = 42 * cos( (degrees - (6*oldMinute)) * PI/180.00); //get min, use that to find out how many degrees to subtract, convert those degrees into a x offset, add to riginal
			double yMin = 42 * sin( (degrees - (6*oldMinute)) * PI/180.00); //get min, use that to find out how many degrees to subtract, convert those degrees into a x offset, add to riginal
			ST7735_Line(64, 32+64, 64 + xMin, 32+64-yMin, backgroundColor);
			
			xHour = 30 * cos( (degrees - (30*newHour)) * PI/180.00); //get hour, use that to find out how many degrees to subtract, convert those degrees into a x offset, add to riginal
			yHour = 30 * sin( (degrees - (30*newHour)) * PI/180.00); //get hour, use that to find out how many degrees to subtract, convert those degrees into a x offset, add to riginal
			ST7735_Line(64, 32+64, 64 + xHour, 32+64-yHour, hourHandColor);
			
			xMin = 42 * cos( (degrees - (6*newMinute)) * PI/180.00); //get min, use that to find out how many degrees to subtract, convert those degrees into a x offset, add to riginal
			yMin = 42 * sin( (degrees - (6*newMinute)) * PI/180.00); //get min, use that to find out how many degrees to subtract, convert those degrees into a x offset, add to riginal
			ST7735_Line(64, 32+64, 64 + xMin, 32+64-yMin, minuteHandColor);
		} else {
			Output_Clear();
			DrawNumericalClock(newHour,newMinute);
		}
	}
	
	oldHour = newHour;
	oldMinute = newMinute;
	
	
	PF2 ^= 0x04;
	
	//uint32_t time_measurementB = get_time_measurement();
	//uint32_t tm = time_measurementA - time_measurementB;
	
	
}

void DrawNumericalClock(uint8_t newHour, uint8_t newMinute){
	ST7735_DrawString(8,8, "      ", backgroundColor); // clear old text
	ST7735_DrawString(0,1, "  Clock (Numeric) ", ST7735_YELLOW);
	//render new text 
	ST7735_SetCursor(8,8);
	if(newHour < 10) {
		ST7735_OutUDec(0);
	}
	ST7735_OutUDec(newHour);
	ST7735_OutChar(':');
	if(newMinute < 10) {
		ST7735_OutUDec(0);
	}
	ST7735_OutUDec(newMinute);
}

void DrawSimpleNumericalClock(uint8_t newHour, uint8_t newMinute){
	ST7735_DrawString(8,8, "      ", backgroundColor); // clear old text
	//ST7735_DrawString(0,1, " Clock (Numeric) ", ST7735_YELLOW);
	//render new text 
	ST7735_SetCursor(8,8);
	if(newHour < 10) {
		ST7735_OutUDec(0);
	}
	ST7735_OutUDec(newHour);
	ST7735_OutChar(':');
	if(newMinute < 10) {
		ST7735_OutUDec(0);
	}
	ST7735_OutUDec(newMinute);
}

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

void SetTimeMenu() {
	Output_Clear();
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_DrawString(2, 1, "Set Time:", clockColor);
	ST7735_DrawString(3, 2, "Hour: ", clockColor);
	ST7735_SetCursor(13, 2);
	if(oldHour < 10) {
		ST7735_OutUDec(0);
	}
	
	ST7735_OutUDec(oldHour);
	ST7735_DrawString(3, 3, "Minute: ", clockColor);
	ST7735_SetCursor(13,3);
	if(oldMinute < 10) {
		ST7735_OutUDec(0);
	}
	ST7735_OutUDec(oldMinute);
	DrawSimpleNumericalClock(oldHour, oldMinute);	
}

void SetAlarmMenu() {
	Output_Clear();
	ST7735_FillScreen(ST7735_BLACK);
	ST7735_DrawString(2, 1, "Set Alarm:", clockColor);
	ST7735_DrawString(3, 2, "Hour: ", clockColor);
	ST7735_SetCursor(13, 2);
	if(alarmHour < 10) {
		ST7735_OutUDec(0);
	}
	ST7735_OutUDec(alarmHour);
	ST7735_DrawString(3, 3, "Minute: ", clockColor);
	ST7735_SetCursor(13,3);
	if(alarmMinute < 10) {
		ST7735_OutUDec(0);
	}
	ST7735_OutUDec(alarmMinute);
	DrawSimpleNumericalClock(alarmHour, alarmMinute);
}

void SetColorMenu(){}
	
void ToggleColors(){
	colorScheme += 1 ;
	switch(colorScheme) {
		case 1:
			clockColor = ST7735_WHITE;
			minuteHandColor = ST7735_BLUE;
			hourHandColor = ST7735_GREEN;
			break;
		case 2:
			clockColor = ST7735_RED;
			minuteHandColor = ST7735_RED;
			hourHandColor = ST7735_RED;
			break;
		case 3:
			clockColor = ST7735_BLUE;
			minuteHandColor = ST7735_BLUE;
			hourHandColor = ST7735_BLUE;
			break;
		case 4:
			clockColor = ST7735_GREEN;
			minuteHandColor = ST7735_GREEN;
			hourHandColor = ST7735_GREEN;
			break;
		default:
			colorScheme = 1;
			clockColor = ST7735_WHITE;
			minuteHandColor = ST7735_BLUE;
			hourHandColor = ST7735_GREEN;
			break;	
	}
}
	
void SetMilitaryTime() {
	//isMilitaryTime = 1;
}
