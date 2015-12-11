/**********************************************
 *  connmgr.h
 *
 *  Created on: 2015-12-02
 *      Author: xifengming
 *       Email: xifengming@vip.sina.com
 *    Comments: socket conn pool manage
 *********************************************/

#ifndef _CONN_MGR_H
#define _CONN_MGR_H

#ifdef __cplusplus
extern "C" {
#endif

enum ConnectType
{
	TCP = 1,
	UDP = 2
};

enum ConnectState
{
	SOCKET_CONNECTING = 0 ,   // 正在连接状态
	SOCKET_CONNECTED  = 1 ,   // 已连接成功状态
	SOCKET_CONNCLOSED = 2	  // 已关闭状态
};

typedef struct tagConnObj       ConnObj;
typedef struct tagConntMgr      ConnMgr;

typedef void (*Noblock)(ConnObj *connobj,int enable);
typedef void (*Keepalive)(ConnObj *connobj,int enable);
typedef void (*Nodelay)(ConnObj *connobj,int enable);

typedef int (*SetConn)(ConnMgr *connmgr,ConnObj *conntobj);
typedef ConnObj *(*GetConn)(ConnMgr *connmgr);
typedef int (*SendData)(ConnObj *conntobj);/*发送回调*/
typedef int (*ReadData)(ConnObj *conntobj,unsigned char *ptr,int len);/*接收回调*/

struct tagConnObj {
	int       fd; /*sockt 对象*/
	int       type;/*传输类型:tcp、udp*/
	time_t    last_time;
	int       activity;/*连接是否正常*/
	char      ip[32];
	unsigned char port;
	SendData  send;
	ReadData  recv;
	unsigned char  *recvptr;/*接收数据指针*/
	unsigned char  *sendptr;/*发送数据指针*/
	unsigned int   recvlen;/*接收数据长度*/
	unsigned int   sendlen;/*发送数据长度*/
	Nodelay        nodelay;
	Keepalive      keepalive;
	Noblock        noblock;
};

ConnObj *CreateNewConnObj(void);
/*建立连接池*/
ConnMgr *ConnMgr_Create(void);
void ConnMgr_Clear(ConnMgr *connmgr);
/*push 一个新连接push到连接池中*/
int setConn(ConnMgr *connmgr,ConnObj *conntobj);
/*pop 从连接对象池中pop一个对象出来*/
ConnObj *getConn(ConnMgr *connmgr);
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
