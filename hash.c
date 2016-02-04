#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "hash.h"
#include "list.h"

struct tagHashNode{
      DataList *List;
};
//-----------------------------------------------------------
struct tagHashTable{
     unsigned int hashsize;
     HashFun      hashfun;
     HashCmpFun   hashcmp;
     HashNode     **buckets;
};
//-----------------------------------------------------------
HashTableInterface *HashTableInterface_Create(unsigned int size, HashFun hashfun,HashCmpFun hashcmp)
{
	HashTableInterface *hashtable_interface = NULL;

	hashtable_interface = (HashTableInterface *)malloc(sizeof(HashTableInterface));

	if (NULL != hashtable_interface){

		hashtable_interface->hash   = hashTable_init(size,hashfun,hashcmp);
		hashtable_interface->push   = hashTable_push;
		hashtable_interface->pop    = hashTable_pop;
		hashtable_interface->remove = hashTable_remove;
		hashtable_interface->clear  = hashTable_clear;
		hashtable_interface->factor = hashTable_loadfactor;
	}

	return hashtable_interface;
}

void HashTableInterface_Destory(HashTableInterface *hashtable_interface)
{
    if (hashtable_interface != NULL){
    	hashtable_interface->clear(hashtable_interface->hash);
         free(hashtable_interface);
         hashtable_interface = NULL;
    }
}

HashTable *hashTable_init(unsigned int size,HashFun hashfun,HashCmpFun hashcmp)
{
     int i=0;

     HashTable *hashtable_obj = (HashTable *)malloc(sizeof(HashTable));

     if (NULL == hashtable_obj){
    	 log_error("HashTable_Create:hashtable obj malloc fail!!!");
         return NULL;
     }

     hashtable_obj->hashsize   = size;
     hashtable_obj->hashfun    = hashfun;
     hashtable_obj->hashcmp    = hashcmp;
     
     hashtable_obj->buckets    = (HashNode **)malloc(size * sizeof(HashNode *));

     if (NULL != hashtable_obj->buckets){
          for(;i<size;i++){
              hashtable_obj->buckets[i] = NULL;
          }
     }else{
         free(hashtable_obj);
         return NULL;
     }

     return hashtable_obj;
}
//-----------------------------------------------------------
HashNode *hashNode_create(void *data)
{
    HashNode *hashnode = (HashNode *)malloc(sizeof(HashNode));

    if (NULL == hashnode){
       log_error("hashNode_create:hash node malloc fail!!!");
       return NULL;
    }

    hashnode->List =  DataList_Create();

    return hashnode;
}
//-----------------------------------------------------------
void hashTable_clear(HashTable *hashtable)
{
    int i=0;

    if (NULL == hashtable){
       log_error("HashTable_Clear:hash table obj is NULL!!!");
       return;
    }

    for(;i<hashtable->hashsize;i++){
        if (NULL != hashtable->buckets[i]){
           DataList_Delete(hashtable->buckets[i]->List); 
           free(hashtable->buckets[i]);
       }
    }

    free(hashtable->buckets);
    free(hashtable);
}
//-----------------------------------------------------------
float hashTable_loadfactor(HashTable *hashtable)
{
   unsigned int touched=0;
                 int i =0;
           float factor=0.00;

   if (NULL == hashtable){
	  log_error("HashTable_Loadfactor:hash table obj is NULL!!!");
      return 0;
   }
   
   for (;i<hashtable->hashsize;i++){
       if (NULL != hashtable->buckets[i]){
             touched++;
       }
   }     

   factor = (float)touched / (float)hashtable->hashsize;

   return factor;
}
//-----------------------------------------------------------
void *hashTable_remove(HashTable *hashtable,void *key)
{
    DataNode *datanode = NULL;
    void      *current = NULL;

    if (NULL == hashtable){
      log_error("HashTable_Remove:hash table is NULL!!!");
       return NULL;
    }

    unsigned int index = hashtable->hashfun(key,strlen(key)) % hashtable->hashsize;

    if (NULL != hashtable->buckets[index]){

      datanode = DataList_Find(hashtable->buckets[index]->List,hashtable->hashcmp,key);

      if (NULL != datanode){
      	 current = DataList_Removenode(hashtable->buckets[index]->List,datanode);
      }
   }

   return current;
}
//-----------------------------------------------------------
void hashTable_push(HashTable *hashtable,void *key,void *data)
{
   if (NULL == hashtable){
	   log_error("HashTable_Add:hash table obj is NULL !!!");
       return;
   }

   unsigned int index = hashtable->hashfun(key,strlen(key)) % hashtable->hashsize;

   if (NULL == hashtable->buckets[index]){
       hashtable->buckets[index] = hashNode_create(data);
   }

   if ( NULL != hashtable->buckets[index]){
       DataList_Addtail(hashtable->buckets[index]->List,data);
   }
}
//-----------------------------------------------------------
void *hashTable_pop(HashTable *hashtable,void *key)
{
    DataNode *datanode = NULL;

   if (NULL == hashtable){
	   log_error("HashTable_Add:hash table obj is NULL !!!");
       return NULL;
   }

   unsigned int index = hashtable->hashfun(key,strlen(key)) % hashtable->hashsize;

   if ( NULL == hashtable->buckets[index]){
	  log_error("HashTable_Get:not find data in hashtable !!!");
      return NULL;
   }else{
      datanode = DataList_Find(hashtable->buckets[index]->List,hashtable->hashcmp,key);
   }

   return DataList_GetCurrentData(datanode);
}
//-----------------------------------------------------------
