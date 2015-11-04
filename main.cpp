/*****************************************************************************
Name        : main.cpp
Author      : tianshan
Date        : 2015年7月13日
Description : 
******************************************************************************/
#include "tcpserver.h"
#include <stdlib.h>
#include "stdio.h"

using namespace cppnetwork;


void show_hex(const char *data, int len)
{
	for(int i = 0; i < len ; i ++) {
		printf("%02x ", (int)data[i]);
	}

	printf("\n");
}

class PduServer : public TcpServer
{
public:
	virtual void on_read(int fd, const char *data, int len)
	{
		TcpConn *conn  = _online_user.get_user(fd);
		if(conn != NULL) {
			conn->set_active_time(time(NULL));
		}
		printf("receive len:%d data:%s \n", len ,data);

		_watch.on_request(len);
		_watch.on_ack(4);
		this->write(fd, "ACK\n", 4);

		return;
	}
};

int main(int argc, char *argv[])
{
	PduServer tcpserver;
	unsigned short port = 9000;

	if(argc > 1) {
		port = (unsigned short)atoi(argv[1]);
	}

	tcpserver.init("0.0.0.0", port);

	tcpserver.dispath();

	while(1) {
		sleep(500);
	}

	return 0;
}


