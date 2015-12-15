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


int setConn(struct tagConntMgr *connmgr, struct tagConnObj *conntobj) {

	if (connmgr != NULL) {
		connmgr->lockerobj->Lock(connmgr->lockerobj->locker);
		if (NULL != conntobj) {
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
		    }
		}

		connmgr->lockerobj->Unlock(connmgr->lockerobj->locker);
	}

	return _connobj;
}

