/**********************************************
* cstr.h
*
*  Created on: 2015-05-10
*      Author: xifengming
*       Email: xifengming@vip.sina.com
*    Comments: string 
 *********************************************/
#ifndef _CSTR_H
#define _CSTR_H


#ifdef __cplusplus
extern "C" {
#endif

#define CSTR_MAX_PREALLOC (1024*1024)

typedef struct tagCStr   CStr;

char  *CStr_Malloc(const char *srcstr);
void   CStr_Free(char *srcstr);
size_t CStr_Len(const char *srcstr);
char   *CStr_Dup(const char *srcstr);
char   *CStr_MackSpace(char *srcstr,size_t addlen);

#ifdef __cplusplus
}
#endif

#endif
