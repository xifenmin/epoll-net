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
#include <sys/time.h>
#include <sys/resource.h>
#include "log.h"
#include "connmgr.h"
#include "cstr.h"

#define STACKSIZE (258 * 1024 * 1024)

static inline int init_limit(void)
{
	struct rlimit lim;

	if(getrlimit(RLIMIT_STACK, &lim) < 0)
	{
		perror("getrlimit fail!");
		exit(1);
	}

	printf("stack begin size:%lu\n",lim.rlim_cur);

	if(lim.rlim_cur < STACKSIZE)
	{
		lim.rlim_cur = STACKSIZE;

		if(setrlimit(RLIMIT_STACK, &lim) < 0)
		{
			perror("setrlimit error!");
			exit(1);
		}
	}

	if(getrlimit(RLIMIT_STACK, &lim) < 0)
	{
		perror("getrlimit error!");
		exit(1);
	}

	printf("stack end size:%lu\n",lim.rlim_cur);

	return 0;
}

ConnMgr *ConnMgr_Create(void) {

	ConnMgr *connmgr = NULL;

	connmgr = (ConnMgr *) malloc(sizeof(ConnMgr));

	if (NULL != connmgr) {
		connmgr->queueInterface  = DataQueueInterface_Create();
		connmgr->lockerInterface = LockerInterface_Create();
		connmgr->set             = setConn;
		connmgr->get             = getConn;
		connmgr->reset           = connobjReset;
	}

	init_limit();

	return connmgr;
}

void ConnMgr_Destory(struct tagConntMgr *connmgr) {

	int i = 0;
	ConnObj *conntobj = NULL;

	if (connmgr == NULL)
		return;

	int size = connmgr->queueInterface->size(connmgr->queueInterface->queue);

	connmgr->lockerInterface->lock(connmgr->lockerInterface->locker);

	for (; i < size; i++) {
		conntobj = connmgr->queueInterface->pop(connmgr->queueInterface->queue);
		if (conntobj != NULL) {
			free(conntobj);
		}
	}

	connmgr->queueInterface->clear(connmgr->queueInterface->queue);
	connmgr->lockerInterface->unlock(connmgr->lockerInterface->locker);

	free(connmgr);
}

void  connobjReset(ConnObj *connobj)
{
   if (NULL != connobj){

		connobj->type      = TCP;
		connobj->activity  = SOCKET_CONNCLOSED;
		connobj->send      = sendData;
		connobj->sendlen   = 0;
		connobj->recv      = readData;
		connobj->close     = closeConnObj;
		connobj->nodelay   = noDelay;
		connobj->keepalive = keepAlive;
		connobj->noblock   = noBlock;

		memset(connobj->ip,0,sizeof(connobj->ip));
		connobj->port      = 0;
   }
}

int setConn(struct tagConntMgr *connmgr, struct tagConnObj *conntobj) {

	if (connmgr != NULL) {
		connmgr->lockerInterface->lock(connmgr->lockerInterface->locker);
		if (NULL != conntobj) {
			connobjReset(conntobj);
			connmgr->queueInterface->push(connmgr->queueInterface->queue,conntobj);
		}
		connmgr->lockerInterface->unlock(connmgr->lockerInterface->locker);
	}
	return 0;
}

struct tagConnObj *getConn(struct tagConntMgr *connmgr) {

	ConnObj *_connobj = NULL;

	if (connmgr != NULL) {

		connmgr->lockerInterface->lock(connmgr->lockerInterface->locker);

		_connobj = connmgr->queueInterface->pop(connmgr->queueInterface->queue);

		if (NULL == _connobj){
			_connobj = CreateNewConnObj();/*建立一个新的连接对象*/
			connobjReset(_connobj);
		}

		connmgr->lockerInterface->unlock(connmgr->lockerInterface->locker);
	}

	return _connobj;
}
