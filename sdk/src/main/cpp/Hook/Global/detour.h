#ifndef _DETOUR_H_
#define _DETOUR_H_

#include <stdio.h>

enum detour_status
{
    DETOUR_ERROR_UNKNOWN = -1,
    DETOUR_OK = 0,
    DETOUR_ERROR_NOT_INITIALIZED,
    DETOUR_ERROR_NOT_EXECUTABLE,
    DETOUR_ERROR_NOT_REGISTERED,
    DETOUR_ERROR_NOT_HOOKED,
    DETOUR_ERROR_ALREADY_REGISTERED,
    DETOUR_ERROR_ALREADY_HOOKED,
    DETOUR_ERROR_SO_NOT_FOUND,
    DETOUR_ERROR_FUNCTION_NOT_FOUND
};

enum detour_status registerInlineHook( uint32_t target_addr, uint32_t new_addr, uint32_t **proto_addr );
enum detour_status inlineUnHook( uint32_t target_addr );
void inlineUnHookAll();
enum detour_status inlineHook( uint32_t target_addr );
void inlineHookAll();

#endif //_DETOUR_H_