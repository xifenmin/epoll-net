#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "log.h"
#include "lock.h"

struct tagLock
{
   sem_t           m_sem; 
   pthread_mutex_t m_mutex;
   pthread_cond_t  m_cond;
};
//-----------------------------------------------------------
LockerInterface *LockerInterface_Create(void)
{
	LockerInterface *locker_interface = NULL;

	locker_interface  = (LockerInterface *)malloc(sizeof(LockerInterface));

    if (NULL == locker_interface){
    	log_error("LockerInterface_Create:LockerInterface malloc fail!!!");
        return NULL;
    }

    locker_interface->locker    = locker_init();
    locker_interface->lock      = locker_lock;
    locker_interface->unlock    = locker_unlock;
    locker_interface->swait     = locker_semwait;
    locker_interface->post      = locker_post;
    locker_interface->cwait     = locker_condwait;
    locker_interface->signal    = locker_signal;
    locker_interface->signalall = locker_signalall;
    locker_interface->clear     = locker_clear;

    return locker_interface;
}
//-----------------------------------------------------------
void LockerInterface_Destory(LockerInterface *locker_interface)
{
    if (locker_interface != NULL){
    	locker_interface->clear(locker_interface->locker);
        free(locker_interface);
        locker_interface = NULL;
    }
}
//-----------------------------------------------------------
Locker *locker_init(void)
{
    Locker *locker = NULL;
    
    locker = (Locker *)malloc(sizeof(Locker));

    if (locker == NULL){
        printf("locker malloc fail!!!\n");
        return NULL;
    }

    sem_init(&locker->m_sem,0,0);
    pthread_mutex_init(&locker->m_mutex,NULL);
    pthread_cond_init(&locker->m_cond,NULL);

    return locker;
}
//-----------------------------------------------------------
int  locker_lock(Locker *locker)
{
    if (NULL == locker){
       return -1;
    }

    return (pthread_mutex_lock(&locker->m_mutex ) == 0);
}
//-----------------------------------------------------------
int locker_unlock(Locker *locker)
{
    if (NULL == locker){
       return -1;
    }

    return pthread_mutex_unlock(&locker->m_mutex ) == 0;
}
//-----------------------------------------------------------
int  locker_semwait(Locker *locker)
{
    if ( NULL == locker){
       return -1;
    }

    return sem_wait(&locker->m_sem ) == 0;
}
//-----------------------------------------------------------
int locker_post(Locker *locker)
{
     if (NULL == locker){
         return -1;
     }

     return sem_post(&locker->m_sem ) == 0;
}
//-----------------------------------------------------------
int locker_condwait(Locker *locker)
{
    if (NULL == locker){
        return -1;
    }

    int ret = 0;
    ret = pthread_cond_wait(&locker->m_cond,&locker->m_mutex );

    return ret;
}
//-----------------------------------------------------------
int locker_signal(Locker *locker)
{
   if (NULL == locker){
        return -1;
   }

   return (pthread_cond_signal(&locker->m_cond) == 0);
}
//-----------------------------------------------------------
int locker_signalall(Locker *locker)
{
   if (NULL == locker){
        return -1;
   }

   return (pthread_cond_broadcast(&locker->m_cond) == 0);
}
//-----------------------------------------------------------
int locker_free(Locker *locker)
{
    if (NULL == locker){
        return -1;
    }

    pthread_mutex_destroy(&locker->m_mutex);
    pthread_cond_destroy(&locker->m_cond);
    sem_destroy(&locker->m_sem);

    return 0;
}
//-----------------------------------------------------------
int  locker_clear(Locker *locker)
{
    if (NULL == locker){
        return -1;
    }

    locker_free(locker);
    free(locker);

    return 0;
}
//----------------------------------------------------------
