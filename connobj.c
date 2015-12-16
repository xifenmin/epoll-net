#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <linux/tcp.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "connobj.h"
#include "connmgr.h"

ConnObj *CreateNewConnObj(void)
{
	ConnObj *connobj = NULL;
	connobj = (ConnObj *)malloc(sizeof(ConnObj));

	if (NULL != connobj)
	{
		connobj->type      = TCP;
		connobj->activity  = SOCKET_CONNCLOSED;
		connobj->send      = sendData;
		connobj->sendptr   = NULL;
		connobj->sendlen   = 0;
		connobj->recvptr   = NULL;
		connobj->recvlen   = 0;
		connobj->recv      = readData;
		connobj->nodelay   = noDelay;
		connobj->keepalive = keepAlive;
		connobj->noblock   = noBlock;

		memset(connobj->ip,0,sizeof(connobj->ip));
		connobj->port      = 0;
	}

	return connobj;
}

int sendData(ConnObj *conntobj) {

	int ret     = 0;
	int len     = 0;
	int sendlen = 0;

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
		printf("connect closed:%d\n",conntobj->fd);
		return 0;
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

