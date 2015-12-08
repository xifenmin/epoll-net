/**********************************************
* threadpool.h
*
*  Created on: 2015-04-20
*      Author: xifengming
*       Email: xifengming@vip.sina.com
*    Comments: thread pool 
 *********************************************/
#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*callback)(void *);
typedef struct tagThreadpool Threadpool;

Threadpool *Threadpool_Create(unsigned int thread_count);
int Threadpool_Destroy(Threadpool *thread_pool);
int Threadpool_Addtask(Threadpool *thread_pool, void (*callback)(void *),char *name,int namelen,void *arg);

#ifdef __cplusplus
}
#endif

#endif

