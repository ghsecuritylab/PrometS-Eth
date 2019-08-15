#include "main.h"

#include "cmsis_os.h"

#include "dpram.h"


osMutexId DpRamMutexHandle;

// Dual Port Ram의 Interrupt 발생
void EXTI15_10_IRQHandler(void)
{
	// External interrupt flag를 clear한다
	__HAL_GPIO_EXTI_CLEAR_IT(DPR_nINT_Pin);

	// 어떤 원인인지를 확인한다


  /* definition and creation of myMutex01 */
  /* definition and creation of DpRamMutex */


}



void dpram_init(void)
{
	// Muxtex를 생성한다
	osMutexDef(DpRamMutex);
	DpRamMutexHandle = osMutexCreate(osMutex(DpRamMutex));



}



