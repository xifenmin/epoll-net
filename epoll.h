/**********************************************
 * epoll.h
 *
 *  Created on: 2015-12-02
 *      Author: xifengming
 *       Email: xifengming@vip.sina.com
 *    Comments: net epoll
 *********************************************/
#ifndef _EPOLL_H
#define _EPOLL_H

#include "connmgr.h"

#ifdef __cplusplus {
extern "C" {
#endif

#define  EPOLL_EVENTS_NUM 1024

#define  EVENT_READ       0x0000ff
#define  EVENT_WRITE      0x00ff00
#define  EVENT_ERR        0xff0000

typedef int (*epoll_addevent)(EpollBase *evb, ConnObj *conn);
typedef int (*epoll_delevent)(EpollBase *evb, ConnObj *conn);
typedef int (*epoll_modifyevent)(EpollBase *evb, ConnObj *conn,int events);
typedef int (*epoll_waitevent)(EpollBase *evb, int timeout);
typedef int (*event_callback)(ConnObj *conn,int events);

typedef struct tagEpollBase EpollBase;
typedef struct tagEpollObj  EpollObj;

EpollObj *Epoll_Create_Obj(int events,event_callback cb);
void Epoll_Destory_Obj(EpollObj *epoll_obj);
EpollBase *Init_EpollBase(int events,event_callback cb);
void  Clear_EpollBase(EpollBase *evb);

/*epoll 事件添加*/
int Epoll_Event_AddConn(EpollBase *evb, ConnObj *conn);
/*epoll 事件删除*/
int Epoll_Event_DelConn(EpollBase *evb, ConnObj *conn);
/*epoll 修改事件*/
int Epoll_Event_ModifyConn(EpollBase *evb, ConnObj *conn,int events /*事件*/);
/*epoll wait*/
int Epoll_Event_Wait(EpollBase *evb, int timeout);
/*epoll_wait 到来时，回调接口*/
int Epoll_Event_Callback(ConnObj *conn,int events);

#ifdef __cplusplus{
}
#endif

#endif
