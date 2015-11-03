/*****************************************************************************
Name        : log.h
Author      : tianshan
Date        : 2015年11月2日
Description : 
******************************************************************************/

#ifndef LOG_H_
#define LOG_H_

#define LOGF(fmt, args...)          printf("INFO\t"fmt"\n", ##args)
#define LOGE(fmt, args...)          printf("ERRO\t"fmt"\n", ##args)
#define LOGW(fmt, args...)          printf("WARN\t"fmt"\n", ##args)
#define LOGI(fmt, args...)          printf("INFO\t"fmt"\n", ##args)
#define LOGD(fmt, args...)          printf("DBUG\t"fmt"\n", ##args)


#endif /* LOG_H_ */
