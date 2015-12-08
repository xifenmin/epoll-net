/**********************************************
 * queue.h
 *
 *  Created on: 2015-04-17
 *      Author: xifengming
 *       Email: xifengming@vip.sina.com
 *    Comments: data struct
 *********************************************/

#ifndef _DATAQUEUE_H
#define _DATAQUEUE_H

#include "list.h"

typedef struct tagDataQueue DataQueue;

#ifdef __cplusplus
extern "C" {
#endif

DataQueue *DataQueue_Create(void);
void DataQueue_Clear(DataQueue *queue);
void DataQueue_Push(DataQueue *queue, void *data);
void *DataQueue_Pop(DataQueue *queue);
unsigned int DataQueue_Size(DataQueue *queue);

#ifdef __cplusplus
}
#endif

#endif
