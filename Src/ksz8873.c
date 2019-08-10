#include "main.h"

// PHY KSZ8873의 Interrupt 발생
void EXTI0_IRQHandler(void)
{
	// External interrupt flag를 clear한다
	__HAL_GPIO_EXTI_CLEAR_IT(PHY_nINT_Pin);

	// 어떤 원인인지를 확인한다



}



