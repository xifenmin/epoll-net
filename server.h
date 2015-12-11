/**********************************************
 *  server.h
 *
 *  Created on: 2015-12-02
 *      Author: xifengming
 *       Email: xifengming@vip.sina.com
 *    Comments: socket server
 *********************************************/

#ifndef _SERVER_H
#define _SERVER_H

#include "connmgr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagServerObj   ServerObj;
typedef int  (*ProcRead)(ConnObj *connobj);

ServerObj *Server_Create(int events);
void Server_Clear(ServerObj *serverobj);
int  StartServer(ServerObj *serverobj,char *ip,unsigned char port,ProcRead procread);
int  Server_Listen(ServerObj *serverobj);
int  Server_Accept(ServerObj *serverobj);
void Server_Process(void *argv);
void Server_Loop(void *argv);

#ifdef __cplusplus
}
#endif

#endif
