#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "epoll.h"

struct tagEpollBase
{
    int epollhandle;
    struct epoll_event *event;
    event_callback   cb;/*event 事件回调函数*/
};

struct tagEpollObj
{
	struct tagEpollBase  *epollbase;
	epoll_addevent       add;
	epoll_delevent       del;
	epoll_modifyevent    modify;
	epoll_waitevent      wait;
};

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

EpollBase *Init_EpollBase(int events)
{
	EpollBase *evb = NULL;

	struct epoll_event *event = NULL;

	int ephandle = 0;
	ephandle     = epoll_create(events);

	if (ephandle < 0) {
		printf("epoll create of size:%d failed! %s",events,strerror(errno));
		return NULL;
	}

	event = malloc(events * sizeof(struct epoll_event));

    if (event == NULL){
    	return NULL;
    }

	evb = (EpollBase *)malloc(sizeof(EpollBase));

	if (evb != NULL){
        evb->epollhandle = ephandle;
        evb->cb          = Epoll_Event_Callback;
        evb->event       = event;
	}

	return evb;
}

void Clear_EpollBase(EpollBase *evb)
{
	if (evb != NULL){
        close(evb->epollhandle);
        if (evb->event != NULL){
        	free(evb->event);
        }
        free(evb);
        evb = NULL;
	}
}

int Epoll_Event_AddConn(EpollBase *evb, ConnObj *conn)
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

int Epoll_Event_DelConn(EpollBase *evb, ConnObj *conn)
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

int Epoll_Event_ModifyConn(EpollBase *evb, ConnObj *conn,int events /*事件*/)
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

int Epoll_Event_Wait(EpollBase *evb, int timeout)
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

int Epoll_Event_Callback(ConnObj *conn,int events)
 {
	int val = 0;
	errno   = 0;
	socklen_t lon = sizeof(int);
	int ret = 0;
	unsigned char recvbuffer[1024*16] = {0};

	int  recvlen = 0;
	int  datalen = 0;
	int  sendlen = 0;

	if (NULL == conn) {
		return -1;
	}

	if (conn->activity == SOCKET_CONNECTING) { /*正在连接*/

		ret = getsockopt(conn->fd, SOL_SOCKET, SO_ERROR, (void *) &val,&lon);

		if (ret == -1) {
			printf("connect getsockopt() errno %d, %s, fd %d",errno,strerror(errno),conn->fd);
			return -1;
		}

		if (val == 0){
			conn->activity = SOCKET_CONNECTED;
		}else{
			conn->activity = SOCKET_CONNCLOSED;
		}

		if (val != 0 ){
			close(conn->fd);
		}
	}

	if (EPOLLIN & events){ /*检测到读事件*/

		recvlen = conn->recv(conn,recvbuffer,sizeof(recvbuffer));
		if (recvlen <0){
			recvlen = conn->recv(conn,recvbuffer,sizeof(recvbuffer));
			if (recvlen >0){
				datalen += recvlen;
			}
		}else{
			datalen += recvlen;
		}

		memcpy(conn->recvptr,recvbuffer,recvlen);
		conn->recvlen = datalen;
	}

	if (EPOLLOUT & events){ /*检测到写事件*/
		if (conn->sendptr != NULL && conn->sendlen > 0 ){
			sendlen = conn->send(conn);
		}
	}

	return datalen;
}
