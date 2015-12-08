#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cstr.h"

struct tagCStr
{
    unsigned int len;
    unsigned int freelen;
    char buf[];
};
//-----------------------------------------------------------
char *CStr_Create(const void *src,size_t datalen)
{
     CStr *str = NULL;
 
     if (NULL != src){
        str = malloc(sizeof(struct tagCStr)+datalen+1);
        if (NULL == str) return NULL;
        memset(str,0,sizeof(struct tagCStr)+datalen+1);
        str->len     = datalen;
        str->freelen = 0;

        if (datalen >0 && src !=NULL){
           memcpy(str->buf,src,datalen);
           str->buf[datalen] = '\0';
           return str->buf;           
        }       
    }

    return NULL;
}
//-----------------------------------------------------------
char *CStr_Malloc(const char *srcstr)
{
   size_t datalen = (srcstr == NULL) ?0:strlen(srcstr);
   return CStr_Create(srcstr,datalen);
}
//-----------------------------------------------------------
void CStr_Free(char *srcstr)
{
   if (NULL != srcstr){
      free(srcstr - sizeof(struct tagCStr));
   }
}
//-----------------------------------------------------------
size_t CStr_Len(const char *srcstr)
{
    CStr *str = (void*)(srcstr-sizeof(CStr));
    return str->len;  
}
//-----------------------------------------------------------
char *CStr_Dup(const char *srcstr)
{
   return CStr_Create(srcstr,CStr_Len(srcstr));
}
//-----------------------------------------------------------
size_t CStr_GetFreeLen(const char *srcstr)
{
   CStr *str = (void *)(srcstr - sizeof(CStr));

   return str->freelen;
}
//-----------------------------------------------------------
char *CStr_MackSpace(char *srcstr,size_t addlen)
{
   CStr *str    = NULL;
   CStr *newstr = NULL;

   size_t len     = 0;
   size_t newlen = 0;

   size_t freelen = CStr_GetFreeLen(srcstr);

   if (freelen >= addlen){
      return srcstr;
   }

   len = CStr_Len(srcstr);
   newlen = len + addlen;

   if (newlen < CSTR_MAX_PREALLOC){
       newlen *=2;
   }else{
       newlen += CSTR_MAX_PREALLOC;
   }

   str = (CStr*)(srcstr - sizeof(CStr));

   newstr = (CStr*)realloc(str,newlen);

   if (NULL == newstr) return NULL;
   
   newstr->freelen = newlen - len;

   return newstr->buf;
}
//-----------------------------------------------------------
