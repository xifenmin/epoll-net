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
#include "log.h"
#include "connobj.h"
#include "connmgr.h"

ConnObj *CreateNewConnObj(void)
{
	ConnObj *connobj = NULL;
	connobj = (ConnObj *)malloc(sizeof(ConnObj));

	if (connobj != NULL){
		connobj->sendptr = NULL;
	}

	return connobj;
}

int sendData(ConnObj *conntobj) {

	int ret     = 0;
	int len     = 0;

	if (NULL == conntobj) {
		return -1;
	}

	len = conntobj->sendlen;

	ret = write(conntobj->fd,conntobj->sendptr,len);

	return ret;
}

int readData(ConnObj *conntobj,unsigned char *ptr,int len)
{
	int nLen = 0;

	if (NULL == conntobj){
		return -1;
	}

	nLen = read(conntobj->fd,ptr,len);

	if (nLen < 0) {
		if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK){
			//尝试再读一次。
			return -1;
		}
	}else if (nLen == 0){
		return 0;
	}

	return nLen;
}

void closeConnObj(ConnObj *conntobj)
{
     if (NULL != conntobj){

 		log_info("connect closed:%d",conntobj->fd);
 		close(conntobj->fd);/*连接关闭*/
 		conntobj->activity = SOCKET_CONNCLOSED;
 		conntobj->fd = -1;
     }
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

