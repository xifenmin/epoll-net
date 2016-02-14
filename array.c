#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "log.h"

DynamicArrayInterface *DynamicArrayInterface_Create(unsigned int ncount,unsigned int itemsize)
{
	DynamicArrayInterface *dynamicArrayInterface = NULL;

	dynamicArrayInterface = (DynamicArrayInterface *)malloc(sizeof(DynamicArrayInterface));

	if (NULL != dynamicArrayInterface)
	{
		dynamicArrayInterface->array  = dynamicArray_init(ncount,itemsize);
		dynamicArrayInterface->push   = dynamicArray_push;
		dynamicArrayInterface->pop    = dynamicArray_pop;
		dynamicArrayInterface->value  = dynamicArray_value;
		dynamicArrayInterface->head   = dynamicArray_head;
		dynamicArrayInterface->end    = dynamicArray_end;
		dynamicArrayInterface->index  = dynamicArray_index;
		dynamicArrayInterface->clear  = dynamicArray_clear;
	}

	return dynamicArrayInterface;
}

void DynamicArrayInterface_Destory(DynamicArrayInterface *dynamicArrayInterface)
{
	if (dynamicArrayInterface != NULL) {
		dynamicArrayInterface->clear(dynamicArrayInterface->array);
		free(dynamicArrayInterface);
		dynamicArrayInterface = NULL;
	}
}

DynamicArray *dynamicArray_init(unsigned int ncount,unsigned int itemsize)
{
	DynamicArray *dynamicArray    = NULL;
	dynamicArray = (DynamicArray *)malloc(sizeof(DynamicArray));

	if (NULL == dynamicArray){
	 	log_error("DynamicArray_Create:DynamicArray obj malloc fail!!!");
	 	return NULL;
	}

	dynamicArray->item = malloc(ncount * itemsize);

	if (NULL == dynamicArray->item){
		log_error("DynamicArray_Create:DynamicArray item malloc fail!!!");
		return NULL;
	}

	dynamicArray->nitem   = 0;/*初始化当前已经使用的item为0*/
	dynamicArray->size    = itemsize;/*每个元素大小*/
	dynamicArray->nmalloc = ncount; /*总共元素个数*/

	return dynamicArray;
}

void dynamicArray_clear(DynamicArray *dynamicArray)
{
   if (NULL == dynamicArray){
	   return;
   }

   if (dynamicArray->item != NULL){
	   free(dynamicArray->item);
	   dynamicArray->item = NULL;
   }

   free(dynamicArray);
   dynamicArray = NULL;
}

void *dynamicArray_push(DynamicArray *dynamicArray)
{
  void *array       = NULL;
  void *new_array   = NULL;
  unsigned int size = 0;

  if (NULL == dynamicArray){
	  return NULL;
  }

  if (dynamicArray->nitem == dynamicArray->nmalloc){ /*代表动态数组已经满了，需要扩容*/

	   size      = dynamicArray->size * dynamicArray->nmalloc;/*当前元素总共的 大小*/
	   new_array = realloc(dynamicArray->item, 2 * size);/*在原来内存基础上，再扩容2倍*/

	   if (new_array == NULL) {
	       return NULL;
	   }

	   dynamicArray->item     = new_array;/*动态数组首地址，指向新的地址*/
	   dynamicArray->nmalloc *= 2;/*元素大小扩大到2倍*/
  }

  array  = (void *)dynamicArray->item + dynamicArray->size * dynamicArray->nitem;/*返回新位置的内存地址*/
  dynamicArray->nitem++;

  return array;
}

void *dynamicArray_pop(DynamicArray *dynamicArray)
{
	void *array = NULL;

	if (NULL == dynamicArray){
		return NULL;
	}

	if (dynamicArray->nitem > 0){
		dynamicArray->nitem--;
		array = dynamicArray->item + dynamicArray->size * dynamicArray->nitem;
	}

	return array;
}

void *dynamicArray_value(DynamicArray *dynamicArray,unsigned int index)
{
	void *array = NULL;

	if (NULL == dynamicArray){
		return NULL;
	}

	if (index < dynamicArray->nitem){
	    array = dynamicArray->item + (dynamicArray->size * index);
	}

	return array;
}

void *dynamicArray_head(DynamicArray *dynamicArray)
{
   void *array = NULL;

   if (NULL == dynamicArray){
	   return NULL;
   }

   array = dynamicArray_value(dynamicArray,0);

   return array;
}

void *dynamicArray_end(DynamicArray *dynamicArray)
{
	void *array = NULL;

	if (NULL == dynamicArray){
		return NULL;
	}

	array = dynamicArray_value(dynamicArray,dynamicArray->nmalloc);

	return array;
}

unsigned int dynamicArray_index(DynamicArray *dynamicArray,void *array)
{
	void *p, *q;
	unsigned int offset, index;

	if (array < dynamicArray->item)
		return -1;

	p = dynamicArray->item;
	q = array;

	offset = (unsigned int)(q - p);

	index = offset/(unsigned int)dynamicArray->size;

	return index;
}
