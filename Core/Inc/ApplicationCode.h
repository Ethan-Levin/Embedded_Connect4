/*
 * ApplicationCode.h
 *
 *  Created on: Dec 30, 2023
 *      Author: Xavion
 */

#include "LCD_Driver.h"
#include "Scheduler.h"
#include "Button_Driver.h"
#include "stm32f4xx_hal.h"

#include <stdio.h>


#ifndef INC_APPLICATIONCODE_H_
#define INC_APPLICATIONCODE_H_

#define ONEPLAYER                 1
#define TWOPLAYER                 2

void ApplicationInit(void);
void LCD_Visual_Demo(void);

void LCD_Start_Screen();
void LCD_Select_Color_Screen();
void LCD_Game_Screen();
void LCD_Score_Screen();

void buttonInit();

void LCD_Polling_Mode();
void LCD_Polling_Color();
void LCD_Touch_Polling_Game();
void LCD_Polling_Restart();

#if (COMPILE_TOUCH_FUNCTIONS == 1)
void LCD_Touch_Polling_Demo(void);
#endif // (COMPILE_TOUCH_FUNCTIONS == 1)

#endif /* INC_APPLICATIONCODE_H_ */
