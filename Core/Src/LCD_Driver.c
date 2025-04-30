/*
 * LCD_Driver.c
 *
 *  Created on: Sep 28, 2023
 *      Author: Xavion
 */

#include "LCD_Driver.h"

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */

static LTDC_HandleTypeDef hltdc;
static RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
static FONT_t *LCD_Currentfonts;
static uint16_t CurrentTextColor   = 0xFFFF;


/*
 * fb[y*W+x] OR fb[y][x]
 * Alternatively, we can modify the linker script to have an end address of 20013DFB instead of 2002FFFF, so it does not place variables in the same region as the frame buffer. In this case it is safe to just specify the raw address as frame buffer.
 */
//uint32_t frameBuffer[(LCD_PIXEL_WIDTH*LCD_PIXEL_WIDTH)/2] = {0};		//16bpp pixel format. We can size to uint32. this ensures 32 bit alignment


//Someone from STM said it was "often accessed" a 1-dim array, and not a 2d array. However you still access it like a 2dim array,  using fb[y*W+x] instead of fb[y][x].
uint16_t frameBuffer[LCD_PIXEL_WIDTH*LCD_PIXEL_HEIGHT] = {0};			//16bpp pixel format.

static grid_t grid[7][6];
static chip_to_drop_t chip;
uint8_t playerTurn        = PLAYER_RED;
uint8_t redScore          = 0;
uint8_t yellowScore       = 0;
uint16_t color_of_pixel   = 0;
uint32_t gameStartTime    = 0;
uint32_t gameEndTime      = 0;
uint32_t gameLengthTime   = 0;

void LCD_Animate_Falling(uint16_t Xpos, uint16_t Ypos, uint16_t radius, uint16_t color){
	for(int i = chip.yPos; i<Ypos; i++){
		LCD_DRAW_CIRCLE_SKIP_BLUE_BLACK(Xpos, i, radius, LCD_COLOR_WHITE);
		LCD_DRAW_CIRCLE_SKIP_BLUE_BLACK(Xpos, i+1, radius, color);
		//HAL_Delay(5);
		//seems to break the code if you include the delay
	}
}

void LCD_Update_Game_Start_Time(){
	gameStartTime = HAL_GetTick();
}

void LCD_Update_Game_End_Time(){
	gameEndTime = HAL_GetTick();
}

void LCD_Update_Game_Length_Time(){
	gameLengthTime = gameEndTime - gameStartTime;
}

void LCD_Set_Player(uint8_t color){
	playerTurn = color;
}

uint8_t LCD_Get_Player(){
	return playerTurn;
}

uint8_t LCD_Find_Best_Move(){

}

void Init_Chip_To_Drop(){
	chip.yPos        = CHIP_Y_POS;
	chip.xPos        = CHIP_X_POS(CHIP_X_START_COLUMN);
	chip.column      = CHIP_X_START_COLUMN;
	LCD_Draw_Chip_To_Drop();
}

void Init_Grid(){
	for(uint8_t i = 0; i<COLUMNS; i++){
		for(uint8_t j =0; j<ROWS; j++){
			grid[i][j].xPos = SQUARE_SIZE/2+SQUARE_SIZE*i;
			grid[i][j].yPos = SQUARE_SIZE/2 + SQUARE_SIZE*j;
			grid[i][j].playerColor = PLAYER_EMPTY;
			//this clears any old information about the colors from the previous game
		}
	}
}


