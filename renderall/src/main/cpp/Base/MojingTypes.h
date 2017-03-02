#ifndef MJ_Types_H
#define MJ_Types_H

//-----------------------------------------------------------------------------------
// ****** Operating System
//
// Type definitions exist for the following operating systems: (MJ_OS_x)
//
//    WIN32    - Win32 (Windows 95/98/ME and Windows NT/2000/XP)
//    DARWIN   - Darwin OS (Mac OS X)
//    LINUX    - Linux
//    ANDROID  - Android
//    IPHONE   - iPhone
#include "BuildType.h"
#ifdef _DEBUG
#define  MJ_BUILD_DEBUG
#endif


#define ENABLE_LOGGER 

#if (defined(__APPLE__) && (defined(__GNUC__) ||\
	defined(__xlC__) || defined(__xlc__))) || defined(__MACOS__)
#  if (defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) || defined(__IPHONE_OS_VERSION_MIN_REQUIRED))
#    define MJ_OS_IPHONE
#	 define MJ_OS_IOS
#  else
#    define MJ_OS_DARWIN
#    define MJ_OS_MAC
#  endif
#elif (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define MJ_OS_WIN32
#elif (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  define MJ_OS_WIN32
#elif defined(__linux__) || defined(__linux)
#  define MJ_OS_LINUX
#else
#  define MJ_OS_OTHER
#endif

#if defined(MJ_OS_WIN32)
#define HAVE_STRUCT_TIMESPEC
#include <stdint.h>
#endif

// DO NOT support Metal on simulator
#if defined(MJ_OS_IOS)
#include <TargetConditionals.h>
#if defined(__OBJC__) && (TARGET_CPU_ARM || TARGET_CPU_ARM64)
#define MJ_IOS_METAL
#endif
#endif

#if defined(ANDROID)
#  define MJ_OS_ANDROID
#include "../googlebreakpad/src/client/linux/handler/exception_handler.h"
#include "../googlebreakpad/src/client/linux/handler/minidump_descriptor.h"

extern bool MojingSDK_MinidumpCallBack(const google_breakpad::MinidumpDescriptor& descriptor,void* context,	bool succeeded);
#define SetValueFunc(_VALUE_NAME_) \
void Set##_VALUE_NAME_(const char* szNewValue){ strcpy(sz##_VALUE_NAME_, szNewValue); }
struct __tagMojingMinidumpContext
{
	__tagMojingMinidumpContext(){ *szApp = *szSDKVersion = *szFunc = *szBrand = *szModel = *szSerial = *szGlassesName = 0; tmNow = 0; m_bInit = m_bSensorOn = m_bInMojingWorld = false; };
	// 应用程序信息
	char szApp[128];	//  当前应用程序名
	char szSDKVersion[128];// 当前SDK版本
	// 崩溃现场信息
	char szFunc[512];	//	出错时正在执行的函数
	time_t tmNow;
	bool m_bInit; 
	bool m_bSensorOn;
	bool m_bInMojingWorld;
	// 当前设备信息
	char szBrand[128];
	char szModel[128];
	char szSerial[128];
	char szGlassesName[128];

	SetValueFunc(App);
	SetValueFunc(SDKVersion);
	SetValueFunc(Func);
	SetValueFunc(Brand);
	SetValueFunc(Model);
	SetValueFunc(Serial);
	SetValueFunc(GlassesName);
};


extern __tagMojingMinidumpContext g_MojingMinidump;
#define USING_MINIDUMP 	__tagMojingMinidumpContext g_MojingMinidump;\
	google_breakpad::MinidumpDescriptor descriptor("/mnt/sdcard/MojingSDK");\
	google_breakpad::ExceptionHandler eh(descriptor, NULL, MojingSDK_MinidumpCallBack, &g_MojingMinidump, true, -1)
#endif// end of define ANDROID

#if defined(__APPLE__)
#define MJ_OS_MAC
//#define MJ_OS_IPHONE
#endif

#if defined(MJ_OS_ANDROID) //|| defined(MJ_OS_MAC)
#define LOG4CPLUS_IMPORT
#endif

