#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <sys/mman.h>
// #include <asm/ptrace.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <unistd.h>
#include "../../Base/MojingTypes.h"
#include "instruction.h"
#include "detour.h"

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

#define PAGE_START(addr) (~(PAGE_SIZE - 1) & (addr))
#define SET_BIT0(addr) (addr | 1)
#define CLEAR_BIT0(addr) (addr & 0xFFFFFFFE)
#define TEST_BIT0(addr) (addr & 1)

#define ACTION_ENABLE 0
#define ACTION_DISABLE 1


#ifdef LOG4CPLUS_IMPORT
#include "../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../LogTraker/LogInterface.h"
#endif

#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

enum hook_status
{
    REGISTERED,
    HOOKED,
};

struct inlineHookItem
{
    uint32_t target_addr;
    uint32_t new_addr;
    uint32_t **proto_addr;
    void *orig_instructions;
    int orig_boundaries[4];
    int trampoline_boundaries[20];
    int count;
    void *trampoline_instructions;
    int length;
    int status;
    int mode;
};

struct inlineHookInfo
{
    struct inlineHookItem item[1024];
    int size;
};

static struct inlineHookInfo info = {0};

static int getAllTids( pid_t pid, pid_t *tids )
{
    char dir_path[32];
    DIR *dir;
    int i;
    struct dirent *entry;
    pid_t tid;

    if ( pid < 0 )
    {
        snprintf( dir_path, sizeof( dir_path ), "/proc/self/task" );
    }
    else
    {
        snprintf( dir_path, sizeof( dir_path ), "/proc/%d/task", pid );
    }

    dir = opendir( dir_path );
    if ( dir == NULL )
    {
        return 0;
    }

    i = 0;
    while ( ( entry = readdir( dir ) ) != NULL )
    {
        tid = atoi( entry->d_name );
        if ( tid != 0 && tid != getpid() )
        {
            tids[i++] = tid;
        }
    }
    closedir( dir );
    return i;
}

static bool doProcessThreadPC( struct inlineHookItem *item, struct pt_regs *regs, int action )
{
    int offset;
    int i;

    switch ( action )
    {
    case ACTION_ENABLE:
        offset = regs->ARM_pc - CLEAR_BIT0( item->target_addr );
        for ( i = 0; i < item->count; ++i )
        {
            if ( offset == item->orig_boundaries[i] )
            {
                regs->ARM_pc = ( uint32_t )item->trampoline_instructions + item->trampoline_boundaries[i];
                return true;
            }
        }
        break;

    case ACTION_DISABLE:
        offset = regs->ARM_pc - ( int )item->trampoline_instructions;
        for ( i = 0; i < item->count; ++i )
        {
            if ( offset == item->trampoline_boundaries[i] )
            {
                regs->ARM_pc = CLEAR_BIT0( item->target_addr ) + item->orig_boundaries[i];
                return true;
            }
        }
        break;
    }

    return false;
}

static void processThreadPC( pid_t tid, struct inlineHookItem *item, int action )
{
    struct pt_regs regs;
    if ( ptrace( PTRACE_GETREGS, tid, NULL, &regs ) == 0 )
    {
        if ( item == NULL )
        {
            int pos;
            for ( pos = 0; pos < info.size; ++pos )
            {
                if ( doProcessThreadPC( &info.item[pos], &regs, action ) == true )
                {
                    break;
                }
            }
        }
        else
        {
            doProcessThreadPC( item, &regs, action );
        }

        ptrace( PTRACE_SETREGS, tid, NULL, &regs );
    }
}

