#include "Button_Driver.h"

void Button_Init(){
		__HAL_RCC_GPIOA_CLK_ENABLE();
		GPIO_InitTypeDef buttonPin;
		buttonPin.Pin         = BUTTON_PIN;
		buttonPin.Mode        = GPIO_MODE_IT_RISING;
		buttonPin.Pull        = GPIO_NOPULL;
		buttonPin.Speed       = GPIO_SPEED_FREQ_HIGH;
		//buttonPin.Alternate   = GPIO_AF1_TIM2;
		HAL_GPIO_Init(BUTTON_PORT, &buttonPin);
		__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_0);

		HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

bool buttonStatus(){
	if(HAL_GPIO_ReadPin(GPIOA, BUTTON_PIN) == BUTTON_PRESSED){
		return true;
	}
	else{
		return false;
	}
}
