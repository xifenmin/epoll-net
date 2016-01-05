/*
 * test_server.c
 *
 *  Created on: 2015年12月11日
 *      Author: admin
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "connobj.h"
#include "server.h"
#include "log.h"
#include "cstr.h"

#define  FILE_NAME "/home/xfm/xfm/net/test_server.log"

typedef struct tagtest
{
	unsigned char byBuffer[1024];
	int lon;
	int lat;
}TEST;

void DebugInfo(unsigned char *puc, int nLen)
{
	int i;
	for (i = 0; i < nLen; i++)
         {
	  printf("%.2x ", puc[i]);
	}
	printf("\n");
}

// 解码
int Decode(const char *data, const int len,char *_data)
{
	int _len   = 0;
	int i      = 1;
	int offset = 1 ;

	if( data[0] != 0x7e || data[len-1] != 0x7e )
	{
		return -1;
	}

	_len  = len ;

	_data[0] = data[0] ;


	for(; i < len - 1; i++)
	{
		if (data[i] == 0x7d && data[i+1] == 0x02)
		{
			_data[offset++] = 0x7e;
			i++;
		}
		else if (data[i] == 0x7d && data[i+1] == 0x01)
		{
			_data[offset++] = 0x7d;
			i++;
		}
		else
		{
			_data[offset++] = data[i];
		}
	}
	_data[offset] = data[len-1] ;

	_len = offset + 1 ;

	return _len;
}

int readdata(ConnObj *connobj)
{
	if (NULL != connobj && connobj->activity == SOCKET_CONNECTED){
		//log_info("fd:%d,read len:%d,addr:%p,info:%s",connobj->fd,connobj->recvlen,connobj,connobj->recvptr);
		loghex(connobj->recvlen,(char *)connobj->recvptr);
		//DebugInfo(connobj->recvptr,connobj->recvlen);
	}
	return 0;
}

int main(int argc,char **argv)
{
	int ret  = 0;
	int port = 0;

	ServerObj *serverobj = NULL;

	if (argc < 2)
       return -1;


	port = atoi(argv[2]);
	ret  = StartServer(serverobj,argv[1],port,readdata);

	Logger_Create(LEVEL_INFO,10,FILE_NAME);

	printf("Start Server:%d\n",ret);

	for(;;){
		sleep(1);
	}

    return 0;
}