static pid_t freeze( struct inlineHookItem *item, int action )
{
    int count;
    pid_t tids[1024];
    pid_t pid;

    pid = -1;
    count = getAllTids( getpid(), tids );
#ifdef _DEBUG
	MOJING_TRACE(g_APIlogger , "Count = " << count);
#endif
    if ( count > 0 )
    {
#ifdef _DEBUG
		MOJING_TRACE(g_APIlogger, "fork begin ... ");
#endif
		
		pid = fork();
#ifdef _DEBUG
		MOJING_TRACE(g_APIlogger, "fork end ... , pid = " << pid);
#endif
		if ( pid == 0 )
        {
            int i;
            for ( i = 0; i < count; ++i )
            {
#ifdef _DEBUG
				MOJING_TRACE(g_APIlogger, "loop :: " << i <<  " / " << count);
#endif
				if ( ptrace( PTRACE_ATTACH, tids[i], NULL, NULL ) == 0 )
                {
#ifdef _DEBUG
					MOJING_TRACE(g_APIlogger, "waitpid :: " << tids[i]);
#endif
					
					waitpid( tids[i], NULL, WUNTRACED );
#ifdef _DEBUG
					MOJING_TRACE(g_APIlogger, "waitpid :: Done" );
#endif
                    processThreadPC( tids[i], item, action );
#ifdef _DEBUG
					MOJING_TRACE(g_APIlogger, "end of loop");
#endif
				
				}
            }
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, "raise SIGSTOP ... begin");
#endif
            raise( SIGSTOP );
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, "raise SIGSTOP ... end");
#endif

            for ( i = 0; i < count; ++i )
            {
#ifdef _DEBUG
				MOJING_TRACE(g_APIlogger, "ptrace " << i << " / " << count);
#endif
				
				ptrace( PTRACE_DETACH, tids[i], NULL, NULL );
            }
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, "raise SIGKILL ... begin");
#endif
            raise( SIGKILL );
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, "raise SIGKILL ... begin");
#endif
		
		}

        else if ( pid > 0 )
        {
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, "waitpid , begin...");
#endif
            waitpid( pid, NULL, WUNTRACED );
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, "waitpid , end");
#endif
		}
		else
		{
#ifdef _DEBUG
			MOJING_WARN(g_APIlogger, "DO NOTHING");
#endif
		}
    }

    return pid;
}

static void unFreeze( pid_t pid )
{
    if ( pid < 0 )
    {
        return;
    }

    kill( pid, SIGCONT );
    wait( NULL );
}

static bool isExecutableAddr( uint32_t addr )
{
    FILE *fp;
    char line[1024];
    uint32_t start;
    uint32_t end;

    fp = fopen( "/proc/self/maps", "r" );
    if ( fp == NULL )
    {
        return false;
    }

    while ( fgets( line, sizeof( line ), fp ) )
    {
        if ( strstr( line, "r-xp" ) )
        {
            start = strtoul( strtok( line, "-" ), NULL, 16 );
            end = strtoul( strtok( NULL, " " ), NULL, 16 );
            if ( addr >= start && addr <= end )
            {
                fclose( fp );
                return true;
            }
        }
    }

    fclose( fp );

    return false;
}

static struct inlineHookItem *findInlineHookItem( uint32_t target_addr )
{
    int i;

    for ( i = 0; i < info.size; ++i )
    {
        if ( info.item[i].target_addr == target_addr )
        {
            return &info.item[i];
        }
    }

    return NULL;
}

static struct inlineHookItem *addInlineHookItem()
{
    struct inlineHookItem *item;

    if ( info.size >= 1024 )
    {
        return NULL;
    }

    item = &info.item[info.size];
    ++info.size;

    return item;
}

static void deleteInlineHookItem( int pos )
{
    info.item[pos] = info.item[info.size - 1];
    --info.size;
}

enum detour_status registerInlineHook( uint32_t target_addr, uint32_t new_addr, uint32_t **proto_addr )
{
	MOJING_FUNC_TRACE(g_APIlogger);
    struct inlineHookItem *item;

    if ( !isExecutableAddr( target_addr ) || !isExecutableAddr( new_addr ) )
    {
        return DETOUR_ERROR_NOT_EXECUTABLE;
    }

