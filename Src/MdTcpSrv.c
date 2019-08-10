#include "MdTcpSrv.h"

#include "cmsis_os.h"
#include "lwip.h"
#include "lwip/sockets.h"
#include "modbus.h"

void mdtcp_ServerSocketTask(void const * argument)
{
	uint8_t rxbuff[8];
	uint8_t txbuff[8];

	MBDU_FRAME rxFrame;

	int32_t sock_id = *((int32_t*)argument);
	int32_t recv_size;
	int32_t parse_state = 0;
	int32_t recv_len = 0;

	for(;;)
	{
		recv_size = recv(sock_id, rxbuff, 8, 0);

		if (  recv_size > 0 )	// 수신된 데이터가 있으면
		{
			if ( parse_state == 0 )		// waiting head
			{
			}

			// 수신된 데이터를
			send(sock_id, txbuff, 7, 0);
		}
		else		// error or close를 수신하였을 때
		{
			// socket을 닫는다
			close(sock_id);

			osDelay(100);

			// thread를 중지한다
			osThreadTerminate(NULL);

			break;
		}

		osDelay(10);
	}
}


void MdTcp_Server_Init(void const *port)
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
	osThreadDef(mdtcpSrvTask, mdtcp_ServerSocketTask, osPriorityNormal, 0, 1024);


	// 모드버스 TCP server socket 생성
	if ( sock_id = socket (AF_INET, SOCK_STREAM, 0) < 0 )
	{
		// error 발생
		return;
	}

	addr.sin_port        = htons(*((uint16_t*)port));
	addr.sin_family      = PF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;

	if ( bind (sock_id, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
	{
		// error 발생
		return;
	}

	// peer의 연결을 대기한다
	listen (sock_id, 8);

	/* Infinite loop */
	for(;;)
	{
		subsock_id = accept (sock_id, &addr_remote, &addr_len);

		if ( subsock_id >= 0 )
		{
		//	osThreadDef(defaultTask, mdtcp_ServerSocketTask, osPriorityNormal, 0, 1024);
			if ( osThreadCreate(osThread(mdtcpSrvTask), &subsock_id) == NULL )
			{
				printf("error");
				close(subsock_id);
			}
		}
		osDelay(100);	// 100ms마다 연결 검사
	}
}

