#include "MdTcpSrv.h"

#include "cmsis_os.h"
#include "lwip.h"
#include "lwip/sockets.h"
#include "modbus.h"

#define	DEBUG_MSG


void mdtcp_ServerSocketTask(void const * argument)
{
	uint8_t rxbuff[8];
	uint8_t txbuff[8];

	MBDU_FRAME rxFrame;
	MBDU_FRAME txFrame;


	int32_t sock_id = *((int32_t*)argument);

	int32_t rxFrameSize = 0;
	int32_t recv_size = 0;
	int32_t recv_len = 0;

	int32_t parse_state = 0;
	uint8_t *pRxPtr = (uint8_t*)&rxFrame;
	uint32_t old_tick, new_tick;

	for(;;)
	{
		// 수신된 데이터가 없으면 수신 대기 상태로 들어간다
		if ( recv_size <= 0 )
		{
			recv_size = recv(sock_id, rxbuff, 6, 0);

			// 수신된 데이터의 tick을 기록한다
			new_tick = HAL_GetTick();
		}

		if ( recv_size <= 0 )	break;		// goto socket close

		recv_len = 0;

		// 얼마만에 오는지 찍어보자
		// printf("mbtcp: recv = %d, tick = %d\r\n", recv_size, new_tick);

		if ( (new_tick -  old_tick) >= (uint32_t)100 )
		{	// 수신 간격이 100msec보다 크다면 다시 parsing한다
			parse_state = 0;
			rxFrameSize = 0;
			pRxPtr = (uint8_t*)&rxFrame;
			// printf("clear\r\n");
		}

		// tick을 update한다
		old_tick = new_tick;

		while ( recv_size-- )
		{
			if ( parse_state == 0 )
			{	// parse head
				*pRxPtr++ = rxbuff[recv_len++];
				rxFrameSize++;

				if ( rxFrameSize >= sizeof(MBDU_HEAD) )
				{
					rxFrame.Head.usLength = ntohs(rxFrame.Head.usLength);
					rxFrame.Head.usTransID = ntohs(rxFrame.Head.usTransID);

					parse_state = 1;	// parse body
				};
			}
			else
			{	// parse body
				*pRxPtr++ = rxbuff[recv_len++];
				rxFrameSize++;

				if( rxFrameSize >= (int32_t)(rxFrame.Head.usLength + sizeof(MBDU_HEAD)) )
				{	// frame 수신 완료


					// 수신한 1 frame를 처리하였으므로 다음을 위하여 변수를 초기화 한다
					pRxPtr = (uint8_t*)&rxFrame;
					rxFrameSize = 0;
					parse_state = 0;	// parse body

					osDelay(10);
				}
			};
		};	// end of while

		// osDelay(10);
	}

	// socket을 닫는다
	close(sock_id);

	printf("mbtcp: sock closed %d\r\n", sock_id);
	osDelay(100);

	// thread를 중지한다
	osThreadTerminate(NULL);
}

/*

#define	MAX_MBTCP_SOCK_NUM	8
const char *str_mbtcp_sock_name[MAX_MBTCP_SOCK_NUM] =
{
	"MBTCP_SOCK0",
	"MBTCP_SOCK1",
	"MBTCP_SOCK2",
	"MBTCP_SOCK3",
	"MBTCP_SOCK4",
	"MBTCP_SOCK5",
	"MBTCP_SOCK6",
	"MBTCP_SOCK7"
};

static osThreadDef_t mbtcpTask[8];
*/

void StartMdTcpServerTask(void const *port)
{
	// port로 접속하는 client의 연결을 관리하는 Task
	// MODBUS Server Socket에 사용한다
	// socket 관리 용 Thread
	// 들러오는 순서대로 child socket를 생성한다

	struct sockaddr_in addr;
	struct sockaddr    addr_remote;
	socklen_t          addr_len;
	int32_t sock_id;
	int32_t subsock_id;

	osThreadDef(mdtcpSrvTask, mdtcp_ServerSocketTask, osPriorityNormal, 1, 1024);

	// 모드버스 TCP server socket 생성
	if ( sock_id = socket (PF_INET, SOCK_STREAM, 0) < 0 )
	{
		// error 발생
		return;
	}

#ifdef DEBUG_MSG
	printf("mbtcp: listen sock id = %d\r\n", sock_id);
#endif

	addr.sin_port        = htons(*((uint16_t*)port));
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;

	if ( bind (sock_id, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
	{
		// error 발생
		return;
	}

	// peer의 연결을 대기한다
	listen (sock_id, 1);	// backlog

	/* Infinite loop */
	for(;;)
	{
		subsock_id = accept (sock_id, &addr_remote, &addr_len);
#ifdef DEBUG_MSG
		printf("mbtcp: accept sock id = %d\r\n", subsock_id);
#endif

		if ( subsock_id >= 0 )
		{
			// osThreadDef(defaultTask, mdtcp_ServerSocketTask, osPriorityNormal, 2, 1024);
			if ( osThreadCreate(osThread(mdtcpSrvTask), &subsock_id) == NULL )
			{
				printf("mbtcp: fail to creat thread error\r\n");
				close(subsock_id);
				printf("mbtcp: close sock id = %d\r\n", subsock_id);
			}
		}
		osDelay(100);	// 10ms마다 연결 검사
	}

	// listen socket을 닫는다
	// close(sock_id);
	// 이 쓰레드를 닫는다
}

