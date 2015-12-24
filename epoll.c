#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include "log.h"
#include "connmgr.h"
#include "epoll.h"
#include "server.h"
#include "cstr.h"

EpollBase *Init_EpollBase(int events)
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

int Epoll_Event_Callback(void *_serverobj,void *connobj,int events)
{

	int val = 0;
	errno = 0;
	socklen_t lon = sizeof(int);
	int ret = 0;

	ConnObj *_connobj = (ConnObj *)connobj;

	ServerObj *serverobj = (ServerObj *)_serverobj;

	unsigned char recvbuffer[1024 * 16] = { 0 };

	int recvlen = 0;
	int datalen = 0;

	char *dptr = NULL;

	if (NULL == serverobj) {
		return -1;
	}

	if (serverobj->connobj == _connobj)
		_connobj = Server_Accept(serverobj);

	if (_connobj == NULL)
		return -1;

	ret = getsockopt(_connobj->fd, SOL_SOCKET, SO_ERROR, (void *) &val, &lon);

	if (ret == -1) {
		log_error("connect getsockopt() errno %d, %s, fd %d", errno,strerror(errno), _connobj->fd);
		return -1;
	}

	if (val == 0) {
		_connobj->activity = SOCKET_CONNECTED;
	} else {
		_connobj->activity = SOCKET_CONNCLOSED;
	}

	if (val != 0) {
		close(_connobj->fd);
	}

	if (EPOLLIN & events) { /*检测到读事件*/

		recvlen = _connobj->recv(_connobj, recvbuffer, sizeof(recvbuffer));
		dptr    = CStr_Malloc((char *)recvbuffer);/*把recvbuffer，拷贝到dptr动态内存中*/

		if (recvlen == 0){
			log_debug("push connobj to conn poll,fd:%d!!!\n",_connobj->fd);
			serverobj->connmgr->set(serverobj->connmgr,_connobj);
			return 0;
		}

		if (recvlen < 0) {

			recvlen = _connobj->recv(_connobj, recvbuffer, sizeof(recvbuffer));

			if (recvlen > 0) {

				if (dptr == NULL){
					dptr    = CStr_Malloc((char *)recvbuffer);
				}

				datalen += recvlen;

			}
		}

		datalen = recvlen;

		_connobj->recvlen = datalen;
		_connobj->recvptr = (unsigned char *)dptr;/*接收数据指针*/
	}

	if (EPOLLOUT & events) { /*检测到写事件*/
		if (_connobj->sendptr != NULL && _connobj->sendlen > 0) {
			datalen = _connobj->send(_connobj);
		}
	}

	return datalen;
}


