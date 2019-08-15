#include "main.h"

#include "cmsis_os.h"

#include "dpram.h"


osMutexId DpRamMutexHandle;

// Dual Port Ram�� Interrupt �߻�
void EXTI15_10_IRQHandler(void)
{
	// External interrupt flag�� clear�Ѵ�
	__HAL_GPIO_EXTI_CLEAR_IT(DPR_nINT_Pin);

	// � ���������� Ȯ���Ѵ�


  /* definition and creation of myMutex01 */
  /* definition and creation of DpRamMutex */


}



void dpram_init(void)
{
	// Muxtex�� �����Ѵ�
	osMutexDef(DpRamMutex);
	DpRamMutexHandle = osMutexCreate(osMutex(DpRamMutex));



}



