#include <stdlib.h>
#include "queue.h"

struct tagDataQueue{
     DataList *list;
};
//--------------------------------------------------------------------------------------------------------
DataQueueInterface *DataQueueInterface_Create(void)
{
	DataQueueInterface *dataqueue_interface = NULL;

	dataqueue_interface = (DataQueueInterface *)malloc(sizeof(DataQueueInterface));

	if (NULL != dataqueue_interface){
		dataqueue_interface->queue = dataQueue_init();
		dataqueue_interface->push  = dataQueue_push;
		dataqueue_interface->pop   = dataQueue_pop;
		dataqueue_interface->size  = dataQueue_size;
		dataqueue_interface->clear = dataQueue_clear;
	}

	return dataqueue_interface;
}

void DataQueueInterface_Destory(DataQueueInterface *dataqueue_interface)
{
    if (dataqueue_interface != NULL){
    	dataqueue_interface->clear(dataqueue_interface->queue);
        free(dataqueue_interface);
        dataqueue_interface = NULL;
    }
}

DataQueue *dataQueue_init(void)
{
   DataQueue *queue = (DataQueue *)malloc(sizeof(DataQueue));

   if (queue){
       queue->list = DataList_Create();
   }

   return queue;
}
//--------------------------------------------------------------------------------------------------------
void dataQueue_clear(DataQueue *queue)
{
     if (queue){
         DataList_Delete(queue->list);
         free(queue);
         queue = NULL;
     }
}
//--------------------------------------------------------------------------------------------------------
void dataQueue_push(DataQueue *queue,void *data)
{
     DataList_Addtail(queue->list,data);
}
//--------------------------------------------------------------------------------------------------------
void *dataQueue_pop(DataQueue *queue)
{
    return DataList_Removehead(queue->list);
}
//--------------------------------------------------------------------------------------------------------
unsigned int dataQueue_size(DataQueue *queue)
{
    return DataList_Getcount(queue->list);
}
//--------------------------------------------------------------------------------------------------------
