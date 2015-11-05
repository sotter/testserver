/*****************************************************************************
Name        : main.cpp
Author      : tianshan
Date        : 2015年7月13日
Description : 
******************************************************************************/
#include "tcpserver.h"
#include <stdlib.h>
#include <errno.h>
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
		_watch.on_request(len);
		_watch.on_ack(4);
		this->write(fd, "ACK\n", 4);

		return;
	}
};

void StayBack()
{
    int     child_pid;

    child_pid = fork();

    if (child_pid == -1)
    {
        perror("fork fail");
        exit(1);
    }
    else if (child_pid > 0)
    {
        exit(0); // parent exit
    }

    if (setpgid(0, 0) == -1)
    {
        perror("setpgrp");
        exit(1);
    }

    if ((child_pid = fork()) == -1)
    {
        perror("fork fail");
        exit(1);
    }
    else if (child_pid > 0)
    {
        exit(0); // parent exit
    }
}

int main(int argc, char *argv[])
{
	StayBack();

	LOG::Instance()->set_log_file("tcpserver.log");

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


