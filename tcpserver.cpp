/*****************************************************************************
 Name        : tcpserver.cpp
 Author      : tianshan
 Date        : 2015年7月10日
 Description : 
 ******************************************************************************/

#include "tcpserver.h"

#include <sys/time.h>
#include <time.h>
#include <string>
#include <fcntl.h>
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "log.h"

using namespace std;

namespace cppnetwork
{
static std::string addr2string(struct sockaddr_in &addr);
static std::string getsockaddr(int fd);
static std::string getpeeraddr(int fd);
static int setnonblocking(int fd);

static std::string addr2string(struct sockaddr_in &addr)
{
	char dest[32];
	unsigned long ad = ntohl(addr.sin_addr.s_addr);
	sprintf(dest, "%d.%d.%d.%d:%d", static_cast<int>((ad >> 24) & 255), static_cast<int>((ad >> 16) & 255),
			static_cast<int>((ad >> 8) & 255), static_cast<int>(ad & 255), ntohs(addr.sin_port));
	return dest;
}

static std::string getsockaddr(int fd)
{
	struct sockaddr_in address;

	memset(&address, 0, sizeof(address));
	unsigned int len = sizeof(address);
	if (getsockname(fd, (struct sockaddr*) &address, &len) != 0) {
		LOGI("getsockname error %s ", strerror(errno));
		return "";
	}

	return addr2string(address);
}

static std::string getpeeraddr(int fd)
{
	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	unsigned int len = sizeof(address);

	if (getpeername(fd, (struct sockaddr*) &address, &len) != 0) {
		LOGI("getsockname error %s ", strerror(errno));
		return "";
	}

	return addr2string(address);
}

//todo: 设置成阻塞超时的模式，不使用直接阻塞的方式
static int setnonblocking(int fd)
{
	//注意返回就的文件描述符属性以便将来恢复文件描述符属性
	int old_option = fcntl(fd, F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd, F_SETFL, new_option);
	return old_option;
}

TcpConn::~TcpConn()
{
	if (_fd > 0) {
		::close(_fd);
		_fd = -1;
	}
}

void TcpConn::set_active_time(time_t now)
{
	_last_active_time = now;
}

time_t TcpConn::get_active_time()
{
	return _last_active_time;
}

bool TcpConn::check_timeout(time_t now, int timeout)
{
	return time(NULL) - timeout > _last_active_time;
}

int TcpConn::get_fd()
{
	return _fd;
}

string TcpConn::info()
{
	if (_peer_addr.empty()) {
		struct sockaddr_in peer_address;
		socklen_t len = sizeof(peer_address);
		if (getpeername(_fd, (struct sockaddr*) &peer_address, &len) != 0) {
			LOGE("getpeername error %s \n", strerror(errno));
		}

		char dest[64] = {0};
		unsigned long ad = ntohl(peer_address.sin_addr.s_addr);
		sprintf(dest, "fd:%d %d.%d.%d.%d:%d", _fd, static_cast<int>((ad >> 24) & 255), static_cast<int>((ad >> 16) & 255),
				static_cast<int>((ad >> 8) & 255), static_cast<int>(ad & 255), ntohs(peer_address.sin_port));

		_peer_addr = dest;
	}

	return _peer_addr;
}


TcpServer::TcpServer()
{
	_server_fd = -1;
	_stop = false;
#ifdef __APPLE__
	_sock_event = new KqueueSockEvent;
#else
	_sock_event = new EpollSockEvent;
#endif
}

TcpServer::~TcpServer()
{
	_stop = true;
	if (_server_fd > 0) {
		close(_server_fd);
		_server_fd = -1;
	}
}

bool TcpServer::init(const char *host, unsigned short port)
{
	if (!set_address(host, port))
		return false;
	if (!listen())
		return false;

	_sock_event->add_event(_server_fd, true, false);

	return true;
}

void *thread_fun(void *arg)
{
	TcpServer *server = (TcpServer*) arg;
	server->event_loop();

	return NULL;
}

void TcpServer::dispath()
{
	pthread_t pid;
	pthread_attr_t thread_attr;
	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&pid, NULL, thread_fun, (void*) this);
}

void TcpServer::event_loop()
{
	IOEvent io_events[1024];

	while (!_stop) {
		int n = _sock_event->get_events(1000, io_events, 1024);

		for (int i = 0; i < n; i++) {
			if (io_events[i]._read_ocurr) {
				on_read_event(io_events[i]._fd);
			}

			if (io_events[i]._write_ocurr) {
				on_write_event(io_events[i]._fd);
			}
		}

		check_timeout();
		_watch.run_stat(time(NULL));
	}
}

void TcpServer::check_timeout()
{
	vector<TcpConn*> timeout_list = _online_user.check_timeout(10);
	for(int i= 0; i < timeout_list.size(); i++) {
		LOGI("%s timeout, close it",  timeout_list[i]->info().c_str());
		sock_close(timeout_list[i]->get_fd());
	}

	return;
}

