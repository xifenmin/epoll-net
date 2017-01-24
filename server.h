/**********************************************
 *  server.h
 *
 *  Created on: 2015-12-02
 *      Author: xifengming
 *       Email: xifenmin@vip.sina.com
 *
 *    Comments: socket server
 *********************************************/
#ifndef _SERVER_H
#define _SERVER_H

#include "lock.h"
#include "queue.h"
#include "threadpool.h"
#include "connmgr.h"
#include "epollevent.h"
#include "array.h"

#ifdef __cplusplus
extern "C" {
#endif

struct tagServerObj
{
	struct tagEpollInterface    *epollInterface;
	DynamicArrayInterface *dynamicarrayInterface;/*动态数组*/
	ConnMgr               *connmgr;/*server 端连接池，管理客户端连接*/
	ConnObj               *connobj;/*server 端连接描述符*/
	LockerInterface       *lockerInterface;/*接收队列锁*/
	DataQueueInterface    *rqueueInterface;/*接收队列*/
	Threadpool            *serverthread;//主 server 线程池
	Threadpool            *datathread;//处理接收数据  线程池
	ProcRead               procread;/*客户端回调*/
};

typedef struct tagServerObj   ServerObj;

ServerObj *Server_Create(int events);
void Server_Clear(ServerObj *serverobj);
ServerObj *StartServer(char *ip,unsigned short port,ProcRead procread);
int  Server_Listen(ServerObj *serverobj);
ConnObj  *Server_Accept(ServerObj *serverobj);
void Server_Process(void *argv);
void Server_Send_Process(void *argv);
void Server_Loop(void *argv);
int  ServerSend(ServerObj *serverobj,ConnObj *connobj,char *data,int len);

#ifdef __cplusplus
}
#endif

#endif
