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
		// ���ŵ� �����Ͱ� ������ ���� ��� ���·� ����
		if ( recv_size <= 0 )
		{
			recv_size = recv(sock_id, rxbuff, 6, 0);

			// ���ŵ� �������� tick�� ����Ѵ�
			new_tick = HAL_GetTick();
		}

		if ( recv_size <= 0 )	break;		// goto socket close

		recv_len = 0;

		// �󸶸��� ������ ����
		// printf("mbtcp: recv = %d, tick = %d\r\n", recv_size, new_tick);

		if ( (new_tick -  old_tick) >= (uint32_t)100 )
		{	// ���� ������ 100msec���� ũ�ٸ� �ٽ� parsing�Ѵ�
			parse_state = 0;
			rxFrameSize = 0;
			pRxPtr = (uint8_t*)&rxFrame;
			// printf("clear\r\n");
		}

		// tick�� update�Ѵ�
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
				{	// frame ���� �Ϸ�


					// ������ 1 frame�� ó���Ͽ����Ƿ� ������ ���Ͽ� ������ �ʱ�ȭ �Ѵ�
					pRxPtr = (uint8_t*)&rxFrame;
					rxFrameSize = 0;
					parse_state = 0;	// parse body

					osDelay(10);
				}
			};
		};	// end of while

		// osDelay(10);
	}

	// socket�� �ݴ´�
	close(sock_id);

	printf("mbtcp: sock closed %d\r\n", sock_id);
	osDelay(100);

	// thread�� �����Ѵ�
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
	// port�� �����ϴ� client�� ������ �����ϴ� Task
	// MODBUS Server Socket�� ����Ѵ�
	// socket ���� �� Thread
	// �鷯���� ������� child socket�� �����Ѵ�

	struct sockaddr_in addr;
	struct sockaddr    addr_remote;
	socklen_t          addr_len;
	int32_t sock_id;
	int32_t subsock_id;

	osThreadDef(mdtcpSrvTask, mdtcp_ServerSocketTask, osPriorityNormal, 1, 1024);

	// ������ TCP server socket ����
	if ( sock_id = socket (PF_INET, SOCK_STREAM, 0) < 0 )
	{
		// error �߻�
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
		// error �߻�
		return;
	}

	// peer�� ������ ����Ѵ�
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
		osDelay(100);	// 10ms���� ���� �˻�
	}

	// listen socket�� �ݴ´�
	// close(sock_id);
	// �� �����带 �ݴ´�
}

