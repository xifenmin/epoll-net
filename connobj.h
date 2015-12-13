/**********************************************
 *  connmgr.h
 *
 *  Created on: 2015-12-13
 *      Author: xifengming
 *       Email: xifengming@vip.sina.com
 *    Comments: socket conn obj
 *********************************************/

#ifndef _CONN_OBJ_H
#define _CONN_OBJ_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagConnObj        ConnObj;

typedef void (*Noblock)(struct tagConnObj *,int enable);
typedef void (*Keepalive)(struct tagConnObj *,int enable);
typedef int  (*SendData)(struct tagConnObj *);/*发送回调*/
typedef int  (*ReadData)(struct tagConnObj *,unsigned char *ptr,int len);/*接收回调*/
typedef void (*Nodelay)(struct tagConnObj *,int enable);
typedef int  (*ProcRead)(struct tagConnObj *);

struct tagConnObj {
	int       fd; /*sockt 对象*/
	int       type;/*传输类型:tcp、udp*/
	time_t    last_time;
	int       activity;/*连接是否正常*/
	char      ip[32];
	unsigned char port;
	unsigned char  *recvptr;/*接收数据指针*/
	unsigned char  *sendptr;/*发送数据指针*/
	unsigned int   recvlen;/*接收数据长度*/
	unsigned int   sendlen;/*发送数据长度*/
	SendData       send;
	ReadData       recv;
	Nodelay        nodelay;
	Keepalive      keepalive;
	Noblock        noblock;
};

ConnObj *CreateNewConnObj(void);
/*发送数据*/
int sendData(ConnObj *conntobj);
/*接收数据*/
int readData(ConnObj *conntobj,unsigned char *ptr,int len);

void noDelay(ConnObj *connobj,int enable);
void keepAlive(ConnObj *connobj,int enable);
void noBlock(ConnObj *connobj,int enable);


#ifdef __cplusplus
}
#endif

#endif