void LCD_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable the LTDC clock */
  __HAL_RCC_LTDC_CLK_ENABLE();

  /* Enable GPIO clock */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /* GPIO Config
   *
    LCD pins
   LCD_TFT R2 <-> PC.10
   LCD_TFT G2 <-> PA.06
   LCD_TFT B2 <-> PD.06
   LCD_TFT R3 <-> PB.00
   LCD_TFT G3 <-> PG.10
   LCD_TFT B3 <-> PG.11
   LCD_TFT R4 <-> PA.11
   LCD_TFT G4 <-> PB.10
   LCD_TFT B4 <-> PG.12
   LCD_TFT R5 <-> PA.12
   LCD_TFT G5 <-> PB.11
   LCD_TFT B5 <-> PA.03
   LCD_TFT R6 <-> PB.01
   LCD_TFT G6 <-> PC.07
   LCD_TFT B6 <-> PB.08
   LCD_TFT R7 <-> PG.06
   LCD_TFT G7 <-> PD.03
   LCD_TFT B7 <-> PB.09
   LCD_TFT HSYNC <-> PC.06
   LCDTFT VSYNC <->  PA.04
   LCD_TFT CLK   <-> PG.07
   LCD_TFT DE   <->  PF.10
  */

  /* GPIOA configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6 |
                           GPIO_PIN_11 | GPIO_PIN_12;
  GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pull = GPIO_NOPULL;
  GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
  GPIO_InitStructure.Alternate= GPIO_AF14_LTDC;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

 /* GPIOB configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_8 | \
                           GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

 /* GPIOC configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_10;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

 /* GPIOD configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_3 | GPIO_PIN_6;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

 /* GPIOF configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_10;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStructure);

 /* GPIOG configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | \
                           GPIO_PIN_11;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);

  /* GPIOB configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStructure.Alternate= GPIO_AF9_LTDC;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* GPIOG configuration */
  GPIO_InitStructure.Pin = GPIO_PIN_10 | GPIO_PIN_12;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStructure);
}

void LTCD_Layer_Init(uint8_t LayerIndex)
{
	LTDC_LayerCfgTypeDef  pLayerCfg;

	pLayerCfg.WindowX0 = 0;	//Configures the Window HORZ START Position.
	pLayerCfg.WindowX1 = LCD_PIXEL_WIDTH;	//Configures the Window HORZ Stop Position.
	pLayerCfg.WindowY0 = 0;	//Configures the Window vertical START Position.
	pLayerCfg.WindowY1 = LCD_PIXEL_HEIGHT;	//Configures the Window vertical Stop Position.
	pLayerCfg.PixelFormat = LCD_PIXEL_FORMAT_1;  //INCORRECT PIXEL FORMAT WILL GIVE WEIRD RESULTS!! IT MAY STILL WORK FOR 1/2 THE DISPLAY!!! //This is our buffers pixel format. 2 bytes for each pixel
	pLayerCfg.Alpha = 255;
	pLayerCfg.Alpha0 = 0;
	pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
	pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
	if (LayerIndex == 0){
		pLayerCfg.FBStartAdress = (uintptr_t)frameBuffer;
	}
	pLayerCfg.ImageWidth = LCD_PIXEL_WIDTH;
	pLayerCfg.ImageHeight = LCD_PIXEL_HEIGHT;
	pLayerCfg.Backcolor.Blue = 0;
	pLayerCfg.Backcolor.Green = 0;
	pLayerCfg.Backcolor.Red = 0;
	if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, LayerIndex) != HAL_OK)
	{
		LCD_Error_Handler();
	}
}

void clearScreen(void)
{
  LCD_Clear(0,LCD_COLOR_WHITE);
}