#ifdef USING_MINIDUMP
class MINIDUMP_HELPER
{
	char szOldFunction[512];
public :
	MINIDUMP_HELPER(const char*pFunction){ strcpy(szOldFunction, g_MojingMinidump.szFunc); g_MojingMinidump.SetFunc(pFunction); };
	virtual ~MINIDUMP_HELPER(){ g_MojingMinidump.SetFunc(szOldFunction); };
};
#define ENTER_MINIDUMP_FUNCTION MINIDUMP_HELPER TempInFunction(__FUNCTION__) // __FUNCSIG__相对于__FUNCTION__，增加了函数的调用类型、返回值、参数表和其他修饰符
#else
#define ENTER_MINIDUMP_FUNCTION
#endif
//-----------------------------------------------------------------------------------
// ***** CPU Architecture
//
// The following CPUs are defined: (MJ_CPU_x)
//
//    X86        - x86 (IA-32)
//    X86_64     - x86_64 (amd64)
//    PPC        - PowerPC
//    PPC64      - PowerPC64
//    MIPS       - MIPS
//    OTHER      - CPU for which no special support is present or needed


#if defined(__x86_64__) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#  define MJ_CPU_X86_64
#  define MJ_64BIT_POINTERS
#elif defined(__i386__) || defined(MJ_OS_WIN32)
#  define MJ_CPU_X86
#elif defined(__powerpc64__)
#  define MJ_CPU_PPC64
#elif defined(__ppc__)
#  define MJ_CPU_PPC
#elif defined(__mips__) || defined(__MIPSEL__)
#  define MJ_CPU_MIPS
#elif defined(__arm__)
#  define MJ_CPU_ARM
#else
#  define MJ_CPU_OTHER
#endif

//-----------------------------------------------------------------------------------
// ***** Co-Processor Architecture
//
// The following co-processors are defined: (MJ_CPU_x)
//
//    SSE        - Available on all modern x86 processors.
//    Altivec    - Available on all modern ppc processors.
//    Neon       - Available on some armv7+ processors.

#if defined(__SSE__) || defined(MJ_OS_WIN32)
#  define  MJ_CPU_SSE
#endif // __SSE__

#if defined( __ALTIVEC__ )
#  define MJ_CPU_ALTIVEC
#endif // __ALTIVEC__

#if defined(__ARM_NEON__)
#  define MJ_CPU_ARM_NEON
#endif // __ARM_NEON__


//-----------------------------------------------------------------------------------
// ***** Compiler
//
//  The following compilers are defined: (MJ_CC_x)
//
//     MSVC     - Microsoft Visual C/C++
//     INTEL    - Intel C++ for Linux / Windows
//     GNU      - GNU C++
//     ARM      - ARM C/C++

#if defined(__INTEL_COMPILER)
// Intel 4.0                    = 400
// Intel 5.0                    = 500
// Intel 6.0                    = 600
// Intel 8.0                    = 800
// Intel 9.0                    = 900
#  define MJ_CC_INTEL       __INTEL_COMPILER

#elif defined(_MSC_VER)
// MSVC 5.0                     = 1100
// MSVC 6.0                     = 1200
// MSVC 7.0 (VC2002)            = 1300
// MSVC 7.1 (VC2003)            = 1310
// MSVC 8.0 (VC2005)            = 1400
// MSVC 9.0 (VC2008)            = 1500
// MSVC 10.0 (VC2010)           = 1600
#  define MJ_CC_MSVC        _MSC_VER

#elif defined(__GNUC__)
#  define MJ_CC_GNU

#elif defined(__CC_ARM)
#  define MJ_CC_ARM

#else
#  error "Mojing does not support this Compiler"
#endif


//-----------------------------------------------------------------------------------
// ***** Compiler Warnings

// Disable MSVC warnings
#if defined(MJ_CC_MSVC)
#  pragma warning(disable : 4127)    // Inconsistent dll linkage
#  pragma warning(disable : 4530)    // Exception handling
#  if (MJ_CC_MSVC<1300)
#    pragma warning(disable : 4514)  // Unreferenced inline function has been removed
#    pragma warning(disable : 4710)  // Function not inlined
#    pragma warning(disable : 4714)  // _force_inline not inlined
#    pragma warning(disable : 4786)  // Debug variable name longer than 255 chars
#  endif // (MJ_CC_MSVC<1300)
#endif // (MJ_CC_MSVC)



