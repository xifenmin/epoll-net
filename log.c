#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <errno.h>
#include "lock.h"
#include "log.h"

#define LEVEL_NAME_LEN	8
#define LOG_BUF_LEN		4096
#define PATH_MAX        256

static struct taglogger loggobj;

struct taglogger {
    char name[PATH_MAX];
    int  level;
    FILE *fp;
	unsigned long long wcurr;//当前写的位置
	unsigned long long wtotal;//总共写入数
	unsigned long long rotatesize;//日志切割大小
    LockerObj *lockerobj;
};

static inline void setlevel(int level)
{
	Logger *logger = &loggobj;

	if (NULL == logger)
		return;

	logger->level = level;
}

static inline char* getLevelName(int level){

   	switch(level){
		case LEVEL_NONE:
			return "[NONE] ";
		case LEVEL_ERROR:
			return "[ERROR] ";
		case LEVEL_WARN:
			return "[WARN] ";
		case LEVEL_INFO:
			return "[INFO] ";
		case LEVEL_DEBUG:
			return "[DEBUG] ";
		case LEVEL_HEX:
			return "[HEX] ";
	}
	return "";
}

static void logclose(void)
{
	Logger *logger = &loggobj;

	if (NULL != logger) {
		if (logger->fp != stdin && logger->fp != stdout) {
			fclose(logger->fp);
		}
	}
}

static void rotate(Logger *logger)
{
	char newpath[PATH_MAX];
	time_t time;
	struct timeval tv;
	struct tm *tm;

	if (NULL == logger)
		return;

	fclose(logger->fp);
	logger->fp = NULL;
	gettimeofday(&tv, NULL);

	time = tv.tv_sec;
	tm   = localtime(&time);

	sprintf(newpath, "%s.%04d%02d%02d-%02d%02d%02d",
		logger->name,
		tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
		tm->tm_hour, tm->tm_min, tm->tm_sec);

	int ret = rename(logger->name,newpath);
	if(ret == -1){
		return;
	}

	logger->fp = fopen(logger->name, "a");

	if(logger->fp == NULL){
		return;
	}
	logger->wcurr = 0;
}

static int logv(char *fmt,int datalen,va_list ap)
{
	Logger *logger = &loggobj;

	const char hex_chars[] = "0123456789ABCDEF";
	unsigned char *b = (unsigned char *)fmt;
	unsigned char *e = b + datalen;

	if (NULL == logger)
		return -1;

	char buf[LOG_BUF_LEN] = {0};
	char hex[LOG_BUF_LEN] = {0};

	int len;
	char *ptr = buf;
	time_t time;
	struct timeval tv;
	struct tm *tm;
	int space = 0;

	unsigned char index = 0;

	gettimeofday(&tv, NULL);

	time = tv.tv_sec;
	tm   = localtime(&time);

	/* %3ld 在数值位数超过3位的时候不起作用, 所以这里转成int */

	len = sprintf(ptr, "%04d-%02d-%02d %02d:%02d:%02d.%03d ",
			      tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour,
			      tm->tm_min, tm->tm_sec, (int) (tv.tv_usec / 1000));

	if (len < 0) {
		return -1;
	}

	ptr += len;

	memcpy(ptr, getLevelName(logger->level), LEVEL_NAME_LEN);
	ptr += LEVEL_NAME_LEN;

	space = sizeof(buf) - (ptr - buf) - 10;

	if (logger->level == LEVEL_HEX)
	{
	   while(index < LOG_BUF_LEN -1 && b < e)
	   {
		   hex[index++] = hex_chars[((*b)>>4) & 0x0F];
		   hex[index++]	= hex_chars[(*b) & 0x0F];
		   b++;
	   }

	   len = vsnprintf(ptr, space, hex, ap);
	}else{

	   len = vsnprintf(ptr, space, fmt, ap);
	}

	if (len < 0) {
		return -1;
	}

	ptr += len > space ? space : len;
	*ptr++ = '\n';
	*ptr = '\0';

	len = ptr - buf;

	logger->lockerobj->Lock(logger->lockerobj->locker);

    fwrite(buf, len, 1, logger->fp);
	fflush(logger->fp);

	logger->wcurr  += len;
	logger->wtotal += len;

	if (logger->rotatesize > 0 && logger->wcurr > logger->rotatesize) {
		rotate(logger);
	}
	logger->lockerobj->Unlock(logger->lockerobj->locker);
	return 0;
}

Logger *Logger_Create(int level,int rotate_size,char *name)
{
	Logger *logger = &loggobj;

	if (NULL != logger) {

		logger->fp          = NULL;

		bzero(logger->name,sizeof(logger->name));
		memcpy(logger->name,name,strlen(name));

		logger->level       = level;
		logger->wcurr       = 0;
		logger->wtotal      = 0;
		logger->rotatesize  = rotate_size * 1024 * 1024;//日志切割默认都是MB;

		logger->lockerobj = LockerObj_Create();

		if(strcmp(name, "stdout") == 0){
			logger->fp = stdout;
		}else if(strcmp(name, "stderr") == 0){
			logger->fp = stderr;
		} else {
			logger->fp = fopen(name, "a");

			if(logger->fp == NULL){
			   printf("open log file faile:file:%s,err:%s\n",name,strerror(errno));
			   return NULL;
		    }
		}
	}

	return logger;
}

void Logger_Destory(void)
{
	Logger *logger = &loggobj;

   if (logger != NULL){
	   Locker_Free(logger->lockerobj->locker);
	   Locker_Clear(logger->lockerobj->locker);
	   logger->lockerobj->locker = NULL;
	   logclose();
   }
}

int logerror(int level,char *fmt, ...)
{
	Logger *logger = &loggobj;

	int ret = 0;
	if (NULL == logger)
		return -1;

	setlevel(level);
	va_list ap;
	va_start(ap, fmt);
	ret = logv(fmt,0,ap);
	va_end(ap);
	return ret;
}

int logdebug(int level,char *fmt, ...)
{
	Logger *logger = &loggobj;

	int ret = 0;
	if (NULL == logger)
	   return -1;

	setlevel(level);
	va_list ap;
	va_start(ap, fmt);
	ret = logv(fmt,0,ap);
	va_end(ap);

	return ret;
}

int logwarn(int level,char *fmt, ...)
{
	Logger *logger = &loggobj;

	int ret = 0;

	if (NULL == logger)
	   return -1;

	setlevel(level);
	va_list ap;
	va_start(ap, fmt);
	ret = logv(fmt,0,ap);
	va_end(ap);
	return ret;
}

int loginfo(int level,char *fmt,...)
{
	Logger *logger = &loggobj;

	int ret = 0;
	if (NULL == logger)
		return -1;

	setlevel(level);
	va_list ap;
	va_start(ap, fmt);
	ret = logv(fmt,0,ap);
	va_end(ap);

	return ret;
}

int loghex(int datalen,char *fmt,...)
{
	Logger *logger = &loggobj;

	int ret = 0;

	if (NULL == logger)
		return -1;

	setlevel(LEVEL_HEX);
	va_list ap;
	va_start(ap,fmt);
	ret = logv(fmt,datalen,ap);
	va_end(ap);

	return ret;
}