void LTCD__Init(void)
{
	hltdc.Instance = LTDC;
	/* Configure horizontal synchronization width */
	hltdc.Init.HorizontalSync = ILI9341_HSYNC;
	/* Configure vertical synchronization height */
	hltdc.Init.VerticalSync = ILI9341_VSYNC;
	/* Configure accumulated horizontal back porch */
	hltdc.Init.AccumulatedHBP = ILI9341_HBP;
	/* Configure accumulated vertical back porch */
	hltdc.Init.AccumulatedVBP = ILI9341_VBP;
	/* Configure accumulated active width */
	hltdc.Init.AccumulatedActiveW = 269;
	/* Configure accumulated active height */
	hltdc.Init.AccumulatedActiveH = 323;
	/* Configure total width */
	hltdc.Init.TotalWidth = 279;
	/* Configure total height */
	hltdc.Init.TotalHeigh = 327;
	/* Configure R,G,B component values for LCD background color */
	hltdc.Init.Backcolor.Red = 0;
	hltdc.Init.Backcolor.Blue = 0;
	hltdc.Init.Backcolor.Green = 0;

	/* LCD clock configuration */
	/* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
	/* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAIN = 192 Mhz */
	/* PLLLCDCLK = PLLSAI_VCO Output/PLLSAIR = 192/4 = 48 Mhz */
	/* LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_8 = 48/4 = 6Mhz */

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
	PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
	PeriphClkInitStruct.PLLSAI.PLLSAIR = 4;
	PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_8;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
	/* Polarity */
	hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
	hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
	hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
	hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;

	LCD_GPIO_Init();

	if (HAL_LTDC_Init(&hltdc) != HAL_OK)
	 {
	   LCD_Error_Handler();
	 }

	ili9341_Init();
}

/* START Draw functions */


/*
 * This is really the only function needed.
 * All drawing consists of is manipulating the array.
 * Adding input sanitation should probably be done.
 */
void LCD_Draw_Chip_To_Drop(){
	if(playerTurn == PLAYER_RED){
		LCD_Draw_Circle_Fill(chip.xPos, chip.yPos, CIRCLE_RADIUS, LCD_COLOR_RED);
	}
	else if(playerTurn == PLAYER_YELLOW){
		LCD_Draw_Circle_Fill(chip.xPos, chip.yPos, CIRCLE_RADIUS, LCD_COLOR_YELLOW);
	}
	//if nothing is updating then there is an error with playerTurn
}

void LCD_Clear_Chip_To_Drop(){
	LCD_Draw_Circle_Fill(chip.xPos, chip.yPos, CIRCLE_RADIUS, LCD_COLOR_WHITE);
}

uint8_t LCD_Get_Chip_To_Drop_Column(){
	return chip.column;
}

void LCD_Update_Chip_To_Drop_Column(int direction){
	if(direction == RIGHT && chip.column != RIGHT_MOST_COLUMN){
		//if its the 7th most do nothing
		chip.column = chip.column + 1;
		chip.xPos   = CHIP_X_POS(chip.column);
	}
	else if(direction == LEFT && chip.column != LEFT_MOST_COLUMN){
		//if its the 1st column do nothing
		chip.column = chip.column - 1;
		chip.xPos   = CHIP_X_POS(chip.column);
	}
}

void LCD_Update_Chip_To_Drop(int dir){
	LCD_Clear_Chip_To_Drop();
	LCD_Update_Chip_To_Drop_Column(dir);
	LCD_Draw_Chip_To_Drop();
}

void LCD_DRAW_CIRCLE_SKIP_BLUE_BLACK(uint16_t Xpos, uint16_t Ypos, uint16_t radius, uint16_t color){
	for(int16_t y=-radius; y<=radius; y++)
	    {
	        for(int16_t x=-radius; x<=radius; x++)
	        {
	        	LCD_Get_Pixel_Color(x+Xpos, y+Ypos);
	            if(x*x+y*y <= radius*radius)
	            {
	            	if(color_of_pixel != LCD_COLOR_BLUE && color_of_pixel != LCD_COLOR_BLACK){
	            		//ignore background grid and lines
	            		LCD_Draw_Pixel(x+Xpos, y+Ypos, color);
	            	}
	            }
	        }
	    }
}

void LCD_Draw_Pixel(uint16_t x, uint16_t y, uint16_t color)
{
	frameBuffer[y*LCD_PIXEL_WIDTH+x] = color;  //You cannot do x*y to set the pixel.
}

void LCD_Get_Pixel_Color(uint16_t x, uint16_t y){
	color_of_pixel = frameBuffer[y*LCD_PIXEL_WIDTH+x];  //Returns the pixel color
}
/*
 * These functions are simple examples. Most computer graphics like OpenGl and stm's graphics library use a state machine. Where you first call some function like SetColor(color), SetPosition(x,y), then DrawSqure(size)
 * Instead all of these are explicit where color, size, and position are passed in.
 * There is tons of ways to handle drawing. I dont think it matters too much.
 */
