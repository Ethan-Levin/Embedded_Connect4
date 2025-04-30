/*
 * LCD_Driver.h
 *
 *  Created on: Sep 28, 2023
 *      Author: Xavion
 */

#ifndef INC_LCD_DRIVER_H_
#define INC_LCD_DRIVER_H_

#include "stm32f4xx_hal.h"
#include "ili9341.h"
#include "fonts.h"
#include "stmpe811.h"

#include "Scheduler.h"
#include <stdbool.h>
#include <stdio.h>

#define COMPILE_TOUCH_FUNCTIONS COMPILE_TOUCH
#define TOUCH_INTERRUPT_ENABLED COMPILE_TOUCH_INTERRUPT_SUPPORT

/**
  * @brief  LCD color RGB565
  */

#define LCD_PIXEL_FORMAT_1     LTDC_PIXEL_FORMAT_RGB565

#define LCD_COLOR_WHITE         0xFFFF
#define LCD_COLOR_BLACK         0x0000
#define LCD_COLOR_GREY          0xF7DE
#define LCD_COLOR_BLUE          0x001F
#define LCD_COLOR_BLUE2         0x051F
#define LCD_COLOR_RED           0xF800
#define LCD_COLOR_MAGENTA       0xF81F
#define LCD_COLOR_GREEN         0x07E0
#define LCD_COLOR_CYAN          0x7FFF
#define LCD_COLOR_YELLOW        0xFFE0

#define SQUARE_SIZE             34
#define ROWS					6
#define COLUMNS					7
#define GRID_OFFSET_VERTICAL    (LCD_PIXEL_WIDTH-COLUMNS*SQUARE_SIZE)
#define GRID_OFFSET_HORIZONTAL  (LCD_PIXEL_HEIGHT-ROWS*SQUARE_SIZE)

#define CIRCLE_RADIUS           12

#define RIGHT_MOST_COLUMN       6
#define LEFT_MOST_COLUMN        0

#define PLAYER_EMPTY            0
#define PLAYER_YELLOW           1
#define PLAYER_RED            	2

#define CHIP_Y_POS				((LCD_PIXEL_HEIGHT-SQUARE_SIZE*ROWS)/2)
#define CHIP_X_POS(COLUMN)		(SQUARE_SIZE/2 + SQUARE_SIZE * COLUMN)
#define CHIP_X_START_COLUMN		3

#define GRID_BACKGROUND_YPOS    (LCD_PIXEL_HEIGHT-SQUARE_SIZE*ROWS)

#define RIGHT                   0
#define LEFT                    1

#define MODE_YPOS               20

#define PICK_YPOS               20
#define COLOR_YPOS              40

#define SCORE_YPOS              80
#define TIMER_YPOS              20
#define NEW_YPOS                230
#define GAME_YPOS               250

#define RESTART_BUTTON_YPOS     200

/* Timing configuration from datahseet
  HSYNC=10 (9+1)
  HBP=20 (29-10+1)
  ActiveW=240 (269-20-10+1)
  HFP=10 (279-240-20-10+1)

  VSYNC=2 (1+1)
  VBP=2 (3-2+1)
  ActiveH=320 (323-2-2+1)
  VFP=4 (327-320-2-2+1)
*/
#define  ILI9341_HSYNC            ((uint32_t)9)   /* Horizontal synchronization */
#define  ILI9341_HBP              ((uint32_t)29)    /* Horizontal back porch      */
#define  ILI9341_HFP              ((uint32_t)2)    /* Horizontal front porch     */
#define  ILI9341_VSYNC            ((uint32_t)1)   /* Vertical synchronization   */
#define  ILI9341_VBP              ((uint32_t)3)    /* Vertical back porch        */
#define  ILI9341_VFP              ((uint32_t)2)    /* Vertical front porch       */
#define  LCD_PIXEL_WIDTH    ((uint16_t)240)
#define  LCD_PIXEL_HEIGHT   ((uint16_t)320)
#define  LCD_PIXELS		     ((uint32_t)LCD_PIXEL_WIDTH * (uint32_t)LCD_PIXEL_HEIGHT)

