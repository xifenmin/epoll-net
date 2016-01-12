#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "log.h"
#include "threadpool.h"
#include "lock.h"
#include "queue.h"

struct tagThreadpool {
	Locker *locker;
	pthread_t *threadmgr;
	DataQueue *queue;
	unsigned int thread_count;
};

typedef struct tagThreadtask {
	callback  cb;
	void *arg;
	char name[125];
} Threadpool_Task;
//---------------------------------------------------------- 
static void *Threadpool_Run(void *threadpool_obj) {

	Threadpool_Task *threadpool_task = NULL;
	Threadpool *thread_pool = (Threadpool *) threadpool_obj;

	if (NULL == thread_pool) {
		log_error("threadpool_run:thread pool obj is null!!!");
		return NULL;
	}

	for (;;) {

		Locker_Lock(thread_pool->locker);

		while (DataQueue_Size(thread_pool->queue) <= 0) {
			Locker_Condwait(thread_pool->locker);
		}

		threadpool_task = DataQueue_Pop(thread_pool->queue);

		if (NULL == threadpool_task) {
			log_error("threadpool_run:thread pool task obj is null!!!");
			break;
		}
		log_info("thread func:%s,pid:%ld", threadpool_task->name,syscall(SYS_gettid));

		(*(threadpool_task->cb))(threadpool_task->arg);

		Locker_Signalall(thread_pool->locker);
		Locker_Unlock(thread_pool->locker);
	}

	Locker_Unlock(thread_pool->locker);
    pthread_exit(NULL);

	return NULL;
}
//-----------------------------------------------------------
int Threadpool_Free(Threadpool *thread_pool) {

	if (NULL == thread_pool) {
		return -1;
	}

	if (thread_pool->threadmgr) {
		free(thread_pool->threadmgr);
		DataQueue_Clear(thread_pool->queue);
		Locker_Clear(thread_pool->locker);
	}

	free(thread_pool);
	return 0;
}
//-----------------------------------------------------------
int Threadpool_Addtask(Threadpool *thread_pool, callback cb,
		char *name, void *arg) {
	Threadpool_Task *threadpool_task = NULL;

	if (NULL == thread_pool || NULL == cb) {
		log_error("threadpool_addtask:thread pool object or callback obj is null !!!");
		return -1;
	}

	threadpool_task = (Threadpool_Task *) malloc(sizeof(Threadpool_Task));

	if (NULL == threadpool_task) {
		log_error("threadpool_addtask:thread pool task obj malloc faill!!!");
		return -1;
	}

	memset(threadpool_task->name, 0, sizeof(threadpool_task->name));

	threadpool_task->cb  = cb;
	threadpool_task->arg = arg;
	memcpy(threadpool_task->name, name, strlen(name));

	Locker_Lock(thread_pool->locker);
	DataQueue_Push(thread_pool->queue, (void *) threadpool_task);
	Locker_Signal(thread_pool->locker);
	Locker_Unlock(thread_pool->locker);

	return 0;
}
//-----------------------------------------------------------
int Threadpool_Destroy(Threadpool *thread_pool) {
	int i = 0;

	if (NULL == thread_pool) {
		log_error("threadpool_destroy:thread pool obj is null!!!");
		return -1;
	}

	Locker_Lock(thread_pool->locker);

	if (!Locker_Signal(thread_pool->locker)) {
		log_error("threadpool_destroy:Locker single all thread fail!!");
		Locker_Unlock(thread_pool->locker);
		return -1;
	}

	for (; i < thread_pool->thread_count; i++) {
		if (pthread_join(thread_pool->threadmgr[i], NULL) != 0) {
			log_info("threadpool_destroy:pthread_join failed,pthrea id:%ld",thread_pool->threadmgr[i]);
		}
	}

	Locker_Unlock(thread_pool->locker);
	Threadpool_Free(thread_pool);

	return 0;
}
//-----------------------------------------------------------
Threadpool *Threadpool_Create(unsigned int thread_count) {

	int i = 0;
	Threadpool *thread_pool = NULL;

	thread_pool = (Threadpool *) malloc(sizeof(Threadpool));
	if (NULL == thread_pool) {
		log_info("threadpool_create:thread pool malloc fail!!!");
		return NULL;
	}

	thread_pool->threadmgr = (pthread_t *) malloc(thread_count * sizeof(pthread_t));

	if (NULL == thread_pool->threadmgr) {
		log_error("threadpool_create:pthread pool object malloc fail!!!");
		return NULL;
	}

	thread_pool->locker = InitLocker();
	thread_pool->queue  = DataQueue_Create();
	thread_pool->thread_count = thread_count;

	for (; i < thread_count; ++i) {

		if (pthread_create(&thread_pool->threadmgr[i], NULL, Threadpool_Run,
				(void*) thread_pool) != 0) {
			Threadpool_Destroy(thread_pool);
			return NULL;
		}

		thread_pool->thread_count++;
		pthread_detach(thread_pool->threadmgr[i]);
	}

	return thread_pool;
}
//-----------------------------------------------------------
