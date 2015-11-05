/*****************************************************************************
Name        : log.cpp
Author      : tianshan
Date        : 2015年11月5日
Description : 
******************************************************************************/


#include "log.h"

LOG* LOG::_instance = NULL;

void LOG::log(int log_level, const char *format, ...)
{
	if (_run_fd < 0) {
		_run_fd = open(_file.c_str(), O_CREAT | O_APPEND | O_RDWR, 0755);
	}

	va_list ap;

	char buffer[256] = { 0 };

	struct tm *p_tm;
	time_t timep = time(NULL);
	p_tm = localtime(&timep);

	int len  = 0;
	int offset = snprintf(buffer, sizeof(buffer) - 1,
			"%d-%02d-%02d-%02d:%02d:%02d\t", p_tm->tm_year + 1900, p_tm->tm_mon + 1,
			p_tm->tm_mday, p_tm->tm_hour, p_tm->tm_min, p_tm->tm_sec);

	len += offset;

	va_start(ap, format);
	len  += vsnprintf(buffer + offset, sizeof(buffer) + offset -1, format, ap);
	va_end(ap);

	if (_run_fd >= 0) {
		::write(_run_fd, buffer, len);
	}

	fprintf(stdout, "%s", buffer);
	fflush(stdout);
}
