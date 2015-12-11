#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <linux/tcp.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "queue.h"
#include "lock.h"
#include "connmgr.h"

struct tagConntMgr {
	LockerObj *lockerobj;
	DataQueue *queue;
	SetConn   set;
	GetConn   get;
};

ConnMgr *ConnMgr_Create(void) {

	ConnMgr *connmgr = NULL;

	connmgr = (ConnMgr *) malloc(sizeof(ConnMgr));

	if (NULL != connmgr) {

		connmgr->queue     = DataQueue_Create();
		connmgr->lockerobj = LockerObj_Create();
		connmgr->set       = setConn;
		connmgr->get       = getConn;
	}

	return connmgr;
}

void ConnMgr_Clear(ConnMgr *connmgr) {
	int i = 0;

	ConnObj *conntobj = NULL;

	if (connmgr == NULL)
		return;

	int size = DataQueue_Size(connmgr->queue);

	connmgr->lockerobj->Lock(connmgr->lockerobj->locker);

	for (; i < size; i++) {
		conntobj = DataQueue_Pop(connmgr->queue);
		if (conntobj != NULL) {
			free(conntobj);
		}
	}

	connmgr->lockerobj->Unlock(connmgr->lockerobj->locker);

	free(connmgr);
}

ConnObj *CreateNewConnObj(void)
{
	ConnObj *connobj = NULL;
	connobj = (ConnObj *)malloc(sizeof(ConnObj));
	return connobj;
}

int setConn(ConnMgr *connmgr, ConnObj *conntobj) {

	if (connmgr != NULL) {
		connmgr->lockerobj->Lock(connmgr->lockerobj->locker);
		if (NULL != conntobj) {
			DataQueue_Push(connmgr->queue, conntobj);
		}
		connmgr->lockerobj->Unlock(connmgr->lockerobj->locker);
	}

	return 0;
}

ConnObj *getConn(ConnMgr *connmgr) {

	ConnObj *_connobj = NULL;

	if (connmgr != NULL) {

		connmgr->lockerobj->Lock(connmgr->lockerobj->locker);
		_connobj = DataQueue_Pop(connmgr->queue);

		if (NULL == _connobj){

			_connobj = CreateNewConnObj();/*建立一个新的连接对象*/

			if (NULL != _connobj) {

				_connobj->fd        = 0;
				_connobj->type      = TCP;
				_connobj->activity  = SOCKET_CONNCLOSED;
				_connobj->send      = sendData;
				_connobj->sendptr   = NULL;
				_connobj->sendlen   = 0;
				_connobj->recvptr   = NULL;
				_connobj->recvlen   = 0;
				_connobj->recv      = readData;
				_connobj->nodelay   = noDelay;
				_connobj->keepalive = keepAlive;
				_connobj->noblock   = noBlock;

				DataQueue_Push(connmgr->queue, _connobj);
		    }
		}

		connmgr->lockerobj->Unlock(connmgr->lockerobj->locker);
	}

	return _connobj;
}

int sendData(ConnObj *conntobj) {

	int ret = 0;
	int len = 0;
	int sendlen=0;
	if (NULL == conntobj) {
		return -1;
	}

	len = conntobj->sendlen;

	while (len > 0) {
		ret = write(conntobj->fd,conntobj->sendptr,len);
		len -= ret;
		conntobj->sendptr += ret;
		sendlen += ret;
	}

	return sendlen;
}

int readData(ConnObj *conntobj,unsigned char *ptr,int len)
{
	int nLen = 0;

	if (NULL == conntobj){
		return -1;
	}

	nLen = read(conntobj->fd,ptr,len);

	if (nLen < 0) {
		if (errno == EAGAIN || errno == EINTR){
			//尝试再读一次。
		}
	}else if (nLen == 0){
		printf("connect closed\n");
		return -1;
	}

	return nLen;
}

void noDelay(ConnObj *connobj,int enable)
{
	int opt = enable? 1 : 0;
	setsockopt(connobj->fd, IPPROTO_TCP, TCP_NODELAY, (void *)&opt, sizeof(opt));
}

void keepAlive(ConnObj *connobj,int enable){
	int opt = enable? 1 : 0;
	setsockopt(connobj->fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&opt, sizeof(opt));
}

void noBlock(ConnObj *connobj,int enable){

	if(enable){
		fcntl(connobj->fd, F_SETFL, O_NONBLOCK | O_RDWR);
	}else{
		fcntl(connobj->fd, F_SETFL, O_RDWR);
	}
}
