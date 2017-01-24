/**********************************************
 *  connmgr.h
 *
 *  Created on: 2015-12-13
 *      Author: xifengming
 *       Email: xifenmin@vip.sina.com
 *
 *    Comments: epoll event
 *********************************************/

#ifndef _EPOLL_EVENT_H
#define _EPOLL_EVENT_H

#include "epoll.h"
#include "connobj.h"
#include "queue.h"
#include "server.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*epoll_addevent)(struct tagEpollBase *, struct tagConnObj *,int events);
typedef int (*epoll_delevent)(struct tagEpollBase *, struct tagConnObj *);
typedef int (*epoll_modifyevent)(struct tagEpollBase *, struct tagConnObj *,int events);
typedef int (*epoll_waitevent)(struct tagEpollBase *,void  *,int timeout);
typedef void (*epoll_clearevent)(struct tagEpollBase *);

struct tagEpollInterface
{
	struct tagEpollBase  *epollbase;
	epoll_addevent       add;
	epoll_delevent       del;
	epoll_modifyevent    modify;
	epoll_waitevent      wait;
	epoll_clearevent     clear;
};

typedef struct tagEpollInterface  EpollInterface;

EpollInterface *EpollInterface_Create(int events);

void EpollInterface_Destory(EpollInterface *epoll_interface);

/*epoll 事件添加*/
int epollEvent_addConn(struct tagEpollBase *evb, struct tagConnObj  *conn,int events);
/*epoll 事件删除*/
int epollEvent_delConn(struct tagEpollBase *evb, struct tagConnObj  *conn);
/*epoll 修改事件*/
int epollEvent_modifyConn(struct tagEpollBase *evb, struct tagConnObj  *conn,int events /*事件*/);
/*epoll wait*/
int epollEvent_wait(struct tagEpollBase *evb,void *_serverobj,int timeout);

void epollEvent_clear(struct tagEpollBase *evb);

#ifdef __cplusplus
}
#endif

#endif