typedef struct{
	uint8_t xPos;
	uint8_t yPos;
	uint8_t playerColor;
}grid_t;

typedef struct{
	uint8_t column;
	uint8_t xPos;
	uint8_t yPos;
}chip_to_drop_t;


void LTCD__Init(void);
void LTCD_Layer_Init(uint8_t LayerIndex);

void LCD_DrawChar(uint16_t Xpos, uint16_t Ypos, const uint16_t *c);
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii);
void LCD_SetTextColor(uint16_t Color);
void LCD_SetFont(FONT_t *fonts);



void LCD_Draw_Rectangle_Fill(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint16_t Height, uint16_t color);
// Draw Circle Filled
void LCD_Draw_Circle_Fill(uint16_t Xpos, uint16_t Ypos, uint16_t radius, uint16_t color);

// Draw Vertical Line
void LCD_Draw_Vertical_Line(uint16_t x, uint16_t y, uint16_t len, uint16_t color);

// Draw Horizontal Line
void LCD_Draw_Horizontal_Line(uint16_t x, uint16_t y, uint16_t len, uint16_t color);

void LCD_Clear(uint8_t LayerIndex, uint16_t Color);

void LCD_Error_Handler(void);

// Demo using provided functions
void visualDemo(void);

bool LCD_Game_Won_Check_Row(uint8_t row, uint8_t player);
bool LCD_Game_Won_Check_Column(uint8_t column, uint8_t player);
bool LCD_Game_Won_Check_Up_Right_Diagonal(uint8_t column, uint8_t row, uint8_t player);
bool LCD_Game_Won_Check_Up_Left_Diagonal(uint8_t column, uint8_t row, uint8_t player);
bool LCD_Game_won(uint8_t column, uint8_t row, uint8_t player);

bool LCD_Game_Tie();
//detects if a tie has occured

void LCD_Animate_Falling(uint16_t Xpos, uint16_t Ypos, uint16_t radius, uint16_t color);

void LCD_Get_Pixel_Color(uint16_t x, uint16_t y);
void LCD_DRAW_CIRCLE_SKIP_BLUE_BLACK(uint16_t Xpos, uint16_t Ypos, uint16_t radius, uint16_t color);

void LCD_Draw_Pixel(uint16_t x, uint16_t y, uint16_t color);

void LCD_Set_Player(uint8_t color);
uint8_t LCD_Get_Player();

void LCD_Draw_Start_Screen();
void LCD_Draw_Select_Color_Screen();
void LCD_Draw_Score_Screen();

void LCD_Draw_Game_Grid();
void Init_Grid();

uint8_t LCD_Find_Best_Move();


bool LCD_Space_Available_Game_Grid();
uint8_t LCD_Get_Row_Game_Grid();

void LCD_Insert_Chip_Game_Grid();

//Chip_To_Drop functions
void Init_Chip_To_Drop();
void LCD_Draw_Chip_To_Drop();
void LCD_Clear_Chip_To_Drop();
void LCD_Update_Chip_To_Drop_Column(int direction);
void LCD_Update_Chip_To_Drop(int direction);
uint8_t LCD_Get_Chip_To_Drop_Column();

//get Time functions
void LCD_Update_Game_Start_Time();
void LCD_Update_Game_End_Time();
void LCD_Update_Game_Length_Time();


void LCD_Error_Handler(void);

#if COMPILE_TOUCH_FUNCTIONS == 1
void InitializeLCDTouch(void);
STMPE811_State_t returnTouchStateAndLocation(STMPE811_TouchData * touchStruct);
void LCD_Touch_Polling_Demo(void);
void DetermineTouchPosition(STMPE811_TouchData * touchStruct);
uint8_t ReadRegisterFromTouchModule(uint8_t RegToRead);
void WriteDataToTouchModule(uint8_t RegToWrite, uint8_t writeData);

#endif // COMPILE_TOUCH_FUNCTIONS


/*        APPLICATION SPECIFIC FUNCTION DECLARATION - PUT YOUR NEWLY CREATED FUNCTIONS HERE       */


#endif /* INC_LCD_DRIVER_H_ */
