#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "epollevent.h"

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
	struct epoll_event event;

	if (evb == NULL || conn == NULL) {
		 return -1;
	}

	event.events   = (uint32_t) (EPOLLIN | EPOLLOUT | EPOLLET);
	event.data.ptr = conn;

	status = epoll_ctl(evb->epollhandle,EPOLL_CTL_ADD,conn->fd, &event);

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

int Epoll_Event_Wait(struct tagEpollBase *evb, int timeout)
 {
	int i = 0;
	int events = 0;

	struct epoll_event *ev = NULL;

	if (evb == NULL) {
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
			evb->cb(ev->data.ptr, events);
		}
	}

	return fd_active_nums;
}


