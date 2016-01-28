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

		connmgr->queue     = DataQueue_Create();
		connmgr->lockerobj = LockerObj_Create();
		connmgr->set       = setConn;
		connmgr->get       = getConn;
		connmgr->reset     = connobjReset;
	}

	init_limit();

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

	DataQueue_Clear(connmgr->queue);

	connmgr->lockerobj->Unlock(connmgr->lockerobj->locker);
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