void LCD_Draw_Circle_Fill(uint16_t Xpos, uint16_t Ypos, uint16_t radius, uint16_t color)
{
    for(int16_t y=-radius; y<=radius; y++)
    {
        for(int16_t x=-radius; x<=radius; x++)
        {
            if(x*x+y*y <= radius*radius)
            {
				LCD_Draw_Pixel(x+Xpos, y+Ypos, color);
            }
        }
    }
}

void LCD_Draw_Rectangle_Fill(uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint16_t Height, uint16_t color){
	for(int16_t y=0; y<Height; y++){
		for(int16_t x=0; x<Length; x++){
			LCD_Draw_Pixel(x+Xpos, y+Ypos, color);
		}
	}
}

void LCD_Draw_Vertical_Line(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
  for (uint16_t i = 0; i < len; i++)
  {
	  LCD_Draw_Pixel(x, i+y, color);
  }
}

void LCD_Draw_Horizontal_Line(uint16_t x, uint16_t y, uint16_t len, uint16_t color)
{
  for (uint16_t i = 0; i < len; i++)
  {
	  LCD_Draw_Pixel(i+x, y, color);
  }
}

void LCD_Clear(uint8_t LayerIndex, uint16_t Color)
{
	if (LayerIndex == 0){
		for (uint32_t i = 0; i < LCD_PIXEL_WIDTH * LCD_PIXEL_HEIGHT; i++){
			frameBuffer[i] = Color;
		}
	}
  // TODO: Add more Layers if needed
}

//This was taken and adapted from stm32's mcu code
void LCD_SetTextColor(uint16_t Color)
{
  CurrentTextColor = Color;
}

//This was taken and adapted from stm32's mcu code
void LCD_SetFont(FONT_t *fonts)
{
  LCD_Currentfonts = fonts;
}

//This was taken and adapted from stm32's mcu code
void LCD_Draw_Char(uint16_t Xpos, uint16_t Ypos, const uint16_t *c)
{
  uint32_t index = 0, counter = 0;
  for(index = 0; index < LCD_Currentfonts->Height; index++)
  {
    for(counter = 0; counter < LCD_Currentfonts->Width; counter++)
    {
      if((((c[index] & ((0x80 << ((LCD_Currentfonts->Width / 12 ) * 8 ) ) >> counter)) == 0x00) && (LCD_Currentfonts->Width <= 12)) || (((c[index] & (0x1 << counter)) == 0x00)&&(LCD_Currentfonts->Width > 12 )))
      {
         //Background If want to overrite text under then add a set color here
      }
      else
      {
    	  LCD_Draw_Pixel(counter + Xpos,index + Ypos,CurrentTextColor);
      }
    }
  }
}

//This was taken and adapted from stm32's mcu code
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii)
{
  Ascii -= 32;
  LCD_Draw_Char(Xpos, Ypos, &LCD_Currentfonts->table[Ascii * LCD_Currentfonts->Height]);
}

bool LCD_Game_Tie(){
	for(int i = 0; i<7; i++){
		//go through each of the the top most slots and see if its filled, if its not filled return false
		if(grid[i][5].playerColor == PLAYER_EMPTY){
			//if a slot is empty then there is no way for a tie yet
			return false;
		}
	}
	LCD_Update_Game_End_Time();
	//stop the clock as a tie occured
	return true;
	//if all are filled return true
}

bool LCD_Space_Available_Game_Grid(){
	if(grid[chip.column][5].playerColor != PLAYER_EMPTY){
	//5 is the highest row so we need to see if its occupied and if it is false
		return false;
	}
	//if there is space return true
	return true;
}

