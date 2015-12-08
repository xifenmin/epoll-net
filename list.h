/**********************************************
* list.h
*
*  Created on: 2015-04-17
*      Author: xifengming
*       Email: xifengming@vip.sina.com
*    Comments: data struct
 *********************************************/
#ifndef _DATALIST_H
#define _DATALIST_H

typedef struct tagDataNode DataNode;
typedef struct tagDataList DataList;

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*CmpFun)(void *src,void *dst);

DataList *DataList_Create(void);
DataNode *DataNode_Create(void *data);

void DataList_Clear(DataList *list);
void DataList_Delete(DataList *list);
void DataList_Addtail(DataList *list, void *data);
void DataList_Addhead(DataList *list,void *data);

void *DataList_Removehead(DataList *list);
void *DataList_Removetail(DataList *list);
void *DataList_Removenode(DataList *list ,DataNode *node);

DataNode *DataList_GetHead(DataList *list);
DataNode *DataList_GetNext(DataNode *node);
void *DataList_GetCurrentData(DataNode *node);

DataNode *DataList_Find(DataList *list,CmpFun cmpfun,void *data);
unsigned int DataList_Getcount(DataList *list);

#ifdef __cplusplus
}
#endif

#endif
