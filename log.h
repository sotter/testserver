/*****************************************************************************
Name        : log.h
Author      : tianshan
Date        : 2015年11月2日
Description : 
******************************************************************************/

#ifndef LOG_H_
#define LOG_H_

#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

class LOG
{
public:

	LOG() : _run_fd(-1), _log_level(1)
	{

	}

	virtual ~LOG()
	{
		if(_run_fd > 0)  {
			::close(_run_fd);
			_run_fd = -1;
		}
	}

	static LOG *Instance()
	{
		if(_instance == NULL) {
			_instance = new LOG();
		}

		return _instance;
	}

	//短连接的打开
	void log(int log_level, const char *format, ...);

	void set_log_level(int log_level)
	{
		_log_level = log_level;
	}

	void set_log_file(const char *file)
	{
		if(file != NULL) {
			_file = file;
		}
	}

private:

	static  LOG* _instance;

	string _file;
	int _run_fd;
	int _log_level;
};

#ifdef __DEBUG
#define LOGF(fmt, args...)          printf("INFO\t"fmt"\n", ##args)
#define LOGE(fmt, args...)          printf("ERRO\t"fmt"\n", ##args)
#define LOGW(fmt, args...)          printf("WARN\t"fmt"\n", ##args)
#define LOGI(fmt, args...)          printf("INFO\t"fmt"\n", ##args)
#define LOGD(fmt, args...)          printf("DBUG\t"fmt"\n", ##args)
#else
#define LOGF(fmt, args...)
#define LOGE(fmt, args...)
#define LOGW(fmt, args...)
#define LOGI(fmt, args...)
#define LOGD(fmt, args...)
#endif

#define LOG_RUNNING(fmt, args...)   LOG::Instance()->log(1, fmt"\n", ##args);

#endif /* LOG_H_ */