uint8_t LCD_Get_Row_Game_Grid(){
	for(int j = 0; j<ROWS; j++){
		if(grid[chip.column][j].playerColor == PLAYER_EMPTY){
			//returns the first empty square
			return j;
		}
	}
	return -1; //out of bounds but should not occur
}


bool LCD_Game_Won_Check_Row(uint8_t row, uint8_t player){
	uint8_t Consecutive = 0;
	//Resets consecutive
	for(int i=0; i<COLUMNS; i++){
		if(grid[i][row].playerColor == player){
			Consecutive++;
		}
		else{
			Consecutive = 0;
		}

		if(Consecutive == 4){
			return true;
		}
	}

	return false;
}

bool LCD_Game_Won_Check_Column(uint8_t column, uint8_t player){
	uint8_t Consecutive = 0;
	for(int j=0; j<ROWS; j++){
		if(grid[column][j].playerColor == player){
			Consecutive++;
		}
		else{
			Consecutive = 0;
		}

		if(Consecutive == 4){
			return true;
		}
	}

	return false;
}


bool LCD_Game_Won_Check_Up_Right_Diagonal(uint8_t column, uint8_t row, uint8_t player){
	uint8_t Consecutive = 0;
	//need to find the bottom left spot and will go up right from there
	while( (row  > 0) && (column > 0) ){
		//look until we get to the bottom left
		row--;
		column--;
	}

	while(row < ROWS && column < COLUMNS){
		//keep going until we encounter top right edge
		if(grid[column][row].playerColor == player){
			Consecutive++;
		}
		else{
			Consecutive = 0;
		}

		if(Consecutive == 4){
			return true;
		}
		row++;
		column++;
	}
	return false;
}

bool LCD_Game_Won_Check_Up_Left_Diagonal(uint8_t column, uint8_t row, uint8_t player){
	uint8_t Consecutive = 0;
	//need to find the bottom left spot and will go up right from there
	while( (row > 0) && (column + 1 < COLUMNS) ){
		//loop until bottom right, stop once we might go out of bounds (-1 or COLUMNS)
		row--;
		column++;
	}

	while(row < ROWS && column >= 0){
		//keep going until we encounter top right edge
		if(grid[column][row].playerColor == player){
			Consecutive++;
		}
		else{
			Consecutive = 0;
		}

		if(Consecutive == 4){
			return true;
		}
		row++;
		column--;
	}
	return false;
}

bool LCD_Game_Won(uint8_t column, uint8_t row, uint8_t player){
	//check all adjacent pieces colors
	if(LCD_Game_Won_Check_Row(row, player)){
		LCD_Update_Game_End_Time();
		//Get the game length as soon as a win is detected
		return true;
	}
	else if(LCD_Game_Won_Check_Column(column, player)){
		LCD_Update_Game_End_Time();
		//Get the game length as soon as a win is detected
		return true;
	}
	else if(LCD_Game_Won_Check_Up_Right_Diagonal(column, row, player)){
		LCD_Update_Game_End_Time();
		//Get the game length as soon as a win is detected
		return true;
	}
	else if(LCD_Game_Won_Check_Up_Left_Diagonal(column, row, player)){
		LCD_Update_Game_End_Time();
		//Get the game length as soon as a win is detected
		return true;
	}
return false;
}

