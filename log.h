/**********************************************
* log.h
*
*  Created on: 2015-12-17
*      Author: xifengming
*       Email: xifenmin@vip.sina.com
*    Comments: log module
 *********************************************/

#ifndef _NET_LOG_H
#define _NET_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#define log_debug(fmt, args...)	\
		logdebug(LEVEL_DEBUG, "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#define log_info(fmt, args...)	\
		loginfo(LEVEL_INFO,  "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#define log_warn(fmt, args...)	\
		logwarn(LEVEL_WARN,  "%s(%d): " fmt, __FILE__, __LINE__, ##args)
#define log_error(fmt, args...)	\
		logerror(LEVEL_ERROR, "%s(%d): " fmt, __FILE__, __LINE__, ##args)

#define log_hex(addr,port,data,datalen,fmt,args...) \
		loghex(data,datalen,"addr:%s,port:%d,%s",addr,port,fmt,##args)

enum LOGEVEL
{
   LEVEL_NONE = 0,
   LEVEL_ERROR,
   LEVEL_WARN,
   LEVEL_DEBUG,
   LEVEL_INFO,
   LEVEL_HEX
};

typedef struct taglogger Logger;

Logger *Logger_Create(int level,int rotate_size,char *name);
void Logger_Destory(void);

int logerror(int level,char *fmt, ...);
int logdebug(int level,char *fmt, ...);
int logwarn(int level,char *fmt, ...);
int logwrite(int level,char *fmt, ...);
int loginfo(int level,char *fmt, ...);
int loghex(char *data,int datalen,char *fmt,...);

#ifdef __cplusplus
}
#endif


#endif
