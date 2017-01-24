/**********************************************
 * hash.h
 *
 *  Created on: 2015-04-30
 *      Author: xifengming
 *       Email: xifenmin@vip.sina.com
 *
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

struct tagHashTableInterface
{
	HashTable *hash;
	void (*push)(struct tagHashTable *hashtable, void *key, void *data);
	void *(*pop)(struct tagHashTable *hashtable, void *key);
	void *(*remove)(struct tagHashTable *hashtable,void *key);
	float (*factor)(struct tagHashTable *);
	void (*clear)(struct tagHashTable *queue);
};

typedef struct tagHashTableInterface HashTableInterface;

HashTableInterface *HashTableInterface_Create(unsigned int size, HashFun hashfun,HashCmpFun hashcmp);

void HashTableInterface_Destory(HashTableInterface *hashtable_interface);

HashTable *hashTable_init(unsigned int size, HashFun hashfun,
		HashCmpFun hashcmp);

void hashTable_clear(HashTable *hashtable);
void *hashTable_remove(HashTable *hashtable, void *key);
float hashTable_loadfactor(HashTable *hashtable);

void hashTable_push(HashTable *hashtable, void *key, void *data);
void *hashTable_pop(HashTable *hashtable, void *key);

#ifdef __cplusplus
}
#endif

#endif
