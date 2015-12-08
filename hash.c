#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
HashTable *HashTable_Create(unsigned int size,HashFun hashfun,HashCmpFun hashcmp)
{
     int i=0;

     HashTable *hashtable_obj = (HashTable *)malloc(sizeof(HashTable));

     if (NULL == hashtable_obj){
          printf("HashTable_Create:hashtable obj malloc fail!!!\n");
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
HashNode *HashNode_Create(void *data)
{
    HashNode *hashnode = (HashNode *)malloc(sizeof(HashNode));

    if (NULL == hashnode){
       printf("HashNode_Create:hash node malloc fail!!!\n");
       return NULL;
    }

    hashnode->List =  DataList_Create();

    return hashnode;
}
//-----------------------------------------------------------
void HashTable_Clear(HashTable *hashtable)
{
    int i=0;

    if (NULL == hashtable){
       printf("HashTable_Clear:hash table obj is NULL!!!\n");
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
float HashTable_Loadfactor(HashTable *hashtable)
{
   unsigned int touched=0;
                 int i =0;
           float factor=0.00;

   if (NULL == hashtable){
      printf("HashTable_Loadfactor:hash table obj is NULL!!!\n");
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
void *HashTable_Remove(HashTable *hashtable,void *key)
{
    DataNode *datanode = NULL;
    void      *current = NULL;

    if (NULL == hashtable){
       printf("HashTable_Remove:hash table is NULL!!!\n");
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
void HashTable_Add(HashTable *hashtable,void *key,void *data)
{
   if (NULL == hashtable){
       printf("HashTable_Add:hash table obj is NULL !!!\n");
       return;
   }

   unsigned int index = hashtable->hashfun(key,strlen(key)) % hashtable->hashsize;

   if (NULL == hashtable->buckets[index]){
       hashtable->buckets[index] = HashNode_Create(data);
   }

   if ( NULL != hashtable->buckets[index]){
       DataList_Addtail(hashtable->buckets[index]->List,data);
   }
}
//-----------------------------------------------------------
void *HashTable_Get(HashTable *hashtable,void *key)
{
    DataNode *datanode = NULL;

   if (NULL == hashtable){
        printf("HashTable_Add:hash table obj is NULL !!!\n");
        return NULL;
   }

   unsigned int index = hashtable->hashfun(key,strlen(key)) % hashtable->hashsize;

   if ( NULL == hashtable->buckets[index]){
      printf("HashTable_Get:not find data in hashtable !!!\n");
      return NULL;
   }else{
      datanode = DataList_Find(hashtable->buckets[index]->List,hashtable->hashcmp,key);
   }

   return DataList_GetCurrentData(datanode);
}
//-----------------------------------------------------------
