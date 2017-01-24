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
#include "log.h"
#include "queue.h"
#include "epollevent.h"
#include "server.h"
#include "cstr.h"

EpollInterface *EpollInterface_Create(int events)
{
	EpollInterface *epoll_interface = NULL;
	epoll_interface = (EpollInterface *)malloc(sizeof(EpollInterface));

	if (NULL != epoll_interface){

		epoll_interface->epollbase = epollBase_init(events);
		epoll_interface->add       = epollEvent_addConn;
		epoll_interface->del       = epollEvent_delConn;
		epoll_interface->modify    = epollEvent_modifyConn;
		epoll_interface->wait      = epollEvent_wait;
		epoll_interface->clear     = epollEvent_clear;
	}

	return epoll_interface;
}

void EpollInterface_Destory(EpollInterface *epoll_interface)
{
    if (epoll_interface != NULL){
    	epollEvent_clear(epoll_interface->epollbase);
    	free(epoll_interface);
    	epoll_interface = NULL;
    }
}

void epollEvent_clear(struct tagEpollBase *evb)
{
	if (evb != NULL){
		epollBase_destory(evb);
	}
}

int epollEvent_addConn(struct tagEpollBase *evb, struct tagConnObj  *conn,int events)
{
	int status;

	if (evb == NULL || conn == NULL) {
		 return -1;
	}

	evb->event->events   = (uint32_t)events;
	evb->event->data.ptr = conn;

	status = epoll_ctl(evb->epollhandle,EPOLL_CTL_ADD,conn->fd,evb->event);

	if (status < 0) {
		log_error("epoll add conn event on %d client handle %d failed:%s",evb->epollhandle,conn->fd,strerror(errno));
		return -1;
	}

	return status;
}

int epollEvent_delConn(struct tagEpollBase *evb, struct tagConnObj  *conn)
{
	int status;

	if (evb == NULL){
		return -1;
	}

	status = epoll_ctl(evb->epollhandle, EPOLL_CTL_DEL,conn->fd,NULL);

	if (status < 0){
		log_error("epoll ctl del conn event on e %d client handle %d failed:%s",evb->epollhandle,conn->fd,strerror(errno));
		return -1;
	}

	return status;
}

int epollEvent_modifyConn(struct tagEpollBase *evb, struct tagConnObj  *conn,int events/*事件*/)
{
	int status = 0;

    if (evb == NULL){
    	return -1;
    }

    evb->event->events = EPOLLET;

    if (events & EVENT_READ)
    	evb->event->events |= EPOLLIN;
    if (events & EVENT_WRITE)
    	evb->event->events |= EPOLLOUT;

    evb->event->data.ptr = conn;

    status = epoll_ctl(evb->epollhandle, EPOLL_CTL_MOD,conn->fd,evb->event);

    if (status < 0){
    	log_error("epoll modify conn event on e %d client handle %d failed:%s",evb->epollhandle,conn->fd,strerror(errno));
		return -1;
    }

    return status;
}

int epollEvent_wait(struct tagEpollBase *evb,void *_serverobj,int timeout)
 {
	int i = 0;
	int events = 0;

	ConnObj *_connobj    = NULL;
	ServerObj *serverobj = (ServerObj *)_serverobj;

	struct epoll_event *ev = NULL;

	if (evb == NULL || NULL == serverobj) {
		return -1;
	}

	int fd_active_nums = epoll_wait(evb->epollhandle, evb->event,EPOLL_EVENTS_NUM,timeout);

	for (; i < fd_active_nums; i++) {
		ev = &evb->event[i];

		if (ev->events & (EPOLLOUT|EPOLLHUP)) {
			events |= EVENT_WRITE;

			if (evb->cb != NULL){
				_connobj = ev->data.ptr;
				evb->cb(serverobj, _connobj, events);
			}
		}

		if (ev->events & EPOLLERR) {
			events |= EVENT_ERR;
		}

		if (ev->events & (EPOLLIN | EPOLLHUP)) {
			events |= EVENT_READ;
			if (evb->cb != NULL) {
				_connobj = ev->data.ptr;
				evb->cb(serverobj, _connobj, events);
		}
	  }
	}
	return fd_active_nums;
}
