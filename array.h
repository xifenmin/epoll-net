/**********************************************
 *  connmgr.h
 *
 *  Created on: 2016-01-28
 *      Author: xifengming
 *       Email: xifengming@vip.sina.com
 *    Comments: dynamic array
 *********************************************/
#ifndef _DYNAMIC_ARRAY_H
#define _DYNAMIC_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagArray DynamicArray;

struct tagArray{
    unsigned int nitem;/*动态数组中，已经使用的元素个数*/
    void   *item;/*动态数组中，元素首地址*/
    unsigned int size;/*每个元素大小*/
    unsigned int nmalloc;/*当前动态数组中，能容纳多少元素*/
};

struct tagDynamicArrayInterface
{
	DynamicArray *array;
    void *(*push)(struct tagArray *);
    void *(*pop)(struct tagArray *);
    void *(*value)(struct tagArray *,unsigned int index);
    void *(*head)(struct tagArray *);
    void *(*end)(struct tagArray *);
    unsigned int (*index)(struct tagArray *,void *array);
    void (*clear)(struct tagArray *);
};

typedef struct tagDynamicArrayInterface DynamicArrayInterface;

DynamicArrayInterface *DynamicArrayInterface_Create(unsigned int ncount,unsigned int itemsize);
void DynamicArrayInterface_Destory(DynamicArrayInterface *dynamicarray_interface);

/*初始化动态数组*/
DynamicArray *dynamicArray_init(unsigned int ncount,unsigned int itemsize);
void dynamicArray_clear(DynamicArray *dynamicarray);
/*动态数组中push*/
void *dynamicArray_push(DynamicArray *dynamicarray);
/*动态数组pop*/
void *dynamicArray_pop(DynamicArray *dynamicarray);
/*下标索引操作*/
void *dynamicArray_value(DynamicArray *dynamicarray,unsigned int index);
/*动态数组头*/
void *dynamicArray_head(DynamicArray *dynamicarray);
/*动态数组尾*/
void *dynamicArray_end(DynamicArray *dynamicarray);
/*得到元素的索引位置*/
unsigned int dynamicArray_index(DynamicArray *dynamicarray,void *array);

#ifdef __cplusplus
}
#endif

#endif /* _DYNAMIC_ARRAY_H */
