#include "main.h"

// PHY KSZ8873�� Interrupt �߻�
void EXTI0_IRQHandler(void)
{
	// External interrupt flag�� clear�Ѵ�
	__HAL_GPIO_EXTI_CLEAR_IT(PHY_nINT_Pin);

	// � ���������� Ȯ���Ѵ�



}



