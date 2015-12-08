#include <stdlib.h>
#include "queue.h"

struct tagDataQueue{
     DataList *list;
};

//--------------------------------------------------------------------------------------------------------
DataQueue *DataQueue_Create(void)
{
   DataQueue *queue = (DataQueue *)malloc(sizeof(DataQueue));

   if (queue){
       queue->list = DataList_Create();
   }

   return queue;
}
//--------------------------------------------------------------------------------------------------------
void DataQueue_Clear(DataQueue *queue)
{
     if (queue){
         DataList_Delete(queue->list);
         free(queue);
     }
}
//--------------------------------------------------------------------------------------------------------
void DataQueue_Push(DataQueue *queue,void *data)
{
     DataList_Addtail(queue->list,data);
}
//--------------------------------------------------------------------------------------------------------
void *DataQueue_Pop(DataQueue *queue)
{
    return DataList_Removetail(queue->list);
}
//--------------------------------------------------------------------------------------------------------
unsigned int DataQueue_Size(DataQueue *queue)
{
    return DataList_Getcount(queue->list);
}
//--------------------------------------------------------------------------------------------------------
