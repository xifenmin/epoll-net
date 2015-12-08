/**********************************************
* lock.h
*
*  Created on: 2015-04-17
*      Author: xifengming
*       Email: xifengming@vip.sina.com
*    Comments: lock
 *********************************************/

#ifndef _LOCK_H
#define _LOCK_H

#ifdef __cplusplus 
extern "C" {
#endif

typedef struct tagLock Locker;

struct tagLockerObj
{
   Locker *locker;
   int (*Lock)(struct tagLock *locker);
   int (*Unlock)(struct tagLock *locker);
   int (*Swait)(struct tagLock *locker);
   int (*Post)(struct tagLock *locker);
   int (*Cwait)(struct tagLock *locker);
   int (*Signal)(struct tagLock *locker);
   int (*Signalall)(struct tagLock *locker);
   int (*Clear)(struct tagLock *locker);
};

typedef struct tagLockerObj LockerObj;

LockerObj *LockerObj_Create(void);
void LockerObj_Clear(LockerObj *locker_obj);

Locker *InitLocker(void);
int Locker_Lock(Locker *locker);
int Locker_Unlock(Locker *locker);
int Locker_Semwait(Locker *locker);
int Locker_Post(Locker *locker);
int Locker_Condwait(Locker *locker);
int Locker_Signal(Locker *locker);
int Locker_Signalall(Locker *locker);
int Locker_Free(Locker *locker);
int Locker_Clear(Locker *locker);

#ifdef __cplusplus
}
#endif

#endif
