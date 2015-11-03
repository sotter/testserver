/*****************************************************************************
 Name        : tcpserver.h
 Author      : tianshan
 Date        : 2015年7月10日
 Description : 简单的TCPSERVER，专为测试使用
 ******************************************************************************/

#ifndef LIBNETWORK_1_0_TEST_TESTSERVER_TCPSERVER_H_
#define LIBNETWORK_1_0_TEST_TESTSERVER_TCPSERVER_H_

#include <set>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <list>
#include <string>
#include <vector>
#include <arpa/inet.h>
#include <map>
#include "sockevent.h"

using namespace std;

namespace cppnetwork
{
class Select;
class TcpServer;

class SockEventHandler
{
public:
	virtual void on_read_event(int fd) = 0;
	virtual void on_write_event(int fd) = 0;
};

class TcpConn
{
public:
	TcpConn(int fd) :
			_fd(fd)
	{
		_last_active_time = time(NULL);
	}

	virtual ~TcpConn();

	int    get_fd();
	string info();
	void   set_active_time(time_t now);
	time_t get_active_time();
	bool   check_timeout(time_t now, int timeout);

private:

	int _fd;
	string _peer_addr;
	time_t _last_active_time;
};

class OnlineUser {
public:

	~OnlineUser()
	{
		map<int, TcpConn*>::iterator iter = _online_users.begin();
		for(; iter != _online_users.end(); ++iter) {
			delete iter->second;
		}
		_online_users.clear();
	}

	TcpConn *get_user(int fd)
	{
		TcpConn *conn = NULL;
		map<int, TcpConn*>::iterator iter = _online_users.find(fd);
		if(iter != _online_users.end())
		{
			conn = iter->second;
		}

		return conn;
	}

	void add_user(int fd, TcpConn *conn)
	{
		//不做insert检查
		_online_users.insert(make_pair(fd, conn));
	}

	void remove_user(int fd)
	{
		map<int, TcpConn*>::iterator iter = _online_users.find(fd);
		if(iter != _online_users.end()) {
			delete iter->second;
			_online_users.erase(iter);
		}
	}

	vector<TcpConn*> check_timeout(int timeout) {
		vector<TcpConn*> timeout_list;
		int now = time(NULL);
		map<int, TcpConn*>::iterator iter = _online_users.begin();
		while (iter != _online_users.end()) {
			if (iter->second->check_timeout(now, timeout)) {
				timeout_list.push_back(iter->second);
				_online_users.erase(iter++);
			} else {
				++iter;
			}
		}

		return timeout_list;
	}

private:
	map<int, TcpConn*> _online_users;
};

class TcpServer : public SockEventHandler
{
public:

	TcpServer();

	virtual ~TcpServer();

	bool init(const char *host, unsigned short port);

	void dispath();

	virtual void on_read_event(int fd);

	virtual void on_write_event(int fd);

	virtual void on_conn(int fd);

	virtual void on_read(int fd, const char *data, int len);

	virtual void on_close(int fd);

	int  read(int fd);

	void write(int fd, const char *data, int len);

	void event_loop();

private:

	bool listen();

	void sock_close(int fd);

	bool set_address(const char *host, unsigned short port);

	void check_timeout();

private:
	bool                _stop;
	int                 _server_fd;
	struct sockaddr_in  _address;
	OnlineUser          _online_user;
	SockEvent          *_sock_event;
};



} /* namespace cppnetwork */

#endif /* LIBNETWORK_1_0_TEST_TESTSERVER_TCPSERVER_H_ */
