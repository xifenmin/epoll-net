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

#include "queue.h"
#include "lock.h"
#include "connobj.h"

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

typedef struct tagConntMgr    ConnMgr;
typedef int (*SetConn)(struct tagConntMgr *,struct tagConnObj *);
typedef struct tagConnObj *(*GetConn)(struct tagConntMgr *);
typedef void (*Reset)(struct tagConnObj *);

struct tagConntMgr {
	LockerObj *lockerobj;
	DataQueue *queue;
	SetConn    set;
	GetConn    get;
	Reset      reset;
};

/*建立连接池*/
ConnMgr *ConnMgr_Create(void);
void ConnMgr_Clear(struct tagConntMgr *connmgr);
/*连接Reset*/
void   connobjReset(ConnObj *connobj);
/*push 一个新连接push到连接池中*/
int setConn(struct tagConntMgr *connmgr,struct tagConnObj *conntobj);
/*pop 从连接对象池中pop一个对象出来*/
ConnObj *getConn(struct tagConntMgr *connmgr);

#ifdef __cplusplus
}
#endif

#endif
