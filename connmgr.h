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

#ifdef __cplusplus {
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

typedef void (*Noblock)(ConnObj *connobj,int enable=1);
typedef void (*Keepalive)(ConnObj *connobj,int enable=1);
typedef void (*Nodelay)(ConnObj *connobj,int enable=1);

typedef int (*SetConn)(ConnMgr *connmgr,ConnObj *conntobj);
typedef ConnObj *(*GetConn)(ConnMgr *connmgr);
typedef int (*SendData)(ConnMgr *connmgr,unsigned char *buffer,int len);

typedef struct tagConnObj       ConnObj;
typedef struct tagConntMgr      ConnMgr;

ConnObj *CreateNewConnObj(void);
/*建立连接池*/
ConnMgr *ConnMgr_Create(void);
void ConnMgr_Clear(ConnMgr *connmgr);
/*push 一个新连接push到连接池中*/
int SetConn(ConnMgr *connmgr,ConnObj *conntobj);
/*pop 从连接对象池中pop一个对象出来*/
ConnObj *GetConn(ConnMgr *connmgr);
/*发送数据*/
int SendData(ConnObj *conntobj,unsigned char *ptr,int len);
/*接收数据*/
int ReadData(ConnObj *conntobj,unsigned char *buffer);

void Nodelay(ConnObj *connobj,int enable=1);
void Keepalive(ConnObj *connobj,int enable=1);
void Noblock(ConnObj *connobj,int enable=1);

#ifdef __cplusplus{
}
#endif

#endif
