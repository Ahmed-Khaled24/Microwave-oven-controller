#include "LCD.h"
#include "LCD-Commands.h"
#include "../GPIO/GPIO_driver.h"
#include "../Systick/Systick.h"
#include "../Keypad/Keypad_interface.h"
#include "LCD_configuration.h"

#define validInput 			1
#define invalidInput 		0 

void LCD_vidSendCommand(uint8_t command) {
	// Select to write on command register [RS = 0]
	// According to datasheet time digram a delay of 40ns should be applied before set E = 1.
	GPIO_vidSetPinValue(LCD_PRS_PORT, LCD_PRS, Low);
	systick_vid1msDelay();

	// Apply the command bits to data bits D0->D7 of the LCD.
	// According to datasheet time digram a delay of 80ns should be applied before the falling edge of E.
	GPIO_vidSetPinValue(LCD_PD0_PORT, LCD_PD0, ((command) & 0x1));
	GPIO_vidSetPinValue(LCD_PD1_PORT, LCD_PD1, ((command >> 1) & 0x1));
	GPIO_vidSetPinValue(LCD_PD2_PORT, LCD_PD2, ((command >> 2) & 0x1));
	GPIO_vidSetPinValue(LCD_PD3_PORT, LCD_PD3, ((command >> 3) & 0x1));
	GPIO_vidSetPinValue(LCD_PD4_PORT, LCD_PD4, ((command >> 4) & 0x1));
	GPIO_vidSetPinValue(LCD_PD5_PORT, LCD_PD5, ((command >> 5) & 0x1));
	GPIO_vidSetPinValue(LCD_PD6_PORT, LCD_PD6, ((command >> 6) & 0x1));
	GPIO_vidSetPinValue(LCD_PD7_PORT, LCD_PD7, ((command >> 7) & 0x1));
	systick_vid1msDelay();

	// Create falling edge for the write operation to start
	// According to datasheet time digram the pulse width must be greater than 230ns
	// [E] --> Low to High
	GPIO_vidSetPinValue(LCD_PE_PORT, LCD_PE, High);
	systick_vid1msDelay();
	// [E] --> High to Low
	GPIO_vidSetPinValue(LCD_PE_PORT, LCD_PE, Low);
	systick_vid1msDelay();

	// If the command is ClearScreen or ResetCursor it will take 1.64ms to apply the command so,
	// a delay of 2ms should be applied, and for any other command it will take 40 microsecond.
	if (command == ClearScreen || command == ResetCursor)
		systick_vidDelay(2);
	else
		systick_vid1msDelay();
}

void LCD_vidScreenInit(void) {

	// Delay 15ms for the LCD to work properly [Datasheet instruction]
	systick_vidDelay(15);

	// Setup tiva pins
	// Initialize all pins.
	GPIO_DIO_vidPinInit(LCD_PD0_PORT, LCD_PD0);
	GPIO_DIO_vidPinInit(LCD_PD1_PORT, LCD_PD1);
	GPIO_DIO_vidPinInit(LCD_PD2_PORT, LCD_PD2);
	GPIO_DIO_vidPinInit(LCD_PD3_PORT, LCD_PD3);
	GPIO_DIO_vidPinInit(LCD_PD4_PORT, LCD_PD4);
	GPIO_DIO_vidPinInit(LCD_PD5_PORT, LCD_PD5);
	GPIO_DIO_vidPinInit(LCD_PD6_PORT, LCD_PD6);
	GPIO_DIO_vidPinInit(LCD_PD7_PORT, LCD_PD7);
	GPIO_DIO_vidPinInit(LCD_PRS_PORT, LCD_PRS);
	GPIO_DIO_vidPinInit(LCD_PE_PORT, LCD_PE);
	
	// Set all pins to output.
	GPIO_vidSetPinDirection(LCD_PD0_PORT, LCD_PD0, High);
	GPIO_vidSetPinDirection(LCD_PD1_PORT, LCD_PD1, High);
	GPIO_vidSetPinDirection(LCD_PD2_PORT, LCD_PD2, High);
	GPIO_vidSetPinDirection(LCD_PD3_PORT, LCD_PD3, High);
	GPIO_vidSetPinDirection(LCD_PD4_PORT, LCD_PD4, High);
	GPIO_vidSetPinDirection(LCD_PD5_PORT, LCD_PD5, High);
	GPIO_vidSetPinDirection(LCD_PD6_PORT, LCD_PD6, High);
	GPIO_vidSetPinDirection(LCD_PD7_PORT, LCD_PD7, High);
	GPIO_vidSetPinDirection(LCD_PRS_PORT, LCD_PRS, High);
	GPIO_vidSetPinDirection(LCD_PE_PORT, LCD_PE, High);

	// Setup the LCD.
	LCD_vidSendCommand(Set_8bit_5x7_2Line);
	LCD_vidSendCommand(ClearScreen);
	LCD_vidSendCommand(SetCursorGoRight);
	LCD_vidSendCommand(SetDisplayOnCursorOnBlinking);

}

