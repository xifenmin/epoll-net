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

	int offset = 0;

	while (len>0) {
		ret = write(conntobj->fd,&conntobj->sendptr[offset],len);

		if (ret <0){
			if (errno == EAGAIN || errno == EWOULDBLOCK){
				break;
			}
			else {
				return -1;
			}
		}

		if (ret < len){
			len    = len - ret;
			offset = offset + ret;
		}
	}

	return offset;
}

int readData(ConnObj *conntobj,unsigned char *ptr,int len)
{
	int nLen   = 0;
	int offset = 0;

	if (NULL == conntobj){
		return -1;
	}

	for(;;){

		offset = read(conntobj->fd,ptr,len);

		if (offset >0){
			nLen = nLen + offset;
			if (nLen == len){ //如果read返回是缓存区的大小一样，就再读取一次.
				continue;
			}
			break;
		}

		if (offset < 0) {
			   if (errno == EINTR ){
				   continue;
			   }
		       if (errno == EAGAIN || errno == EWOULDBLOCK){
				  return -2;
		       }
		}else if (offset == 0){
			nLen = offset;
			break;
		}
	}

	return nLen;
}

void closeConnObj(ConnObj *conntobj)
{
     if (NULL != conntobj && conntobj->fd >0){

 		log_info("connect closed:%d",conntobj->fd);
 		close(conntobj->fd);/*杩炴帴鍏抽棴*/
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
	setsockopt(connobj->fd, SOL_SOCKET,SO_KEEPALIVE,(void *)&opt, sizeof(opt));
}

void noBlock(ConnObj *connobj,int enable){

	if(enable){
		fcntl(connobj->fd, F_SETFL, O_NONBLOCK | O_RDWR);
	}else{
		fcntl(connobj->fd, F_SETFL, O_RDWR);
	}
}

