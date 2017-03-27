#ifndef MJ_Std_h
#define MJ_Std_h

#include "MojingTypes.h"
#include <stdarg.h> // for va_list args
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#if !defined(MJ_OS_WINCE) && defined(MJ_CC_MSVC) && (MJ_CC_MSVC >= 1400)
#define MJ_MSVC_SAFESTRING
#include <errno.h>
#endif

// Wide-char funcs
#include <wchar.h>
#include <wctype.h>

namespace Baofeng
{
	namespace Mojing
	{

#if defined(MJ_OS_WIN32)
		inline char* MJ_CDECL MJ_itoa(int val, char *dest, UPInt destsize, int radix)
		{
#if defined(MJ_MSVC_SAFESTRING)
			_itoa_s(val, dest, destsize, radix);
			return dest;
#else
			MJ_UNUSED(destsize);
			return itoa(val, dest, radix);
#endif
		}
#else // MJ_OS_WIN32
		inline char* MJ_itoa(int val, char* dest, unsigned int len, int radix)
		{
			if (val == 0)
			{
				if (len > 1)
				{
					dest[0] = '0';
					dest[1] = '\0';  
				}
				return dest;
			}

			int cur = val;
			unsigned int i    = 0; 
			unsigned int sign = 0;

			if (val < 0)
			{
				val = -val;
				sign = 1;
			}

			while ((val != 0) && (i < (len - 1 - sign)))        
			{
				cur    = val % radix;
				val   /= radix;

				if (radix == 16)
				{
					switch(cur)
					{
					case 10:
						dest[i] = 'a';
						break;
					case 11:
						dest[i] = 'b';
						break;
					case 12:
						dest[i] = 'c';
						break;
					case 13:
						dest[i] = 'd';
						break;
					case 14:
						dest[i] = 'e';
						break;
					case 15:
						dest[i] = 'f';
						break;
					default:
						dest[i] = (char)('0' + cur);
						break;
					}
				} 
				else
				{
					dest[i] = (char)('0' + cur);
				}
				++i;
			}

			if (sign)
			{
				dest[i++] = '-';
			}

			for (unsigned int j = 0; j < i / 2; ++j)
			{
				char tmp        = dest[j];
				dest[j]         = dest[i - 1 - j];
				dest[i - 1 - j] = tmp;
			}
			dest[i] = '\0';

			return dest;
		}

#endif


		// String functions

		inline UPInt MJ_CDECL MJ_strlen(const char* str)
		{
			return strlen(str);
		}

		inline char* MJ_CDECL MJ_strcpy(char* dest, UPInt destsize, const char* src)
		{
#if defined(MJ_MSVC_SAFESTRING)
			strcpy_s(dest, destsize, src);
			return dest;
#elif defined(MJ_OS_ANDROID)
			strlcpy( dest, src, destsize );
			return dest;
#else
			MJ_UNUSED(destsize);
			return strcpy(dest, src);
#endif
		}

		inline char* MJ_CDECL MJ_strncpy(char* dest, UPInt destsize, const char* src, UPInt count)
		{
			// handle an invalid destination the same way for all cases
			if (destsize == 0 || dest == NULL || src == NULL)
			{
				MJ_ASSERT(dest != NULL && destsize > 0 && src != NULL);
				return dest;
			}
			if (dest == src)
			{
				return dest;
			}
			// special case if count is 0, just set the first character to null byte
			if (count == 0)
			{
				dest[0] = '\0';
				return dest;
			}

			if (count >= destsize)
			{
				// if count is larger than or equal to the destination buffer size, completely fill the 
				// destination buffer and put the zero byte at the end 
				strncpy(dest, src, destsize);
				dest[destsize - 1] = '\0';
			}
			else
			{
				// otherwise place it right after the count
				strncpy(dest, src, count);
				dest[count] = '\0';
			}
			return dest;
		}

		inline char * MJ_CDECL MJ_strcat(char* dest, UPInt destsize, const char* src)
		{
#if defined(MJ_MSVC_SAFESTRING)
			strcat_s(dest, destsize, src);
			return dest;
#elif defined(MJ_OS_ANDROID)
			strlcat( dest, src, destsize );
			return dest;
#else
			MJ_UNUSED(destsize);
			return strcat(dest, src);
#endif
		}