void LCD_Insert_Chip_Game_Grid(){
	if((LCD_Space_Available_Game_Grid())){
		//check for space
		uint8_t row = LCD_Get_Row_Game_Grid();
		//returns the highest row for a column
		if(playerTurn == PLAYER_RED){
			//if its Reds turn it will draw red
			//LCD_Draw_Circle_Fill(grid[chip.column][row].xPos, LCD_PIXEL_HEIGHT-grid[chip.column][row].yPos, CIRCLE_RADIUS, LCD_COLOR_RED);
			LCD_Animate_Falling(grid[chip.column][row].xPos, LCD_PIXEL_HEIGHT-grid[chip.column][row].yPos, CIRCLE_RADIUS, LCD_COLOR_RED);
			grid[chip.column][row].playerColor = PLAYER_RED;
			playerTurn = PLAYER_YELLOW;
			//update playerTurn to be other player
			LCD_Draw_Chip_To_Drop();
			//Update the display for the chip to drop
			if(LCD_Game_Won(chip.column, row, PLAYER_RED)){
				redScore++;
				removeSchedulerEvent(POLLING_GAME_EVENT);
				addSchedulerEvent(SCORE_SCREEN_EVENT);
			}
		}
		else{
			//if its Yellows turn it will draw yellow
			//LCD_Draw_Circle_Fill(grid[chip.column][row].xPos, LCD_PIXEL_HEIGHT-grid[chip.column][row].yPos, CIRCLE_RADIUS, LCD_COLOR_YELLOW);
			LCD_Animate_Falling(grid[chip.column][row].xPos, LCD_PIXEL_HEIGHT-grid[chip.column][row].yPos, CIRCLE_RADIUS, LCD_COLOR_YELLOW);
			grid[chip.column][row].playerColor = PLAYER_YELLOW;
			//update the memory of the grid
			playerTurn = PLAYER_RED;
			//update playerTurn to be c other player
			LCD_Draw_Chip_To_Drop();
			//Update the display for the chip to drop
			if(LCD_Game_Won(chip.column, row, PLAYER_YELLOW)){
				yellowScore++;
				removeSchedulerEvent(POLLING_GAME_EVENT);
				addSchedulerEvent(SCORE_SCREEN_EVENT);
			}			//Check if game is over
		}
		if(LCD_Game_Tie()){
			removeSchedulerEvent(POLLING_GAME_EVENT);
			addSchedulerEvent(SCORE_SCREEN_EVENT);
			//go to next screen if a tie occurs
		}
	}

}
void LCD_Draw_Start_Screen(){
	LCD_Clear(0, LCD_COLOR_WHITE);

	LCD_Draw_Vertical_Line(LCD_PIXEL_WIDTH/2,40,LCD_PIXEL_HEIGHT-40,LCD_COLOR_BLACK);
	LCD_Draw_Horizontal_Line(0,40,LCD_PIXEL_WIDTH,LCD_COLOR_BLACK);

	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	LCD_DisplayChar(80+2,  MODE_YPOS,'M');
	LCD_DisplayChar(100,   MODE_YPOS,'O');
	LCD_DisplayChar(120-2, MODE_YPOS,'D');
	LCD_DisplayChar(135-2, MODE_YPOS,'E');

	LCD_DisplayChar(50, 160,'1');
	LCD_DisplayChar(180, 160,'2');
}

void LCD_Draw_Select_Color_Screen(){
	LCD_Clear(0, LCD_COLOR_RED);

	LCD_Draw_Vertical_Line(LCD_PIXEL_WIDTH/2,0,LCD_PIXEL_HEIGHT,LCD_COLOR_BLACK);

	LCD_Draw_Rectangle_Fill(LCD_PIXEL_WIDTH/2, 0, LCD_PIXEL_WIDTH/2, LCD_PIXEL_HEIGHT, LCD_COLOR_YELLOW);

	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	LCD_DisplayChar(95,  PICK_YPOS,'P');
	LCD_DisplayChar(110, PICK_YPOS,'I');
	LCD_DisplayChar(125, PICK_YPOS,'C');
	LCD_DisplayChar(140, PICK_YPOS,'K');

	LCD_DisplayChar(90,  COLOR_YPOS,'C');
	LCD_DisplayChar(105, COLOR_YPOS,'O');
	LCD_DisplayChar(120, COLOR_YPOS,'L');
	LCD_DisplayChar(135, COLOR_YPOS,'O');
	LCD_DisplayChar(150, COLOR_YPOS,'R');

}

