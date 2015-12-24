/**********************************************
* log.h
*
*  Created on: 2015-12-17
*      Author: xifengming
*       Email: xifengming@vip.sina.com
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
#define log_hex(len,fmt,args...)	\
		loghex(len,fmt,__FILE__, __LINE__, ##args)

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
int loghex(int datalen,char *fmt,...);

#ifdef __cplusplus
}
#endif


#endif
