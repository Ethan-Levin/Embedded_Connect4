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

uint8_t playerMode        = 0;


void ApplicationInit(void)
{
	initialise_monitor_handles(); // Allows printf functionality
    LTCD__Init();
    LTCD_Layer_Init(0);
    LCD_Clear(0,LCD_COLOR_WHITE);

    buttonInit();
    addSchedulerEvent(START_MENU_EVENT);

    #if COMPILE_TOUCH_FUNCTIONS == 1
	InitializeLCDTouch();

	// This is the orientation for the board to be direclty up where the buttons are vertically above the screen
	// Top left would be low x value, high y value. Bottom right would be low x value, low y value.
	StaticTouchData.orientation = STMPE811_Orientation_Portrait_2;

	#endif // COMPILE_TOUCH_FUNCTIONS
}

void LCD_Visual_Demo(void)
{
	visualDemo();
}

void LCD_Start_Screen(){
	LCD_Draw_Start_Screen();
	addSchedulerEvent(POLLING_MODE_SELECT_EVENT);
	removeSchedulerEvent(START_MENU_EVENT);
}

void LCD_Select_Color_Screen(){
	LCD_Draw_Select_Color_Screen();
	removeSchedulerEvent(COLOR_SELECT_EVENT);
	addSchedulerEvent(POLLING_COLOR_SELECT_EVENT);
}

void LCD_Game_Screen(){
	LCD_Update_Game_Start_Time();
	//update the game starting time
	LCD_Draw_Game_Grid();
	removeSchedulerEvent(BUILD_NEW_GAME_EVENT);
	addSchedulerEvent(POLLING_GAME_EVENT);
}

void LCD_Score_Screen(){
	LCD_Draw_Score_Screen();
	removeSchedulerEvent(SCORE_SCREEN_EVENT);
	addSchedulerEvent(POLLING_RESTART_EVENT);
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


void LCD_Polling_Mode(){
	if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed) {
		/* Touch valid */
		if(StaticTouchData.x > LCD_PIXEL_WIDTH/2){
			//2 player mode
			playerMode = TWOPLAYER;
		}
		else{
			playerMode = ONEPLAYER;
		}
		removeSchedulerEvent(POLLING_MODE_SELECT_EVENT);
		addSchedulerEvent(COLOR_SELECT_EVENT);
	}
}

void LCD_Polling_Color(){
	if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed) {
			/* Touch valid */
			if(StaticTouchData.x < LCD_PIXEL_WIDTH/2){
				//left side
				LCD_Set_Player(PLAYER_RED);
			}
			else{
				//right side
				LCD_Set_Player(PLAYER_YELLOW);
			}
			removeSchedulerEvent(POLLING_COLOR_SELECT_EVENT);
			addSchedulerEvent(BUILD_NEW_GAME_EVENT);
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
	}
	/* Touch not pressed */
	//do nothing
}

void LCD_Polling_Restart(){
	if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed) {
		/* Touch valid */
		if(StaticTouchData.y < LCD_PIXEL_HEIGHT/2){
			//Bottom half
			removeSchedulerEvent(POLLING_RESTART_EVENT);
			addSchedulerEvent(START_MENU_EVENT);
		}
	}
}



void EXTI0_IRQHandler(){
	HAL_NVIC_DisableIRQ(EXTI0_IRQn);

	uint32_t eventsToRun = getScheduledEvents();
	//only want to do stuff if the game screen is active
	if(eventsToRun & POLLING_GAME_EVENT){
		LCD_Insert_Chip_Game_Grid();
		//allows for you to insert a chip if the game is active
    }
	__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}