void LCD_Draw_Game_Grid(){
	LCD_Clear(0, LCD_COLOR_WHITE);
	//Clears screen

	Init_Grid();
	Init_Chip_To_Drop();

	LCD_Draw_Rectangle_Fill(0, GRID_BACKGROUND_YPOS, LCD_PIXEL_WIDTH, SQUARE_SIZE*ROWS, LCD_COLOR_BLUE);

	for(uint8_t i=1; i<7; i++){
		LCD_Draw_Vertical_Line(SQUARE_SIZE*i,GRID_OFFSET_HORIZONTAL,SQUARE_SIZE*ROWS,LCD_COLOR_BLACK);
		//each tile is 34x34 so 180 is 6 height
		//15 is the offset from the edge, 34 is the size
	}
	for(uint8_t i =1; i<7; i++){
		LCD_Draw_Horizontal_Line(0,LCD_PIXEL_HEIGHT-(SQUARE_SIZE*i),LCD_PIXEL_WIDTH,LCD_COLOR_BLACK);
		//want each horizontal line to go across the screen
	}
	for(uint8_t i = 0; i<COLUMNS; i++){
		for(uint8_t j =0; j<ROWS; j++){
			LCD_Draw_Circle_Fill(grid[i][j].xPos, LCD_PIXEL_HEIGHT-grid[i][j].yPos, CIRCLE_RADIUS, LCD_COLOR_WHITE);
		}
	}
}

void LCD_Draw_Score_Screen(){
	LCD_Update_Game_Length_Time();

	LCD_Clear(0, LCD_COLOR_GREY);

	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	LCD_DisplayChar(95,TIMER_YPOS,'T');
	LCD_DisplayChar(105,TIMER_YPOS,'i');
	LCD_DisplayChar(115,TIMER_YPOS,'m');
	LCD_DisplayChar(130,TIMER_YPOS,'e');
	LCD_DisplayChar(140,TIMER_YPOS,'r');


	char seconds[1000];
	//converts the ms to seconds and allowing for 1000*9 seconds to be displayed
	sprintf(seconds, "%ld", gameLengthTime/1000);

	for(uint32_t i = 0; seconds[i]!='\0'; i++){
		LCD_DisplayChar(105+i*15, TIMER_YPOS+30, seconds[i]);
	}

	LCD_DisplayChar(95,SCORE_YPOS,'S');
	LCD_DisplayChar(107,SCORE_YPOS,'c');
	LCD_DisplayChar(117,SCORE_YPOS,'o');
	LCD_DisplayChar(127,SCORE_YPOS,'r');
	LCD_DisplayChar(135,SCORE_YPOS,'e');



	LCD_Draw_Circle_Fill(103, SCORE_YPOS+60, 16, LCD_COLOR_BLACK);
	LCD_Draw_Circle_Fill(144, SCORE_YPOS+60, 16, LCD_COLOR_BLACK);
	//Outlines for the circles

	LCD_Draw_Circle_Fill(103, SCORE_YPOS+60, 15, LCD_COLOR_RED);
	LCD_Draw_Circle_Fill(144, SCORE_YPOS+60, 15, LCD_COLOR_YELLOW);

	char redScoreToDisplay[4];
	//displays up to 9999
	sprintf(redScoreToDisplay, "%d", redScore);
	for(int i = 0; redScoreToDisplay[i] != '\0'; i++){
		LCD_DisplayChar(97+i*15, SCORE_YPOS+51, redScoreToDisplay[i]);
	}

	char yellowScoreToDisplay[4];
	sprintf(yellowScoreToDisplay, "%d", yellowScore);
	for(int i = 0; redScoreToDisplay[i] != '\0'; i++){
		LCD_DisplayChar(137+i*15, SCORE_YPOS+51, yellowScoreToDisplay[i]);
	}

//	LCD_DisplayChar(97, SCORE_YPOS+51, redScoreToDisplay);
//	LCD_DisplayChar(137, SCORE_YPOS+51, yellowScoreToDisplay);
	//RIGHT NOW ONLY CAN DISPLAY UP TO 9 FOR BOTH SIDES OTHERWISE OVERFLOW
	LCD_DisplayChar(116, SCORE_YPOS+48, '-');

	LCD_Draw_Rectangle_Fill(18, RESTART_BUTTON_YPOS-2, 204, 104, LCD_COLOR_BLACK);
	LCD_Draw_Rectangle_Fill(20, RESTART_BUTTON_YPOS, 200, 100, LCD_COLOR_CYAN);
	LCD_Draw_Rectangle_Fill(25, RESTART_BUTTON_YPOS+5, 190, 90, LCD_COLOR_BLUE2);

	LCD_DisplayChar(95,  NEW_YPOS,'N');
	LCD_DisplayChar(110, NEW_YPOS,'E');
	LCD_DisplayChar(125, NEW_YPOS,'W');

	LCD_DisplayChar(90,  GAME_YPOS,'G');
	LCD_DisplayChar(105, GAME_YPOS,'A');
	LCD_DisplayChar(120, GAME_YPOS,'M');
	LCD_DisplayChar(135, GAME_YPOS,'E');
}


