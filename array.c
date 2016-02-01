#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "log.h"

DynamicArray *DynamicArray_Create(unsigned int ncount,unsigned int itemsize)
{
	DynamicArray *dynamic_array    = NULL;
	dynamic_array = (DynamicArray *)malloc(sizeof(DynamicArray));

	if (NULL == dynamic_array){
	 	log_error("DynamicArray_Create:DynamicArray obj malloc fail!!!");
	 	return NULL;
	}

	dynamic_array->item = malloc(ncount * itemsize);

	if (NULL == dynamic_array->item){
		log_error("DynamicArray_Create:DynamicArray item malloc fail!!!");
		return NULL;
	}

	dynamic_array->nitem   = 0;/*初始化当前已经使用的item为0*/
	dynamic_array->size    = itemsize;/*每个元素大小*/
	dynamic_array->nmalloc = ncount; /*总共元素个数*/

	return dynamic_array;
}

void DynamicArray_Clear(DynamicArray *dynamicarray)
{
   if (NULL == dynamicarray){
	   return;
   }

   if (dynamicarray->item != NULL){
	   free(dynamicarray->item);
	   dynamicarray->item = NULL;
   }

   free(dynamicarray);
   dynamicarray = NULL;
}

void *dynamicArray_push(DynamicArray *dynamicarray)
{
  void *array       = NULL;
  void *new_array   = NULL;
  unsigned int size = 0;

  if (NULL == dynamicarray){
	  return NULL;
  }

  if (dynamicarray->nitem == dynamicarray->nmalloc){ /*代表动态数组已经满了，需要扩容*/

	   size      = dynamicarray->size * dynamicarray->nmalloc;/*当前元素总共的 大小*/
	   new_array = realloc(dynamicarray->item, 2 * size);/*在原来内存基础上，再扩容2倍*/

	   if (new_array == NULL) {
	       return NULL;
	   }

	   dynamicarray->item     = new_array;/*动态数组首地址，指向新的地址*/
	   dynamicarray->nmalloc *= 2;/*元素大小扩大到2倍*/
  }

  array  = (void *)dynamicarray->item + dynamicarray->size * dynamicarray->nitem;/*返回新位置的内存地址*/
  dynamicarray->nitem++;

  return array;
}

void *dynamicArray_pop(DynamicArray *dynamicarray)
{
	void *array = NULL;

	if (NULL == dynamicarray){
		return NULL;
	}

	if (dynamicarray->nitem > 0){
		dynamicarray->nitem--;
		array = dynamicarray->item + dynamicarray->size * dynamicarray->nitem;
	}

	return array;
}

void *dynamicArray_value(DynamicArray *dynamicarray,unsigned int index)
{
	void *array = NULL;

	if (NULL == dynamicarray){
		return NULL;
	}

	if (index < dynamicarray->nitem){
	    array = dynamicarray->item + (dynamicarray->size * index);
	}

	return array;
}

void *dynamicArray_head(DynamicArray *dynamicarray)
{
   void *array = NULL;

   if (NULL == dynamicarray){
	   return NULL;
   }

   array = dynamicArray_value(dynamicarray,0);

   return array;
}

void *dynamicArray_end(DynamicArray *dynamicarray)
{
	void *array = NULL;

	if (NULL == dynamicarray){
		return NULL;
	}

	array = dynamicArray_value(dynamicarray,dynamicarray->nmalloc);

	return array;
}

unsigned int dynamicArray_index(DynamicArray *dynamicarray,void *array)
{
	void *p, *q;
	unsigned int offset, index;

	if (array < dynamicarray->item)
		return -1;

	p = dynamicarray->item;
	q = array;

	offset = (unsigned int)(q - p);

	index = offset/(unsigned int)dynamicarray->size;

	return index;
}