		inline int MJ_CDECL MJ_strcmp(const char* dest, const char* src)
		{
			return strcmp(dest, src);
		}

		inline const char* MJ_CDECL MJ_strchr(const char* str, char c)
		{
			return strchr(str, c);
		}

		inline char* MJ_CDECL MJ_strchr(char* str, char c)
		{
			return strchr(str, c);
		}

		inline const char* MJ_strrchr(const char* str, char c)
		{
			UPInt len = MJ_strlen(str);
			for (UPInt i = len; i > 0; i--)
			if (str[i] == c)
				return str + i;
			return 0;
		}

		inline const UByte* MJ_CDECL MJ_memrchr(const UByte* str, UPInt size, UByte c)
		{
			for (SPInt i = (SPInt)size - 1; i >= 0; i--)
			{
				if (str[i] == c)
					return str + i;
			}
			return 0;
		}

		inline char* MJ_CDECL MJ_strrchr(char* str, char c)
		{
			UPInt len = MJ_strlen(str);
			for (UPInt i = len; i > 0; i--)
			if (str[i] == c)
				return str + i;
			return 0;
		}


		double MJ_CDECL MJ_strtod(const char* string, char** tailptr);

		inline long MJ_CDECL MJ_strtol(const char* string, char** tailptr, int radix)
		{
			return strtol(string, tailptr, radix);
		}

		inline long MJ_CDECL MJ_strtoul(const char* string, char** tailptr, int radix)
		{
			return strtoul(string, tailptr, radix);
		}

		inline int MJ_CDECL MJ_strncmp(const char* ws1, const char* ws2, UPInt size)
		{
			return strncmp(ws1, ws2, size);
		}

		inline UInt64 MJ_CDECL MJ_strtouq(const char *nptr, char **endptr, int base)
		{
#if defined(MJ_CC_MSVC) && !defined(MJ_OS_WINCE)
			return _strtoui64(nptr, endptr, base);
#else
			return strtoull(nptr, endptr, base);
#endif
		}

		inline SInt64 MJ_CDECL MJ_strtoq(const char *nptr, char **endptr, int base)
		{
#if defined(MJ_CC_MSVC) && !defined(MJ_OS_WINCE)
			return _strtoi64(nptr, endptr, base);
#else
			return strtoll(nptr, endptr, base);
#endif
		}


		inline SInt64 MJ_CDECL MJ_atoq(const char* string)
		{
#if defined(MJ_CC_MSVC) && !defined(MJ_OS_WINCE)
			return _atoi64(string);
#else
			return atoll(string);
#endif
		}

		inline UInt64 MJ_CDECL MJ_atouq(const char* string)
		{
			return MJ_strtouq(string, NULL, 10);
		}


		// Implemented in GStd.cpp in platform-specific manner.
		int MJ_CDECL MJ_stricmp(const char* dest, const char* src);
		int MJ_CDECL MJ_strnicmp(const char* dest, const char* src, UPInt count);

		inline int MJ_CDECL MJ_sprintf(char *dest, UPInt destsize, const char* format, ...)
		{
			if (destsize <= 0 || dest == NULL)
			{
				MJ_ASSERT(destsize > 0);
				return -1;
			}
			va_list argList;
			va_start(argList, format);
			int ret;
#if defined(MJ_CC_MSVC)
#if defined(MJ_MSVC_SAFESTRING)
			ret = _vsnprintf_s(dest, destsize, _TRUNCATE, format, argList);
#else
			// FIXME: this is a security issue on Windows platforms that don't have _vsnprintf_s
			MJ_UNUSED(destsize);
			ret = _vsnprintf(dest, destsize - 1, format, argList); // -1 for space for the null character
			dest[destsize-1] = 0;	// may leave trash in the destination...
#endif
#else
			ret = vsnprintf( dest, destsize, format, argList );
			// In the event of the output string being greater than the buffer size, vsnprintf should 
			// return the size of the string before truncation. In that case we zero-terminate the
			// string to ensure that the result is the same as _vsnprintf_s would return for the
			// MSVC compiler. vsnprintf is supposed to zero-terminate in this case, but to be sure
			// we zero-terminate it ourselves. 
			if ( ret >= (int)destsize )
			{
				dest[destsize - 1] = '\0';
			}
#endif
			// If an error occurs, vsnprintf should return -1, in which case we set zero byte to null character.
			MJ_ASSERT(ret >= 0);	// ensure the format string is not malformed
			if (ret < 0)
			{
				dest[0] = '\0';
			}
			va_end(argList);
			return ret;
		}