void LCD_vidClearScreen(void) {
	LCD_vidSendCommand(ClearScreen);
}

void LCD_vidWriteChar(unsigned char c) {
	//set RS for data registers (Read/write) and wait for delay
	GPIO_vidSetPinValue(LCD_PRS_PORT, LCD_PRS, High);
	systick_vid1msDelay();

	//add character bits inputted in the pins D0 -> D7 and wait for delay
	GPIO_vidSetPinValue(LCD_PD0_PORT, LCD_PD0, ((c) & 0x1));
	GPIO_vidSetPinValue(LCD_PD1_PORT, LCD_PD1, ((c >> 1) & 0x1));
	GPIO_vidSetPinValue(LCD_PD2_PORT, LCD_PD2, ((c >> 2) & 0x1));
	GPIO_vidSetPinValue(LCD_PD3_PORT, LCD_PD3, ((c >> 3) & 0x1));
	GPIO_vidSetPinValue(LCD_PD4_PORT, LCD_PD4, ((c >> 4) & 0x1));
	GPIO_vidSetPinValue(LCD_PD5_PORT, LCD_PD5, ((c >> 5) & 0x1));
	GPIO_vidSetPinValue(LCD_PD6_PORT, LCD_PD6, ((c >> 6) & 0x1));
	GPIO_vidSetPinValue(LCD_PD7_PORT, LCD_PD7, ((c >> 7) & 0x1));
	systick_vid1msDelay();

	// Create falling edge for the write operation to start
    // According to datasheet time digram the pulse width must be greater than 230ns
    // [E] --> Low to High
	GPIO_vidSetPinValue(LCD_PE_PORT, LCD_PE, High);
	systick_vid1msDelay();
	// [E] --> High to Low
	GPIO_vidSetPinValue(LCD_PE_PORT, LCD_PE, Low);
	systick_vid1msDelay();
}

void LCD_vidWriteString(unsigned char* string, uint8_t stringSize) {
	uint8_t i;
	for (i = 0; i < stringSize; i++) {
		if (i == 16) LCD_vidSendCommand(GoToSecondLine);
		LCD_vidWriteChar(string[i]);
	}
}

// Starts countdown on screen with the timer given
void LCD_vidCountDown(int16_t timer, uint8_t isStandardForm) {
/* 
	you have to input the time as minutesSeconds, ex: 1234 is 12 minutes 34 seconds , 123 is 1 minutes 23 seconds
	or as a minutes only format as its converted by the MinToTimerStandard function ex:4.5 becomes 430.
*/ 
	int8_t min, sec;
	
	if (isStandardForm == 0) {
		timer = ((timer / 60) * 100) + (timer % 60); // Converting the seconds form to standard form needed here.
	}

	min = timer / 100, sec = (int16_t)timer % 100; 		// divide the lower 2 digits to seconds and upper 2 digits to minutes
					
	while (min > -1) { 									// keeps looping till the minutes reach 0
		while (sec > -1) { 								// keeps looping till the seconds reach 0
			int8_t temp = min;
			
			goToRowColumn(2, 10);						// To count in the bottom-right corner of the screen and keep the already wirtten data unchaged.
			LCD_vidWriteChar((temp / 10) + '0');
			LCD_vidWriteChar((temp % 10) + '0');
			LCD_vidWriteChar(':');
			if (sec < 10) { 							// If the seconds are in the units write a 0 before it
				LCD_vidWriteChar('0');
				LCD_vidWriteChar(sec + '0');
			}
			else { 										// If not write the string normally
				int8_t temp = sec;
				LCD_vidWriteChar((temp / 10) + '0');
				LCD_vidWriteChar((temp % 10) + '0');
			}
			
			systick_vidDelay(1000); 					// wait a second to decrement a unit digit in the seconds			
			sec--; 
			goToRowColumn(1, 1);
		}
		sec = 59;										// set the seconds to a whole minute
		min--;
	}
}