    item = findInlineHookItem( target_addr );
    if ( item != NULL )
    {
        if ( item->status == REGISTERED )
        {
            return DETOUR_ERROR_ALREADY_REGISTERED;
        }
        else if ( item->status == HOOKED )
        {
            return DETOUR_ERROR_ALREADY_HOOKED;
        }
        else
        {
            return DETOUR_ERROR_UNKNOWN;
        }
    }
	MOJING_TRACE(g_APIlogger , "addInlineHookItem....");
    item = addInlineHookItem();
	MOJING_TRACE(g_APIlogger, "addInlineHookItem done , totle = " << info.size);

    item->target_addr = target_addr;
    item->new_addr = new_addr;
    item->proto_addr = proto_addr;

    item->length = TEST_BIT0( item->target_addr ) ? 12 : 8;
    item->orig_instructions = malloc( item->length );
    memcpy( item->orig_instructions, ( void * )CLEAR_BIT0( item->target_addr ), item->length );

    item->trampoline_instructions = mmap( NULL, PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0 );
    relocateInstruction( item->target_addr, item->orig_instructions, item->length, item->trampoline_instructions, item->orig_boundaries, item->trampoline_boundaries, &item->count );

    item->status = REGISTERED;

    return DETOUR_OK;
}

static void doInlineUnHook( struct inlineHookItem *item, int pos )
{
    mprotect( ( void * )PAGE_START( CLEAR_BIT0( item->target_addr ) ), PAGE_SIZE * 2, PROT_READ | PROT_WRITE | PROT_EXEC );
    memcpy( ( void * )CLEAR_BIT0( item->target_addr ), item->orig_instructions, item->length );
    mprotect( ( void * )PAGE_START( CLEAR_BIT0( item->target_addr ) ), PAGE_SIZE * 2, PROT_READ | PROT_EXEC );
    munmap( item->trampoline_instructions, PAGE_SIZE );
    free( item->orig_instructions );

    deleteInlineHookItem( pos );

    cacheflush( CLEAR_BIT0( item->target_addr ), CLEAR_BIT0( item->target_addr ) + item->length, 0 );
}

enum detour_status inlineUnHook( uint32_t target_addr )
{
    int i;
    for ( i = 0; i < info.size; ++i )
    {
        if ( info.item[i].target_addr == target_addr && info.item[i].status == HOOKED )
        {
            pid_t pid;

            pid = freeze( &info.item[i], ACTION_DISABLE );

            doInlineUnHook( &info.item[i], i );

            unFreeze( pid );

            return DETOUR_OK;
        }
    }

    return DETOUR_ERROR_NOT_HOOKED;
}

void inlineUnHookAll()
{
    pid_t pid;
    int i;

    pid = freeze( NULL, ACTION_DISABLE );
    for ( i = 0; i < info.size; ++i )
    {
        if ( info.item[i].status == HOOKED )
        {
            doInlineUnHook( &info.item[i], i );
            --i;
        }
    }

    unFreeze( pid );
}

