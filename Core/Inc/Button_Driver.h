#ifndef BUTTON_DRIVER_H_
#define BUTTON_DRIVER_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>

#define BUTTON_PORT      GPIOA
#define BUTTON_PIN       GPIO_PIN_0
#define BUTTON_PRESSED   1
#define BUTTON_UNPRESSED 0

void Button_Init();
//initializes button

bool buttonStatus();
//tells whether the button is pressed or unpressed

#endif