// These four functions to implement TakeInput().
// 1.
void printTimer(uint8_t* timer) {
	// Loop 4 times as the timer consist of 4 digits.
	uint8_t i;
	goToRowColumn(2 , 10) ;				// To take the input in the bottom-right corner of the screen and keep the already wirtten data unchaged.
	for (i = 0; i < 4; i++) {
		if (i == 2)   LCD_vidWriteChar(':');
		LCD_vidWriteChar(timer[i]);
	}
}
// 2. 
void shiftTimerLeft(uint8_t* timer) {
	uint8_t i;
	for (i = 1; i < 4; i++) {
		timer[i - 1] = timer[i];
	}
}

// 3.
uint16_t convertTimerToInt(uint8_t* timer){
	uint8_t i;
	uint16_t timerInt = 0;
	for (i = 0; i < 4; i++) {
		uint16_t currentDigit = timer[i] - '0';
		if (i == 0)
			timerInt += (currentDigit * 1000);
		else if (i == 1)
			timerInt += (currentDigit * 100);
		else if (i == 2)
			timerInt += (currentDigit * 10);
		else if (i == 3)
			timerInt += currentDigit;	
	}
	return timerInt;
}

// 4.
uint16_t validateInput(uint8_t* timer){
	// If the user inputed a valid number the function returns it, else it returns 0.	
	uint8_t i;
	uint16_t timerInt = 0;	
	uint16_t temp_validator = 0;
	
	// Check for letters.
	for(i = 0 ; i < 4; i++){
		if(timer[i] < '0' || timer[i] > '9')
			return timerInt;	
	}
	
	// Ensure that seconds <= 59.
	if(timer[2] > '5')
		return timerInt;
	
	// Ensure that timer in range (1 sec to 30 min)
	temp_validator = convertTimerToInt(timer);
	if( temp_validator < 1 || temp_validator > 3000)
		return timerInt;
	else 
		timerInt = temp_validator;
	
	return timerInt;
}


uint16_t LCD_u16TakeInput(void){
/* 
	 This function takes 4-digit (numbers) input from the user for example (1, 2, 3, 4)
	 Important note(1): user must enter the numbers from left to right 1 -> 2 -> 3 -> 4 
	 this produces the number 1234 which is 12 minutes and 34 seconds.
	 Important note(2): the 4-digit input must be less than or equal 3000, if bigger 
	 the function returns 0.
*/
		uint16_t timerInt = 0;
	 	uint8_t i;
		uint8_t currentInput;
		uint16_t InputResult; 
		
	// Create a timer.
	uint8_t timer[4] = {'0', '0', '0', '0'};
	
	// Print the current status of the timer to the screen [ 00:00 ].
	printTimer(timer);

	// Loop 4 times to read from the keypad.

	for (i = 0; i < 4; i++) {
		uint8_t pressedButton = '\0';
		
		// Wait till the user press a button
		while (pressedButton == '\0') {
			pressedButton = KEYPAD_u8GetButton();
		}

		// Convert pressedButton to char to print on the screen, then add it to the right most cell in the timer.
		currentInput = pressedButton ;
		timer[3] = currentInput;

		// Print the timer with the new input on the LCD. 
		// For example the user pressed 1 >>>> screen shows [ 00:01 ]
		
		printTimer(timer);

		// Shift the timer digits left to take the next input except for the last iteration.
		if (i < 3)
			shiftTimerLeft(timer);
	}
	
	InputResult = validateInput(timer);
	return InputResult;
}


// Cursor go to the provided location.
void goToRowColumn(uint8_t row, uint8_t column) {
	uint8_t i;
	LCD_vidSendCommand(ResetCursor);

	if(row == 2) 
		LCD_vidSendCommand(GoToSecondLine);

	for (i = 0; i < column-1; i++) 
		LCD_vidSendCommand(ShiftCursorRight);
}


