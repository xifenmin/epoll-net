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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagDataQueue DataQueue;

struct tagDataQueueInterface
{
	DataQueue *queue;
	void (*push)(struct tagDataQueue *queue,void *data);
	void *(*pop)(struct tagDataQueue *queue);
	unsigned int (*size)(struct tagDataQueue *queue);
	void (*clear)(struct tagDataQueue *queue);
};

typedef struct tagDataQueueInterface DataQueueInterface;

DataQueueInterface *DataQueueInterface_Create(void);

void DataQueueInterface_Destory(DataQueueInterface *dataqueue_interface);

DataQueue *dataQueue_init(void);
void dataQueue_clear(DataQueue *queue);
void dataQueue_push(DataQueue *queue, void *data);
void *dataQueue_pop(DataQueue *queue);
unsigned int dataQueue_size(DataQueue *queue);

#ifdef __cplusplus
}
#endif

#endif
