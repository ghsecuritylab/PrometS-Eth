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

		if (  recv_size > 0 )	// ���ŵ� �����Ͱ� ������
		{
			if ( parse_state == 0 )		// waiting head
			{
			}

			// ���ŵ� �����͸�
			send(sock_id, txbuff, 7, 0);
		}
		else		// error or close�� �����Ͽ��� ��
		{
			// socket�� �ݴ´�
			close(sock_id);

			osDelay(100);

			// thread�� �����Ѵ�
			osThreadTerminate(NULL);

			break;
		}

		osDelay(10);
	}
}


void MdTcp_Server_Init(void const *port)
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
	osThreadDef(mdtcpSrvTask, mdtcp_ServerSocketTask, osPriorityNormal, 0, 1024);


	// ������ TCP server socket ����
	if ( sock_id = socket (AF_INET, SOCK_STREAM, 0) < 0 )
	{
		// error �߻�
		return;
	}

	addr.sin_port        = htons(*((uint16_t*)port));
	addr.sin_family      = PF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;

	if ( bind (sock_id, (struct sockaddr *)&addr, sizeof(addr)) < 0 )
	{
		// error �߻�
		return;
	}

	// peer�� ������ ����Ѵ�
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
		osDelay(100);	// 100ms���� ���� �˻�
	}
}