// *** Linux Unicode - must come before Standard Includes

#ifdef MJ_OS_LINUX
// Use glibc unicode functions on linux.
#  ifndef  _GNU_SOURCE
#    define _GNU_SOURCE
#  endif
#endif

//-----------------------------------------------------------------------------------
// ***** Standard Includes
//
#include    <stddef.h>
#include    <limits.h>
#include    <float.h>


// MSVC Based Memory Leak checking - for now
#if defined(MJ_CC_MSVC) && defined(MJ_BUILD_DEBUG)
#  define _CRTDBG_MAP_ALLOC
#  include <stdlib.h>
#  include <crtdbg.h>

// Uncomment this to help debug memory leaks under Visual Studio in OVR apps only.
// This shouldn't be defined in customer releases.
#  ifndef MJ_DEFINE_NEW
#    define MJ_DEFINE_NEW new(__FILE__, __LINE__)
#    define new MJ_DEFINE_NEW
#  endif

#endif


//-----------------------------------------------------------------------------------
// ***** Type definitions for Common Systems

namespace Baofeng
{
		typedef char            Char;

		// Pointer-sized integer
		typedef size_t          UPInt;
		typedef ptrdiff_t       SPInt;


#if defined(MJ_OS_WIN32)

		typedef char            SByte;  // 8 bit Integer (Byte)
		typedef unsigned char   UByte;
		typedef short           SInt16; // 16 bit Integer (Word)
		typedef unsigned short  UInt16;
		typedef long            SInt32; // 32 bit Integer
		typedef unsigned long   UInt32;
		typedef __int64         SInt64; // 64 bit Integer (QWord)
		typedef unsigned __int64 UInt64;


#elif defined(MJ_OS_MAC) || defined(MJ_OS_IPHONE) || defined(MJ_CC_GNU)

		typedef int             SByte  __attribute__((__mode__ (__QI__)));
		typedef unsigned int    UByte  __attribute__((__mode__ (__QI__)));
		typedef int             SInt16 __attribute__((__mode__ (__HI__)));
		typedef unsigned int    UInt16 __attribute__((__mode__ (__HI__)));
		typedef int             SInt32 __attribute__((__mode__ (__SI__)));
		typedef unsigned int    UInt32 __attribute__((__mode__ (__SI__)));
		typedef int             SInt64 __attribute__((__mode__ (__DI__)));
		typedef unsigned int    UInt64 __attribute__((__mode__ (__DI__)));

#else

#include <sys/types.h>
		typedef int8_t          SByte;
		typedef uint8_t         UByte;
		typedef int16_t         SInt16;
		typedef uint16_t        UInt16;
		typedef int32_t         SInt32;
		typedef uint32_t        UInt32;
		typedef int64_t         SInt64;
		typedef uint64_t        UInt64;

#endif


		// ***** BaseTypes Namespace

		// BaseTypes namespace is explicitly declared to allow base types to be used
		// by customers directly without other contents of OVR namespace.
		//
		// Its is expected that GFx samples will declare 'using namespace OVR::BaseTypes'
		// to allow using these directly without polluting the target scope with other
		// OVR declarations, such as Ptr<>, String or Mutex.
		namespace BaseTypes
		{
			using Baofeng::UPInt;
			using Baofeng::SPInt;
			using Baofeng::UByte;
			using Baofeng::SByte;
			using Baofeng::UInt16;
			using Baofeng::SInt16;
			using Baofeng::UInt32;
			using Baofeng::SInt32;
			using Baofeng::UInt64;
			using Baofeng::SInt64;
		} // OVR::BaseTypes

	} // OVR


	//-----------------------------------------------------------------------------------
	// ***** Macro Definitions
	//
	// We define the following:
	//
	//  MJ_BYTE_ORDER      - Defined to either MJ_LITTLE_ENDIAN or MJ_BIG_ENDIAN
	//  MJ_FORCE_INLINE    - Forces inline expansion of function
	//  MJ_ASM             - Assembly language prefix
	//  MJ_STR             - Prefixes string with L"" if building unicode
	// 
	//  MJ_STDCALL         - Use stdcall calling convention (Pascal arg order)
	//  MJ_CDECL           - Use cdecl calling convention (C argument order)
	//  MJ_FASTCALL        - Use fastcall calling convention (registers)
	//

	// Byte order constants, MJ_BYTE_ORDER is defined to be one of these.
