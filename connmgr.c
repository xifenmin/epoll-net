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

#include "connmgr.h"

ConnMgr *ConnMgr_Create(void) {

	ConnMgr *connmgr = NULL;

	connmgr = (ConnMgr *) malloc(sizeof(ConnMgr));

	if (NULL != connmgr) {

		connmgr->queue     = DataQueue_Create();
		connmgr->lockerobj = LockerObj_Create();
		connmgr->set       = setConn;
		connmgr->get       = getConn;
		connmgr->reset     = connobjReset;
	}

	return connmgr;
}

void ConnMgr_Clear(struct tagConntMgr *connmgr) {
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

void  connobjReset(ConnObj *connobj)
{
   if (NULL != connobj){

		connobj->type      = TCP;
		connobj->activity  = SOCKET_CONNCLOSED;
		connobj->send      = sendData;
		connobj->sendptr   = NULL; //还没处理内存释放
		connobj->sendlen   = 0;
		connobj->recvptr   = NULL;//还没处理内存释放
		connobj->recvlen   = 0;
		connobj->recv      = readData;
		connobj->nodelay   = noDelay;
		connobj->keepalive = keepAlive;
		connobj->noblock   = noBlock;

		memset(connobj->ip,0,sizeof(connobj->ip));
		connobj->port      = 0;
   }
}

int setConn(struct tagConntMgr *connmgr, struct tagConnObj *conntobj) {

	if (connmgr != NULL) {
		connmgr->lockerobj->Lock(connmgr->lockerobj->locker);
		if (NULL != conntobj) {
			connobjReset(conntobj);
			DataQueue_Push(connmgr->queue, conntobj);
		}
		connmgr->lockerobj->Unlock(connmgr->lockerobj->locker);
	}

	return 0;
}

struct tagConnObj *getConn(struct tagConntMgr *connmgr) {

	ConnObj *_connobj = NULL;

	if (connmgr != NULL) {

		connmgr->lockerobj->Lock(connmgr->lockerobj->locker);
		_connobj = DataQueue_Pop(connmgr->queue);

		if (NULL == _connobj){

			_connobj = CreateNewConnObj();/*建立一个新的连接对象*/
			connobjReset(_connobj);
		}

		connmgr->lockerobj->Unlock(connmgr->lockerobj->locker);
	}

	return _connobj;
}

