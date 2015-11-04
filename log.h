/*****************************************************************************
Name        : log.h
Author      : tianshan
Date        : 2015年11月2日
Description : 
******************************************************************************/

#ifndef LOG_H_
#define LOG_H_

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

#define LOG_RUNNING(fmt, args...)   printf("RUNNING\t"fmt"\n", ##args);

#endif /* LOG_H_ */