#define MJ_LITTLE_ENDIAN       1
#define MJ_BIG_ENDIAN          2


	// Force inline substitute - goes before function declaration
#if defined(MJ_CC_MSVC)
#  define MJ_FORCE_INLINE  __forceinline
#elif defined(MJ_CC_GNU)
#  define MJ_FORCE_INLINE  inline __attribute__((always_inline))
#else
#  define MJ_FORCE_INLINE  inline
#endif  // MJ_CC_MSVC


#if defined(MJ_OS_WIN32)

	// ***** Win32

	// Byte order
#define MJ_BYTE_ORDER    MJ_LITTLE_ENDIAN

	// Calling convention - goes after function return type but before function name
#ifdef __cplusplus_cli
#  define MJ_FASTCALL      __stdcall
#else
#  define MJ_FASTCALL      __fastcall
#endif

#define MJ_STDCALL         __stdcall
#define MJ_CDECL           __cdecl


	// Assembly macros
#if defined(MJ_CC_MSVC)
#  define MJ_ASM           _asm
#else
#  define MJ_ASM           asm
#endif // (MJ_CC_MSVC)

#ifdef UNICODE
#  define MJ_STR(str)      L##str
#else
#  define MJ_STR(str)      str
#endif // UNICODE

#else

	// **** Standard systems

#if (defined(BYTE_ORDER) && (BYTE_ORDER == BIG_ENDIAN))|| \
	(defined(_BYTE_ORDER) && (_BYTE_ORDER == _BIG_ENDIAN))
#  define MJ_BYTE_ORDER    MJ_BIG_ENDIAN
#elif (defined(__ARMEB__) || defined(MJ_CPU_PPC) || defined(MJ_CPU_PPC64))
#  define MJ_BYTE_ORDER    MJ_BIG_ENDIAN
#else
#  define MJ_BYTE_ORDER    MJ_LITTLE_ENDIAN
#endif

	// Assembly macros
#define MJ_ASM                  __asm__
#define MJ_ASM_PROC(procname)   MJ_ASM
#define MJ_ASM_END              MJ_ASM

	// Calling convention - goes after function return type but before function name
#define MJ_FASTCALL
#define MJ_STDCALL
#define MJ_CDECL

#endif // defined(MJ_OS_WIN32)



	//-----------------------------------------------------------------------------------
	// ***** MJ_DEBUG_BREAK, MJ_ASSERT
	//
	// If not in debug build, macros do nothing
#ifndef MJ_BUILD_DEBUG

#  define MJ_DEBUG_BREAK  ((void)0)
#  define MJ_ASSERT(p)    ((void)0)

#else 

	// Microsoft Win32 specific debugging support
#if defined(MJ_OS_WIN32)
#  ifdef MJ_CPU_X86
#    if defined(__cplusplus_cli)
#      define MJ_DEBUG_BREAK   do { __debugbreak(); } while(0)
#    elif defined(MJ_CC_GNU)
#      define MJ_DEBUG_BREAK   do { MJ_ASM("int $3\n\t"); } while(0)
#    else
#      define MJ_DEBUG_BREAK   do { MJ_ASM int 3 } while (0)
#    endif
#  else
#    define MJ_DEBUG_BREAK     do { __debugbreak(); } while(0)
#  endif
	// Android specific debugging support
#elif defined(MJ_OS_ANDROID)
#  define MJ_DEBUG_BREAK       do { __builtin_trap(); } while(0)
	// Unix specific debugging support
#elif defined(MJ_CPU_X86) || defined(MJ_CPU_X86_64)
#  define MJ_DEBUG_BREAK       do { MJ_ASM("int $3\n\t"); } while(0)
#else
#  define MJ_DEBUG_BREAK       do { *((int *) 0) = 1; } while(0)
#endif

	// This will cause compiler breakpoint
