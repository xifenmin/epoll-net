#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "connmgr.h"
#include "queue.h"

struct tagConnObj {
	int       fd; /*sockt 对象*/
	int       type;/*传输类型:tcp、udp*/
	time_t    last_time;
	int       activity;/*连接是否正常*/
	char      ip[32];
	unsigned char port;
	SendData  send;
	ReadData  recv;
	unsigned char *recvptr;/*接收数据指针*/
	unsigned char *sendptr;/*发送数据指针*/
	unsigned int  recvlen;/*接收数据长度*/
	unsigned int  sendlen;/*发送数据长度*/
	Nodelay   nodelay;
	Keepalive keepalive;
	Noblock   noblock;
};

struct tagConntMgr {
	DataQueue *queue;
	LockerObj *lockerobj;
	SetConn   set;
	GetConn   get;
};

ConnMgr *ConnMgr_Create(void) {

	ConnMgr *connmgr = NULL;

	connmgr = (ConnMgr *) malloc(sizeof(ConnMgr));

	if (NULL != connmgr) {

		connmgr->queue     = DataQueue_Create();
		connmgr->lockerobj = LockerObj_Create();
		connmgr->set       = SetConn;
		connmgr->get       = GetConn;
	}
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

int SetConn(ConnMgr *connmgr, ConnObj *conntobj) {

	if (connmgr != NULL) {
		connmgr->lockerobj->Lock(connmgr->lockerobj->locker);
		if (NULL != conntobj) {
			DataQueue_Push(connmgr->queue, conntobj);
		}
		connmgr->lockerobj->Unlock(connmgr->lockerobj->locker);
	}

	return 0;
}

ConnObj *GetConn(ConnMgr *connmgr) {

	ConnObj *_conntobj = NULL;

	if (connmgr != NULL) {

		connmgr->lockerobj->Lock(connmgr->lockerobj->locker);
		_connobj = DataQueue_Pop(connmgr->queue);

		if (NULL == _connobj){

			_conntobj = CreateNewConnObj();/*建立一个新的连接对象*/

			if (NULL != _conntobj) {

				_conntobj->fd        = 0;
			    _conntobj->type      = TCP;
				_conntobj->activity  = SOCKET_CONNCLOSED;
				_conntobj->send      = SendData;
				_conntobj->recv      = ReadData;
				_conntobj->nodelay   = Nodelay;
				_conntobj->keepalive = Keepalive;
				_conntobj->noblock   = Noblock;

				DataQueue_Push(connmgr->queue, _conntobj);
		    }
		}

		connmgr->lockerobj->Unlock(connmgr->lockerobj->locker);
	}

	return _conntobj;
}

int SendData(ConnObj *conntobj) {

	int ret = 0;
	int send_len = 0;

	if (NULL == conntobj) {
		return -1;
	}

	while (len > 0) {
		ret = write(conntobj->fd,conntobj->sendptr, conntobj->sendlen);
		len -= ret;
		ptr += ret;
		send_len += ret;
	}

	return send_len;
}

int ReadData(ConnObj *conntobj,unsigned char *ptr,int len)
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

void Nodelay(ConnObj *connobj,int enable=1)
{
	int opt = enable? 1 : 0;
	setsockopt(connobj->fd, IPPROTO_TCP, TCP_NODELAY, (void *)&opt, sizeof(opt));
}

void Keepalive(ConnObj *connobj,int enable=1){
	int opt = enable? 1 : 0;
	setsockopt(connobj->fd, SOL_SOCKET, SO_KEEPALIVE, (void *)&opt, sizeof(opt));
}

void Noblock(ConnObj *connobj,int enable=1){

	noblock_ = enable;

	if(enable){
		fcntl(connobj->fd, F_SETFL, O_NONBLOCK | O_RDWR);
	}else{
		fcntl(connobj->fd, F_SETFL, O_RDWR);
	}
}
