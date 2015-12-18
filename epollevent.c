#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "queue.h"
#include "epollevent.h"
#include "server.h"

EpollObj *Epoll_Create_Obj(int events)
{
	EpollObj *epoll_obj = NULL;

	epoll_obj = (EpollObj *)malloc(sizeof(EpollObj));

	if (NULL != epoll_obj){

		epoll_obj->epollbase = Init_EpollBase(events);
		epoll_obj->add       = Epoll_Event_AddConn;
		epoll_obj->del       = Epoll_Event_DelConn;
		epoll_obj->modify    = Epoll_Event_ModifyConn;
		epoll_obj->wait      = Epoll_Event_Wait;
	}

	return epoll_obj;
}

void Epoll_Destory_Obj(EpollObj *epoll_obj)
{
    if (epoll_obj != NULL){
    	Clear_EpollBase(epoll_obj->epollbase);
    	free(epoll_obj);
    	epoll_obj = NULL;
    }
}

int Epoll_Event_AddConn(struct tagEpollBase *evb, struct tagConnObj  *conn)
{
	int status;

	if (evb == NULL || conn == NULL) {
		 return -1;
	}

	evb->event->events   = (uint32_t) (EPOLLIN | EPOLLOUT | EPOLLET);
	evb->event->data.ptr = conn;

	status = epoll_ctl(evb->epollhandle,EPOLL_CTL_ADD,conn->fd,evb->event);

	if (status < 0) {
		printf("epoll ctl on e %d client handle %d failed:%s",evb->epollhandle,conn->fd,strerror(errno));
		return -1;
	}

	return status;
}

int Epoll_Event_DelConn(struct tagEpollBase *evb, struct tagConnObj  *conn)
{
	int status;

	if (evb == NULL){
		return -1;
	}

	status = epoll_ctl(evb->epollhandle, EPOLL_CTL_DEL,conn->fd,NULL);

	if (status < 0){
		printf("epoll ctl on e %d client handle %d failed:%s",evb->epollhandle,conn->fd,strerror(errno));
		return -1;
	}
	return status;
}

int Epoll_Event_ModifyConn(struct tagEpollBase *evb, struct tagConnObj  *conn,int events/*事件*/)
{
	int status = 0;

    if (evb == NULL){
    	return -1;
    }

    if (events & EVENT_READ)
    	events |= EPOLLIN;
    if (events & EVENT_WRITE)
    	events |= EPOLLOUT;

    evb->event->events = events;

    status = epoll_ctl(evb->epollhandle, EPOLL_CTL_MOD,conn->fd,evb->event);

    if (status < 0){
		printf("epoll ctl on e %d client handle %d failed:%s",evb->epollhandle,conn->fd,strerror(errno));
		return -1;
    }

    return status;

}

int Epoll_Event_Wait(struct tagEpollBase *evb,void *_serverobj,int timeout)
 {
	int i = 0;
	int events = 0;
	int len    = 0;

	ConnObj *_connobj = NULL;
	ServerObj *serverobj = (ServerObj *)_serverobj;

	struct epoll_event *ev = NULL;

	if (evb == NULL || NULL == serverobj) {
		return -1;
	}

	int fd_active_nums = epoll_wait(evb->epollhandle, evb->event,EPOLL_EVENTS_NUM, timeout);

	for (; i < fd_active_nums; i++) {

		ev = &evb->event[i];

		if (ev->events & EPOLLOUT) {
			events |= EVENT_WRITE;
		}

		if (ev->events & EPOLLERR) {
			events |= EVENT_ERR;
		}

		if (ev->events & (EPOLLIN | EPOLLHUP)) {
			events |= EVENT_READ;
		}

		if (evb->cb != NULL) {

			_connobj = ev->data.ptr;

			len = evb->cb(serverobj,_connobj,events);

			if (len >0){
				Locker_Lock(serverobj->lockerobj->locker);
				DataQueue_Push(serverobj->dataqueue,_connobj);
				Locker_Unlock(serverobj->lockerobj->locker);
				Locker_Signal(serverobj->lockerobj->locker);
			}
		}
	}

	return fd_active_nums;
}