static void doInlineHook( struct inlineHookItem *item )
{
    mprotect( ( void * )PAGE_START( CLEAR_BIT0( item->target_addr ) ), PAGE_SIZE * 2, PROT_READ | PROT_WRITE | PROT_EXEC );
    if ( TEST_BIT0( item->target_addr ) )
    {
        int i;

        i = 0;
        if ( CLEAR_BIT0( item->target_addr ) % 4 != 0 )
        {
            ( ( uint16_t * )CLEAR_BIT0( item->target_addr ) )[i++] = 0xBF00; // NOP
        }
        ( ( uint16_t * )CLEAR_BIT0( item->target_addr ) )[i++] = 0xF8DF;
        ( ( uint16_t * )CLEAR_BIT0( item->target_addr ) )[i++] = 0xF000; // LDR.W PC, [PC]
        ( ( uint16_t * )CLEAR_BIT0( item->target_addr ) )[i++] = item->new_addr & 0xFFFF;
        ( ( uint16_t * )CLEAR_BIT0( item->target_addr ) )[i++] = item->new_addr >> 16;
    }
    else
    {
        ( ( uint32_t * )( item->target_addr ) )[0] = 0xe51ff004; // LDR PC, [PC, #-4]
        ( ( uint32_t * )( item->target_addr ) )[1] = item->new_addr;
    }

    mprotect( ( void * )PAGE_START( CLEAR_BIT0( item->target_addr ) ), PAGE_SIZE * 2, PROT_READ | PROT_EXEC );
    if ( item->proto_addr != NULL )
    {
        *( item->proto_addr ) = TEST_BIT0( item->target_addr ) ? ( uint32_t * )SET_BIT0( ( uint32_t )item->trampoline_instructions ) : ( uint32_t * )item->trampoline_instructions;
        //(addr & 1)
        //(addr | 1)
        // *(item->proto_addr) = ( item->target_addr & 1) ?
        //                       (uint32_t*)((uint32_t)item->trampoline_instructions | 1) :
        //                    (uint32_t*)item->trampoline_instructions;

        //(uint32_t *) SET_BIT0(*(uint32_t*)item->trampoline_instructions) :
    }

    item->status = HOOKED;
    cacheflush( CLEAR_BIT0( item->target_addr ), CLEAR_BIT0( item->target_addr ) + item->length, 0 );
}

enum detour_status inlineHook( uint32_t target_addr )
{
	MOJING_FUNC_TRACE(g_APIlogger);
    int i;
    struct inlineHookItem *item;

    item = NULL;
    for ( i = 0; i < info.size; ++i )
    {
		MOJING_TRACE(g_APIlogger , "Find in function " << i << " / " << info.size);
        if ( info.item[i].target_addr == target_addr )
        {
            item = &info.item[i];
            break;
        }
    }

    if ( item == NULL )
    {
        return DETOUR_ERROR_NOT_REGISTERED;
    }
	
	MOJING_TRACE(g_APIlogger, "status = " << item->status);
    if ( item->status == REGISTERED )
    {
		pid_t pid;
//#ifdef _DEBUG
//		MOJING_TRACE(g_APIlogger, "freeze ...");
//#endif
//		pid = freeze(item, ACTION_ENABLE);
//#ifdef _DEBUG
//		MOJING_TRACE(g_APIlogger, "doInlineHook ...");
//#endif
        doInlineHook( item );
//#ifdef _DEBUG
//		MOJING_TRACE(g_APIlogger, "unFreeze ...");
//#endif
//        unFreeze( pid );

        return DETOUR_OK;
    }
    else if ( item->status == HOOKED )
    {
        return DETOUR_ERROR_ALREADY_HOOKED;
    }
    else
    {
        return DETOUR_ERROR_UNKNOWN;
    }
}

void inlineHookAll()
{
    pid_t pid;
    int i;
#ifdef _DEBUG
	MOJING_TRACE(g_APIlogger, "freeze ...");
#endif
    pid = freeze( NULL, ACTION_ENABLE );
#ifdef _DEBUG
	MOJING_TRACE(g_APIlogger, "freeze ... end");
#endif // endif
    for ( i = 0; i < info.size; ++i )
    {
        if ( info.item[i].status == REGISTERED )
        {
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger , "do hook " << i << " / " << info.size);
#endif // endif
            doInlineHook( &info.item[i] );
        }
		else
		{
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, "skip hook " << i << " / " << info.size);
#endif // endif
		}
	    usleep(10 * 1000);
    }
#ifdef _DEBUG
	MOJING_TRACE(g_APIlogger, "unFreeze ..." );
#endif // endif
    unFreeze( pid );
#ifdef _DEBUG
	MOJING_TRACE(g_APIlogger, "unFreeze ... done");
#endif // endif
}
