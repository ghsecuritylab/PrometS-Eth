#ifndef	_MODBUS_H_
#define	_MODBUS_H_

	#ifndef _STDINT
	#include <stdint.h>
	#endif

// -----------------------------------------
// MODBUS Function Codes-CALSS 2
#define READ_COIL_STATUS			1
#define READ_INPUT_STATUS			2
#define READ_HOLDING_REGISTERS		3
#define READ_INPUT_REGISTERS		4

#define FORCE_SINGLE_COIL			5
#define PRESET_SINGLE_REGISTER		6

#define READ_EXCEPTION_STATUS		7
#define FETCH_COMM_EVENT_COUNTER	11
#define FETCH_COMM_EVENT_LOG		12

#define FORCE_MULTIPLE_COILS		15
#define PRESET_MULTIPLE_REGISTERS	16

#define REPORT_SLAVE_ID				17
#define READ_GENERAL_REFERENCE		20
#define WRITE_GENERAL_REFERENCE		21
#define MASK_WRITE_4X_REGISTER		22
#define MASK_RD_WR_4X_REGISTER		23
#define READ_FIFO_QUEUE				24

// Neopis User defined function codes(65~72, 100~110)
#define READ_FAULT_WAVE				65
#define WRITE_TIME					66


// -----------------------------------------
// Mode Bus Error Code- Standard
#define	ErrIllegalFunction			1
#define	ErrIllegalAddress			2
#define	ErrIllegalDataValue			3
#define	ErrSlaveDeviceFail			4		// 실행중 error 발생
#define	ErrAcknowledge				5		// slave가 request를 받아 들였으나 시간이 오래걸리는 경우 이 응답이 온다, 그러므로 계속 polling해서 종료 시킨다
#define	ErrSlaveDeviceBusy			6		// Slave가 어떤 작업을 진행중이라서, 현재 request를 진행할 수 없을 때
#define	ErrNegativeACK				7
#define	ErrMemoryParity				8
#define	ErrGateWayPath				10		// 서브 slave device의 path가 설정 되지 않았을 때
#define	FailTargetResponse			11		// 서브 slave device가 응답이 없을 때

// -----------------------------------------
#define	MBR_BO	0
#define	MBR_BI	1
#define	MBR_AO	2
#define	MBR_AI	3
// -----------------------------------------


typedef struct {
	uint16_t	usTransID;
	uint16_t	usProtocolID;
	uint16_t	usLength;
} MBDU_HEAD, *PMBDU_HEAD;			// MBAP Header : Modbus Application Protocol Header

typedef struct {
	uint8_t	ucUnitID;				// big endian에서 순서를 바꾼다
	uint8_t	ucFunctn;
	uint8_t	ucData[254];
} MBDU_BODY, *PMBDU_BODY;			// ADU : Application Data Unit

typedef struct {
	MBDU_HEAD	Head;
	MBDU_BODY	Body;
} MBDU_FRAME, *PMBDU_FRAME;


#ifdef __cplusplus
extern "C" {
#endif



#ifdef __cplusplus
}
#endif


#endif	// _MODBUS_H_


