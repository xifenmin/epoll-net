/**********************************************
* lock.h
*
*  Created on: 2015-04-17
*      Author: xifengming
*       Email: xifenmin@vip.sina.com
*    Comments: lock
 *********************************************/

#ifndef _LOCK_H
#define _LOCK_H

#ifdef __cplusplus 
extern "C" {
#endif

typedef struct tagLock Locker;

struct tagLockerInterface
{
   Locker *locker;
   int (*lock)(struct tagLock *locker);
   int (*unlock)(struct tagLock *locker);
   int (*swait)(struct tagLock *locker);
   int (*post)(struct tagLock *locker);
   int (*cwait)(struct tagLock *locker);
   int (*signal)(struct tagLock *locker);
   int (*signalall)(struct tagLock *locker);
   int (*clear)(struct tagLock *locker);
};

typedef struct tagLockerInterface LockerInterface;

LockerInterface *LockerInterface_Create(void);
void LockerInterface_Destory(LockerInterface *locker_interface);

Locker *locker_init(void);
int locker_lock(Locker *locker);
int locker_unlock(Locker *locker);
int locker_semwait(Locker *locker);
int locker_post(Locker *locker);
int locker_condwait(Locker *locker);
int locker_signal(Locker *locker);
int locker_signalall(Locker *locker);
int locker_free(Locker *locker);
int locker_clear(Locker *locker);

#ifdef __cplusplus
}
#endif

#endif
