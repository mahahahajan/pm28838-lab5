/* ESP8266 Driver Module */

#include "ESPDriver.h"


/* constants */

#define PF2       (*((volatile uint32_t *)0x40025010))


/* module fields */


extern uint8_t oldHour;
extern uint8_t oldMinute;

volatile uint8_t vp2val = 0;
volatile uint8_t vp3val = 0;
volatile uint8_t vp4val = 0;
volatile uint8_t vp5val = 0;
volatile uint8_t vp6val = 0;
volatile uint8_t vp7val = 0;
volatile uint32_t vp74val = 0;    // VP74
// These 6 variables contain the most recent Blynk to TM4C123 message
// Blynk to TM4C123 uses VP0 to VP15
char serial_buf[64];
char Pin_Number[2]   = "99";       // Initialize to invalid pin number
char Pin_Integer[8]  = "0000";     //
char Pin_Float[8]    = "0.0000";   //
uint32_t pin_num; 
uint32_t pin_int;


/* module internal functions */


 
// ----------------------------------- TM4C_to_Blynk ------------------------------
// Send data to the Blynk App
// It uses Virtual Pin numbers between 70 and 99
// so that the ESP8266 knows to forward the data to the Blynk App
void TM4C_to_Blynk(uint32_t pin,uint32_t value){
  if((pin < 70)||(pin > 99 && pin != 555)){
    return; // ignore illegal requests
  }
// your account will be temporarily halted 
  // if you send too much data
  ESP8266_OutUDec(pin);       // Send the Virtual Pin #
  ESP8266_OutChar(',');
  ESP8266_OutUDec(value);      // Send the current value
  ESP8266_OutChar(',');
  ESP8266_OutString("0.0\n");  // Null value not used in this example
}
 

void Blynk_to_TM4C_READ(void){int j; char data;

// Check to see if a there is data in the RXD buffer
  if(ESP8266_GetMessage(serial_buf)){  // returns false if no message
    // Read the data from the UART5
#ifdef DEBUG1
    j = 0;
    do{
      data = serial_buf[j];
      UART_OutChar(data);        // Debug only
      j++;
    }while(data != '\n');
    UART_OutChar('\r');        
#endif
           
// Rip the 3 fields out of the CSV data. The sequence of data from the 8266 is:
// Pin #, Integer Value, Float Value.
    strcpy(Pin_Number, strtok(serial_buf, ","));
    strcpy(Pin_Integer, strtok(NULL, ","));       // Integer value that is determined by the Blynk App
    strcpy(Pin_Float, strtok(NULL, ","));         // Not used
    pin_num = atoi(Pin_Number);     // Need to convert ASCII to integer
    pin_int = atoi(Pin_Integer);  
  // ---------------------------- VP #1 ----------------------------------------
  // This VP is the LED select button
    if(pin_num == 0x01)  {  
      //LED = pin_int;
      //PortF_Output(LED<<2); // Blue LED
			
			                          // Parse incoming data       
			
#ifdef DEBUG3
      Output_Color(ST7735_CYAN);
      ST7735_OutString("Rcv VP1 data=");
      ST7735_OutUDec(LED);
      ST7735_OutChar('\n');
#endif
    } else if (pin_num == 0x02) {  
			if (vp2val == 1 && pin_int == 0) {
				Switches_Handler(5);
				vp2val = 0;
			}
			if (pin_int == 1) vp2val = 1;
		} else if (pin_num == 0x03) {  
			if (vp3val == 1 && pin_int == 0) {
				Switches_Handler(4);
				vp3val = 0;
			}
			if (pin_int == 1) vp3val = 1;
		} else if (pin_num == 0x04) {  
			if (vp4val == 1 && pin_int == 0) {
				Switches_Handler(3);
				vp4val = 0;
			}
			if (pin_int == 1) vp4val = 1;
		} else if (pin_num == 0x05) {  
			if (vp5val == 1 && pin_int == 0) {
				Switches_Handler(2);
				vp5val = 0;
			}
			if (pin_int == 1) vp5val = 1;
		} else if (pin_num == 0x06) {  
			if (vp6val == 1 && pin_int == 0) {
				Switches_Handler(1);
				vp6val = 0;
			}
			if (pin_int == 1) vp6val = 1;
		} else if (pin_num == 555 || (pin_num >= 550 && pin_num <= 560)) {
			DrawHands(pin_int + 1, atoi(Pin_Float));
		}
#ifdef DEBUG1
    UART_OutString(" Pin_Number = ");
    UART_OutString(Pin_Number);
    UART_OutString("   Pin_Integer = ");
    UART_OutString(Pin_Integer);
    UART_OutString("   Pin_Float = ");
    UART_OutString(Pin_Float);
    UART_OutString("\n\r");
#endif
  }  
}

void Send_Information(void){
  uint32_t new_vp74val;
  new_vp74val = ADC0_InSeq3();
// your account will be temporarily halted if you send too much data
  if(new_vp74val != vp74val){
    TM4C_to_Blynk(74, new_vp74val);  // VP74
#ifdef DEBUG3
    Output_Color(ST7735_WHITE);
    ST7735_OutString("Send 74 data=");
    ST7735_OutUDec(thisF);
    ST7735_OutChar('\n');
#endif
  }
  vp74val = new_vp74val;
}


/* module external methods */
uint8_t esp_ready = 0;
void ESP_Init(void) {
	
#ifdef DEBUG1
  UART_Init(5);         // Enable Debug Serial Port
  UART_OutString("\n\rEE445L Lab 4D\n\r");
#endif
  ESP8266_Init();       // Enable ESP8266 Serial Port
  ESP8266_Reset();      // Reset the WiFi module
  ESP8266_SetupWiFi();  // Setup communications to Blynk Server  
	
	//ADC0_InitSWTriggerSeq3_Ch9();
	ADC0_InitSWTriggerSeq3(0);
  
  Timer_InitTask2(&Blynk_to_TM4C_READ);
	// check for receive data from Blynk App every 10ms
	
	Timer_InitTask3(&Send_Information);
	// Send data back to Blynk App every 1/2 second
	esp_ready = 1;
}

void ESP_Request_Time(void) {
	if (esp_ready) TM4C_to_Blynk(555, 1);
}
