/**********************************************
 * hash.h
 *
 *  Created on: 2015-04-30
 *      Author: xifengming
 *       Email: xifengming@vip.sina.com
 *    Comments: data struct hash
 *********************************************/
#ifndef _HASH_H
#define _HASH_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagHashNode HashNode;

typedef int (*HashCmpFun)(void *src, void *dst);
typedef int (*HashFun)(void *data, size_t len);

typedef struct tagHashTable HashTable;

HashTable *HashTable_Create(unsigned int size, HashFun hashfun,
		HashCmpFun hashcmp);

void HashTable_Clear(HashTable *hashtable);
void *HashTable_Remove(HashTable *hashtable, void *key);
float HashTable_Loadfactor(HashTable *hashtable);

void HashTable_Add(HashTable *hashtable, void *key, void *data);
void *HashTable_Get(HashTable *hashtable, void *key);

#ifdef __cplusplus
}
#endif

#endif
