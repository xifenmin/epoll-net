/*
 * test_server.c
 *
 *  Created on: 2015年12月11日
 *      Author: admin
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "connobj.h"
#include "server.h"

int readdata(ConnObj *connobj)
{
	if (NULL != connobj){
		printf("fd:%d,read len:%d\n",connobj->fd,connobj->recvlen);
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

	printf("Start Server:%d\n",ret);
	for(;;){
		sleep(1);
	}

    return 0;
}


