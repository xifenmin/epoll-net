#include <stdlib.h>
#include <stdio.h>
#include "log.h"
#include "list.h"

struct tagDataNode {
	void *data;
	struct tagDataNode *next;
	struct tagDataNode *prev;
};

struct tagDataList {
	struct tagDataNode *head;
	struct tagDataNode *tail;
	unsigned int count;
};
//--------------------------------------------------------------------------------------------------------
DataList *DataList_Create(void) {

	DataList *list;

	list = (DataList *) malloc(sizeof(DataList));
	if (list) {
		list->head = NULL;
		list->tail = NULL;
		list->count = 0;
	}

	return list;
}
//--------------------------------------------------------------------------------------------------------
DataNode *DataNode_Create(void *data) {

	DataNode *node;

	node = (DataNode *) malloc(sizeof(DataNode));

	if (node) {
		node->next = NULL;
		node->prev = NULL;
		node->data = data;
	}

	return node;
}
//--------------------------------------------------------------------------------------------------------
DataNode *DataList_Find(DataList *list, CmpFun cmpfun, void *data) {
	int result = -1;

	if (list) {
		DataNode *current = list->head;
		for (; current != NULL; current = current->next) {
			result = cmpfun(data, current->data);
			if (0 == result) {
				return current;
			}
		}
	}

	return NULL;
}
//--------------------------------------------------------------------------------------------------------
void DataList_Clear(DataList *list) {
	while (list->head != NULL) {
		DataList_Removetail(list);
	}
}
//--------------------------------------------------------------------------------------------------------
void DataList_Delete(DataList *list) {
	if (list) {
		DataList_Clear(list);
		free(list);
	}
}
//--------------------------------------------------------------------------------------------------------
void DataList_Addtail(DataList *list, void *data) {
	DataNode *node;
	node = DataNode_Create(data);

	if (list->tail != NULL) {
		list->tail->next = node;
		node->prev = list->tail;
		list->tail = node;
	} else {
		list->head = node;
		list->tail = node;
	}
	//printf("add node:%p ,node data:%p\n",node,data);
	list->count++;
}
//----------------------------e----------------------------------------------------------------------------
void DataList_Addhead(DataList *list, void *data) {
	DataNode *node;
	node = DataNode_Create(data);
	if (list->head != NULL) {
		list->head = node;
		node->next = list->head;
		list->head->prev = node;

	} else {
		list->head = node;
		list->tail = node;
	}

	list->count++;
}
//--------------------------------------------------------------------------------------------------------
void *DataList_Removehead(DataList *list) {
	void *data = NULL;

	if (list->head != NULL) {
		DataNode *temp = list->head;
		list->head = list->head->next;
		if (list->head == NULL) {
			list->tail = NULL;
		} else {
			list->head->prev = NULL;
			if (list->head->next != NULL) {
				list->head->next->prev = list->head;
			} else {
				list->tail = list->head;
			}
		}
		data = temp->data;
		free(temp);
		list->count--;
	}
	return data;
}
//--------------------------------------------------------------------------------------------------------
void *DataList_Removetail(DataList *list) {
	void *data = NULL;
	if (list->tail != NULL) {

		DataNode *temp = list->tail;
		list->tail = list->tail->prev;

		if (list->tail == NULL) {
			list->head = NULL;
		} else {
			list->tail->next = NULL;
			if (list->tail->prev != NULL) {
				list->tail->prev->next = list->tail;
			} else {
				list->head = list->tail;
			}
		}

		data = temp->data;
		free(temp);
		//printf("remove node:%p ,node data:%p\n",temp,data);
		list->count--;
	}
	return data;
}
//--------------------------------------------------------------------------------------------------------
void *DataList_Removenode(DataList *list, DataNode *node) {
	void *data;

	if (node == list->head) {
		data = DataList_Removehead(list);
	} else if (node == list->tail) {
		data = DataList_Removetail(list);
	} else {
		node->prev->next = node->next;
		node->next->prev = node->prev;
		data = node->data;
		free(node);
		list->count--;
	}

	return data;
}
//--------------------------------------------------------------------------------------------------------
unsigned int DataList_Getcount(DataList *list) {
	return list->count;
}
//--------------------------------------------------------------------------------------------------------
DataNode *DataList_GetHead(DataList *list) {
	if (list) {
		return list->head;
	}

	return NULL;
}
//--------------------------------------------------------------------------------------------------------
DataNode *DataList_GetNext(DataNode *node) {
	if (node) {
		return node->next;
	}

	return NULL;
}
//--------------------------------------------------------------------------------------------------------
void *DataList_GetCurrentData(DataNode *node) {
	if (node) {
		return node->data;
	}
	return NULL;
}
