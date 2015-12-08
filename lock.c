#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "lock.h"

struct tagLock
{
   sem_t           m_sem; 
   pthread_mutex_t m_mutex;
   pthread_cond_t  m_cond;
};
//-----------------------------------------------------------
LockerObj *LockerObj_Create(void)
{
    LockerObj *locker_obj = NULL;

    locker_obj  = (LockerObj *)malloc(sizeof(LockerObj));

    if (NULL == locker_obj){
        printf("LockerObj_Create:LockerObj malloc fail!!!\n");
        return NULL;
    }

    locker_obj->locker    = InitLocker();

    locker_obj->Lock      = Locker_Lock;
    locker_obj->Unlock    = Locker_Unlock;
    locker_obj->Swait     = Locker_Semwait;
    locker_obj->Post      = Locker_Post;
    locker_obj->Cwait     = Locker_Condwait;
    locker_obj->Signal    = Locker_Signal;
    locker_obj->Signalall = Locker_Signalall;
    locker_obj->Clear     = Locker_Clear;

    return locker_obj;
}
//-----------------------------------------------------------
void LockerObj_Clear(LockerObj *locker_obj)
{
    if (locker_obj != NULL){
         locker_obj->Clear(locker_obj->locker);
         free(locker_obj);
         locker_obj = NULL;
    }
}
//-----------------------------------------------------------
Locker *InitLocker(void)
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
int  Locker_Lock(Locker *locker)
{
    if (NULL == locker){
       return -1;
    }

    return (pthread_mutex_lock(&locker->m_mutex ) == 0);
}
//-----------------------------------------------------------
int Locker_Unlock(Locker *locker)
{
    if (NULL == locker){
       return -1;
    }

    return pthread_mutex_unlock(&locker->m_mutex ) == 0;
}
//-----------------------------------------------------------
int  Locker_Semwait(Locker *locker)
{
    if ( NULL == locker){
       return -1;
    }

    return sem_wait(&locker->m_sem ) == 0;
}
//-----------------------------------------------------------
int Locker_Post(Locker *locker)
{
     if (NULL == locker){
         return -1;
     }

     return sem_post(&locker->m_sem ) == 0;
}
//-----------------------------------------------------------
int Locker_Condwait(Locker *locker)
{
    if (NULL == locker){
        return -1;
    }

    int ret = 0;
    Locker_Lock(locker);
    ret = pthread_cond_wait(&locker->m_cond,&locker->m_mutex );
    Locker_Unlock(locker);

    return ret;
}
//-----------------------------------------------------------
int Locker_Signal(Locker *locker)
{
   if (NULL == locker){
        return -1;
   }

   return (pthread_cond_signal(&locker->m_cond) == 0);
}
//-----------------------------------------------------------
int Locker_Signalall(Locker *locker)
{
   if (NULL == locker){
        return -1;
   }

   return (pthread_cond_broadcast(&locker->m_cond) == 0);
}
//-----------------------------------------------------------
int Locker_Free(Locker *locker)
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
int  Locker_Clear(Locker *locker)
{
    if (NULL == locker){
        return -1;
    }

    Locker_Free(locker);
    free(locker);

    return 0;
}
//----------------------------------------------------------
