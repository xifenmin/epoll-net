/**********************************************
 *  connmgr.h
 *
 *  Created on: 2015-12-13
 *      Author: xifengming
 *       Email: xifengming@vip.sina.com
 *    Comments: epoll event
 *********************************************/

#ifndef _EPOLL_EVENT_H
#define _EPOLL_EVENT_H

#include "epoll.h"
#include "connobj.h"
#include "queue.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagEpollObj  EpollObj;

typedef int (*epoll_addevent)(struct tagEpollBase *, struct tagConnObj *);
typedef int (*epoll_delevent)(struct tagEpollBase *, struct tagConnObj *);
typedef int (*epoll_modifyevent)(struct tagEpollBase *, struct tagConnObj *,int events);
typedef int (*epoll_waitevent)(struct tagEpollBase *,struct tagDataQueue *,int timeout);

struct tagEpollObj
{
	struct tagEpollBase  *epollbase;
	epoll_addevent       add;
	epoll_delevent       del;
	epoll_modifyevent    modify;
	epoll_waitevent      wait;
};

EpollObj *Epoll_Create_Obj(int events);
void Epoll_Destory_Obj(EpollObj *epoll_obj);

/*epoll 事件添加*/
int Epoll_Event_AddConn(struct tagEpollBase *evb, struct tagConnObj  *conn);
/*epoll 事件删除*/
int Epoll_Event_DelConn(struct tagEpollBase *evb, struct tagConnObj  *conn);
/*epoll 修改事件*/
int Epoll_Event_ModifyConn(struct tagEpollBase *evb, struct tagConnObj  *conn,int events /*事件*/);
/*epoll wait*/
int Epoll_Event_Wait(struct tagEpollBase *evb,struct tagDataQueue *queue,int timeout);


#ifdef __cplusplus
}
#endif

#endif
