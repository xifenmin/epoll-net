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

#ifdef __cplusplus
extern "C" {
#endif

#define  EPOLL_EVENTS_NUM 1024
#define  EVENT_READ       0x0000ff
#define  EVENT_WRITE      0x00ff00
#define  EVENT_ERR        0xff0000

typedef int (*event_callback)(void *,int events);

struct tagEpollBase
{
    int epollhandle;
    struct epoll_event *event;
    event_callback      cb;/*event 事件回调函数*/
};

typedef struct tagEpollBase EpollBase;


EpollBase *Init_EpollBase(int events);
void  Clear_EpollBase(EpollBase *evb);
/*epoll_wait 到来时，回调接口*/
int Epoll_Event_Callback(void *_serverobj,int events);

#ifdef __cplusplus
}
#endif

#endif
