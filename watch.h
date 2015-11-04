/*****************************************************************************
 Name        : watch.h
 Author      : tianshan
 Date        : 2015年11月4日
 Description :
 ******************************************************************************/

#ifndef WATCH_H_
#define WATCH_H_

#include <time.h>
#include <stdio.h>
#include <string>
#include "log.h"

using namespace std;

class Watch
{
public:
	Watch()
	{
		_connected_num = 0;
		_request_num = 0;
		_last_stat_time = time(NULL);
	}

	void on_connect()
	{
		_connected_num++;
	}
	void on_close()
	{
		_connected_num--;
	}

	void on_request(int len)
	{
		_request_num++;
		_read_num += len;
	}

	void on_ack(int len)
	{
		_write_num += len;
	}

	void run_stat(time_t now)
	{
		if (now - _last_stat_time < 1) {
			return ;
		}

		int interval = now - _last_stat_time;
		if (interval <= 0) {
			interval = 1;
		}

		LOG_RUNNING("current conn:%d, QPS:%d, read flux:%d, write flux:%d", _connected_num,
				_request_num / interval, _read_num / interval, _write_num / interval);

		_last_stat_time = now;
		_request_num = 0;
		_read_num = 0;
		_write_num = 0;
	}

private:

	int _connected_num;
	int _request_num;
	int _read_num;
	int _write_num;
	time_t _last_stat_time;

};

#endif /* WATCH_H_ */