#define MJ_ASSERT(p)           do { if (!(p))  { MJ_DEBUG_BREAK; } } while(0)

#endif // MJ_BUILD_DEBUG


	// Compile-time assert; produces compiler error if condition is false
#define MJ_COMPILER_ASSERT(x)  { int zero = 0; switch(zero) {case 0: case x:;} }



	//-----------------------------------------------------------------------------------
	// ***** MJ_UNUSED - Unused Argument handling

	// Macro to quiet compiler warnings about unused parameters/variables.
#if defined(MJ_CC_GNU)
#  define   MJ_UNUSED(a)   do {__typeof__ (&a) __attribute__ ((unused)) __tmp = &a; } while(0)
#else
#  define   MJ_UNUSED(a)   (a)
#endif

#define     MJ_UNUSED1(a1) MJ_UNUSED(a1)
#define     MJ_UNUSED2(a1,a2) MJ_UNUSED(a1); MJ_UNUSED(a2)
#define     MJ_UNUSED3(a1,a2,a3) MJ_UNUSED2(a1,a2); MJ_UNUSED(a3)
#define     MJ_UNUSED4(a1,a2,a3,a4) MJ_UNUSED3(a1,a2,a3); MJ_UNUSED(a4)
#define     MJ_UNUSED5(a1,a2,a3,a4,a5) MJ_UNUSED4(a1,a2,a3,a4); MJ_UNUSED(a5)
#define     MJ_UNUSED6(a1,a2,a3,a4,a5,a6) MJ_UNUSED4(a1,a2,a3,a4); MJ_UNUSED2(a5,a6)
#define     MJ_UNUSED7(a1,a2,a3,a4,a5,a6,a7) MJ_UNUSED4(a1,a2,a3,a4); MJ_UNUSED3(a5,a6,a7)
#define     MJ_UNUSED8(a1,a2,a3,a4,a5,a6,a7,a8) MJ_UNUSED4(a1,a2,a3,a4); MJ_UNUSED4(a5,a6,a7,a8)
#define     MJ_UNUSED9(a1,a2,a3,a4,a5,a6,a7,a8,a9) MJ_UNUSED4(a1,a2,a3,a4); MJ_UNUSED5(a5,a6,a7,a8,a9)

// #define max(a,b) (((a) > (b)) ? (a) : (b))
// #define min(a,b) (((a) < (b)) ? (a) : (b))

	//-----------------------------------------------------------------------------------
	// ***** Configuration Macros

	// SF Build type
#ifdef MJ_BUILD_DEBUG
#  define MJ_BUILD_STRING  "Debug"
#else
#  define MJ_BUILD_STRING  "Release"
#endif


	//// Enables SF Debugging information
	//# define MJ_BUILD_DEBUG

	// MJ_DEBUG_STATEMENT injects a statement only in debug builds.
	// MJ_DEBUG_SELECT injects first argument in debug builds, second argument otherwise.
#ifdef MJ_BUILD_DEBUG
#define MJ_DEBUG_STATEMENT(s)   s
#define MJ_DEBUG_SELECT(d, nd)  d
#else
#define MJ_DEBUG_STATEMENT(s)
#define MJ_DEBUG_SELECT(d, nd)  nd
#endif


#define MJ_ENABLE_THREADS
	//
	// Prevents OVR from defining new within
	// type macros, so developers can override
	// new using the #define new new(...) trick
	// - used with MJ_DEFINE_NEW macro
	//# define MJ_BUILD_DEFINE_NEW
	//

