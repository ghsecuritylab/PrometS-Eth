#ifndef	_MAP_H_
#define	_MAP_H_

	#ifndef _STDINT
	#include <stdint.h>
	#endif




typedef struct {
	uint8_t	ucUnitID;
	uint8_t	ucFunctn;
	uint8_t	ucData[254];




} mapPrometS_t;



#ifdef __cplusplus
extern "C" {
#endif



extern void map_Init(void);


extern int32_t map_ReadBits(int32_t nReg, int32_t usBitStart, int32_t usQuantity, void *pRet);

#ifdef __cplusplus
}
#endif


#endif	// _MAP_H_


