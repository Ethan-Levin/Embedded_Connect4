/*
 * ApplicationCode.c
 *
 *  Created on: Dec 30, 2023 (updated 11/12/2024) Thanks Donavon! 
 *      Author: Xavion
 */

#include "ApplicationCode.h"

/* Static variables */


extern void initialise_monitor_handles(void); 

#if COMPILE_TOUCH_FUNCTIONS == 1
static STMPE811_TouchData StaticTouchData;
#endif // COMPILE_TOUCH_FUNCTIONS

void ApplicationInit(void)
{
	initialise_monitor_handles(); // Allows printf functionality
    LTCD__Init();
    LTCD_Layer_Init(0);
    LCD_Clear(0,LCD_COLOR_WHITE);

    buttonInit();
    //DONT FORGOT TO REMOVE THIS LATER
    addSchedulerEvent(POLLING_GAME_EVENT);



    #if COMPILE_TOUCH_FUNCTIONS == 1
	InitializeLCDTouch();

	// This is the orientation for the board to be direclty up where the buttons are vertically above the screen
	// Top left would be low x value, high y value. Bottom right would be low x value, low y value.
	StaticTouchData.orientation = STMPE811_Orientation_Portrait_2;

	#endif // COMPILE_TOUCH_FUNCTIONS
}

void LCD_Visual_Demo(void)
{
	LCD_Draw_Game_Grid();
	//visualDemo();
}

void buttonInit(){
	Button_Init();
}

#if COMPILE_TOUCH_FUNCTIONS == 1
void LCD_Touch_Polling_Demo(void)
{
	LCD_Clear(0,LCD_COLOR_GREEN);
	while (1) {
		/* If touch pressed */
		if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed) {
			/* Touch valid */
			printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
			LCD_Clear(0, LCD_COLOR_RED);
		} else {
			/* Touch not pressed */
			printf("Not Pressed\n\n");
			LCD_Clear(0, LCD_COLOR_GREEN);
		}
	}
}
#endif // COMPILE_TOUCH_FUNCTIONS

void LCD_TOUCH_POLLING_DEMO(){
	while (1) {
			/* If touch pressed */
			if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed) {
				/* Touch valid */
				printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
				LCD_Clear(0, LCD_COLOR_RED);
			} else {
				/* Touch not pressed */
				printf("Not Pressed\n\n");
				LCD_Clear(0, LCD_COLOR_GREEN);
			}
		}
}

void LCD_Touch_Polling_Game(){
	/* If touch pressed */
	if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed) {
		/* Touch valid */
		if(StaticTouchData.x < LCD_PIXEL_WIDTH/2){
			//left side
			LCD_Update_Chip_To_Drop(LEFT);
		}
		else{
			//right side
			LCD_Update_Chip_To_Drop(RIGHT);
		}
	} else {
		/* Touch not pressed */
		//do nothing
	}
}


void EXTI0_IRQHandler(){
	HAL_NVIC_DisableIRQ(EXTI0_IRQn);
	LCD_Clear(0, LCD_COLOR_RED);
	__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}