		inline UPInt MJ_CDECL MJ_vsprintf(char *dest, UPInt destsize, const char * format, va_list argList)
		{
			UPInt ret;
#if defined(MJ_CC_MSVC)
#if defined(MJ_MSVC_SAFESTRING)
			dest[0] = '\0';
			int rv = vsnprintf_s(dest, destsize, _TRUNCATE, format, argList);
			if (rv == -1)
			{
				dest[destsize - 1] = '\0';
				ret = destsize - 1;
			}
			else
				ret = (UPInt)rv;
#else
			MJ_UNUSED(destsize);
			int rv = _vsnprintf(dest, destsize - 1, format, argList);
			MJ_ASSERT(rv != -1);
			ret = (UPInt)rv;
			dest[destsize-1] = 0;
#endif
#else
			MJ_UNUSED(destsize);
			ret = (UPInt)vsprintf(dest, format, argList);
			MJ_ASSERT(ret < destsize);
#endif
			return ret;
		}

		// Returns the number of characters in the formatted string.
		inline UPInt MJ_CDECL MJ_vscprintf(const char * format, va_list argList)
		{
			UPInt ret;
#if defined(MJ_CC_MSVC)
			ret = (UPInt)_vscprintf(format, argList);
#else    
			ret = (UPInt) vsnprintf(NULL, 0, format, argList);
#endif
			return ret;
		}


		wchar_t* MJ_CDECL MJ_wcscpy(wchar_t* dest, UPInt destsize, const wchar_t* src);
		wchar_t* MJ_CDECL MJ_wcsncpy(wchar_t* dest, UPInt destsize, const wchar_t* src, UPInt count);
		wchar_t* MJ_CDECL MJ_wcscat(wchar_t* dest, UPInt destsize, const wchar_t* src);
		UPInt    MJ_CDECL MJ_wcslen(const wchar_t* str);
		int      MJ_CDECL MJ_wcscmp(const wchar_t* a, const wchar_t* b);
		int      MJ_CDECL MJ_wcsicmp(const wchar_t* a, const wchar_t* b);

		inline int MJ_CDECL MJ_wcsicoll(const wchar_t* a, const wchar_t* b)
		{
#if defined(MJ_OS_WIN32)
#if defined(MJ_CC_MSVC) && (MJ_CC_MSVC >= 1400)
			return ::_wcsicoll(a, b);
#else
			return ::wcsicoll(a, b);
#endif
#else
			// not supported, use regular wcsicmp
			return MJ_wcsicmp(a, b);
#endif
		}

		inline int MJ_CDECL MJ_wcscoll(const wchar_t* a, const wchar_t* b)
		{
#if defined(MJ_OS_WIN32) || defined(MJ_OS_LINUX)
			return wcscoll(a, b);
#else
			// not supported, use regular wcscmp
			return MJ_wcscmp(a, b);
#endif
		}

#ifndef MJ_NO_WCTYPE

		inline int MJ_CDECL UnicodeCharIs(const UInt16* table, wchar_t charCode)
		{
			unsigned offset = table[charCode >> 8];
			if (offset == 0) return 0;
			if (offset == 1) return 1;
			return (table[offset + ((charCode >> 4) & 15)] & (1 << (charCode & 15))) != 0;
		}

		extern const UInt16 UnicodeAlnumBits[];
		extern const UInt16 UnicodeAlphaBits[];
		extern const UInt16 UnicodeDigitBits[];
		extern const UInt16 UnicodeSpaceBits[];
		extern const UInt16 UnicodeXDigitBits[];

		// Uncomment if necessary
		//extern const UInt16 UnicodeCntrlBits[];
		//extern const UInt16 UnicodeGraphBits[];
		//extern const UInt16 UnicodeLowerBits[];
		//extern const UInt16 UnicodePrintBits[];
		//extern const UInt16 UnicodePunctBits[];
		//extern const UInt16 UnicodeUpperBits[];

