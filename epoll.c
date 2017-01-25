#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <time.h>
#include "log.h"
#include "connmgr.h"
#include "epoll.h"
#include "server.h"
#include "cstr.h"

EpollBase *epollBase_init(int events)
{
	EpollBase *evb = NULL;

	struct epoll_event *event = NULL;

	int ephandle = 0;
	ephandle     = epoll_create(events);

	if (ephandle < 0) {
		log_error("epoll create of size:%d failed! %s",events,strerror(errno));
		return NULL;
	}

	event = malloc(events * sizeof(struct epoll_event));

    if (event == NULL){
    	return NULL;
    }

	evb = (EpollBase *)malloc(sizeof(EpollBase));

	if (evb != NULL){
        evb->epollhandle = ephandle;
        evb->cb          = epollEvent_callback;
        evb->event       = event;
	}

	return evb;
}

void epollBase_destory(EpollBase *evb)
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

void event_write(ServerObj *serverobj,ConnObj *connobj)
{
	int datalen    = 0;
	int ret        = -1;

	if (connobj->sendptr != NULL && connobj->sendlen > 0 ) {
		datalen = connobj->send(connobj);

	    log_info("send data ip:%s,port:%d,data:%s,len:%d",connobj->ip,connobj->port,connobj->sendptr,datalen);

	    if (connobj->sendptr != NULL) {
	    	CStr_Free((char *)connobj->sendptr);
	    	connobj->sendptr = NULL;
	    	connobj->sendlen = 0;
	    }

	    ret = serverobj->epollInterface->modify(serverobj->epollInterface->epollbase,connobj,EVENT_READ|EPOLLERR);

	    if (ret < 0){
	    	CStr_Free((char *) connobj->sendptr);
	    	connobj->sendptr = NULL;
	    	connobj->sendlen = 0;
		}
	}
}

void event_read(ServerObj *serverobj,ConnObj *connobj)
{
	Item     *item = NULL;
	char *dptr     = NULL;

	unsigned char recvbuffer[1024 * 16] = { 0 };

	int recvlen = connobj->recv(connobj, recvbuffer, sizeof(recvbuffer));

	if (recvlen == 0){
		serverobj->epollInterface->del(serverobj->epollInterface->epollbase,connobj);

		log_debug("read error push connobj to conn poll,fd:%d!!!",connobj->fd);

		connobj->close(connobj);
		serverobj->connmgr->set(serverobj->connmgr,connobj);
		return;
    }

	if (recvlen > 0) {

		if (dptr == NULL){
			dptr = CStr_Malloc((char *)recvbuffer,recvlen);
		}

		item = (Item *) malloc(sizeof(struct tagConnItem));

		if (item != NULL) {
			item->connobj = connobj;
			item->recvptr = dptr;
			item->recvlen = recvlen;

			connobj->last_time = time(NULL);

			serverobj->lockerInterface->lock(serverobj->lockerInterface->locker);
			serverobj->rqueueInterface->push(serverobj->rqueueInterface->queue,item);
			serverobj->lockerInterface->unlock(serverobj->lockerInterface->locker);
			serverobj->lockerInterface->post(serverobj->lockerInterface->locker);
		}
	}
}

int epollEvent_callback(void *_serverobj,void *connobj,int events)
{
	int val       = 0;
	errno         = 0;
	socklen_t lon = sizeof(int);
	int ret       = 0;

	ConnObj *_connobj = (ConnObj *)connobj;

	ServerObj *serverobj = (ServerObj *)_serverobj;

	if (NULL == serverobj) {
		return -1;
	}

	if (serverobj->connobj == _connobj) /*濡傛灉鏄湇鍔＄*/
		_connobj = Server_Accept(serverobj);

	if (_connobj == NULL)
		return -1;

	ret = getsockopt(_connobj->fd, SOL_SOCKET, SO_ERROR, (void *) &val, &lon);

	if (ret <0) {

		log_debug("getsockopt (SO_ERROR):%s,push connobj to conn poll,fd:%d!!!\n",strerror(errno),_connobj->fd);
		return ret;
	}

	if (val == 0) {
		_connobj->activity = SOCKET_CONNECTED;
	} else {
		_connobj->activity = SOCKET_CONNCLOSED;
	}

	if (val != 0) {
		close(_connobj->fd);
	}

	if (EVENT_READ & events) { /*妫�祴鍒拌浜嬩欢*/

		event_read(serverobj,_connobj);
	}

	if (EVENT_WRITE & events) { /*妫�祴鍒板啓浜嬩欢*/
		event_write(serverobj,_connobj);
	}

	return 0;
}