void visualDemo(void)
{
	uint16_t x;
	uint16_t y;
	// This for loop just illustrates how with using logic and for loops, you can create interesting things
	// this may or not be useful ;)
	for(y=0; y<LCD_PIXEL_HEIGHT; y++){
		for(x=0; x < LCD_PIXEL_WIDTH; x++){
			if (x & 32)
				frameBuffer[x*y] = LCD_COLOR_WHITE;
			else
				frameBuffer[x*y] = LCD_COLOR_BLACK;
		}
	}

	HAL_Delay(1500);
	LCD_Clear(0, LCD_COLOR_GREEN);
	HAL_Delay(1500);
	LCD_Clear(0, LCD_COLOR_RED);
	HAL_Delay(1500);
	LCD_Clear(0, LCD_COLOR_WHITE);
	LCD_Draw_Vertical_Line(10,10,250,LCD_COLOR_MAGENTA);
	HAL_Delay(1500);
	LCD_Draw_Vertical_Line(230,10,250,LCD_COLOR_MAGENTA);
	HAL_Delay(1500);
	LCD_Draw_Horizontal_Line(230,10,LCD_PIXEL_WIDTH,LCD_COLOR_MAGENTA);
	HAL_Delay(1500);

	LCD_Draw_Circle_Fill(125,150,20,LCD_COLOR_BLACK);
	HAL_Delay(2000);

	LCD_Clear(0,LCD_COLOR_BLUE);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_SetFont(&Font16x24);

	LCD_DisplayChar(100,140,'H');
	LCD_DisplayChar(115,140,'e');
	LCD_DisplayChar(125,140,'l');
	LCD_DisplayChar(130,140,'l');
	LCD_DisplayChar(140,140,'o');

	LCD_DisplayChar(100,160,'W');
	LCD_DisplayChar(115,160,'o');
	LCD_DisplayChar(125,160,'r');
	LCD_DisplayChar(130,160,'l');
	LCD_DisplayChar(140,160,'d');
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void LCD_Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

// Touch Functionality   //

#if COMPILE_TOUCH_FUNCTIONS == 1

void InitializeLCDTouch(void)
{
  if(STMPE811_Init() != STMPE811_State_Ok)
  {
	 for(;;); // Hang code due to error in initialzation
  }
}

STMPE811_State_t returnTouchStateAndLocation(STMPE811_TouchData * touchStruct)
{
	return STMPE811_ReadTouch(touchStruct);
}

void DetermineTouchPosition(STMPE811_TouchData * touchStruct)
{
	STMPE811_DetermineTouchPosition(touchStruct);
}

uint8_t ReadRegisterFromTouchModule(uint8_t RegToRead)
{
	return STMPE811_Read(RegToRead);
}

void WriteDataToTouchModule(uint8_t RegToWrite, uint8_t writeData)
{
	STMPE811_Write(RegToWrite, writeData);
}

#endif // COMPILE_TOUCH_FUNCTIONS
