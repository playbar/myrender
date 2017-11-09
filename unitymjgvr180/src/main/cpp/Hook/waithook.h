#ifndef __WAITHOOK_H__
#define __WAITHOOK_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

bool phrase_dev_num(char *devno, int *pmajor, int *pminor);
bool phrase_proc_base_addr(char *addr, void **pbase_addr, void **pend_addr);
void* getimagebase(const char *target);
void waitLoading(const char *file);
void waitLoadingHook(const char *file);
//void starthookthread();

#ifdef __cplusplus
}
#endif

#endif