bool TcpServer::listen()
{
	_server_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (_server_fd < 0) {
		LOG_RUNNING("listen socket error:%s ", strerror(errno));
		return false;
	}

	int reuse = 1;

	setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)); // optional, but recommended
	if (::bind(_server_fd, (struct sockaddr *) &_address, sizeof(_address)) < 0) {
		LOG_RUNNING("tcp server bind error :%s ", strerror(errno));
		return false;
	}

	if (::listen(_server_fd, 128) < 0) {
		LOG_RUNNING("tcp server listen error : %s", strerror(errno));
		return false;
	}

	LOG_RUNNING("tcp server listen %s ", getsockaddr(_server_fd).c_str());

	return true;
}

bool TcpServer::set_address(const char *host, unsigned short port)
{
	memset(static_cast<void *>(&_address), 0, sizeof(_address));

	_address.sin_family = AF_INET;
	_address.sin_port = htons(static_cast<short>(port));

	bool rc = true;
	if (host == NULL || host[0] == '\0') {
		_address.sin_addr.s_addr = htonl(INADDR_ANY);
	} else {
		char c;
		const char *p = host;
		bool is_ipaddr = true;
		while ((c = (*p++)) != '\0') {
			if ((c != '.') && (!((c >= '0') && (c <= '9')))) {
				is_ipaddr = false;
				break;
			}
		}

		if (is_ipaddr) {
			_address.sin_addr.s_addr = inet_addr(host);
		} else {
			struct timeval begin, end;
			gettimeofday(&begin, NULL);
			struct hostent *hostname = gethostbyname(host);
			gettimeofday(&end, NULL);

			if (hostname != NULL) {
				memcpy(&(_address.sin_addr), *(hostname->h_addr_list), sizeof(struct in_addr));
				LOGI("GET DNS OK, %s => %s", host, inet_ntoa(_address.sin_addr));
			} else {
				rc = false;
			}
		}
	}

	return rc;
}

void TcpServer::on_read_event(int fd)
{
	if (fd == _server_fd) {
		struct sockaddr_in client_addr;
		int len = sizeof(client_addr);
		int fd = ::accept(_server_fd, (struct sockaddr*) &client_addr, (socklen_t*) &len);
		if (fd > 0) {

			int recv_send_timeout = 1000; //读写超时时间为1s;
			//设置发送超时
			setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *) &recv_send_timeout, sizeof(int));
			//设置接收超时
			setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *) &recv_send_timeout, sizeof(int));

			int buffer = 1 * 1024;
			setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &buffer, sizeof(buffer));
			setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &buffer, sizeof(buffer));

			_sock_event->add_event(fd, true, false);
			on_conn(fd);
		} else {
			LOG_RUNNING("accept fail :%s ", strerror(errno));
		}
	} else {
		read(fd);
	}
}

void TcpServer::on_write_event(int fd)
{
	return;
}

void TcpServer::on_conn(int fd)
{
	_watch.on_connect();
	TcpConn *conn = new TcpConn(fd);
	LOGI("new connection:%s", conn->info().c_str());
	_online_user.add_user(fd, conn);
	return;
}

void TcpServer::sock_close(int fd)
{
	_watch.on_close();
	_sock_event->remove_event(fd);
	::close(fd);
}

//SIMPLE TCP SERVER 没有分包机制
void TcpServer::on_read(int fd, const char *data, int len)
{
	TcpConn *conn  = _online_user.get_user(fd);
	if(conn != NULL) {
		conn->set_active_time(time(NULL));
	}

	LOGI("on read from fd:%d %s len:%d %*.s", fd, getpeeraddr(fd).c_str(), len, len, data);

}

void TcpServer::on_close(int fd)
{
	LOGI("%d on_close", fd);
	_online_user.remove_user(fd);
}

int TcpServer::read(int fd)
{
	int offset = 0;
	char buffer[1500];
	int buffer_len = 1500;

	int recv_bytes = (int) ::recv(fd, (void*) buffer, buffer_len - offset, 0);

	bool broken = false;
	if (recv_bytes > 0) {
		on_read(fd, buffer, recv_bytes);
	} else if(recv_bytes < 0) { //读到错误
		broken = true;
		sock_close(fd);
	} else { //对端关闭了socket
		LOGI("remote close the socket");
		broken = true;
		sock_close(fd);
	}

	if(broken)  {
		on_close(fd);
	}

	return offset;
}

void TcpServer::write(int fd, const char *data, int len)
{
	if (data == NULL || len < 0) {
		return;
	}

	int write_bytes = (int) ::write(fd, data , len );

	if(write_bytes < 0) {
		LOGI("fd:%d write fail:%s", fd, strerror(errno));
		sock_close(fd);
	}


	if (write_bytes == len) {
		LOGI("send fd:%d len:%d finish ", fd, write_bytes);
	}

	return;
}

} /* namespace cppnetwork */