		inline int MJ_CDECL MJ_iswalnum(wchar_t charCode) { return UnicodeCharIs(UnicodeAlnumBits, charCode); }
		inline int MJ_CDECL MJ_iswalpha(wchar_t charCode) { return UnicodeCharIs(UnicodeAlphaBits, charCode); }
		inline int MJ_CDECL MJ_iswdigit(wchar_t charCode) { return UnicodeCharIs(UnicodeDigitBits, charCode); }
		inline int MJ_CDECL MJ_iswspace(wchar_t charCode) { return UnicodeCharIs(UnicodeSpaceBits, charCode); }
		inline int MJ_CDECL MJ_iswxdigit(wchar_t charCode) { return UnicodeCharIs(UnicodeXDigitBits, charCode); }

		// Uncomment if necessary
		//inline int MJ_CDECL MJ_iswcntrl (wchar_t charCode) { return UnicodeCharIs(UnicodeCntrlBits,  charCode); }
		//inline int MJ_CDECL MJ_iswgraph (wchar_t charCode) { return UnicodeCharIs(UnicodeGraphBits,  charCode); }
		//inline int MJ_CDECL MJ_iswlower (wchar_t charCode) { return UnicodeCharIs(UnicodeLowerBits,  charCode); }
		//inline int MJ_CDECL MJ_iswprint (wchar_t charCode) { return UnicodeCharIs(UnicodePrintBits,  charCode); }
		//inline int MJ_CDECL MJ_iswpunct (wchar_t charCode) { return UnicodeCharIs(UnicodePunctBits,  charCode); }
		//inline int MJ_CDECL MJ_iswupper (wchar_t charCode) { return UnicodeCharIs(UnicodeUpperBits,  charCode); }

		int MJ_CDECL MJ_towupper(wchar_t charCode);
		int MJ_CDECL MJ_towlower(wchar_t charCode);

#else // MJ_NO_WCTYPE

		inline int MJ_CDECL MJ_iswspace(wchar_t c)
		{
			return iswspace(c);
		}

		inline int MJ_CDECL MJ_iswdigit(wchar_t c)
		{
			return iswdigit(c);
		}

		inline int MJ_CDECL MJ_iswxdigit(wchar_t c)
		{
			return iswxdigit(c);
		}

		inline int MJ_CDECL MJ_iswalpha(wchar_t c)
		{
			return iswalpha(c);
		}

		inline int MJ_CDECL MJ_iswalnum(wchar_t c)
		{
			return iswalnum(c);
		}

		inline wchar_t MJ_CDECL MJ_towlower(wchar_t c)
		{
			return (wchar_t)towlower(c);
		}

		inline wchar_t MJ_towupper(wchar_t c)
		{
			return (wchar_t)towupper(c);
		}

#endif // MJ_NO_WCTYPE

		// ASCII versions of tolower and toupper. Don't use "char"
		inline int MJ_CDECL MJ_tolower(int c)
		{
			return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c;
		}

		inline int MJ_CDECL MJ_toupper(int c)
		{
			return (c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c;
		}



		inline double MJ_CDECL MJ_wcstod(const wchar_t* string, wchar_t** tailptr)
		{
#if defined(MJ_OS_OTHER)
			MJ_UNUSED(tailptr);
			char buffer[64];
			char* tp = NULL;
			UPInt max = MJ_wcslen(string);
			if (max > 63) max = 63;
			unsigned char c = 0;
			for (UPInt i=0; i < max; i++)
			{
				c = (unsigned char)string[i];
				buffer[i] = ((c) < 128 ? (char)c : '!');
			}
			buffer[max] = 0;
			return MJ_strtod(buffer, &tp);
#else
			return wcstod(string, tailptr);
#endif
		}

		inline long MJ_CDECL MJ_wcstol(const wchar_t* string, wchar_t** tailptr, int radix)
		{
#if defined(MJ_OS_OTHER)
			MJ_UNUSED(tailptr);
			char buffer[64];
			char* tp = NULL;
			UPInt max = MJ_wcslen(string);
			if (max > 63) max = 63;
			unsigned char c = 0;
			for (UPInt i=0; i < max; i++)
			{
				c = (unsigned char)string[i];
				buffer[i] = ((c) < 128 ? (char)c : '!');
			}
			buffer[max] = 0;
			return strtol(buffer, &tp, radix);
#else
			return wcstol(string, tailptr, radix);
#endif
		}
	}
} // OVR

#endif // MJ_Std_h
