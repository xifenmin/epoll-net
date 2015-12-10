#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "connmgr.h"
#include "server.h"
#include "threadpool.h"

struct tagServerObj
{
	EpollObj   *epollobj;
	ConnMgr    *connmgr;/*server 端连接池，管理客户端连接*/
	ConnObj    *connobj;/*server 端连接描述符*/
	LockerObj  *lockerobj;
	Threadpool *thread_pool;//server 线程池
	DataQueue  *data_queue;/*接收数据队列*/
	Proc_Read   procread;/*客户端回调*/
};

int StartServer(ServerObj *serverobj,char ip,Proc_Read procread,unsigned char port)
{
	int ret = 0;
	int server_socket = 0;

	if (serverobj == NULL){

		serverobj = Server_Create(1024);

		if (NULL != serverobj){

			server_socket = socket(AF_INET,SOCK_STREAM,0);

			serverobj->connobj->fd   = server_socket;
			serverobj->connobj->ip   = ip;
			serverobj->connobj->port = port;
			serverobj->procread      = procread;/*注册调用者接收回调函数*/

	        ret = Server_Listen(serverobj);

	        Threadpool_Addtask(serverobj->thread_pool,&Server_Loop,"Server_Loop",strlen("Server_Loop"),serverobj);
	        Threadpool_Addtask(serverobj->thread_pool,&Server_Loop,"Server_Process",strlen("Server_Process"),serverobj);
		}
	}

	return ret;
}

ServerObj *Server_Create(int events)
{
	ServerObj *serverobj = NULL;

	serverobj = (ServerObj *)malloc(sizeof(ServerObj));

	if (NULL != serverobj){

		serverobj->epollobj    = Epoll_Create_Obj(events,cb);
		serverobj->connmgr     = ConnMgr_Create();
		serverobj->lockerobj   = LockerObj_Create();
		serverobj->connobj     = CreateNewConnObj();
		serverobj->data_queue  = DataQueue_Create();
		serverobj->thread_pool = Threadpool_Create(1);/*Proactor 模式，单线程*/
	}

	return serverobj;
}

void Server_Clear(ServerObj *serverobj)
{
    if (serverobj != NULL){
    	Threadpool_Destroy(serverobj->thread_pool);
    	ConnMgr_Clear(serverobj->connmgr);
    	Locker_Lock(serverobj->lockerobj);
    	if (serverobj->connobj != NULL){
    		free(serverobj->connobj);
    		serverobj->connobj = NULL;
    	}
    	Locker_Unlock(serverobj->lockerobj);
    	Epoll_Clear(serverobj->epollbase);
    }
}

int Server_Listen(ServerObj *serverobj)
 {
	struct sockaddr_in addr;
	int sock = -1;

	if (NULL == serverobj) {
		return -1;
	}

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port   = htons((short)serverobj->connobj->port);

	inet_pton(AF_INET,serverobj->connobj->ip, &addr.sin_addr);

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		goto sock_err;
	}
	if (setsockopt(serverobj->connobj->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
		goto sock_err;
	}
	if (bind(serverobj->connobj->fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
		goto sock_err;
	}
	if (listen(serverobj->connobj->fd, 1024) == -1) {
		goto sock_err;
	}

	return 0;

sock_err:

	if(serverobj->connobj->fd >= 0){
		close(serverobj->connobj->fd);
	}

	return -1;
}

int  Server_Accept(ServerObj *serverobj)
{
	int client_sock;
    struct sockaddr_in addr;

	socklen_t addrlen = sizeof(addr);
	ConnObj *_connobj = NULL;
	struct linger opt = {1,0};

	while((client_sock = accept(serverobj->connobj->fd, (struct sockaddr *)&addr, &addrlen)) == -1){
		if(errno != EINTR){
			return -1;
		}
	}

	setsockopt(client_sock, SOL_SOCKET, SO_LINGER, (void *)&opt, sizeof(opt));

	_connobj = serverobj->connmgr->get(serverobj->connmgr);

	if (_connobj != NULL){

		_connobj->ip        = inet_ntoa(addr.sin_addr);
		_connobj->port      = ntohs(addr.sin_port);
		_connobj->last_time = time(NULL);
		_connobj->fd        = client_sock;
		_connobj->activity  = SOCKET_CONNECTING;

		_connobj->keepalive();
		_connobj->nodelay();
		_connobj->noblock();

		if (!serverobj->epollobj->add(serverobj->epollobj->epollbase,_connobj)){/*设置客户端socket epoll事件*/
           /*add fail*/
			serverobj->connmgr->set(serverobj->connmgr,_connobj);/*把连接对象，放回到连接池中*/
			goto sock_err;
		}
	}else{
		goto sock_err;
	}

	return 0;

sock_err:

	if (client_sock != 0){
		close(client_sock);
		return -1;
	}
}

void Server_Process(void *argv)
{
	ConnObj *_connobj    = NULL;
	ServerObj *serverobj = (ServerObj *)argv;

	for(;;){

		if (NULL != serverobj){
		   _connobj = DataQueue_Pop(serverobj->data_queue);

		   if (NULL != _connobj){
                //回调用户接口函数
			   serverobj->procread(_connobj);
		   }
		}
	}
}

void Server_Loop(void *argv)
{
	int datalen = 0;

	ConnObj *_connobj    = NULL;
	ServerObj *serverobj = (ServerObj *)argv;

	if (NULL != serverobj){

		for(;;){
			datalen = serverobj->epollobj->wait(serverobj->epollobj->epollbase,0);
			if (datalen >0){
				_connobj = (ConnObj *)serverobj->epollobj->epollbase->event->data.ptr;
				if (NULL != _connobj){
					DataQueue_Push(serverobj->data_queue,_connobj);
				}
			}
		}
	}
}