#define JSON_IO(_TYPE_ , _PREV_NAME_ , _NAME_ )\
virtual void _NAME_##ToJson(JSON* pJson){ pJson->AddNumberItem(#_NAME_, _PREV_NAME_##_NAME_); }; \
	virtual bool _NAME_##FromJson(JSON* pJson)\
	{\
		JSON* pSubJson = pJson->GetItemByName(#_NAME_); \
		if (pSubJson) \
		{\
			Set##_NAME_((_TYPE_)pSubJson->GetDoubleValue()); \
			return true; \
		}; \
		return false; \
	}; 

#define JSON_IO_ARRAY(_TYPE_ ,_PREV_NAME_, _NAME_, _SIZE_)\
	virtual void _NAME_##ToJson(JSON* pJson){ JSON* pArray = JSON::CreateArray(); for (int i = 0; i < _SIZE_; i++)pArray->AddArrayNumber(_PREV_NAME_##_NAME_[i]); pJson->AddItem(#_NAME_, pArray); }; \
	virtual bool _NAME_##FromJson(JSON* pJson)\
	{\
		bool bRet = true;\
		JSON* pJsonOBJ = pJson->GetItemByName(#_NAME_); \
		if (pJsonOBJ &&pJsonOBJ->HasItems() && pJsonOBJ->GetArraySize() == _SIZE_) \
		{\
			for (int i = 0; i < _SIZE_; i++)\
			{\
				_PREV_NAME_##_NAME_[i] = (_TYPE_)pJsonOBJ->GetArrayNumber(i); \
			}\
			return true;\
		}\
		return false;\
	}

#define JSON_IO_V2( _PREV_NAME_ , _NAME_ )\
	virtual void _NAME_##ToJson(JSON* pJson){ JSON* pJsonOBJ = pJson->CreateObject();  pJsonOBJ->AddNumberItem("x", _PREV_NAME_##_NAME_.x); pJsonOBJ->AddNumberItem("y", _PREV_NAME_##_NAME_.y); pJson->AddItem(#_NAME_, pJsonOBJ); }; \
	virtual bool _NAME_##FromJson(JSON* pJson)\
	{\
		JSON* pJsonOBJ = pJson->GetItemByName(#_NAME_); \
		if (pJsonOBJ) \
		{\
		JSON* pJsonOBJ_X = pJsonOBJ->GetItemByName("x"); \
		JSON* pJsonOBJ_Y = pJsonOBJ->GetItemByName("y"); \
		if (pJsonOBJ_X && pJsonOBJ_Y && pJsonOBJ_Z)\
			{\
			_PREV_NAME_##_NAME_.x = pJsonOBJ_X->GetDoubleValue(); \
			_PREV_NAME_##_NAME_.y = pJsonOBJ_Y->GetDoubleValue(); \
			return true; \
			}\
		}; \
		return false; \
	};

#define JSON_IO_V3( _PREV_NAME_ , _NAME_ )\
	virtual void _NAME_##ToJson(JSON* pJson){ JSON* pJsonOBJ = pJson->CreateArray(); pJsonOBJ->AddArrayNumber(_PREV_NAME_##_NAME_.x); pJsonOBJ->AddArrayNumber(_PREV_NAME_##_NAME_.y); pJsonOBJ->AddArrayNumber( _PREV_NAME_##_NAME_.z); pJson->AddItem(#_NAME_, pJsonOBJ); }; \
	virtual bool _NAME_##FromJson(JSON* pJson)\
	{\
		JSON* pJsonOBJ = pJson->GetItemByName(#_NAME_); \
		if (pJsonOBJ && pJsonOBJ->Type == JSON_Array&& pJsonOBJ->GetArraySize() == 3) \
		{\
		_PREV_NAME_##_NAME_.x = pJsonOBJ->GetArrayNumber(0); \
		_PREV_NAME_##_NAME_.y = pJsonOBJ->GetArrayNumber(1); \
		_PREV_NAME_##_NAME_.z = pJsonOBJ->GetArrayNumber(2); \
			return true; \
		} \
		return false; \
	};


#define JSON_IO_V4(_PREV_NAME_ , _NAME_ )\
	virtual void _NAME_##ToJson(JSON* pJson){ JSON* pJsonOBJ = pJson->CreateArray(); pJsonOBJ->AddArrayNumber(_PREV_NAME_##_NAME_.x); pJsonOBJ->AddArrayNumber(_PREV_NAME_##_NAME_.y); pJsonOBJ->AddArrayNumber(_PREV_NAME_##_NAME_.z); pJsonOBJ->AddArrayNumber(_PREV_NAME_##_NAME_.w); pJson->AddItem(#_NAME_, pJsonOBJ); }; \
	virtual bool _NAME_##FromJson(JSON* pJson)\
	{\
		JSON* pJsonOBJ = pJson->GetItemByName(#_NAME_); \
		if (pJsonOBJ && pJsonOBJ->Type == JSON_Array&& pJsonOBJ->GetArraySize() == 4) \
		{\
			_PREV_NAME_##_NAME_.x = pJsonOBJ->GetArrayNumber(0); \
			_PREV_NAME_##_NAME_.y = pJsonOBJ->GetArrayNumber(1); \
			_PREV_NAME_##_NAME_.z = pJsonOBJ->GetArrayNumber(2); \
			_PREV_NAME_##_NAME_.w = pJsonOBJ->GetArrayNumber(3); \
			return true; \
		}; \
		return false; \
	};


#define JSON_IO_M4(_PREV_NAME_ , _NAME_ )\
	virtual void _NAME_##ToJson(JSON* pJson)\
	{\
		JSON* pJsonOBJ = pJson->CreateArray(); \
		int i = 0, j; \
		while (i < 4)\
		{\
			j = 0; \
			JSON* pArray = pJsonOBJ->CreateArray(); \
			while (j < 4)\
			{\
				pArray->AddArrayNumber(_PREV_NAME_##_NAME_.M[i][j]); \
				j++; \
			}\
			pJsonOBJ->AddArrayElement(pArray); \
			i++; \
		}\
		pJson->AddItem(#_NAME_, pJsonOBJ); \
	}\
	virtual bool _NAME_##FromJson(JSON* pJson)\
	{\
		JSON* pJsonOBJ = pJson->GetItemByName(#_NAME_); \
		if (pJsonOBJ)\
		{\
			int i = 0, j; \
			while (i < 4)\
			{\
				j = 0; \
				JSON* pArray = pJsonOBJ->GetItemByIndex(i); \
				while (j < 4)\
				{\
				_PREV_NAME_##_NAME_.M[i][j] = pArray->GetArrayNumber(j); \
					j++; \
				}\
				i++;\
			}\
			return true;\
		}\
		return  false;\
	}




#define JSON_IO_STR( _PREV_NAME_ , _NAME_ )\
	virtual void _NAME_##ToJson(JSON* pJson){ pJson->AddStringItem(#_NAME_, _PREV_NAME_##_NAME_); }; \
	virtual bool _NAME_##FromJson(JSON* pJson)\
	{\
		JSON* pSubJson = pJson->GetItemByName(#_NAME_); \
		if (pSubJson) \
		{\
			Set##_NAME_(pSubJson->GetStringValue()); \
			return true; \
		}; \
		return false; \
	}; 


#define CLASS_MEMBER(_TYPE_ , _PREV_NAME_ , _NAME_ )\
	protected:\
	_TYPE_ _PREV_NAME_##_NAME_;\
	public:\
	virtual void Set##_NAME_(_TYPE_ V){ _PREV_NAME_##_NAME_ = V; };\
	virtual _TYPE_ Get##_NAME_()const{ return _PREV_NAME_##_NAME_; };

#define CLASS_MEMBER_REF(_TYPE_ , _PREV_NAME_ , _NAME_ )\
	protected:\
	_TYPE_ _PREV_NAME_##_NAME_; \
	public:\
	virtual _TYPE_ & Get##_NAME_(){ return _PREV_NAME_##_NAME_; };

#define CLASS_MEMBER_STR(_TYPE_ , _PREV_NAME_ , _NAME_ )\
	protected:\
	_TYPE_ _PREV_NAME_##_NAME_; \
	public:\
	virtual void Set##_NAME_(_TYPE_ V){ _PREV_NAME_##_NAME_ = V; }; \
	virtual void Set##_NAME_(const char*  V){ _PREV_NAME_##_NAME_ = ((V!=NULL) ? (V) : ""); }; \
	virtual const char* Get##_NAME_()const{ return _PREV_NAME_##_NAME_; };

#define CLASS_MEMBER_ARRAY(_TYPE_ , _PREV_NAME_ , _NAME_ , _SIZE_)\
	protected:\
	_TYPE_ _PREV_NAME_##_NAME_[_SIZE_]; \
	public:\
	virtual void Set##_NAME_(_TYPE_ V){ memcpy(_PREV_NAME_##_NAME_, V, sizeof(_TYPE_) * _SIZE_); }; \
	virtual _TYPE_ Get##_NAME_(int index)const{ return _PREV_NAME_##_NAME_[index]; };

#define CLASS_INTERFACE(_TYPE_ , _PREV_NAME_ , _NAME_ )\
	protected:\
	_TYPE_ _PREV_NAME_##_NAME_; \
	virtual void Set##_NAME_(_TYPE_ V){ _PREV_NAME_##_NAME_ = V; }; \
	public:\
	virtual _TYPE_ Get##_NAME_()const{ return _PREV_NAME_##_NAME_; };

#define CLASS_INTERFACE_STR(_TYPE_ , _PREV_NAME_ , _NAME_ )\
	protected:\
	_TYPE_ _PREV_NAME_##_NAME_; \
	virtual void Set##_NAME_(_TYPE_ V){ _PREV_NAME_##_NAME_ = V; }; \
	virtual void Set##_NAME_(const char*  V){ _PREV_NAME_##_NAME_ = V; }; \
	public:\
	virtual const char* Get##_NAME_()const{ return _PREV_NAME_##_NAME_; };


#define CLASS_MEMBER_J(_TYPE_ , _PREV_NAME_ , _NAME_ )\
	CLASS_MEMBER(_TYPE_, _PREV_NAME_, _NAME_)\
	JSON_IO(_TYPE_, _PREV_NAME_, _NAME_)

#define CLASS_MEMBER_V2_J(_TYPE_ , _PREV_NAME_ , _NAME_ )\
	CLASS_MEMBER(_TYPE_, _PREV_NAME_, _NAME_)\
	JSON_IO_V2(_PREV_NAME_, _NAME_)
	
#define CLASS_MEMBER_V3_J(_TYPE_ , _PREV_NAME_ , _NAME_ )\
	CLASS_MEMBER(_TYPE_, _PREV_NAME_, _NAME_)\
	JSON_IO_V3(_PREV_NAME_, _NAME_)

#define CLASS_MEMBER_V4_J(_TYPE_ , _PREV_NAME_ , _NAME_ )\
	CLASS_MEMBER(_TYPE_, _PREV_NAME_, _NAME_)\
	JSON_IO_V4(_PREV_NAME_, _NAME_)

#define CLASS_MEMBER_M4_J(_TYPE_ , _PREV_NAME_ , _NAME_ )\
	CLASS_MEMBER(_TYPE_, _PREV_NAME_, _NAME_)\
	JSON_IO_M4(_PREV_NAME_, _NAME_)

#define CLASS_MEMBER_STR_J(_TYPE_ , _PREV_NAME_ , _NAME_ )\
	CLASS_MEMBER_STR(_TYPE_, _PREV_NAME_, _NAME_)\
	JSON_IO_STR( _PREV_NAME_, _NAME_)

#define CLASS_INTERFACE_J(_TYPE_ , _PREV_NAME_ , _NAME_ )\
	CLASS_INTERFACE(_TYPE_, _PREV_NAME_, _NAME_)\
	JSON_IO(_TYPE_, _PREV_NAME_, _NAME_)

#define CLASS_INTERFACE_STR_J(_TYPE_ , _PREV_NAME_ , _NAME_ )\
	CLASS_INTERFACE_STR(_TYPE_, _PREV_NAME_, _NAME_)\
	JSON_IO_STR( _PREV_NAME_, _NAME_)
// #ifndef max
// #define max(a,b) (((a) > (b)) ? (a) : (b))
// #endif // max
// #ifndef min
// #define min(a,b) (((a) < (b)) ? (a) : (b))
// #endif

enum EyeTextureType
{
	TEXTURE_OPEN_GL = 0,
	TEXTURE_LEFT_EYE = 1,
	TEXTURE_RIGHT_EYE = 2,
	TEXTURE_BOTH_EYE = TEXTURE_LEFT_EYE | TEXTURE_RIGHT_EYE,
	TEXTURE_IS_UNITY_METAL_RENDER_TEXTURE = 4
};

#endif  // MJ_Types_h
