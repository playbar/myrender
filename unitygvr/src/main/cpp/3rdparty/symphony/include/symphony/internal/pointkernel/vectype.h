/*C/C++ implementations of OpenCL vector types*/

#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4244) // possible loss of data
#endif // _MSC_VER

#include<cmath>

namespace symphony {
SYMPHONY_GCC_IGNORE_BEGIN("-Weffc++");
// scalar typedef
#if defined(_MSC_VER)
  typedef signed   __int8         cl_char;
  typedef unsigned __int8         cl_uchar;
  typedef signed   __int16        cl_short;
  typedef unsigned __int16        cl_ushort;
  typedef signed   __int32        cl_int;
  typedef unsigned __int32        cl_uint;
  typedef signed   __int64        cl_long;
  typedef unsigned __int64        cl_ulong;

  typedef unsigned __int16        cl_half;
  typedef float                   cl_float;
  typedef double                  cl_double;
#else
  typedef int8_t          cl_char;
  typedef uint8_t         cl_uchar;
  typedef int16_t         cl_short    __attribute__((aligned(2)));
  typedef uint16_t        cl_ushort   __attribute__((aligned(2)));
  typedef int32_t         cl_int      __attribute__((aligned(4)));
  typedef uint32_t        cl_uint     __attribute__((aligned(4)));
  typedef int64_t         cl_long     __attribute__((aligned(8)));
  typedef uint64_t        cl_ulong    __attribute__((aligned(8)));

  typedef uint16_t        cl_half     __attribute__((aligned(2)));
  typedef float           cl_float    __attribute__((aligned(4)));
  typedef double          cl_double   __attribute__((aligned(8)));
#endif
// Need CL_ALIGNED defined in CL header
#if !defined(SYMPHONY_HAVE_OPENCL)
#if defined( __GNUC__ )
  #define CL_ALIGNED(_x)          __attribute__ ((aligned(_x)))
#elif defined( _WIN32) && (_MSC_VER)
  /* Alignment keys neutered on windows because MSVC cannot swallow function arguments with alignment requirements    */
  /* http://msdn.microsoft.com/en-us/library/373ak2y1%%28VS.71%%29.aspx                                               */
  /* #include <crtdefs.h>                                                                                             */
  /* #define CL_ALIGNED(_x)          _CRT_ALIGN(_x)                                                                   */
  #define CL_ALIGNED(_x)
#else
   #warning  Need to implement some method to align data here
   #define  CL_ALIGNED(_x)
#endif
#endif // !defined(SYMPHONY_HAVE_OPENCL)

inline float dot(float p, float q)
{ return p * q; }

inline float distance(float p, float q)
{ return p - q; }

inline float length(float p)
{ return fabs(p); }

inline float hypot(float p, float q)
{ return sqrt(p * p + q * q); }

inline float normalize(float p)
{
  float l = length(p);
  if (fabs(l-0.0) < 1e-6) {
    SYMPHONY_FATAL("Cannot normalize a zero-vector!");
  }

  return p / l;
}

inline double dot(double p, double q)
{ return p * q; }

inline double distance(double p, double q)
{ return p - q; }

inline double length(double p)
{ return fabs(p); }

inline double hypot(double p, double q)
{ return sqrt(p * p + q * q); }

inline double normalize(double p)
{
  double l = length(p);
  if (fabs(l-0.0) < 1e-6) {
    SYMPHONY_FATAL("Cannot normalize a zero-vector!");
  }

  return p / l;
}

class char2{
public:
  static const size_t N = 2; // Number of elements in vector
  union {
	 cl_char CL_ALIGNED(2) s[2];
#if defined(__GNUC__)
    __extension__ struct { cl_char x, y; };
    __extension__ struct { cl_char s0, s1; };
#endif
  };

  char2(){}

  char2(cl_char i0,cl_char i1){
    s[0] = i0;
    s[1] = i1;

  }

  char2(cl_char i0) {
  for (int i = 0; i < 2; ++i) {
		s[i] = i0;
	 }
  }

  char2(const char2& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
  }

  char2& operator+=(const char2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  char2& operator+=(cl_char rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  char2& operator-=(const char2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  char2& operator-=(cl_char rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  char2& operator*=(const char2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  char2& operator*=(cl_char rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  char2& operator/=(const char2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  char2& operator/=(cl_char rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  char2& operator%=(const char2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  char2& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  char2& operator&=(const char2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  char2& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  char2& operator|=(const char2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  char2& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  char2& operator^=(const char2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  char2& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  char2& operator<<=(const char2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  char2& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  char2& operator>>=(const char2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  char2& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline char2 operator+(char2 lhs, const char2& rhs)
{
  lhs += rhs;
  return lhs;
}

inline char2 operator+(char2 lhs, cl_char rhs)
{
  lhs += rhs;
  return lhs;
}

inline char2 operator+(cl_char lhs, const char2& rhs)
{
  auto tmp = char2(lhs);
  return tmp + rhs;
}

inline char2 operator-(char2 lhs, const char2& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline char2 operator-(char2 lhs, cl_char rhs)
{
  lhs -= rhs;
  return lhs;
}

inline char2 operator-(cl_char lhs, const char2& rhs)
{
  auto tmp = char2(lhs);
  return tmp - rhs;
}

inline char2 operator*(char2 lhs, const char2& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline char2 operator*(char2 lhs, cl_char rhs)
{
  lhs *= rhs;
  return lhs;
}

inline char2 operator*(cl_char lhs, const char2& rhs)
{
  auto tmp = char2(lhs);
  return tmp * rhs;
}

inline char2 operator/(char2 lhs, const char2& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline char2 operator/(char2 lhs, cl_char rhs)
{
  lhs /= rhs;
  return lhs;
}

inline char2 operator/(cl_char lhs, const char2& rhs)
{
  auto tmp = char2(lhs);
  return tmp / rhs;
}

inline char2 operator%(char2 lhs, const char2& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline char2 operator%(char2 lhs, cl_char rhs)
{
  lhs %= rhs;
  return lhs;
}

inline char2 operator%(cl_char lhs, const char2& rhs)
{
  auto tmp = char2(lhs);
  return tmp % rhs;
}

inline char2 operator&(char2 lhs, const char2& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline char2 operator&(char2 lhs, cl_char rhs)
{
  lhs &= rhs;
  return lhs;
}

inline char2 operator&(cl_char lhs, const char2& rhs)
{
  auto tmp = char2(lhs);
  return tmp & rhs;
}

inline char2 operator|(char2 lhs, const char2& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline char2 operator|(char2 lhs, cl_char rhs)
{
  lhs |= rhs;
  return lhs;
}

inline char2 operator|(cl_char lhs, const char2& rhs)
{
  auto tmp = char2(lhs);
  return tmp | rhs;
}

inline char2 operator^(char2 lhs, const char2& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline char2 operator^(char2 lhs, cl_char rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline char2 operator^(cl_char lhs, const char2& rhs)
{
  auto tmp = char2(lhs);
  return tmp ^ rhs;
}

inline char2 operator<<(char2 lhs, const char2& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline char2 operator<<(char2 lhs, cl_char rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline char2 operator<<(cl_char lhs, const char2& rhs)
{
  auto tmp = char2(lhs);
  return tmp << rhs;
}

inline char2 operator>>(char2 lhs, const char2& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline char2 operator>>(char2 lhs, cl_char rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline char2 operator>>(cl_char lhs, const char2& rhs)
{
  auto tmp = char2(lhs);
  return tmp >> rhs;
}

inline bool operator==(const char2& lhs, const char2& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]);
}

inline bool operator==(const char2& lhs, const cl_char& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs);
}

inline bool operator<(const char2& lhs, const char2& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const char2& lhs, const cl_char& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const char2& lhs, const char2& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const char2& lhs, const cl_char& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const char2& lhs, const char2& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const char2& lhs, const cl_char& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const char2& lhs, const char2& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const char2& lhs, const cl_char& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const char2& lhs, const char2& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const char2& lhs, const cl_char& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline char2 vload2(size_t offset, const char2* p)
{
  return *(p + offset);
}

inline void vstore2(char2 data, size_t offset, char2* p)
{
  *(p + offset) = data;
}


class char3{
public:
  static const size_t N = 4; // Number of elements in vector
  union {
	 cl_char CL_ALIGNED(4) s[4];
#if defined(__GNUC__)
    __extension__ struct { cl_char x, y, z, w; };
    __extension__ struct { cl_char s0, s1, s2, s3; };
#endif
  };

  char3(){}

  char3(cl_char i0,cl_char i1,cl_char i2){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;

  }

  char3(cl_char i0) {
  for (int i = 0; i < 3; ++i) {
		s[i] = i0;
	 }
  }

  char3(const char3& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
  }

  char3& operator+=(const char3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  char3& operator+=(cl_char rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  char3& operator-=(const char3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  char3& operator-=(cl_char rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  char3& operator*=(const char3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  char3& operator*=(cl_char rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  char3& operator/=(const char3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  char3& operator/=(cl_char rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  char3& operator%=(const char3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  char3& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  char3& operator&=(const char3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  char3& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  char3& operator|=(const char3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  char3& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  char3& operator^=(const char3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  char3& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  char3& operator<<=(const char3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  char3& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  char3& operator>>=(const char3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  char3& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline char3 operator+(char3 lhs, const char3& rhs)
{
  lhs += rhs;
  return lhs;
}

inline char3 operator+(char3 lhs, cl_char rhs)
{
  lhs += rhs;
  return lhs;
}

inline char3 operator+(cl_char lhs, const char3& rhs)
{
  auto tmp = char3(lhs);
  return tmp + rhs;
}

inline char3 operator-(char3 lhs, const char3& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline char3 operator-(char3 lhs, cl_char rhs)
{
  lhs -= rhs;
  return lhs;
}

inline char3 operator-(cl_char lhs, const char3& rhs)
{
  auto tmp = char3(lhs);
  return tmp - rhs;
}

inline char3 operator*(char3 lhs, const char3& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline char3 operator*(char3 lhs, cl_char rhs)
{
  lhs *= rhs;
  return lhs;
}

inline char3 operator*(cl_char lhs, const char3& rhs)
{
  auto tmp = char3(lhs);
  return tmp * rhs;
}

inline char3 operator/(char3 lhs, const char3& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline char3 operator/(char3 lhs, cl_char rhs)
{
  lhs /= rhs;
  return lhs;
}

inline char3 operator/(cl_char lhs, const char3& rhs)
{
  auto tmp = char3(lhs);
  return tmp / rhs;
}

inline char3 operator%(char3 lhs, const char3& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline char3 operator%(char3 lhs, cl_char rhs)
{
  lhs %= rhs;
  return lhs;
}

inline char3 operator%(cl_char lhs, const char3& rhs)
{
  auto tmp = char3(lhs);
  return tmp % rhs;
}

inline char3 operator&(char3 lhs, const char3& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline char3 operator&(char3 lhs, cl_char rhs)
{
  lhs &= rhs;
  return lhs;
}

inline char3 operator&(cl_char lhs, const char3& rhs)
{
  auto tmp = char3(lhs);
  return tmp & rhs;
}

inline char3 operator|(char3 lhs, const char3& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline char3 operator|(char3 lhs, cl_char rhs)
{
  lhs |= rhs;
  return lhs;
}

inline char3 operator|(cl_char lhs, const char3& rhs)
{
  auto tmp = char3(lhs);
  return tmp | rhs;
}

inline char3 operator^(char3 lhs, const char3& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline char3 operator^(char3 lhs, cl_char rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline char3 operator^(cl_char lhs, const char3& rhs)
{
  auto tmp = char3(lhs);
  return tmp ^ rhs;
}

inline char3 operator<<(char3 lhs, const char3& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline char3 operator<<(char3 lhs, cl_char rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline char3 operator<<(cl_char lhs, const char3& rhs)
{
  auto tmp = char3(lhs);
  return tmp << rhs;
}

inline char3 operator>>(char3 lhs, const char3& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline char3 operator>>(char3 lhs, cl_char rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline char3 operator>>(cl_char lhs, const char3& rhs)
{
  auto tmp = char3(lhs);
  return tmp >> rhs;
}

inline bool operator==(const char3& lhs, const char3& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]);
}

inline bool operator==(const char3& lhs, const cl_char& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs);
}

inline bool operator<(const char3& lhs, const char3& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const char3& lhs, const cl_char& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const char3& lhs, const char3& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const char3& lhs, const cl_char& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const char3& lhs, const char3& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const char3& lhs, const cl_char& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const char3& lhs, const char3& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const char3& lhs, const cl_char& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const char3& lhs, const char3& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const char3& lhs, const cl_char& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline char3 vload3(size_t offset, const char3* p)
{
  return *(p + offset);
}

inline void vstore3(char3 data, size_t offset, char3* p)
{
  *(p + offset) = data;
}


class char4{
public:
  static const size_t N = 4; // Number of elements in vector
  union {
	 cl_char CL_ALIGNED(4) s[4];
#if defined(__GNUC__)
    __extension__ struct { cl_char x, y, z, w; };
    __extension__ struct { cl_char s0, s1, s2, s3; };
#endif
  };

  char4(){}

  char4(cl_char i0,cl_char i1,cl_char i2,cl_char i3
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;

  }

  char4(cl_char i0) {
  for (int i = 0; i < 4; ++i) {
		s[i] = i0;
	 }
  }

  char4(const char2& a0, const char2& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
  }

  char4(const char4& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
  }

  char4& operator+=(const char4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  char4& operator+=(cl_char rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  char4& operator-=(const char4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  char4& operator-=(cl_char rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  char4& operator*=(const char4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  char4& operator*=(cl_char rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  char4& operator/=(const char4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  char4& operator/=(cl_char rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  char4& operator%=(const char4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  char4& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  char4& operator&=(const char4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  char4& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  char4& operator|=(const char4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  char4& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  char4& operator^=(const char4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  char4& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  char4& operator<<=(const char4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  char4& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  char4& operator>>=(const char4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  char4& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline char4 operator+(char4 lhs, const char4& rhs)
{
  lhs += rhs;
  return lhs;
}

inline char4 operator+(char4 lhs, cl_char rhs)
{
  lhs += rhs;
  return lhs;
}

inline char4 operator+(cl_char lhs, const char4& rhs)
{
  auto tmp = char4(lhs);
  return tmp + rhs;
}

inline char4 operator-(char4 lhs, const char4& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline char4 operator-(char4 lhs, cl_char rhs)
{
  lhs -= rhs;
  return lhs;
}

inline char4 operator-(cl_char lhs, const char4& rhs)
{
  auto tmp = char4(lhs);
  return tmp - rhs;
}

inline char4 operator*(char4 lhs, const char4& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline char4 operator*(char4 lhs, cl_char rhs)
{
  lhs *= rhs;
  return lhs;
}

inline char4 operator*(cl_char lhs, const char4& rhs)
{
  auto tmp = char4(lhs);
  return tmp * rhs;
}

inline char4 operator/(char4 lhs, const char4& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline char4 operator/(char4 lhs, cl_char rhs)
{
  lhs /= rhs;
  return lhs;
}

inline char4 operator/(cl_char lhs, const char4& rhs)
{
  auto tmp = char4(lhs);
  return tmp / rhs;
}

inline char4 operator%(char4 lhs, const char4& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline char4 operator%(char4 lhs, cl_char rhs)
{
  lhs %= rhs;
  return lhs;
}

inline char4 operator%(cl_char lhs, const char4& rhs)
{
  auto tmp = char4(lhs);
  return tmp % rhs;
}

inline char4 operator&(char4 lhs, const char4& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline char4 operator&(char4 lhs, cl_char rhs)
{
  lhs &= rhs;
  return lhs;
}

inline char4 operator&(cl_char lhs, const char4& rhs)
{
  auto tmp = char4(lhs);
  return tmp & rhs;
}

inline char4 operator|(char4 lhs, const char4& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline char4 operator|(char4 lhs, cl_char rhs)
{
  lhs |= rhs;
  return lhs;
}

inline char4 operator|(cl_char lhs, const char4& rhs)
{
  auto tmp = char4(lhs);
  return tmp | rhs;
}

inline char4 operator^(char4 lhs, const char4& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline char4 operator^(char4 lhs, cl_char rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline char4 operator^(cl_char lhs, const char4& rhs)
{
  auto tmp = char4(lhs);
  return tmp ^ rhs;
}

inline char4 operator<<(char4 lhs, const char4& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline char4 operator<<(char4 lhs, cl_char rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline char4 operator<<(cl_char lhs, const char4& rhs)
{
  auto tmp = char4(lhs);
  return tmp << rhs;
}

inline char4 operator>>(char4 lhs, const char4& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline char4 operator>>(char4 lhs, cl_char rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline char4 operator>>(cl_char lhs, const char4& rhs)
{
  auto tmp = char4(lhs);
  return tmp >> rhs;
}

inline bool operator==(const char4& lhs, const char4& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]);
}

inline bool operator==(const char4& lhs, const cl_char& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs);
}

inline bool operator<(const char4& lhs, const char4& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const char4& lhs, const cl_char& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const char4& lhs, const char4& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const char4& lhs, const cl_char& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const char4& lhs, const char4& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const char4& lhs, const cl_char& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const char4& lhs, const char4& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const char4& lhs, const cl_char& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const char4& lhs, const char4& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const char4& lhs, const cl_char& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline char4 vload4(size_t offset, const char4* p)
{
  return *(p + offset);
}

inline void vstore4(char4 data, size_t offset, char4* p)
{
  *(p + offset) = data;
}


class char8{
public:
  static const size_t N = 8; // Number of elements in vector
  union {
	 cl_char CL_ALIGNED(8) s[8];
#if defined(__GNUC__)
    __extension__ struct { cl_char x, y, z, w; };
    __extension__ struct { cl_char s0, s1, s2, s3, s4, s5, s6, s7; };
#endif
  };

  char8(){}

  char8(cl_char i0,cl_char i1,cl_char i2,cl_char i3,
          cl_char i4,cl_char i5,cl_char i6,cl_char i7
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;
    s[4] = i4;
    s[5] = i5;
    s[6] = i6;
    s[7] = i7;

  }

  char8(cl_char i0) {
  for (int i = 0; i < 8; ++i) {
		s[i] = i0;
	 }
  }

  char8(const char2& a0, const char2& a1, const char2& a2, const char2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
  }

  char8(const char2& a0, const char2& a1, const char4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
  }

  char8(const char2& a0, const char3& a1, const char3& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
  }

  char8(const char2& a0, const char4& a1, const char2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  char8(const char3& a0, const char2& a1, const char3& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
  }

  char8(const char3& a0, const char3& a1, const char2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  char8(const char4& a0, const char2& a1, const char2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  char8(const char4& a0, const char4& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
  }

  char8(const char8& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
  }

  char8& operator+=(const char8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  char8& operator+=(cl_char rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  char8& operator-=(const char8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  char8& operator-=(cl_char rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  char8& operator*=(const char8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  char8& operator*=(cl_char rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  char8& operator/=(const char8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  char8& operator/=(cl_char rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  char8& operator%=(const char8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  char8& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  char8& operator&=(const char8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  char8& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  char8& operator|=(const char8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  char8& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  char8& operator^=(const char8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  char8& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  char8& operator<<=(const char8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  char8& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  char8& operator>>=(const char8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  char8& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline char8 operator+(char8 lhs, const char8& rhs)
{
  lhs += rhs;
  return lhs;
}

inline char8 operator+(char8 lhs, cl_char rhs)
{
  lhs += rhs;
  return lhs;
}

inline char8 operator+(cl_char lhs, const char8& rhs)
{
  auto tmp = char8(lhs);
  return tmp + rhs;
}

inline char8 operator-(char8 lhs, const char8& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline char8 operator-(char8 lhs, cl_char rhs)
{
  lhs -= rhs;
  return lhs;
}

inline char8 operator-(cl_char lhs, const char8& rhs)
{
  auto tmp = char8(lhs);
  return tmp - rhs;
}

inline char8 operator*(char8 lhs, const char8& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline char8 operator*(char8 lhs, cl_char rhs)
{
  lhs *= rhs;
  return lhs;
}

inline char8 operator*(cl_char lhs, const char8& rhs)
{
  auto tmp = char8(lhs);
  return tmp * rhs;
}

inline char8 operator/(char8 lhs, const char8& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline char8 operator/(char8 lhs, cl_char rhs)
{
  lhs /= rhs;
  return lhs;
}

inline char8 operator/(cl_char lhs, const char8& rhs)
{
  auto tmp = char8(lhs);
  return tmp / rhs;
}

inline char8 operator%(char8 lhs, const char8& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline char8 operator%(char8 lhs, cl_char rhs)
{
  lhs %= rhs;
  return lhs;
}

inline char8 operator%(cl_char lhs, const char8& rhs)
{
  auto tmp = char8(lhs);
  return tmp % rhs;
}

inline char8 operator&(char8 lhs, const char8& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline char8 operator&(char8 lhs, cl_char rhs)
{
  lhs &= rhs;
  return lhs;
}

inline char8 operator&(cl_char lhs, const char8& rhs)
{
  auto tmp = char8(lhs);
  return tmp & rhs;
}

inline char8 operator|(char8 lhs, const char8& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline char8 operator|(char8 lhs, cl_char rhs)
{
  lhs |= rhs;
  return lhs;
}

inline char8 operator|(cl_char lhs, const char8& rhs)
{
  auto tmp = char8(lhs);
  return tmp | rhs;
}

inline char8 operator^(char8 lhs, const char8& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline char8 operator^(char8 lhs, cl_char rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline char8 operator^(cl_char lhs, const char8& rhs)
{
  auto tmp = char8(lhs);
  return tmp ^ rhs;
}

inline char8 operator<<(char8 lhs, const char8& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline char8 operator<<(char8 lhs, cl_char rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline char8 operator<<(cl_char lhs, const char8& rhs)
{
  auto tmp = char8(lhs);
  return tmp << rhs;
}

inline char8 operator>>(char8 lhs, const char8& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline char8 operator>>(char8 lhs, cl_char rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline char8 operator>>(cl_char lhs, const char8& rhs)
{
  auto tmp = char8(lhs);
  return tmp >> rhs;
}

inline bool operator==(const char8& lhs, const char8& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]) &&
  (lhs.s[4] == rhs.s[4]) &&
  (lhs.s[5] == rhs.s[5]) &&
  (lhs.s[6] == rhs.s[6]) &&
  (lhs.s[7] == rhs.s[7]);
}

inline bool operator==(const char8& lhs, const cl_char& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs) &&
  (lhs.s[4] == rhs) &&
  (lhs.s[5] == rhs) &&
  (lhs.s[6] == rhs) &&
  (lhs.s[7] == rhs);
}

inline bool operator<(const char8& lhs, const char8& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const char8& lhs, const cl_char& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const char8& lhs, const char8& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const char8& lhs, const cl_char& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const char8& lhs, const char8& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const char8& lhs, const cl_char& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const char8& lhs, const char8& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const char8& lhs, const cl_char& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const char8& lhs, const char8& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const char8& lhs, const cl_char& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline char8 vload8(size_t offset, const char8* p)
{
  return *(p + offset);
}

inline void vstore8(char8 data, size_t offset, char8* p)
{
  *(p + offset) = data;
}


class char16{
public:
  static const size_t N = 16; // Number of elements in vector
  union {
	 cl_char CL_ALIGNED(16) s[16];
#if defined(__GNUC__)
    __extension__ struct { cl_char x, y, z, w; };
    __extension__ struct { cl_char s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15; };
#endif
  };

  char16(){}

  char16(cl_char i0,cl_char i1,cl_char i2,cl_char i3,
          cl_char i4,cl_char i5,cl_char i6,cl_char i7,
          cl_char i8,cl_char i9,cl_char i10,cl_char i11,
          cl_char i12,cl_char i13,cl_char i14,cl_char i15
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;
    s[4] = i4;
    s[5] = i5;
    s[6] = i6;
    s[7] = i7;
    s[8] = i8;
    s[9] = i9;
    s[10] = i10;
    s[11] = i11;
    s[12] = i12;
    s[13] = i13;
    s[14] = i14;
    s[15] = i15;

  }

  char16(cl_char i0) {
  for (int i = 0; i < 16; ++i) {
		s[i] = i0;
	 }
  }

  char16(const char2& a0, const char2& a1, const char2& a2, const char2& a3, const char2& a4, const char2& a5, const char2& a6, const char2& a7) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a6.s[0];
    s[13] = a6.s[1];
    s[14] = a7.s[0];
    s[15] = a7.s[1];
  }

  char16(const char2& a0, const char2& a1, const char2& a2, const char2& a3, const char2& a4, const char2& a5, const char4& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a6.s[0];
    s[13] = a6.s[1];
    s[14] = a6.s[2];
    s[15] = a6.s[3];
  }

  char16(const char2& a0, const char2& a1, const char2& a2, const char2& a3, const char2& a4, const char3& a5, const char3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a5.s[2];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  char16(const char2& a0, const char2& a1, const char2& a2, const char2& a3, const char2& a4, const char4& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a5.s[2];
    s[13] = a5.s[3];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char2& a0, const char2& a1, const char2& a2, const char2& a3, const char3& a4, const char2& a5, const char3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  char16(const char2& a0, const char2& a1, const char2& a2, const char2& a3, const char3& a4, const char3& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char2& a0, const char2& a1, const char2& a2, const char2& a3, const char4& a4, const char2& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char2& a0, const char2& a1, const char2& a2, const char2& a3, const char4& a4, const char4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  char16(const char2& a0, const char2& a1, const char2& a2, const char2& a3, const char8& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a4.s[4];
    s[13] = a4.s[5];
    s[14] = a4.s[6];
    s[15] = a4.s[7];
  }

  char16(const char2& a0, const char2& a1, const char2& a2, const char3& a3, const char2& a4, const char2& a5, const char3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  char16(const char2& a0, const char2& a1, const char2& a2, const char3& a3, const char2& a4, const char3& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char2& a0, const char2& a1, const char2& a2, const char3& a3, const char3& a4, const char2& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char2& a0, const char2& a1, const char2& a2, const char3& a3, const char3& a4, const char4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  char16(const char2& a0, const char2& a1, const char2& a2, const char3& a3, const char4& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char2& a0, const char2& a1, const char2& a2, const char4& a3, const char2& a4, const char2& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char2& a0, const char2& a1, const char2& a2, const char4& a3, const char2& a4, const char4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  char16(const char2& a0, const char2& a1, const char2& a2, const char4& a3, const char3& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char2& a0, const char2& a1, const char2& a2, const char4& a3, const char4& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char2& a0, const char2& a1, const char2& a2, const char8& a3, const char2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a3.s[4];
    s[11] = a3.s[5];
    s[12] = a3.s[6];
    s[13] = a3.s[7];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  char16(const char2& a0, const char2& a1, const char3& a2, const char2& a3, const char2& a4, const char2& a5, const char3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  char16(const char2& a0, const char2& a1, const char3& a2, const char2& a3, const char2& a4, const char3& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char2& a0, const char2& a1, const char3& a2, const char2& a3, const char3& a4, const char2& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char2& a0, const char2& a1, const char3& a2, const char2& a3, const char3& a4, const char4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  char16(const char2& a0, const char2& a1, const char3& a2, const char2& a3, const char4& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char2& a0, const char2& a1, const char3& a2, const char3& a3, const char2& a4, const char2& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char2& a0, const char2& a1, const char3& a2, const char3& a3, const char2& a4, const char4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  char16(const char2& a0, const char2& a1, const char3& a2, const char3& a3, const char3& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char2& a0, const char2& a1, const char3& a2, const char3& a3, const char4& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char2& a0, const char2& a1, const char3& a2, const char4& a3, const char2& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char2& a0, const char2& a1, const char3& a2, const char4& a3, const char3& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char2& a0, const char2& a1, const char4& a2, const char2& a3, const char2& a4, const char2& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char2& a0, const char2& a1, const char4& a2, const char2& a3, const char2& a4, const char4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  char16(const char2& a0, const char2& a1, const char4& a2, const char2& a3, const char3& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char2& a0, const char2& a1, const char4& a2, const char2& a3, const char4& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char2& a0, const char2& a1, const char4& a2, const char3& a3, const char2& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char2& a0, const char2& a1, const char4& a2, const char3& a3, const char3& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char2& a0, const char2& a1, const char4& a2, const char4& a3, const char2& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char2& a0, const char2& a1, const char4& a2, const char4& a3, const char4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  char16(const char2& a0, const char2& a1, const char4& a2, const char8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  char16(const char2& a0, const char2& a1, const char8& a2, const char2& a3, const char2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a2.s[4];
    s[9] = a2.s[5];
    s[10] = a2.s[6];
    s[11] = a2.s[7];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  char16(const char2& a0, const char2& a1, const char8& a2, const char4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a2.s[4];
    s[9] = a2.s[5];
    s[10] = a2.s[6];
    s[11] = a2.s[7];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  char16(const char2& a0, const char3& a1, const char2& a2, const char2& a3, const char2& a4, const char2& a5, const char3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  char16(const char2& a0, const char3& a1, const char2& a2, const char2& a3, const char2& a4, const char3& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char2& a0, const char3& a1, const char2& a2, const char2& a3, const char3& a4, const char2& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char2& a0, const char3& a1, const char2& a2, const char2& a3, const char3& a4, const char4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  char16(const char2& a0, const char3& a1, const char2& a2, const char2& a3, const char4& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char2& a0, const char3& a1, const char2& a2, const char3& a3, const char2& a4, const char2& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char2& a0, const char3& a1, const char2& a2, const char3& a3, const char2& a4, const char4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  char16(const char2& a0, const char3& a1, const char2& a2, const char3& a3, const char3& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char2& a0, const char3& a1, const char2& a2, const char3& a3, const char4& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char2& a0, const char3& a1, const char2& a2, const char4& a3, const char2& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char2& a0, const char3& a1, const char2& a2, const char4& a3, const char3& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char2& a0, const char3& a1, const char3& a2, const char2& a3, const char2& a4, const char2& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char2& a0, const char3& a1, const char3& a2, const char2& a3, const char2& a4, const char4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  char16(const char2& a0, const char3& a1, const char3& a2, const char2& a3, const char3& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char2& a0, const char3& a1, const char3& a2, const char2& a3, const char4& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char2& a0, const char3& a1, const char3& a2, const char3& a3, const char2& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char2& a0, const char3& a1, const char3& a2, const char3& a3, const char3& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char2& a0, const char3& a1, const char3& a2, const char4& a3, const char2& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char2& a0, const char3& a1, const char3& a2, const char4& a3, const char4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  char16(const char2& a0, const char3& a1, const char3& a2, const char8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  char16(const char2& a0, const char3& a1, const char4& a2, const char2& a3, const char2& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char2& a0, const char3& a1, const char4& a2, const char2& a3, const char3& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char2& a0, const char3& a1, const char4& a2, const char3& a3, const char2& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char2& a0, const char3& a1, const char4& a2, const char3& a3, const char4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  char16(const char2& a0, const char3& a1, const char4& a2, const char4& a3, const char3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  char16(const char2& a0, const char3& a1, const char8& a2, const char3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a2.s[4];
    s[10] = a2.s[5];
    s[11] = a2.s[6];
    s[12] = a2.s[7];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  char16(const char2& a0, const char4& a1, const char2& a2, const char2& a3, const char2& a4, const char2& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char2& a0, const char4& a1, const char2& a2, const char2& a3, const char2& a4, const char4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  char16(const char2& a0, const char4& a1, const char2& a2, const char2& a3, const char3& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char2& a0, const char4& a1, const char2& a2, const char2& a3, const char4& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char2& a0, const char4& a1, const char2& a2, const char3& a3, const char2& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char2& a0, const char4& a1, const char2& a2, const char3& a3, const char3& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char2& a0, const char4& a1, const char2& a2, const char4& a3, const char2& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char2& a0, const char4& a1, const char2& a2, const char4& a3, const char4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  char16(const char2& a0, const char4& a1, const char2& a2, const char8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  char16(const char2& a0, const char4& a1, const char3& a2, const char2& a3, const char2& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char2& a0, const char4& a1, const char3& a2, const char2& a3, const char3& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char2& a0, const char4& a1, const char3& a2, const char3& a3, const char2& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char2& a0, const char4& a1, const char3& a2, const char3& a3, const char4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  char16(const char2& a0, const char4& a1, const char3& a2, const char4& a3, const char3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  char16(const char2& a0, const char4& a1, const char4& a2, const char2& a3, const char2& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char2& a0, const char4& a1, const char4& a2, const char2& a3, const char4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  char16(const char2& a0, const char4& a1, const char4& a2, const char3& a3, const char3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  char16(const char2& a0, const char4& a1, const char4& a2, const char4& a3, const char2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  char16(const char2& a0, const char4& a1, const char8& a2, const char2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  char16(const char2& a0, const char8& a1, const char2& a2, const char2& a3, const char2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  char16(const char2& a0, const char8& a1, const char2& a2, const char4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  char16(const char2& a0, const char8& a1, const char3& a2, const char3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  char16(const char2& a0, const char8& a1, const char4& a2, const char2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a2.s[3];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  char16(const char3& a0, const char2& a1, const char2& a2, const char2& a3, const char2& a4, const char2& a5, const char3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  char16(const char3& a0, const char2& a1, const char2& a2, const char2& a3, const char2& a4, const char3& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char3& a0, const char2& a1, const char2& a2, const char2& a3, const char3& a4, const char2& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char3& a0, const char2& a1, const char2& a2, const char2& a3, const char3& a4, const char4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  char16(const char3& a0, const char2& a1, const char2& a2, const char2& a3, const char4& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char3& a0, const char2& a1, const char2& a2, const char3& a3, const char2& a4, const char2& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char3& a0, const char2& a1, const char2& a2, const char3& a3, const char2& a4, const char4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  char16(const char3& a0, const char2& a1, const char2& a2, const char3& a3, const char3& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char3& a0, const char2& a1, const char2& a2, const char3& a3, const char4& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char3& a0, const char2& a1, const char2& a2, const char4& a3, const char2& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char3& a0, const char2& a1, const char2& a2, const char4& a3, const char3& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char3& a0, const char2& a1, const char3& a2, const char2& a3, const char2& a4, const char2& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char3& a0, const char2& a1, const char3& a2, const char2& a3, const char2& a4, const char4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  char16(const char3& a0, const char2& a1, const char3& a2, const char2& a3, const char3& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char3& a0, const char2& a1, const char3& a2, const char2& a3, const char4& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char3& a0, const char2& a1, const char3& a2, const char3& a3, const char2& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char3& a0, const char2& a1, const char3& a2, const char3& a3, const char3& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char3& a0, const char2& a1, const char3& a2, const char4& a3, const char2& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char3& a0, const char2& a1, const char3& a2, const char4& a3, const char4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  char16(const char3& a0, const char2& a1, const char3& a2, const char8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  char16(const char3& a0, const char2& a1, const char4& a2, const char2& a3, const char2& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char3& a0, const char2& a1, const char4& a2, const char2& a3, const char3& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char3& a0, const char2& a1, const char4& a2, const char3& a3, const char2& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char3& a0, const char2& a1, const char4& a2, const char3& a3, const char4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  char16(const char3& a0, const char2& a1, const char4& a2, const char4& a3, const char3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  char16(const char3& a0, const char2& a1, const char8& a2, const char3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a2.s[4];
    s[10] = a2.s[5];
    s[11] = a2.s[6];
    s[12] = a2.s[7];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  char16(const char3& a0, const char3& a1, const char2& a2, const char2& a3, const char2& a4, const char2& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char3& a0, const char3& a1, const char2& a2, const char2& a3, const char2& a4, const char4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  char16(const char3& a0, const char3& a1, const char2& a2, const char2& a3, const char3& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char3& a0, const char3& a1, const char2& a2, const char2& a3, const char4& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char3& a0, const char3& a1, const char2& a2, const char3& a3, const char2& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char3& a0, const char3& a1, const char2& a2, const char3& a3, const char3& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char3& a0, const char3& a1, const char2& a2, const char4& a3, const char2& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char3& a0, const char3& a1, const char2& a2, const char4& a3, const char4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  char16(const char3& a0, const char3& a1, const char2& a2, const char8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  char16(const char3& a0, const char3& a1, const char3& a2, const char2& a3, const char2& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char3& a0, const char3& a1, const char3& a2, const char2& a3, const char3& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char3& a0, const char3& a1, const char3& a2, const char3& a3, const char2& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char3& a0, const char3& a1, const char3& a2, const char3& a3, const char4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  char16(const char3& a0, const char3& a1, const char3& a2, const char4& a3, const char3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  char16(const char3& a0, const char3& a1, const char4& a2, const char2& a3, const char2& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char3& a0, const char3& a1, const char4& a2, const char2& a3, const char4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  char16(const char3& a0, const char3& a1, const char4& a2, const char3& a3, const char3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  char16(const char3& a0, const char3& a1, const char4& a2, const char4& a3, const char2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  char16(const char3& a0, const char3& a1, const char8& a2, const char2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  char16(const char3& a0, const char4& a1, const char2& a2, const char2& a3, const char2& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char3& a0, const char4& a1, const char2& a2, const char2& a3, const char3& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char3& a0, const char4& a1, const char2& a2, const char3& a3, const char2& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char3& a0, const char4& a1, const char2& a2, const char3& a3, const char4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  char16(const char3& a0, const char4& a1, const char2& a2, const char4& a3, const char3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  char16(const char3& a0, const char4& a1, const char3& a2, const char2& a3, const char2& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char3& a0, const char4& a1, const char3& a2, const char2& a3, const char4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  char16(const char3& a0, const char4& a1, const char3& a2, const char3& a3, const char3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  char16(const char3& a0, const char4& a1, const char3& a2, const char4& a3, const char2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  char16(const char3& a0, const char4& a1, const char4& a2, const char2& a3, const char3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  char16(const char3& a0, const char4& a1, const char4& a2, const char3& a3, const char2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  char16(const char3& a0, const char8& a1, const char2& a2, const char3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a1.s[4];
    s[8] = a1.s[5];
    s[9] = a1.s[6];
    s[10] = a1.s[7];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  char16(const char3& a0, const char8& a1, const char3& a2, const char2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a1.s[4];
    s[8] = a1.s[5];
    s[9] = a1.s[6];
    s[10] = a1.s[7];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a2.s[2];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  char16(const char4& a0, const char2& a1, const char2& a2, const char2& a3, const char2& a4, const char2& a5, const char2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  char16(const char4& a0, const char2& a1, const char2& a2, const char2& a3, const char2& a4, const char4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  char16(const char4& a0, const char2& a1, const char2& a2, const char2& a3, const char3& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char4& a0, const char2& a1, const char2& a2, const char2& a3, const char4& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char4& a0, const char2& a1, const char2& a2, const char3& a3, const char2& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char4& a0, const char2& a1, const char2& a2, const char3& a3, const char3& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char4& a0, const char2& a1, const char2& a2, const char4& a3, const char2& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char4& a0, const char2& a1, const char2& a2, const char4& a3, const char4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  char16(const char4& a0, const char2& a1, const char2& a2, const char8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  char16(const char4& a0, const char2& a1, const char3& a2, const char2& a3, const char2& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char4& a0, const char2& a1, const char3& a2, const char2& a3, const char3& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char4& a0, const char2& a1, const char3& a2, const char3& a3, const char2& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char4& a0, const char2& a1, const char3& a2, const char3& a3, const char4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  char16(const char4& a0, const char2& a1, const char3& a2, const char4& a3, const char3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  char16(const char4& a0, const char2& a1, const char4& a2, const char2& a3, const char2& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char4& a0, const char2& a1, const char4& a2, const char2& a3, const char4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  char16(const char4& a0, const char2& a1, const char4& a2, const char3& a3, const char3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  char16(const char4& a0, const char2& a1, const char4& a2, const char4& a3, const char2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  char16(const char4& a0, const char2& a1, const char8& a2, const char2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  char16(const char4& a0, const char3& a1, const char2& a2, const char2& a3, const char2& a4, const char3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  char16(const char4& a0, const char3& a1, const char2& a2, const char2& a3, const char3& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char4& a0, const char3& a1, const char2& a2, const char3& a3, const char2& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char4& a0, const char3& a1, const char2& a2, const char3& a3, const char4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  char16(const char4& a0, const char3& a1, const char2& a2, const char4& a3, const char3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  char16(const char4& a0, const char3& a1, const char3& a2, const char2& a3, const char2& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char4& a0, const char3& a1, const char3& a2, const char2& a3, const char4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  char16(const char4& a0, const char3& a1, const char3& a2, const char3& a3, const char3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  char16(const char4& a0, const char3& a1, const char3& a2, const char4& a3, const char2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  char16(const char4& a0, const char3& a1, const char4& a2, const char2& a3, const char3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  char16(const char4& a0, const char3& a1, const char4& a2, const char3& a3, const char2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  char16(const char4& a0, const char4& a1, const char2& a2, const char2& a3, const char2& a4, const char2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  char16(const char4& a0, const char4& a1, const char2& a2, const char2& a3, const char4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  char16(const char4& a0, const char4& a1, const char2& a2, const char3& a3, const char3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  char16(const char4& a0, const char4& a1, const char2& a2, const char4& a3, const char2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  char16(const char4& a0, const char4& a1, const char3& a2, const char2& a3, const char3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  char16(const char4& a0, const char4& a1, const char3& a2, const char3& a3, const char2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  char16(const char4& a0, const char4& a1, const char4& a2, const char2& a3, const char2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  char16(const char4& a0, const char4& a1, const char4& a2, const char4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  char16(const char4& a0, const char4& a1, const char8& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a2.s[4];
    s[13] = a2.s[5];
    s[14] = a2.s[6];
    s[15] = a2.s[7];
  }

  char16(const char4& a0, const char8& a1, const char2& a2, const char2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a1.s[4];
    s[9] = a1.s[5];
    s[10] = a1.s[6];
    s[11] = a1.s[7];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  char16(const char4& a0, const char8& a1, const char4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a1.s[4];
    s[9] = a1.s[5];
    s[10] = a1.s[6];
    s[11] = a1.s[7];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a2.s[2];
    s[15] = a2.s[3];
  }

  char16(const char8& a0, const char2& a1, const char2& a2, const char2& a3, const char2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  char16(const char8& a0, const char2& a1, const char2& a2, const char4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  char16(const char8& a0, const char2& a1, const char3& a2, const char3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  char16(const char8& a0, const char2& a1, const char4& a2, const char2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a2.s[3];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  char16(const char8& a0, const char3& a1, const char2& a2, const char3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  char16(const char8& a0, const char3& a1, const char3& a2, const char2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a2.s[2];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  char16(const char8& a0, const char4& a1, const char2& a2, const char2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  char16(const char8& a0, const char4& a1, const char4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a2.s[2];
    s[15] = a2.s[3];
  }

  char16(const char8& a0, const char8& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a1.s[4];
    s[13] = a1.s[5];
    s[14] = a1.s[6];
    s[15] = a1.s[7];
  }

  char16(const char16& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a0.s[8];
    s[9] = a0.s[9];
    s[10] = a0.s[10];
    s[11] = a0.s[11];
    s[12] = a0.s[12];
    s[13] = a0.s[13];
    s[14] = a0.s[14];
    s[15] = a0.s[15];
  }

  char16& operator+=(const char16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  char16& operator+=(cl_char rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  char16& operator-=(const char16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  char16& operator-=(cl_char rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  char16& operator*=(const char16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  char16& operator*=(cl_char rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  char16& operator/=(const char16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  char16& operator/=(cl_char rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  char16& operator%=(const char16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  char16& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  char16& operator&=(const char16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  char16& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  char16& operator|=(const char16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  char16& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  char16& operator^=(const char16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  char16& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  char16& operator<<=(const char16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  char16& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  char16& operator>>=(const char16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  char16& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline char16 operator+(char16 lhs, const char16& rhs)
{
  lhs += rhs;
  return lhs;
}

inline char16 operator+(char16 lhs, cl_char rhs)
{
  lhs += rhs;
  return lhs;
}

inline char16 operator+(cl_char lhs, const char16& rhs)
{
  auto tmp = char16(lhs);
  return tmp + rhs;
}

inline char16 operator-(char16 lhs, const char16& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline char16 operator-(char16 lhs, cl_char rhs)
{
  lhs -= rhs;
  return lhs;
}

inline char16 operator-(cl_char lhs, const char16& rhs)
{
  auto tmp = char16(lhs);
  return tmp - rhs;
}

inline char16 operator*(char16 lhs, const char16& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline char16 operator*(char16 lhs, cl_char rhs)
{
  lhs *= rhs;
  return lhs;
}

inline char16 operator*(cl_char lhs, const char16& rhs)
{
  auto tmp = char16(lhs);
  return tmp * rhs;
}

inline char16 operator/(char16 lhs, const char16& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline char16 operator/(char16 lhs, cl_char rhs)
{
  lhs /= rhs;
  return lhs;
}

inline char16 operator/(cl_char lhs, const char16& rhs)
{
  auto tmp = char16(lhs);
  return tmp / rhs;
}

inline char16 operator%(char16 lhs, const char16& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline char16 operator%(char16 lhs, cl_char rhs)
{
  lhs %= rhs;
  return lhs;
}

inline char16 operator%(cl_char lhs, const char16& rhs)
{
  auto tmp = char16(lhs);
  return tmp % rhs;
}

inline char16 operator&(char16 lhs, const char16& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline char16 operator&(char16 lhs, cl_char rhs)
{
  lhs &= rhs;
  return lhs;
}

inline char16 operator&(cl_char lhs, const char16& rhs)
{
  auto tmp = char16(lhs);
  return tmp & rhs;
}

inline char16 operator|(char16 lhs, const char16& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline char16 operator|(char16 lhs, cl_char rhs)
{
  lhs |= rhs;
  return lhs;
}

inline char16 operator|(cl_char lhs, const char16& rhs)
{
  auto tmp = char16(lhs);
  return tmp | rhs;
}

inline char16 operator^(char16 lhs, const char16& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline char16 operator^(char16 lhs, cl_char rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline char16 operator^(cl_char lhs, const char16& rhs)
{
  auto tmp = char16(lhs);
  return tmp ^ rhs;
}

inline char16 operator<<(char16 lhs, const char16& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline char16 operator<<(char16 lhs, cl_char rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline char16 operator<<(cl_char lhs, const char16& rhs)
{
  auto tmp = char16(lhs);
  return tmp << rhs;
}

inline char16 operator>>(char16 lhs, const char16& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline char16 operator>>(char16 lhs, cl_char rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline char16 operator>>(cl_char lhs, const char16& rhs)
{
  auto tmp = char16(lhs);
  return tmp >> rhs;
}

inline bool operator==(const char16& lhs, const char16& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]) &&
  (lhs.s[4] == rhs.s[4]) &&
  (lhs.s[5] == rhs.s[5]) &&
  (lhs.s[6] == rhs.s[6]) &&
  (lhs.s[7] == rhs.s[7]) &&
  (lhs.s[8] == rhs.s[8]) &&
  (lhs.s[9] == rhs.s[9]) &&
  (lhs.s[10] == rhs.s[10]) &&
  (lhs.s[11] == rhs.s[11]) &&
  (lhs.s[12] == rhs.s[12]) &&
  (lhs.s[13] == rhs.s[13]) &&
  (lhs.s[14] == rhs.s[14]) &&
  (lhs.s[15] == rhs.s[15]);
}

inline bool operator==(const char16& lhs, const cl_char& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs) &&
  (lhs.s[4] == rhs) &&
  (lhs.s[5] == rhs) &&
  (lhs.s[6] == rhs) &&
  (lhs.s[7] == rhs) &&
  (lhs.s[8] == rhs) &&
  (lhs.s[9] == rhs) &&
  (lhs.s[10] == rhs) &&
  (lhs.s[11] == rhs) &&
  (lhs.s[12] == rhs) &&
  (lhs.s[13] == rhs) &&
  (lhs.s[14] == rhs) &&
  (lhs.s[15] == rhs);
}

inline bool operator<(const char16& lhs, const char16& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const char16& lhs, const cl_char& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const char16& lhs, const char16& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const char16& lhs, const cl_char& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const char16& lhs, const char16& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const char16& lhs, const cl_char& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const char16& lhs, const char16& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const char16& lhs, const cl_char& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const char16& lhs, const char16& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const char16& lhs, const cl_char& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline char16 vload16(size_t offset, const char16* p)
{
  return *(p + offset);
}

inline void vstore16(char16 data, size_t offset, char16* p)
{
  *(p + offset) = data;
}


class uchar2{
public:
  static const size_t N = 2; // Number of elements in vector
  union {
	 cl_uchar CL_ALIGNED(2) s[2];
#if defined(__GNUC__)
    __extension__ struct { cl_uchar x, y; };
    __extension__ struct { cl_uchar s0, s1; };
#endif
  };

  uchar2(){}

  uchar2(cl_uchar i0,cl_uchar i1){
    s[0] = i0;
    s[1] = i1;

  }

  uchar2(cl_uchar i0) {
  for (int i = 0; i < 2; ++i) {
		s[i] = i0;
	 }
  }

  uchar2(const uchar2& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
  }

  uchar2& operator+=(const uchar2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  uchar2& operator+=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  uchar2& operator-=(const uchar2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  uchar2& operator-=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  uchar2& operator*=(const uchar2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  uchar2& operator*=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  uchar2& operator/=(const uchar2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  uchar2& operator/=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  uchar2& operator%=(const uchar2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  uchar2& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  uchar2& operator&=(const uchar2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  uchar2& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  uchar2& operator|=(const uchar2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  uchar2& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  uchar2& operator^=(const uchar2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  uchar2& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  uchar2& operator<<=(const uchar2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  uchar2& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  uchar2& operator>>=(const uchar2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  uchar2& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline uchar2 operator+(uchar2 lhs, const uchar2& rhs)
{
  lhs += rhs;
  return lhs;
}

inline uchar2 operator+(uchar2 lhs, cl_uchar rhs)
{
  lhs += rhs;
  return lhs;
}

inline uchar2 operator+(cl_uchar lhs, const uchar2& rhs)
{
  auto tmp = uchar2(lhs);
  return tmp + rhs;
}

inline uchar2 operator-(uchar2 lhs, const uchar2& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uchar2 operator-(uchar2 lhs, cl_uchar rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uchar2 operator-(cl_uchar lhs, const uchar2& rhs)
{
  auto tmp = uchar2(lhs);
  return tmp - rhs;
}

inline uchar2 operator*(uchar2 lhs, const uchar2& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uchar2 operator*(uchar2 lhs, cl_uchar rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uchar2 operator*(cl_uchar lhs, const uchar2& rhs)
{
  auto tmp = uchar2(lhs);
  return tmp * rhs;
}

inline uchar2 operator/(uchar2 lhs, const uchar2& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uchar2 operator/(uchar2 lhs, cl_uchar rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uchar2 operator/(cl_uchar lhs, const uchar2& rhs)
{
  auto tmp = uchar2(lhs);
  return tmp / rhs;
}

inline uchar2 operator%(uchar2 lhs, const uchar2& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uchar2 operator%(uchar2 lhs, cl_uchar rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uchar2 operator%(cl_uchar lhs, const uchar2& rhs)
{
  auto tmp = uchar2(lhs);
  return tmp % rhs;
}

inline uchar2 operator&(uchar2 lhs, const uchar2& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uchar2 operator&(uchar2 lhs, cl_uchar rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uchar2 operator&(cl_uchar lhs, const uchar2& rhs)
{
  auto tmp = uchar2(lhs);
  return tmp & rhs;
}

inline uchar2 operator|(uchar2 lhs, const uchar2& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uchar2 operator|(uchar2 lhs, cl_uchar rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uchar2 operator|(cl_uchar lhs, const uchar2& rhs)
{
  auto tmp = uchar2(lhs);
  return tmp | rhs;
}

inline uchar2 operator^(uchar2 lhs, const uchar2& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uchar2 operator^(uchar2 lhs, cl_uchar rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uchar2 operator^(cl_uchar lhs, const uchar2& rhs)
{
  auto tmp = uchar2(lhs);
  return tmp ^ rhs;
}

inline uchar2 operator<<(uchar2 lhs, const uchar2& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uchar2 operator<<(uchar2 lhs, cl_uchar rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uchar2 operator<<(cl_uchar lhs, const uchar2& rhs)
{
  auto tmp = uchar2(lhs);
  return tmp << rhs;
}

inline uchar2 operator>>(uchar2 lhs, const uchar2& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uchar2 operator>>(uchar2 lhs, cl_uchar rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uchar2 operator>>(cl_uchar lhs, const uchar2& rhs)
{
  auto tmp = uchar2(lhs);
  return tmp >> rhs;
}

inline bool operator==(const uchar2& lhs, const uchar2& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]);
}

inline bool operator==(const uchar2& lhs, const cl_uchar& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs);
}

inline bool operator<(const uchar2& lhs, const uchar2& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const uchar2& lhs, const cl_uchar& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uchar2& lhs, const uchar2& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uchar2& lhs, const cl_uchar& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const uchar2& lhs, const uchar2& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const uchar2& lhs, const cl_uchar& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const uchar2& lhs, const uchar2& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const uchar2& lhs, const cl_uchar& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const uchar2& lhs, const uchar2& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const uchar2& lhs, const cl_uchar& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline uchar2 vload2(size_t offset, const uchar2* p)
{
  return *(p + offset);
}

inline void vstore2(uchar2 data, size_t offset, uchar2* p)
{
  *(p + offset) = data;
}


class uchar3{
public:
  static const size_t N = 4; // Number of elements in vector
  union {
	 cl_uchar CL_ALIGNED(4) s[4];
#if defined(__GNUC__)
    __extension__ struct { cl_uchar x, y, z, w; };
    __extension__ struct { cl_uchar s0, s1, s2, s3; };
#endif
  };

  uchar3(){}

  uchar3(cl_uchar i0,cl_uchar i1,cl_uchar i2){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;

  }

  uchar3(cl_uchar i0) {
  for (int i = 0; i < 3; ++i) {
		s[i] = i0;
	 }
  }

  uchar3(const uchar3& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
  }

  uchar3& operator+=(const uchar3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  uchar3& operator+=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  uchar3& operator-=(const uchar3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  uchar3& operator-=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  uchar3& operator*=(const uchar3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  uchar3& operator*=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  uchar3& operator/=(const uchar3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  uchar3& operator/=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  uchar3& operator%=(const uchar3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  uchar3& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  uchar3& operator&=(const uchar3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  uchar3& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  uchar3& operator|=(const uchar3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  uchar3& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  uchar3& operator^=(const uchar3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  uchar3& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  uchar3& operator<<=(const uchar3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  uchar3& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  uchar3& operator>>=(const uchar3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  uchar3& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline uchar3 operator+(uchar3 lhs, const uchar3& rhs)
{
  lhs += rhs;
  return lhs;
}

inline uchar3 operator+(uchar3 lhs, cl_uchar rhs)
{
  lhs += rhs;
  return lhs;
}

inline uchar3 operator+(cl_uchar lhs, const uchar3& rhs)
{
  auto tmp = uchar3(lhs);
  return tmp + rhs;
}

inline uchar3 operator-(uchar3 lhs, const uchar3& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uchar3 operator-(uchar3 lhs, cl_uchar rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uchar3 operator-(cl_uchar lhs, const uchar3& rhs)
{
  auto tmp = uchar3(lhs);
  return tmp - rhs;
}

inline uchar3 operator*(uchar3 lhs, const uchar3& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uchar3 operator*(uchar3 lhs, cl_uchar rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uchar3 operator*(cl_uchar lhs, const uchar3& rhs)
{
  auto tmp = uchar3(lhs);
  return tmp * rhs;
}

inline uchar3 operator/(uchar3 lhs, const uchar3& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uchar3 operator/(uchar3 lhs, cl_uchar rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uchar3 operator/(cl_uchar lhs, const uchar3& rhs)
{
  auto tmp = uchar3(lhs);
  return tmp / rhs;
}

inline uchar3 operator%(uchar3 lhs, const uchar3& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uchar3 operator%(uchar3 lhs, cl_uchar rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uchar3 operator%(cl_uchar lhs, const uchar3& rhs)
{
  auto tmp = uchar3(lhs);
  return tmp % rhs;
}

inline uchar3 operator&(uchar3 lhs, const uchar3& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uchar3 operator&(uchar3 lhs, cl_uchar rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uchar3 operator&(cl_uchar lhs, const uchar3& rhs)
{
  auto tmp = uchar3(lhs);
  return tmp & rhs;
}

inline uchar3 operator|(uchar3 lhs, const uchar3& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uchar3 operator|(uchar3 lhs, cl_uchar rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uchar3 operator|(cl_uchar lhs, const uchar3& rhs)
{
  auto tmp = uchar3(lhs);
  return tmp | rhs;
}

inline uchar3 operator^(uchar3 lhs, const uchar3& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uchar3 operator^(uchar3 lhs, cl_uchar rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uchar3 operator^(cl_uchar lhs, const uchar3& rhs)
{
  auto tmp = uchar3(lhs);
  return tmp ^ rhs;
}

inline uchar3 operator<<(uchar3 lhs, const uchar3& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uchar3 operator<<(uchar3 lhs, cl_uchar rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uchar3 operator<<(cl_uchar lhs, const uchar3& rhs)
{
  auto tmp = uchar3(lhs);
  return tmp << rhs;
}

inline uchar3 operator>>(uchar3 lhs, const uchar3& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uchar3 operator>>(uchar3 lhs, cl_uchar rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uchar3 operator>>(cl_uchar lhs, const uchar3& rhs)
{
  auto tmp = uchar3(lhs);
  return tmp >> rhs;
}

inline bool operator==(const uchar3& lhs, const uchar3& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]);
}

inline bool operator==(const uchar3& lhs, const cl_uchar& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs);
}

inline bool operator<(const uchar3& lhs, const uchar3& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const uchar3& lhs, const cl_uchar& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uchar3& lhs, const uchar3& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uchar3& lhs, const cl_uchar& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const uchar3& lhs, const uchar3& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const uchar3& lhs, const cl_uchar& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const uchar3& lhs, const uchar3& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const uchar3& lhs, const cl_uchar& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const uchar3& lhs, const uchar3& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const uchar3& lhs, const cl_uchar& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline uchar3 vload3(size_t offset, const uchar3* p)
{
  return *(p + offset);
}

inline void vstore3(uchar3 data, size_t offset, uchar3* p)
{
  *(p + offset) = data;
}


class uchar4{
public:
  static const size_t N = 4; // Number of elements in vector
  union {
	 cl_uchar CL_ALIGNED(4) s[4];
#if defined(__GNUC__)
    __extension__ struct { cl_uchar x, y, z, w; };
    __extension__ struct { cl_uchar s0, s1, s2, s3; };
#endif
  };

  uchar4(){}

  uchar4(cl_uchar i0,cl_uchar i1,cl_uchar i2,cl_uchar i3
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;

  }

  uchar4(cl_uchar i0) {
  for (int i = 0; i < 4; ++i) {
		s[i] = i0;
	 }
  }

  uchar4(const uchar2& a0, const uchar2& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
  }

  uchar4(const uchar4& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
  }

  uchar4& operator+=(const uchar4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  uchar4& operator+=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  uchar4& operator-=(const uchar4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  uchar4& operator-=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  uchar4& operator*=(const uchar4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  uchar4& operator*=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  uchar4& operator/=(const uchar4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  uchar4& operator/=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  uchar4& operator%=(const uchar4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  uchar4& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  uchar4& operator&=(const uchar4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  uchar4& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  uchar4& operator|=(const uchar4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  uchar4& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  uchar4& operator^=(const uchar4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  uchar4& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  uchar4& operator<<=(const uchar4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  uchar4& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  uchar4& operator>>=(const uchar4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  uchar4& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline uchar4 operator+(uchar4 lhs, const uchar4& rhs)
{
  lhs += rhs;
  return lhs;
}

inline uchar4 operator+(uchar4 lhs, cl_uchar rhs)
{
  lhs += rhs;
  return lhs;
}

inline uchar4 operator+(cl_uchar lhs, const uchar4& rhs)
{
  auto tmp = uchar4(lhs);
  return tmp + rhs;
}

inline uchar4 operator-(uchar4 lhs, const uchar4& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uchar4 operator-(uchar4 lhs, cl_uchar rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uchar4 operator-(cl_uchar lhs, const uchar4& rhs)
{
  auto tmp = uchar4(lhs);
  return tmp - rhs;
}

inline uchar4 operator*(uchar4 lhs, const uchar4& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uchar4 operator*(uchar4 lhs, cl_uchar rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uchar4 operator*(cl_uchar lhs, const uchar4& rhs)
{
  auto tmp = uchar4(lhs);
  return tmp * rhs;
}

inline uchar4 operator/(uchar4 lhs, const uchar4& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uchar4 operator/(uchar4 lhs, cl_uchar rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uchar4 operator/(cl_uchar lhs, const uchar4& rhs)
{
  auto tmp = uchar4(lhs);
  return tmp / rhs;
}

inline uchar4 operator%(uchar4 lhs, const uchar4& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uchar4 operator%(uchar4 lhs, cl_uchar rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uchar4 operator%(cl_uchar lhs, const uchar4& rhs)
{
  auto tmp = uchar4(lhs);
  return tmp % rhs;
}

inline uchar4 operator&(uchar4 lhs, const uchar4& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uchar4 operator&(uchar4 lhs, cl_uchar rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uchar4 operator&(cl_uchar lhs, const uchar4& rhs)
{
  auto tmp = uchar4(lhs);
  return tmp & rhs;
}

inline uchar4 operator|(uchar4 lhs, const uchar4& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uchar4 operator|(uchar4 lhs, cl_uchar rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uchar4 operator|(cl_uchar lhs, const uchar4& rhs)
{
  auto tmp = uchar4(lhs);
  return tmp | rhs;
}

inline uchar4 operator^(uchar4 lhs, const uchar4& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uchar4 operator^(uchar4 lhs, cl_uchar rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uchar4 operator^(cl_uchar lhs, const uchar4& rhs)
{
  auto tmp = uchar4(lhs);
  return tmp ^ rhs;
}

inline uchar4 operator<<(uchar4 lhs, const uchar4& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uchar4 operator<<(uchar4 lhs, cl_uchar rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uchar4 operator<<(cl_uchar lhs, const uchar4& rhs)
{
  auto tmp = uchar4(lhs);
  return tmp << rhs;
}

inline uchar4 operator>>(uchar4 lhs, const uchar4& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uchar4 operator>>(uchar4 lhs, cl_uchar rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uchar4 operator>>(cl_uchar lhs, const uchar4& rhs)
{
  auto tmp = uchar4(lhs);
  return tmp >> rhs;
}

inline bool operator==(const uchar4& lhs, const uchar4& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]);
}

inline bool operator==(const uchar4& lhs, const cl_uchar& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs);
}

inline bool operator<(const uchar4& lhs, const uchar4& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const uchar4& lhs, const cl_uchar& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uchar4& lhs, const uchar4& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uchar4& lhs, const cl_uchar& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const uchar4& lhs, const uchar4& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const uchar4& lhs, const cl_uchar& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const uchar4& lhs, const uchar4& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const uchar4& lhs, const cl_uchar& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const uchar4& lhs, const uchar4& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const uchar4& lhs, const cl_uchar& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline uchar4 vload4(size_t offset, const uchar4* p)
{
  return *(p + offset);
}

inline void vstore4(uchar4 data, size_t offset, uchar4* p)
{
  *(p + offset) = data;
}


class uchar8{
public:
  static const size_t N = 8; // Number of elements in vector
  union {
	 cl_uchar CL_ALIGNED(8) s[8];
#if defined(__GNUC__)
    __extension__ struct { cl_uchar x, y, z, w; };
    __extension__ struct { cl_uchar s0, s1, s2, s3, s4, s5, s6, s7; };
#endif
  };

  uchar8(){}

  uchar8(cl_uchar i0,cl_uchar i1,cl_uchar i2,cl_uchar i3,
          cl_uchar i4,cl_uchar i5,cl_uchar i6,cl_uchar i7
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;
    s[4] = i4;
    s[5] = i5;
    s[6] = i6;
    s[7] = i7;

  }

  uchar8(cl_uchar i0) {
  for (int i = 0; i < 8; ++i) {
		s[i] = i0;
	 }
  }

  uchar8(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
  }

  uchar8(const uchar2& a0, const uchar2& a1, const uchar4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
  }

  uchar8(const uchar2& a0, const uchar3& a1, const uchar3& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
  }

  uchar8(const uchar2& a0, const uchar4& a1, const uchar2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  uchar8(const uchar3& a0, const uchar2& a1, const uchar3& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
  }

  uchar8(const uchar3& a0, const uchar3& a1, const uchar2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  uchar8(const uchar4& a0, const uchar2& a1, const uchar2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  uchar8(const uchar4& a0, const uchar4& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
  }

  uchar8(const uchar8& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
  }

  uchar8& operator+=(const uchar8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  uchar8& operator+=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  uchar8& operator-=(const uchar8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  uchar8& operator-=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  uchar8& operator*=(const uchar8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  uchar8& operator*=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  uchar8& operator/=(const uchar8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  uchar8& operator/=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  uchar8& operator%=(const uchar8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  uchar8& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  uchar8& operator&=(const uchar8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  uchar8& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  uchar8& operator|=(const uchar8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  uchar8& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  uchar8& operator^=(const uchar8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  uchar8& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  uchar8& operator<<=(const uchar8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  uchar8& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  uchar8& operator>>=(const uchar8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  uchar8& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline uchar8 operator+(uchar8 lhs, const uchar8& rhs)
{
  lhs += rhs;
  return lhs;
}

inline uchar8 operator+(uchar8 lhs, cl_uchar rhs)
{
  lhs += rhs;
  return lhs;
}

inline uchar8 operator+(cl_uchar lhs, const uchar8& rhs)
{
  auto tmp = uchar8(lhs);
  return tmp + rhs;
}

inline uchar8 operator-(uchar8 lhs, const uchar8& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uchar8 operator-(uchar8 lhs, cl_uchar rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uchar8 operator-(cl_uchar lhs, const uchar8& rhs)
{
  auto tmp = uchar8(lhs);
  return tmp - rhs;
}

inline uchar8 operator*(uchar8 lhs, const uchar8& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uchar8 operator*(uchar8 lhs, cl_uchar rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uchar8 operator*(cl_uchar lhs, const uchar8& rhs)
{
  auto tmp = uchar8(lhs);
  return tmp * rhs;
}

inline uchar8 operator/(uchar8 lhs, const uchar8& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uchar8 operator/(uchar8 lhs, cl_uchar rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uchar8 operator/(cl_uchar lhs, const uchar8& rhs)
{
  auto tmp = uchar8(lhs);
  return tmp / rhs;
}

inline uchar8 operator%(uchar8 lhs, const uchar8& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uchar8 operator%(uchar8 lhs, cl_uchar rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uchar8 operator%(cl_uchar lhs, const uchar8& rhs)
{
  auto tmp = uchar8(lhs);
  return tmp % rhs;
}

inline uchar8 operator&(uchar8 lhs, const uchar8& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uchar8 operator&(uchar8 lhs, cl_uchar rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uchar8 operator&(cl_uchar lhs, const uchar8& rhs)
{
  auto tmp = uchar8(lhs);
  return tmp & rhs;
}

inline uchar8 operator|(uchar8 lhs, const uchar8& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uchar8 operator|(uchar8 lhs, cl_uchar rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uchar8 operator|(cl_uchar lhs, const uchar8& rhs)
{
  auto tmp = uchar8(lhs);
  return tmp | rhs;
}

inline uchar8 operator^(uchar8 lhs, const uchar8& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uchar8 operator^(uchar8 lhs, cl_uchar rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uchar8 operator^(cl_uchar lhs, const uchar8& rhs)
{
  auto tmp = uchar8(lhs);
  return tmp ^ rhs;
}

inline uchar8 operator<<(uchar8 lhs, const uchar8& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uchar8 operator<<(uchar8 lhs, cl_uchar rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uchar8 operator<<(cl_uchar lhs, const uchar8& rhs)
{
  auto tmp = uchar8(lhs);
  return tmp << rhs;
}

inline uchar8 operator>>(uchar8 lhs, const uchar8& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uchar8 operator>>(uchar8 lhs, cl_uchar rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uchar8 operator>>(cl_uchar lhs, const uchar8& rhs)
{
  auto tmp = uchar8(lhs);
  return tmp >> rhs;
}

inline bool operator==(const uchar8& lhs, const uchar8& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]) &&
  (lhs.s[4] == rhs.s[4]) &&
  (lhs.s[5] == rhs.s[5]) &&
  (lhs.s[6] == rhs.s[6]) &&
  (lhs.s[7] == rhs.s[7]);
}

inline bool operator==(const uchar8& lhs, const cl_uchar& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs) &&
  (lhs.s[4] == rhs) &&
  (lhs.s[5] == rhs) &&
  (lhs.s[6] == rhs) &&
  (lhs.s[7] == rhs);
}

inline bool operator<(const uchar8& lhs, const uchar8& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const uchar8& lhs, const cl_uchar& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uchar8& lhs, const uchar8& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uchar8& lhs, const cl_uchar& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const uchar8& lhs, const uchar8& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const uchar8& lhs, const cl_uchar& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const uchar8& lhs, const uchar8& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const uchar8& lhs, const cl_uchar& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const uchar8& lhs, const uchar8& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const uchar8& lhs, const cl_uchar& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline uchar8 vload8(size_t offset, const uchar8* p)
{
  return *(p + offset);
}

inline void vstore8(uchar8 data, size_t offset, uchar8* p)
{
  *(p + offset) = data;
}


class uchar16{
public:
  static const size_t N = 16; // Number of elements in vector
  union {
	 cl_uchar CL_ALIGNED(16) s[16];
#if defined(__GNUC__)
    __extension__ struct { cl_uchar x, y, z, w; };
    __extension__ struct { cl_uchar s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15; };
#endif
  };

  uchar16(){}

  uchar16(cl_uchar i0,cl_uchar i1,cl_uchar i2,cl_uchar i3,
          cl_uchar i4,cl_uchar i5,cl_uchar i6,cl_uchar i7,
          cl_uchar i8,cl_uchar i9,cl_uchar i10,cl_uchar i11,
          cl_uchar i12,cl_uchar i13,cl_uchar i14,cl_uchar i15
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;
    s[4] = i4;
    s[5] = i5;
    s[6] = i6;
    s[7] = i7;
    s[8] = i8;
    s[9] = i9;
    s[10] = i10;
    s[11] = i11;
    s[12] = i12;
    s[13] = i13;
    s[14] = i14;
    s[15] = i15;

  }

  uchar16(cl_uchar i0) {
  for (int i = 0; i < 16; ++i) {
		s[i] = i0;
	 }
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3, const uchar2& a4, const uchar2& a5, const uchar2& a6, const uchar2& a7) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a6.s[0];
    s[13] = a6.s[1];
    s[14] = a7.s[0];
    s[15] = a7.s[1];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3, const uchar2& a4, const uchar2& a5, const uchar4& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a6.s[0];
    s[13] = a6.s[1];
    s[14] = a6.s[2];
    s[15] = a6.s[3];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3, const uchar2& a4, const uchar3& a5, const uchar3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a5.s[2];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3, const uchar2& a4, const uchar4& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a5.s[2];
    s[13] = a5.s[3];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3, const uchar3& a4, const uchar2& a5, const uchar3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3, const uchar3& a4, const uchar3& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3, const uchar4& a4, const uchar2& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3, const uchar4& a4, const uchar4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3, const uchar8& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a4.s[4];
    s[13] = a4.s[5];
    s[14] = a4.s[6];
    s[15] = a4.s[7];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar3& a3, const uchar2& a4, const uchar2& a5, const uchar3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar3& a3, const uchar2& a4, const uchar3& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar3& a3, const uchar3& a4, const uchar2& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar3& a3, const uchar3& a4, const uchar4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar3& a3, const uchar4& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar4& a3, const uchar2& a4, const uchar2& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar4& a3, const uchar2& a4, const uchar4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar4& a3, const uchar3& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar4& a3, const uchar4& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar2& a2, const uchar8& a3, const uchar2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a3.s[4];
    s[11] = a3.s[5];
    s[12] = a3.s[6];
    s[13] = a3.s[7];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar3& a2, const uchar2& a3, const uchar2& a4, const uchar2& a5, const uchar3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar3& a2, const uchar2& a3, const uchar2& a4, const uchar3& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar3& a2, const uchar2& a3, const uchar3& a4, const uchar2& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar3& a2, const uchar2& a3, const uchar3& a4, const uchar4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar3& a2, const uchar2& a3, const uchar4& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar3& a2, const uchar3& a3, const uchar2& a4, const uchar2& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar3& a2, const uchar3& a3, const uchar2& a4, const uchar4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar3& a2, const uchar3& a3, const uchar3& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar3& a2, const uchar3& a3, const uchar4& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar3& a2, const uchar4& a3, const uchar2& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar3& a2, const uchar4& a3, const uchar3& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar4& a2, const uchar2& a3, const uchar2& a4, const uchar2& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar4& a2, const uchar2& a3, const uchar2& a4, const uchar4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar4& a2, const uchar2& a3, const uchar3& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar4& a2, const uchar2& a3, const uchar4& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar4& a2, const uchar3& a3, const uchar2& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar4& a2, const uchar3& a3, const uchar3& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar4& a2, const uchar4& a3, const uchar2& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar4& a2, const uchar4& a3, const uchar4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar4& a2, const uchar8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar8& a2, const uchar2& a3, const uchar2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a2.s[4];
    s[9] = a2.s[5];
    s[10] = a2.s[6];
    s[11] = a2.s[7];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uchar16(const uchar2& a0, const uchar2& a1, const uchar8& a2, const uchar4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a2.s[4];
    s[9] = a2.s[5];
    s[10] = a2.s[6];
    s[11] = a2.s[7];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar2& a2, const uchar2& a3, const uchar2& a4, const uchar2& a5, const uchar3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar2& a2, const uchar2& a3, const uchar2& a4, const uchar3& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar2& a2, const uchar2& a3, const uchar3& a4, const uchar2& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar2& a2, const uchar2& a3, const uchar3& a4, const uchar4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar2& a2, const uchar2& a3, const uchar4& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar2& a2, const uchar3& a3, const uchar2& a4, const uchar2& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar2& a2, const uchar3& a3, const uchar2& a4, const uchar4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar2& a2, const uchar3& a3, const uchar3& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar2& a2, const uchar3& a3, const uchar4& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar2& a2, const uchar4& a3, const uchar2& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar2& a2, const uchar4& a3, const uchar3& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar3& a2, const uchar2& a3, const uchar2& a4, const uchar2& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar3& a2, const uchar2& a3, const uchar2& a4, const uchar4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar3& a2, const uchar2& a3, const uchar3& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar3& a2, const uchar2& a3, const uchar4& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar3& a2, const uchar3& a3, const uchar2& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar3& a2, const uchar3& a3, const uchar3& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar3& a2, const uchar4& a3, const uchar2& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar3& a2, const uchar4& a3, const uchar4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar3& a2, const uchar8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar4& a2, const uchar2& a3, const uchar2& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar4& a2, const uchar2& a3, const uchar3& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar4& a2, const uchar3& a3, const uchar2& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar4& a2, const uchar3& a3, const uchar4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar4& a2, const uchar4& a3, const uchar3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uchar16(const uchar2& a0, const uchar3& a1, const uchar8& a2, const uchar3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a2.s[4];
    s[10] = a2.s[5];
    s[11] = a2.s[6];
    s[12] = a2.s[7];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  uchar16(const uchar2& a0, const uchar4& a1, const uchar2& a2, const uchar2& a3, const uchar2& a4, const uchar2& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar2& a0, const uchar4& a1, const uchar2& a2, const uchar2& a3, const uchar2& a4, const uchar4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uchar16(const uchar2& a0, const uchar4& a1, const uchar2& a2, const uchar2& a3, const uchar3& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar2& a0, const uchar4& a1, const uchar2& a2, const uchar2& a3, const uchar4& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar2& a0, const uchar4& a1, const uchar2& a2, const uchar3& a3, const uchar2& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar2& a0, const uchar4& a1, const uchar2& a2, const uchar3& a3, const uchar3& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar2& a0, const uchar4& a1, const uchar2& a2, const uchar4& a3, const uchar2& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar2& a0, const uchar4& a1, const uchar2& a2, const uchar4& a3, const uchar4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uchar16(const uchar2& a0, const uchar4& a1, const uchar2& a2, const uchar8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  uchar16(const uchar2& a0, const uchar4& a1, const uchar3& a2, const uchar2& a3, const uchar2& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar2& a0, const uchar4& a1, const uchar3& a2, const uchar2& a3, const uchar3& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar2& a0, const uchar4& a1, const uchar3& a2, const uchar3& a3, const uchar2& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar2& a0, const uchar4& a1, const uchar3& a2, const uchar3& a3, const uchar4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uchar16(const uchar2& a0, const uchar4& a1, const uchar3& a2, const uchar4& a3, const uchar3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uchar16(const uchar2& a0, const uchar4& a1, const uchar4& a2, const uchar2& a3, const uchar2& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar2& a0, const uchar4& a1, const uchar4& a2, const uchar2& a3, const uchar4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uchar16(const uchar2& a0, const uchar4& a1, const uchar4& a2, const uchar3& a3, const uchar3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uchar16(const uchar2& a0, const uchar4& a1, const uchar4& a2, const uchar4& a3, const uchar2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uchar16(const uchar2& a0, const uchar4& a1, const uchar8& a2, const uchar2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  uchar16(const uchar2& a0, const uchar8& a1, const uchar2& a2, const uchar2& a3, const uchar2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uchar16(const uchar2& a0, const uchar8& a1, const uchar2& a2, const uchar4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  uchar16(const uchar2& a0, const uchar8& a1, const uchar3& a2, const uchar3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  uchar16(const uchar2& a0, const uchar8& a1, const uchar4& a2, const uchar2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a2.s[3];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3, const uchar2& a4, const uchar2& a5, const uchar3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3, const uchar2& a4, const uchar3& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3, const uchar3& a4, const uchar2& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3, const uchar3& a4, const uchar4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3, const uchar4& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar2& a2, const uchar3& a3, const uchar2& a4, const uchar2& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar2& a2, const uchar3& a3, const uchar2& a4, const uchar4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar2& a2, const uchar3& a3, const uchar3& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar2& a2, const uchar3& a3, const uchar4& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar2& a2, const uchar4& a3, const uchar2& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar2& a2, const uchar4& a3, const uchar3& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar3& a2, const uchar2& a3, const uchar2& a4, const uchar2& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar3& a2, const uchar2& a3, const uchar2& a4, const uchar4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar3& a2, const uchar2& a3, const uchar3& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar3& a2, const uchar2& a3, const uchar4& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar3& a2, const uchar3& a3, const uchar2& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar3& a2, const uchar3& a3, const uchar3& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar3& a2, const uchar4& a3, const uchar2& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar3& a2, const uchar4& a3, const uchar4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar3& a2, const uchar8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar4& a2, const uchar2& a3, const uchar2& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar4& a2, const uchar2& a3, const uchar3& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar4& a2, const uchar3& a3, const uchar2& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar4& a2, const uchar3& a3, const uchar4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar4& a2, const uchar4& a3, const uchar3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uchar16(const uchar3& a0, const uchar2& a1, const uchar8& a2, const uchar3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a2.s[4];
    s[10] = a2.s[5];
    s[11] = a2.s[6];
    s[12] = a2.s[7];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  uchar16(const uchar3& a0, const uchar3& a1, const uchar2& a2, const uchar2& a3, const uchar2& a4, const uchar2& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar3& a0, const uchar3& a1, const uchar2& a2, const uchar2& a3, const uchar2& a4, const uchar4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uchar16(const uchar3& a0, const uchar3& a1, const uchar2& a2, const uchar2& a3, const uchar3& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar3& a0, const uchar3& a1, const uchar2& a2, const uchar2& a3, const uchar4& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar3& a0, const uchar3& a1, const uchar2& a2, const uchar3& a3, const uchar2& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar3& a0, const uchar3& a1, const uchar2& a2, const uchar3& a3, const uchar3& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar3& a0, const uchar3& a1, const uchar2& a2, const uchar4& a3, const uchar2& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar3& a0, const uchar3& a1, const uchar2& a2, const uchar4& a3, const uchar4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uchar16(const uchar3& a0, const uchar3& a1, const uchar2& a2, const uchar8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  uchar16(const uchar3& a0, const uchar3& a1, const uchar3& a2, const uchar2& a3, const uchar2& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar3& a0, const uchar3& a1, const uchar3& a2, const uchar2& a3, const uchar3& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar3& a0, const uchar3& a1, const uchar3& a2, const uchar3& a3, const uchar2& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar3& a0, const uchar3& a1, const uchar3& a2, const uchar3& a3, const uchar4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uchar16(const uchar3& a0, const uchar3& a1, const uchar3& a2, const uchar4& a3, const uchar3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uchar16(const uchar3& a0, const uchar3& a1, const uchar4& a2, const uchar2& a3, const uchar2& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar3& a0, const uchar3& a1, const uchar4& a2, const uchar2& a3, const uchar4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uchar16(const uchar3& a0, const uchar3& a1, const uchar4& a2, const uchar3& a3, const uchar3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uchar16(const uchar3& a0, const uchar3& a1, const uchar4& a2, const uchar4& a3, const uchar2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uchar16(const uchar3& a0, const uchar3& a1, const uchar8& a2, const uchar2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  uchar16(const uchar3& a0, const uchar4& a1, const uchar2& a2, const uchar2& a3, const uchar2& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar3& a0, const uchar4& a1, const uchar2& a2, const uchar2& a3, const uchar3& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar3& a0, const uchar4& a1, const uchar2& a2, const uchar3& a3, const uchar2& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar3& a0, const uchar4& a1, const uchar2& a2, const uchar3& a3, const uchar4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uchar16(const uchar3& a0, const uchar4& a1, const uchar2& a2, const uchar4& a3, const uchar3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uchar16(const uchar3& a0, const uchar4& a1, const uchar3& a2, const uchar2& a3, const uchar2& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar3& a0, const uchar4& a1, const uchar3& a2, const uchar2& a3, const uchar4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uchar16(const uchar3& a0, const uchar4& a1, const uchar3& a2, const uchar3& a3, const uchar3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uchar16(const uchar3& a0, const uchar4& a1, const uchar3& a2, const uchar4& a3, const uchar2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uchar16(const uchar3& a0, const uchar4& a1, const uchar4& a2, const uchar2& a3, const uchar3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uchar16(const uchar3& a0, const uchar4& a1, const uchar4& a2, const uchar3& a3, const uchar2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uchar16(const uchar3& a0, const uchar8& a1, const uchar2& a2, const uchar3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a1.s[4];
    s[8] = a1.s[5];
    s[9] = a1.s[6];
    s[10] = a1.s[7];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  uchar16(const uchar3& a0, const uchar8& a1, const uchar3& a2, const uchar2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a1.s[4];
    s[8] = a1.s[5];
    s[9] = a1.s[6];
    s[10] = a1.s[7];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a2.s[2];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  uchar16(const uchar4& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3, const uchar2& a4, const uchar2& a5, const uchar2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uchar16(const uchar4& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3, const uchar2& a4, const uchar4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uchar16(const uchar4& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3, const uchar3& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar4& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3, const uchar4& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar4& a0, const uchar2& a1, const uchar2& a2, const uchar3& a3, const uchar2& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar4& a0, const uchar2& a1, const uchar2& a2, const uchar3& a3, const uchar3& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar4& a0, const uchar2& a1, const uchar2& a2, const uchar4& a3, const uchar2& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar4& a0, const uchar2& a1, const uchar2& a2, const uchar4& a3, const uchar4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uchar16(const uchar4& a0, const uchar2& a1, const uchar2& a2, const uchar8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  uchar16(const uchar4& a0, const uchar2& a1, const uchar3& a2, const uchar2& a3, const uchar2& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar4& a0, const uchar2& a1, const uchar3& a2, const uchar2& a3, const uchar3& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar4& a0, const uchar2& a1, const uchar3& a2, const uchar3& a3, const uchar2& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar4& a0, const uchar2& a1, const uchar3& a2, const uchar3& a3, const uchar4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uchar16(const uchar4& a0, const uchar2& a1, const uchar3& a2, const uchar4& a3, const uchar3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uchar16(const uchar4& a0, const uchar2& a1, const uchar4& a2, const uchar2& a3, const uchar2& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar4& a0, const uchar2& a1, const uchar4& a2, const uchar2& a3, const uchar4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uchar16(const uchar4& a0, const uchar2& a1, const uchar4& a2, const uchar3& a3, const uchar3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uchar16(const uchar4& a0, const uchar2& a1, const uchar4& a2, const uchar4& a3, const uchar2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uchar16(const uchar4& a0, const uchar2& a1, const uchar8& a2, const uchar2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  uchar16(const uchar4& a0, const uchar3& a1, const uchar2& a2, const uchar2& a3, const uchar2& a4, const uchar3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uchar16(const uchar4& a0, const uchar3& a1, const uchar2& a2, const uchar2& a3, const uchar3& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar4& a0, const uchar3& a1, const uchar2& a2, const uchar3& a3, const uchar2& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar4& a0, const uchar3& a1, const uchar2& a2, const uchar3& a3, const uchar4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uchar16(const uchar4& a0, const uchar3& a1, const uchar2& a2, const uchar4& a3, const uchar3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uchar16(const uchar4& a0, const uchar3& a1, const uchar3& a2, const uchar2& a3, const uchar2& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar4& a0, const uchar3& a1, const uchar3& a2, const uchar2& a3, const uchar4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uchar16(const uchar4& a0, const uchar3& a1, const uchar3& a2, const uchar3& a3, const uchar3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uchar16(const uchar4& a0, const uchar3& a1, const uchar3& a2, const uchar4& a3, const uchar2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uchar16(const uchar4& a0, const uchar3& a1, const uchar4& a2, const uchar2& a3, const uchar3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uchar16(const uchar4& a0, const uchar3& a1, const uchar4& a2, const uchar3& a3, const uchar2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uchar16(const uchar4& a0, const uchar4& a1, const uchar2& a2, const uchar2& a3, const uchar2& a4, const uchar2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uchar16(const uchar4& a0, const uchar4& a1, const uchar2& a2, const uchar2& a3, const uchar4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uchar16(const uchar4& a0, const uchar4& a1, const uchar2& a2, const uchar3& a3, const uchar3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uchar16(const uchar4& a0, const uchar4& a1, const uchar2& a2, const uchar4& a3, const uchar2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uchar16(const uchar4& a0, const uchar4& a1, const uchar3& a2, const uchar2& a3, const uchar3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uchar16(const uchar4& a0, const uchar4& a1, const uchar3& a2, const uchar3& a3, const uchar2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uchar16(const uchar4& a0, const uchar4& a1, const uchar4& a2, const uchar2& a3, const uchar2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uchar16(const uchar4& a0, const uchar4& a1, const uchar4& a2, const uchar4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  uchar16(const uchar4& a0, const uchar4& a1, const uchar8& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a2.s[4];
    s[13] = a2.s[5];
    s[14] = a2.s[6];
    s[15] = a2.s[7];
  }

  uchar16(const uchar4& a0, const uchar8& a1, const uchar2& a2, const uchar2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a1.s[4];
    s[9] = a1.s[5];
    s[10] = a1.s[6];
    s[11] = a1.s[7];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  uchar16(const uchar4& a0, const uchar8& a1, const uchar4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a1.s[4];
    s[9] = a1.s[5];
    s[10] = a1.s[6];
    s[11] = a1.s[7];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a2.s[2];
    s[15] = a2.s[3];
  }

  uchar16(const uchar8& a0, const uchar2& a1, const uchar2& a2, const uchar2& a3, const uchar2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uchar16(const uchar8& a0, const uchar2& a1, const uchar2& a2, const uchar4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  uchar16(const uchar8& a0, const uchar2& a1, const uchar3& a2, const uchar3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  uchar16(const uchar8& a0, const uchar2& a1, const uchar4& a2, const uchar2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a2.s[3];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  uchar16(const uchar8& a0, const uchar3& a1, const uchar2& a2, const uchar3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  uchar16(const uchar8& a0, const uchar3& a1, const uchar3& a2, const uchar2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a2.s[2];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  uchar16(const uchar8& a0, const uchar4& a1, const uchar2& a2, const uchar2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  uchar16(const uchar8& a0, const uchar4& a1, const uchar4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a2.s[2];
    s[15] = a2.s[3];
  }

  uchar16(const uchar8& a0, const uchar8& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a1.s[4];
    s[13] = a1.s[5];
    s[14] = a1.s[6];
    s[15] = a1.s[7];
  }

  uchar16(const uchar16& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a0.s[8];
    s[9] = a0.s[9];
    s[10] = a0.s[10];
    s[11] = a0.s[11];
    s[12] = a0.s[12];
    s[13] = a0.s[13];
    s[14] = a0.s[14];
    s[15] = a0.s[15];
  }

  uchar16& operator+=(const uchar16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  uchar16& operator+=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  uchar16& operator-=(const uchar16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  uchar16& operator-=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  uchar16& operator*=(const uchar16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  uchar16& operator*=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  uchar16& operator/=(const uchar16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  uchar16& operator/=(cl_uchar rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  uchar16& operator%=(const uchar16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  uchar16& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  uchar16& operator&=(const uchar16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  uchar16& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  uchar16& operator|=(const uchar16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  uchar16& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  uchar16& operator^=(const uchar16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  uchar16& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  uchar16& operator<<=(const uchar16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  uchar16& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  uchar16& operator>>=(const uchar16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  uchar16& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline uchar16 operator+(uchar16 lhs, const uchar16& rhs)
{
  lhs += rhs;
  return lhs;
}

inline uchar16 operator+(uchar16 lhs, cl_uchar rhs)
{
  lhs += rhs;
  return lhs;
}

inline uchar16 operator+(cl_uchar lhs, const uchar16& rhs)
{
  auto tmp = uchar16(lhs);
  return tmp + rhs;
}

inline uchar16 operator-(uchar16 lhs, const uchar16& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uchar16 operator-(uchar16 lhs, cl_uchar rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uchar16 operator-(cl_uchar lhs, const uchar16& rhs)
{
  auto tmp = uchar16(lhs);
  return tmp - rhs;
}

inline uchar16 operator*(uchar16 lhs, const uchar16& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uchar16 operator*(uchar16 lhs, cl_uchar rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uchar16 operator*(cl_uchar lhs, const uchar16& rhs)
{
  auto tmp = uchar16(lhs);
  return tmp * rhs;
}

inline uchar16 operator/(uchar16 lhs, const uchar16& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uchar16 operator/(uchar16 lhs, cl_uchar rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uchar16 operator/(cl_uchar lhs, const uchar16& rhs)
{
  auto tmp = uchar16(lhs);
  return tmp / rhs;
}

inline uchar16 operator%(uchar16 lhs, const uchar16& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uchar16 operator%(uchar16 lhs, cl_uchar rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uchar16 operator%(cl_uchar lhs, const uchar16& rhs)
{
  auto tmp = uchar16(lhs);
  return tmp % rhs;
}

inline uchar16 operator&(uchar16 lhs, const uchar16& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uchar16 operator&(uchar16 lhs, cl_uchar rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uchar16 operator&(cl_uchar lhs, const uchar16& rhs)
{
  auto tmp = uchar16(lhs);
  return tmp & rhs;
}

inline uchar16 operator|(uchar16 lhs, const uchar16& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uchar16 operator|(uchar16 lhs, cl_uchar rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uchar16 operator|(cl_uchar lhs, const uchar16& rhs)
{
  auto tmp = uchar16(lhs);
  return tmp | rhs;
}

inline uchar16 operator^(uchar16 lhs, const uchar16& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uchar16 operator^(uchar16 lhs, cl_uchar rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uchar16 operator^(cl_uchar lhs, const uchar16& rhs)
{
  auto tmp = uchar16(lhs);
  return tmp ^ rhs;
}

inline uchar16 operator<<(uchar16 lhs, const uchar16& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uchar16 operator<<(uchar16 lhs, cl_uchar rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uchar16 operator<<(cl_uchar lhs, const uchar16& rhs)
{
  auto tmp = uchar16(lhs);
  return tmp << rhs;
}

inline uchar16 operator>>(uchar16 lhs, const uchar16& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uchar16 operator>>(uchar16 lhs, cl_uchar rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uchar16 operator>>(cl_uchar lhs, const uchar16& rhs)
{
  auto tmp = uchar16(lhs);
  return tmp >> rhs;
}

inline bool operator==(const uchar16& lhs, const uchar16& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]) &&
  (lhs.s[4] == rhs.s[4]) &&
  (lhs.s[5] == rhs.s[5]) &&
  (lhs.s[6] == rhs.s[6]) &&
  (lhs.s[7] == rhs.s[7]) &&
  (lhs.s[8] == rhs.s[8]) &&
  (lhs.s[9] == rhs.s[9]) &&
  (lhs.s[10] == rhs.s[10]) &&
  (lhs.s[11] == rhs.s[11]) &&
  (lhs.s[12] == rhs.s[12]) &&
  (lhs.s[13] == rhs.s[13]) &&
  (lhs.s[14] == rhs.s[14]) &&
  (lhs.s[15] == rhs.s[15]);
}

inline bool operator==(const uchar16& lhs, const cl_uchar& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs) &&
  (lhs.s[4] == rhs) &&
  (lhs.s[5] == rhs) &&
  (lhs.s[6] == rhs) &&
  (lhs.s[7] == rhs) &&
  (lhs.s[8] == rhs) &&
  (lhs.s[9] == rhs) &&
  (lhs.s[10] == rhs) &&
  (lhs.s[11] == rhs) &&
  (lhs.s[12] == rhs) &&
  (lhs.s[13] == rhs) &&
  (lhs.s[14] == rhs) &&
  (lhs.s[15] == rhs);
}

inline bool operator<(const uchar16& lhs, const uchar16& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const uchar16& lhs, const cl_uchar& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uchar16& lhs, const uchar16& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uchar16& lhs, const cl_uchar& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const uchar16& lhs, const uchar16& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const uchar16& lhs, const cl_uchar& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const uchar16& lhs, const uchar16& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const uchar16& lhs, const cl_uchar& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const uchar16& lhs, const uchar16& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const uchar16& lhs, const cl_uchar& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline uchar16 vload16(size_t offset, const uchar16* p)
{
  return *(p + offset);
}

inline void vstore16(uchar16 data, size_t offset, uchar16* p)
{
  *(p + offset) = data;
}


class short2{
public:
  static const size_t N = 2; // Number of elements in vector
  union {
	 cl_short CL_ALIGNED(4) s[2];
#if defined(__GNUC__)
    __extension__ struct { cl_short x, y; };
    __extension__ struct { cl_short s0, s1; };
#endif
  };

  short2(){}

  short2(cl_short i0,cl_short i1){
    s[0] = i0;
    s[1] = i1;

  }

  short2(cl_short i0) {
  for (int i = 0; i < 2; ++i) {
		s[i] = i0;
	 }
  }

  short2(const short2& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
  }

  short2& operator+=(const short2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  short2& operator+=(cl_short rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  short2& operator-=(const short2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  short2& operator-=(cl_short rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  short2& operator*=(const short2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  short2& operator*=(cl_short rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  short2& operator/=(const short2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  short2& operator/=(cl_short rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  short2& operator%=(const short2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  short2& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  short2& operator&=(const short2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  short2& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  short2& operator|=(const short2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  short2& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  short2& operator^=(const short2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  short2& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  short2& operator<<=(const short2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  short2& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  short2& operator>>=(const short2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  short2& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline short2 operator+(short2 lhs, const short2& rhs)
{
  lhs += rhs;
  return lhs;
}

inline short2 operator+(short2 lhs, cl_short rhs)
{
  lhs += rhs;
  return lhs;
}

inline short2 operator+(cl_short lhs, const short2& rhs)
{
  auto tmp = short2(lhs);
  return tmp + rhs;
}

inline short2 operator-(short2 lhs, const short2& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline short2 operator-(short2 lhs, cl_short rhs)
{
  lhs -= rhs;
  return lhs;
}

inline short2 operator-(cl_short lhs, const short2& rhs)
{
  auto tmp = short2(lhs);
  return tmp - rhs;
}

inline short2 operator*(short2 lhs, const short2& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline short2 operator*(short2 lhs, cl_short rhs)
{
  lhs *= rhs;
  return lhs;
}

inline short2 operator*(cl_short lhs, const short2& rhs)
{
  auto tmp = short2(lhs);
  return tmp * rhs;
}

inline short2 operator/(short2 lhs, const short2& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline short2 operator/(short2 lhs, cl_short rhs)
{
  lhs /= rhs;
  return lhs;
}

inline short2 operator/(cl_short lhs, const short2& rhs)
{
  auto tmp = short2(lhs);
  return tmp / rhs;
}

inline short2 operator%(short2 lhs, const short2& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline short2 operator%(short2 lhs, cl_short rhs)
{
  lhs %= rhs;
  return lhs;
}

inline short2 operator%(cl_short lhs, const short2& rhs)
{
  auto tmp = short2(lhs);
  return tmp % rhs;
}

inline short2 operator&(short2 lhs, const short2& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline short2 operator&(short2 lhs, cl_short rhs)
{
  lhs &= rhs;
  return lhs;
}

inline short2 operator&(cl_short lhs, const short2& rhs)
{
  auto tmp = short2(lhs);
  return tmp & rhs;
}

inline short2 operator|(short2 lhs, const short2& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline short2 operator|(short2 lhs, cl_short rhs)
{
  lhs |= rhs;
  return lhs;
}

inline short2 operator|(cl_short lhs, const short2& rhs)
{
  auto tmp = short2(lhs);
  return tmp | rhs;
}

inline short2 operator^(short2 lhs, const short2& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline short2 operator^(short2 lhs, cl_short rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline short2 operator^(cl_short lhs, const short2& rhs)
{
  auto tmp = short2(lhs);
  return tmp ^ rhs;
}

inline short2 operator<<(short2 lhs, const short2& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline short2 operator<<(short2 lhs, cl_short rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline short2 operator<<(cl_short lhs, const short2& rhs)
{
  auto tmp = short2(lhs);
  return tmp << rhs;
}

inline short2 operator>>(short2 lhs, const short2& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline short2 operator>>(short2 lhs, cl_short rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline short2 operator>>(cl_short lhs, const short2& rhs)
{
  auto tmp = short2(lhs);
  return tmp >> rhs;
}

inline bool operator==(const short2& lhs, const short2& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]);
}

inline bool operator==(const short2& lhs, const cl_short& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs);
}

inline bool operator<(const short2& lhs, const short2& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const short2& lhs, const cl_short& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const short2& lhs, const short2& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const short2& lhs, const cl_short& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const short2& lhs, const short2& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const short2& lhs, const cl_short& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const short2& lhs, const short2& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const short2& lhs, const cl_short& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const short2& lhs, const short2& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const short2& lhs, const cl_short& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline short2 vload2(size_t offset, const short2* p)
{
  return *(p + offset);
}

inline void vstore2(short2 data, size_t offset, short2* p)
{
  *(p + offset) = data;
}


class short3{
public:
  static const size_t N = 4; // Number of elements in vector
  union {
	 cl_short CL_ALIGNED(8) s[4];
#if defined(__GNUC__)
    __extension__ struct { cl_short x, y, z, w; };
    __extension__ struct { cl_short s0, s1, s2, s3; };
#endif
  };

  short3(){}

  short3(cl_short i0,cl_short i1,cl_short i2){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;

  }

  short3(cl_short i0) {
  for (int i = 0; i < 3; ++i) {
		s[i] = i0;
	 }
  }

  short3(const short3& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
  }

  short3& operator+=(const short3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  short3& operator+=(cl_short rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  short3& operator-=(const short3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  short3& operator-=(cl_short rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  short3& operator*=(const short3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  short3& operator*=(cl_short rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  short3& operator/=(const short3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  short3& operator/=(cl_short rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  short3& operator%=(const short3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  short3& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  short3& operator&=(const short3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  short3& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  short3& operator|=(const short3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  short3& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  short3& operator^=(const short3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  short3& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  short3& operator<<=(const short3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  short3& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  short3& operator>>=(const short3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  short3& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline short3 operator+(short3 lhs, const short3& rhs)
{
  lhs += rhs;
  return lhs;
}

inline short3 operator+(short3 lhs, cl_short rhs)
{
  lhs += rhs;
  return lhs;
}

inline short3 operator+(cl_short lhs, const short3& rhs)
{
  auto tmp = short3(lhs);
  return tmp + rhs;
}

inline short3 operator-(short3 lhs, const short3& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline short3 operator-(short3 lhs, cl_short rhs)
{
  lhs -= rhs;
  return lhs;
}

inline short3 operator-(cl_short lhs, const short3& rhs)
{
  auto tmp = short3(lhs);
  return tmp - rhs;
}

inline short3 operator*(short3 lhs, const short3& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline short3 operator*(short3 lhs, cl_short rhs)
{
  lhs *= rhs;
  return lhs;
}

inline short3 operator*(cl_short lhs, const short3& rhs)
{
  auto tmp = short3(lhs);
  return tmp * rhs;
}

inline short3 operator/(short3 lhs, const short3& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline short3 operator/(short3 lhs, cl_short rhs)
{
  lhs /= rhs;
  return lhs;
}

inline short3 operator/(cl_short lhs, const short3& rhs)
{
  auto tmp = short3(lhs);
  return tmp / rhs;
}

inline short3 operator%(short3 lhs, const short3& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline short3 operator%(short3 lhs, cl_short rhs)
{
  lhs %= rhs;
  return lhs;
}

inline short3 operator%(cl_short lhs, const short3& rhs)
{
  auto tmp = short3(lhs);
  return tmp % rhs;
}

inline short3 operator&(short3 lhs, const short3& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline short3 operator&(short3 lhs, cl_short rhs)
{
  lhs &= rhs;
  return lhs;
}

inline short3 operator&(cl_short lhs, const short3& rhs)
{
  auto tmp = short3(lhs);
  return tmp & rhs;
}

inline short3 operator|(short3 lhs, const short3& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline short3 operator|(short3 lhs, cl_short rhs)
{
  lhs |= rhs;
  return lhs;
}

inline short3 operator|(cl_short lhs, const short3& rhs)
{
  auto tmp = short3(lhs);
  return tmp | rhs;
}

inline short3 operator^(short3 lhs, const short3& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline short3 operator^(short3 lhs, cl_short rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline short3 operator^(cl_short lhs, const short3& rhs)
{
  auto tmp = short3(lhs);
  return tmp ^ rhs;
}

inline short3 operator<<(short3 lhs, const short3& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline short3 operator<<(short3 lhs, cl_short rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline short3 operator<<(cl_short lhs, const short3& rhs)
{
  auto tmp = short3(lhs);
  return tmp << rhs;
}

inline short3 operator>>(short3 lhs, const short3& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline short3 operator>>(short3 lhs, cl_short rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline short3 operator>>(cl_short lhs, const short3& rhs)
{
  auto tmp = short3(lhs);
  return tmp >> rhs;
}

inline bool operator==(const short3& lhs, const short3& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]);
}

inline bool operator==(const short3& lhs, const cl_short& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs);
}

inline bool operator<(const short3& lhs, const short3& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const short3& lhs, const cl_short& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const short3& lhs, const short3& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const short3& lhs, const cl_short& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const short3& lhs, const short3& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const short3& lhs, const cl_short& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const short3& lhs, const short3& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const short3& lhs, const cl_short& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const short3& lhs, const short3& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const short3& lhs, const cl_short& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline short3 vload3(size_t offset, const short3* p)
{
  return *(p + offset);
}

inline void vstore3(short3 data, size_t offset, short3* p)
{
  *(p + offset) = data;
}


class short4{
public:
  static const size_t N = 4; // Number of elements in vector
  union {
	 cl_short CL_ALIGNED(8) s[4];
#if defined(__GNUC__)
    __extension__ struct { cl_short x, y, z, w; };
    __extension__ struct { cl_short s0, s1, s2, s3; };
#endif
  };

  short4(){}

  short4(cl_short i0,cl_short i1,cl_short i2,cl_short i3
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;

  }

  short4(cl_short i0) {
  for (int i = 0; i < 4; ++i) {
		s[i] = i0;
	 }
  }

  short4(const short2& a0, const short2& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
  }

  short4(const short4& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
  }

  short4& operator+=(const short4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  short4& operator+=(cl_short rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  short4& operator-=(const short4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  short4& operator-=(cl_short rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  short4& operator*=(const short4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  short4& operator*=(cl_short rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  short4& operator/=(const short4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  short4& operator/=(cl_short rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  short4& operator%=(const short4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  short4& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  short4& operator&=(const short4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  short4& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  short4& operator|=(const short4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  short4& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  short4& operator^=(const short4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  short4& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  short4& operator<<=(const short4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  short4& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  short4& operator>>=(const short4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  short4& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline short4 operator+(short4 lhs, const short4& rhs)
{
  lhs += rhs;
  return lhs;
}

inline short4 operator+(short4 lhs, cl_short rhs)
{
  lhs += rhs;
  return lhs;
}

inline short4 operator+(cl_short lhs, const short4& rhs)
{
  auto tmp = short4(lhs);
  return tmp + rhs;
}

inline short4 operator-(short4 lhs, const short4& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline short4 operator-(short4 lhs, cl_short rhs)
{
  lhs -= rhs;
  return lhs;
}

inline short4 operator-(cl_short lhs, const short4& rhs)
{
  auto tmp = short4(lhs);
  return tmp - rhs;
}

inline short4 operator*(short4 lhs, const short4& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline short4 operator*(short4 lhs, cl_short rhs)
{
  lhs *= rhs;
  return lhs;
}

inline short4 operator*(cl_short lhs, const short4& rhs)
{
  auto tmp = short4(lhs);
  return tmp * rhs;
}

inline short4 operator/(short4 lhs, const short4& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline short4 operator/(short4 lhs, cl_short rhs)
{
  lhs /= rhs;
  return lhs;
}

inline short4 operator/(cl_short lhs, const short4& rhs)
{
  auto tmp = short4(lhs);
  return tmp / rhs;
}

inline short4 operator%(short4 lhs, const short4& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline short4 operator%(short4 lhs, cl_short rhs)
{
  lhs %= rhs;
  return lhs;
}

inline short4 operator%(cl_short lhs, const short4& rhs)
{
  auto tmp = short4(lhs);
  return tmp % rhs;
}

inline short4 operator&(short4 lhs, const short4& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline short4 operator&(short4 lhs, cl_short rhs)
{
  lhs &= rhs;
  return lhs;
}

inline short4 operator&(cl_short lhs, const short4& rhs)
{
  auto tmp = short4(lhs);
  return tmp & rhs;
}

inline short4 operator|(short4 lhs, const short4& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline short4 operator|(short4 lhs, cl_short rhs)
{
  lhs |= rhs;
  return lhs;
}

inline short4 operator|(cl_short lhs, const short4& rhs)
{
  auto tmp = short4(lhs);
  return tmp | rhs;
}

inline short4 operator^(short4 lhs, const short4& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline short4 operator^(short4 lhs, cl_short rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline short4 operator^(cl_short lhs, const short4& rhs)
{
  auto tmp = short4(lhs);
  return tmp ^ rhs;
}

inline short4 operator<<(short4 lhs, const short4& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline short4 operator<<(short4 lhs, cl_short rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline short4 operator<<(cl_short lhs, const short4& rhs)
{
  auto tmp = short4(lhs);
  return tmp << rhs;
}

inline short4 operator>>(short4 lhs, const short4& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline short4 operator>>(short4 lhs, cl_short rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline short4 operator>>(cl_short lhs, const short4& rhs)
{
  auto tmp = short4(lhs);
  return tmp >> rhs;
}

inline bool operator==(const short4& lhs, const short4& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]);
}

inline bool operator==(const short4& lhs, const cl_short& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs);
}

inline bool operator<(const short4& lhs, const short4& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const short4& lhs, const cl_short& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const short4& lhs, const short4& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const short4& lhs, const cl_short& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const short4& lhs, const short4& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const short4& lhs, const cl_short& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const short4& lhs, const short4& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const short4& lhs, const cl_short& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const short4& lhs, const short4& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const short4& lhs, const cl_short& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline short4 vload4(size_t offset, const short4* p)
{
  return *(p + offset);
}

inline void vstore4(short4 data, size_t offset, short4* p)
{
  *(p + offset) = data;
}


class short8{
public:
  static const size_t N = 8; // Number of elements in vector
  union {
	 cl_short CL_ALIGNED(16) s[8];
#if defined(__GNUC__)
    __extension__ struct { cl_short x, y, z, w; };
    __extension__ struct { cl_short s0, s1, s2, s3, s4, s5, s6, s7; };
#endif
  };

  short8(){}

  short8(cl_short i0,cl_short i1,cl_short i2,cl_short i3,
          cl_short i4,cl_short i5,cl_short i6,cl_short i7
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;
    s[4] = i4;
    s[5] = i5;
    s[6] = i6;
    s[7] = i7;

  }

  short8(cl_short i0) {
  for (int i = 0; i < 8; ++i) {
		s[i] = i0;
	 }
  }

  short8(const short2& a0, const short2& a1, const short2& a2, const short2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
  }

  short8(const short2& a0, const short2& a1, const short4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
  }

  short8(const short2& a0, const short3& a1, const short3& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
  }

  short8(const short2& a0, const short4& a1, const short2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  short8(const short3& a0, const short2& a1, const short3& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
  }

  short8(const short3& a0, const short3& a1, const short2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  short8(const short4& a0, const short2& a1, const short2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  short8(const short4& a0, const short4& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
  }

  short8(const short8& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
  }

  short8& operator+=(const short8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  short8& operator+=(cl_short rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  short8& operator-=(const short8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  short8& operator-=(cl_short rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  short8& operator*=(const short8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  short8& operator*=(cl_short rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  short8& operator/=(const short8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  short8& operator/=(cl_short rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  short8& operator%=(const short8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  short8& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  short8& operator&=(const short8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  short8& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  short8& operator|=(const short8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  short8& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  short8& operator^=(const short8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  short8& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  short8& operator<<=(const short8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  short8& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  short8& operator>>=(const short8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  short8& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline short8 operator+(short8 lhs, const short8& rhs)
{
  lhs += rhs;
  return lhs;
}

inline short8 operator+(short8 lhs, cl_short rhs)
{
  lhs += rhs;
  return lhs;
}

inline short8 operator+(cl_short lhs, const short8& rhs)
{
  auto tmp = short8(lhs);
  return tmp + rhs;
}

inline short8 operator-(short8 lhs, const short8& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline short8 operator-(short8 lhs, cl_short rhs)
{
  lhs -= rhs;
  return lhs;
}

inline short8 operator-(cl_short lhs, const short8& rhs)
{
  auto tmp = short8(lhs);
  return tmp - rhs;
}

inline short8 operator*(short8 lhs, const short8& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline short8 operator*(short8 lhs, cl_short rhs)
{
  lhs *= rhs;
  return lhs;
}

inline short8 operator*(cl_short lhs, const short8& rhs)
{
  auto tmp = short8(lhs);
  return tmp * rhs;
}

inline short8 operator/(short8 lhs, const short8& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline short8 operator/(short8 lhs, cl_short rhs)
{
  lhs /= rhs;
  return lhs;
}

inline short8 operator/(cl_short lhs, const short8& rhs)
{
  auto tmp = short8(lhs);
  return tmp / rhs;
}

inline short8 operator%(short8 lhs, const short8& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline short8 operator%(short8 lhs, cl_short rhs)
{
  lhs %= rhs;
  return lhs;
}

inline short8 operator%(cl_short lhs, const short8& rhs)
{
  auto tmp = short8(lhs);
  return tmp % rhs;
}

inline short8 operator&(short8 lhs, const short8& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline short8 operator&(short8 lhs, cl_short rhs)
{
  lhs &= rhs;
  return lhs;
}

inline short8 operator&(cl_short lhs, const short8& rhs)
{
  auto tmp = short8(lhs);
  return tmp & rhs;
}

inline short8 operator|(short8 lhs, const short8& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline short8 operator|(short8 lhs, cl_short rhs)
{
  lhs |= rhs;
  return lhs;
}

inline short8 operator|(cl_short lhs, const short8& rhs)
{
  auto tmp = short8(lhs);
  return tmp | rhs;
}

inline short8 operator^(short8 lhs, const short8& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline short8 operator^(short8 lhs, cl_short rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline short8 operator^(cl_short lhs, const short8& rhs)
{
  auto tmp = short8(lhs);
  return tmp ^ rhs;
}

inline short8 operator<<(short8 lhs, const short8& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline short8 operator<<(short8 lhs, cl_short rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline short8 operator<<(cl_short lhs, const short8& rhs)
{
  auto tmp = short8(lhs);
  return tmp << rhs;
}

inline short8 operator>>(short8 lhs, const short8& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline short8 operator>>(short8 lhs, cl_short rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline short8 operator>>(cl_short lhs, const short8& rhs)
{
  auto tmp = short8(lhs);
  return tmp >> rhs;
}

inline bool operator==(const short8& lhs, const short8& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]) &&
  (lhs.s[4] == rhs.s[4]) &&
  (lhs.s[5] == rhs.s[5]) &&
  (lhs.s[6] == rhs.s[6]) &&
  (lhs.s[7] == rhs.s[7]);
}

inline bool operator==(const short8& lhs, const cl_short& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs) &&
  (lhs.s[4] == rhs) &&
  (lhs.s[5] == rhs) &&
  (lhs.s[6] == rhs) &&
  (lhs.s[7] == rhs);
}

inline bool operator<(const short8& lhs, const short8& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const short8& lhs, const cl_short& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const short8& lhs, const short8& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const short8& lhs, const cl_short& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const short8& lhs, const short8& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const short8& lhs, const cl_short& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const short8& lhs, const short8& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const short8& lhs, const cl_short& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const short8& lhs, const short8& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const short8& lhs, const cl_short& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline short8 vload8(size_t offset, const short8* p)
{
  return *(p + offset);
}

inline void vstore8(short8 data, size_t offset, short8* p)
{
  *(p + offset) = data;
}


class short16{
public:
  static const size_t N = 16; // Number of elements in vector
  union {
	 cl_short CL_ALIGNED(32) s[16];
#if defined(__GNUC__)
    __extension__ struct { cl_short x, y, z, w; };
    __extension__ struct { cl_short s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15; };
#endif
  };

  short16(){}

  short16(cl_short i0,cl_short i1,cl_short i2,cl_short i3,
          cl_short i4,cl_short i5,cl_short i6,cl_short i7,
          cl_short i8,cl_short i9,cl_short i10,cl_short i11,
          cl_short i12,cl_short i13,cl_short i14,cl_short i15
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;
    s[4] = i4;
    s[5] = i5;
    s[6] = i6;
    s[7] = i7;
    s[8] = i8;
    s[9] = i9;
    s[10] = i10;
    s[11] = i11;
    s[12] = i12;
    s[13] = i13;
    s[14] = i14;
    s[15] = i15;

  }

  short16(cl_short i0) {
  for (int i = 0; i < 16; ++i) {
		s[i] = i0;
	 }
  }

  short16(const short2& a0, const short2& a1, const short2& a2, const short2& a3, const short2& a4, const short2& a5, const short2& a6, const short2& a7) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a6.s[0];
    s[13] = a6.s[1];
    s[14] = a7.s[0];
    s[15] = a7.s[1];
  }

  short16(const short2& a0, const short2& a1, const short2& a2, const short2& a3, const short2& a4, const short2& a5, const short4& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a6.s[0];
    s[13] = a6.s[1];
    s[14] = a6.s[2];
    s[15] = a6.s[3];
  }

  short16(const short2& a0, const short2& a1, const short2& a2, const short2& a3, const short2& a4, const short3& a5, const short3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a5.s[2];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  short16(const short2& a0, const short2& a1, const short2& a2, const short2& a3, const short2& a4, const short4& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a5.s[2];
    s[13] = a5.s[3];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short2& a0, const short2& a1, const short2& a2, const short2& a3, const short3& a4, const short2& a5, const short3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  short16(const short2& a0, const short2& a1, const short2& a2, const short2& a3, const short3& a4, const short3& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short2& a0, const short2& a1, const short2& a2, const short2& a3, const short4& a4, const short2& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short2& a0, const short2& a1, const short2& a2, const short2& a3, const short4& a4, const short4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  short16(const short2& a0, const short2& a1, const short2& a2, const short2& a3, const short8& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a4.s[4];
    s[13] = a4.s[5];
    s[14] = a4.s[6];
    s[15] = a4.s[7];
  }

  short16(const short2& a0, const short2& a1, const short2& a2, const short3& a3, const short2& a4, const short2& a5, const short3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  short16(const short2& a0, const short2& a1, const short2& a2, const short3& a3, const short2& a4, const short3& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short2& a0, const short2& a1, const short2& a2, const short3& a3, const short3& a4, const short2& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short2& a0, const short2& a1, const short2& a2, const short3& a3, const short3& a4, const short4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  short16(const short2& a0, const short2& a1, const short2& a2, const short3& a3, const short4& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short2& a0, const short2& a1, const short2& a2, const short4& a3, const short2& a4, const short2& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short2& a0, const short2& a1, const short2& a2, const short4& a3, const short2& a4, const short4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  short16(const short2& a0, const short2& a1, const short2& a2, const short4& a3, const short3& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short2& a0, const short2& a1, const short2& a2, const short4& a3, const short4& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short2& a0, const short2& a1, const short2& a2, const short8& a3, const short2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a3.s[4];
    s[11] = a3.s[5];
    s[12] = a3.s[6];
    s[13] = a3.s[7];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  short16(const short2& a0, const short2& a1, const short3& a2, const short2& a3, const short2& a4, const short2& a5, const short3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  short16(const short2& a0, const short2& a1, const short3& a2, const short2& a3, const short2& a4, const short3& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short2& a0, const short2& a1, const short3& a2, const short2& a3, const short3& a4, const short2& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short2& a0, const short2& a1, const short3& a2, const short2& a3, const short3& a4, const short4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  short16(const short2& a0, const short2& a1, const short3& a2, const short2& a3, const short4& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short2& a0, const short2& a1, const short3& a2, const short3& a3, const short2& a4, const short2& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short2& a0, const short2& a1, const short3& a2, const short3& a3, const short2& a4, const short4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  short16(const short2& a0, const short2& a1, const short3& a2, const short3& a3, const short3& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short2& a0, const short2& a1, const short3& a2, const short3& a3, const short4& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short2& a0, const short2& a1, const short3& a2, const short4& a3, const short2& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short2& a0, const short2& a1, const short3& a2, const short4& a3, const short3& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short2& a0, const short2& a1, const short4& a2, const short2& a3, const short2& a4, const short2& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short2& a0, const short2& a1, const short4& a2, const short2& a3, const short2& a4, const short4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  short16(const short2& a0, const short2& a1, const short4& a2, const short2& a3, const short3& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short2& a0, const short2& a1, const short4& a2, const short2& a3, const short4& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short2& a0, const short2& a1, const short4& a2, const short3& a3, const short2& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short2& a0, const short2& a1, const short4& a2, const short3& a3, const short3& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short2& a0, const short2& a1, const short4& a2, const short4& a3, const short2& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short2& a0, const short2& a1, const short4& a2, const short4& a3, const short4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  short16(const short2& a0, const short2& a1, const short4& a2, const short8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  short16(const short2& a0, const short2& a1, const short8& a2, const short2& a3, const short2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a2.s[4];
    s[9] = a2.s[5];
    s[10] = a2.s[6];
    s[11] = a2.s[7];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  short16(const short2& a0, const short2& a1, const short8& a2, const short4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a2.s[4];
    s[9] = a2.s[5];
    s[10] = a2.s[6];
    s[11] = a2.s[7];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  short16(const short2& a0, const short3& a1, const short2& a2, const short2& a3, const short2& a4, const short2& a5, const short3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  short16(const short2& a0, const short3& a1, const short2& a2, const short2& a3, const short2& a4, const short3& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short2& a0, const short3& a1, const short2& a2, const short2& a3, const short3& a4, const short2& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short2& a0, const short3& a1, const short2& a2, const short2& a3, const short3& a4, const short4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  short16(const short2& a0, const short3& a1, const short2& a2, const short2& a3, const short4& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short2& a0, const short3& a1, const short2& a2, const short3& a3, const short2& a4, const short2& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short2& a0, const short3& a1, const short2& a2, const short3& a3, const short2& a4, const short4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  short16(const short2& a0, const short3& a1, const short2& a2, const short3& a3, const short3& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short2& a0, const short3& a1, const short2& a2, const short3& a3, const short4& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short2& a0, const short3& a1, const short2& a2, const short4& a3, const short2& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short2& a0, const short3& a1, const short2& a2, const short4& a3, const short3& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short2& a0, const short3& a1, const short3& a2, const short2& a3, const short2& a4, const short2& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short2& a0, const short3& a1, const short3& a2, const short2& a3, const short2& a4, const short4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  short16(const short2& a0, const short3& a1, const short3& a2, const short2& a3, const short3& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short2& a0, const short3& a1, const short3& a2, const short2& a3, const short4& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short2& a0, const short3& a1, const short3& a2, const short3& a3, const short2& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short2& a0, const short3& a1, const short3& a2, const short3& a3, const short3& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short2& a0, const short3& a1, const short3& a2, const short4& a3, const short2& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short2& a0, const short3& a1, const short3& a2, const short4& a3, const short4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  short16(const short2& a0, const short3& a1, const short3& a2, const short8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  short16(const short2& a0, const short3& a1, const short4& a2, const short2& a3, const short2& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short2& a0, const short3& a1, const short4& a2, const short2& a3, const short3& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short2& a0, const short3& a1, const short4& a2, const short3& a3, const short2& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short2& a0, const short3& a1, const short4& a2, const short3& a3, const short4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  short16(const short2& a0, const short3& a1, const short4& a2, const short4& a3, const short3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  short16(const short2& a0, const short3& a1, const short8& a2, const short3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a2.s[4];
    s[10] = a2.s[5];
    s[11] = a2.s[6];
    s[12] = a2.s[7];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  short16(const short2& a0, const short4& a1, const short2& a2, const short2& a3, const short2& a4, const short2& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short2& a0, const short4& a1, const short2& a2, const short2& a3, const short2& a4, const short4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  short16(const short2& a0, const short4& a1, const short2& a2, const short2& a3, const short3& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short2& a0, const short4& a1, const short2& a2, const short2& a3, const short4& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short2& a0, const short4& a1, const short2& a2, const short3& a3, const short2& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short2& a0, const short4& a1, const short2& a2, const short3& a3, const short3& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short2& a0, const short4& a1, const short2& a2, const short4& a3, const short2& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short2& a0, const short4& a1, const short2& a2, const short4& a3, const short4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  short16(const short2& a0, const short4& a1, const short2& a2, const short8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  short16(const short2& a0, const short4& a1, const short3& a2, const short2& a3, const short2& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short2& a0, const short4& a1, const short3& a2, const short2& a3, const short3& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short2& a0, const short4& a1, const short3& a2, const short3& a3, const short2& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short2& a0, const short4& a1, const short3& a2, const short3& a3, const short4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  short16(const short2& a0, const short4& a1, const short3& a2, const short4& a3, const short3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  short16(const short2& a0, const short4& a1, const short4& a2, const short2& a3, const short2& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short2& a0, const short4& a1, const short4& a2, const short2& a3, const short4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  short16(const short2& a0, const short4& a1, const short4& a2, const short3& a3, const short3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  short16(const short2& a0, const short4& a1, const short4& a2, const short4& a3, const short2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  short16(const short2& a0, const short4& a1, const short8& a2, const short2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  short16(const short2& a0, const short8& a1, const short2& a2, const short2& a3, const short2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  short16(const short2& a0, const short8& a1, const short2& a2, const short4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  short16(const short2& a0, const short8& a1, const short3& a2, const short3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  short16(const short2& a0, const short8& a1, const short4& a2, const short2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a2.s[3];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  short16(const short3& a0, const short2& a1, const short2& a2, const short2& a3, const short2& a4, const short2& a5, const short3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  short16(const short3& a0, const short2& a1, const short2& a2, const short2& a3, const short2& a4, const short3& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short3& a0, const short2& a1, const short2& a2, const short2& a3, const short3& a4, const short2& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short3& a0, const short2& a1, const short2& a2, const short2& a3, const short3& a4, const short4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  short16(const short3& a0, const short2& a1, const short2& a2, const short2& a3, const short4& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short3& a0, const short2& a1, const short2& a2, const short3& a3, const short2& a4, const short2& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short3& a0, const short2& a1, const short2& a2, const short3& a3, const short2& a4, const short4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  short16(const short3& a0, const short2& a1, const short2& a2, const short3& a3, const short3& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short3& a0, const short2& a1, const short2& a2, const short3& a3, const short4& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short3& a0, const short2& a1, const short2& a2, const short4& a3, const short2& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short3& a0, const short2& a1, const short2& a2, const short4& a3, const short3& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short3& a0, const short2& a1, const short3& a2, const short2& a3, const short2& a4, const short2& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short3& a0, const short2& a1, const short3& a2, const short2& a3, const short2& a4, const short4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  short16(const short3& a0, const short2& a1, const short3& a2, const short2& a3, const short3& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short3& a0, const short2& a1, const short3& a2, const short2& a3, const short4& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short3& a0, const short2& a1, const short3& a2, const short3& a3, const short2& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short3& a0, const short2& a1, const short3& a2, const short3& a3, const short3& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short3& a0, const short2& a1, const short3& a2, const short4& a3, const short2& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short3& a0, const short2& a1, const short3& a2, const short4& a3, const short4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  short16(const short3& a0, const short2& a1, const short3& a2, const short8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  short16(const short3& a0, const short2& a1, const short4& a2, const short2& a3, const short2& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short3& a0, const short2& a1, const short4& a2, const short2& a3, const short3& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short3& a0, const short2& a1, const short4& a2, const short3& a3, const short2& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short3& a0, const short2& a1, const short4& a2, const short3& a3, const short4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  short16(const short3& a0, const short2& a1, const short4& a2, const short4& a3, const short3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  short16(const short3& a0, const short2& a1, const short8& a2, const short3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a2.s[4];
    s[10] = a2.s[5];
    s[11] = a2.s[6];
    s[12] = a2.s[7];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  short16(const short3& a0, const short3& a1, const short2& a2, const short2& a3, const short2& a4, const short2& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short3& a0, const short3& a1, const short2& a2, const short2& a3, const short2& a4, const short4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  short16(const short3& a0, const short3& a1, const short2& a2, const short2& a3, const short3& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short3& a0, const short3& a1, const short2& a2, const short2& a3, const short4& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short3& a0, const short3& a1, const short2& a2, const short3& a3, const short2& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short3& a0, const short3& a1, const short2& a2, const short3& a3, const short3& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short3& a0, const short3& a1, const short2& a2, const short4& a3, const short2& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short3& a0, const short3& a1, const short2& a2, const short4& a3, const short4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  short16(const short3& a0, const short3& a1, const short2& a2, const short8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  short16(const short3& a0, const short3& a1, const short3& a2, const short2& a3, const short2& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short3& a0, const short3& a1, const short3& a2, const short2& a3, const short3& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short3& a0, const short3& a1, const short3& a2, const short3& a3, const short2& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short3& a0, const short3& a1, const short3& a2, const short3& a3, const short4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  short16(const short3& a0, const short3& a1, const short3& a2, const short4& a3, const short3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  short16(const short3& a0, const short3& a1, const short4& a2, const short2& a3, const short2& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short3& a0, const short3& a1, const short4& a2, const short2& a3, const short4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  short16(const short3& a0, const short3& a1, const short4& a2, const short3& a3, const short3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  short16(const short3& a0, const short3& a1, const short4& a2, const short4& a3, const short2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  short16(const short3& a0, const short3& a1, const short8& a2, const short2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  short16(const short3& a0, const short4& a1, const short2& a2, const short2& a3, const short2& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short3& a0, const short4& a1, const short2& a2, const short2& a3, const short3& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short3& a0, const short4& a1, const short2& a2, const short3& a3, const short2& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short3& a0, const short4& a1, const short2& a2, const short3& a3, const short4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  short16(const short3& a0, const short4& a1, const short2& a2, const short4& a3, const short3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  short16(const short3& a0, const short4& a1, const short3& a2, const short2& a3, const short2& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short3& a0, const short4& a1, const short3& a2, const short2& a3, const short4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  short16(const short3& a0, const short4& a1, const short3& a2, const short3& a3, const short3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  short16(const short3& a0, const short4& a1, const short3& a2, const short4& a3, const short2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  short16(const short3& a0, const short4& a1, const short4& a2, const short2& a3, const short3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  short16(const short3& a0, const short4& a1, const short4& a2, const short3& a3, const short2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  short16(const short3& a0, const short8& a1, const short2& a2, const short3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a1.s[4];
    s[8] = a1.s[5];
    s[9] = a1.s[6];
    s[10] = a1.s[7];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  short16(const short3& a0, const short8& a1, const short3& a2, const short2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a1.s[4];
    s[8] = a1.s[5];
    s[9] = a1.s[6];
    s[10] = a1.s[7];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a2.s[2];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  short16(const short4& a0, const short2& a1, const short2& a2, const short2& a3, const short2& a4, const short2& a5, const short2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  short16(const short4& a0, const short2& a1, const short2& a2, const short2& a3, const short2& a4, const short4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  short16(const short4& a0, const short2& a1, const short2& a2, const short2& a3, const short3& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short4& a0, const short2& a1, const short2& a2, const short2& a3, const short4& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short4& a0, const short2& a1, const short2& a2, const short3& a3, const short2& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short4& a0, const short2& a1, const short2& a2, const short3& a3, const short3& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short4& a0, const short2& a1, const short2& a2, const short4& a3, const short2& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short4& a0, const short2& a1, const short2& a2, const short4& a3, const short4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  short16(const short4& a0, const short2& a1, const short2& a2, const short8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  short16(const short4& a0, const short2& a1, const short3& a2, const short2& a3, const short2& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short4& a0, const short2& a1, const short3& a2, const short2& a3, const short3& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short4& a0, const short2& a1, const short3& a2, const short3& a3, const short2& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short4& a0, const short2& a1, const short3& a2, const short3& a3, const short4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  short16(const short4& a0, const short2& a1, const short3& a2, const short4& a3, const short3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  short16(const short4& a0, const short2& a1, const short4& a2, const short2& a3, const short2& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short4& a0, const short2& a1, const short4& a2, const short2& a3, const short4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  short16(const short4& a0, const short2& a1, const short4& a2, const short3& a3, const short3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  short16(const short4& a0, const short2& a1, const short4& a2, const short4& a3, const short2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  short16(const short4& a0, const short2& a1, const short8& a2, const short2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  short16(const short4& a0, const short3& a1, const short2& a2, const short2& a3, const short2& a4, const short3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  short16(const short4& a0, const short3& a1, const short2& a2, const short2& a3, const short3& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short4& a0, const short3& a1, const short2& a2, const short3& a3, const short2& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short4& a0, const short3& a1, const short2& a2, const short3& a3, const short4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  short16(const short4& a0, const short3& a1, const short2& a2, const short4& a3, const short3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  short16(const short4& a0, const short3& a1, const short3& a2, const short2& a3, const short2& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short4& a0, const short3& a1, const short3& a2, const short2& a3, const short4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  short16(const short4& a0, const short3& a1, const short3& a2, const short3& a3, const short3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  short16(const short4& a0, const short3& a1, const short3& a2, const short4& a3, const short2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  short16(const short4& a0, const short3& a1, const short4& a2, const short2& a3, const short3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  short16(const short4& a0, const short3& a1, const short4& a2, const short3& a3, const short2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  short16(const short4& a0, const short4& a1, const short2& a2, const short2& a3, const short2& a4, const short2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  short16(const short4& a0, const short4& a1, const short2& a2, const short2& a3, const short4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  short16(const short4& a0, const short4& a1, const short2& a2, const short3& a3, const short3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  short16(const short4& a0, const short4& a1, const short2& a2, const short4& a3, const short2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  short16(const short4& a0, const short4& a1, const short3& a2, const short2& a3, const short3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  short16(const short4& a0, const short4& a1, const short3& a2, const short3& a3, const short2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  short16(const short4& a0, const short4& a1, const short4& a2, const short2& a3, const short2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  short16(const short4& a0, const short4& a1, const short4& a2, const short4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  short16(const short4& a0, const short4& a1, const short8& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a2.s[4];
    s[13] = a2.s[5];
    s[14] = a2.s[6];
    s[15] = a2.s[7];
  }

  short16(const short4& a0, const short8& a1, const short2& a2, const short2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a1.s[4];
    s[9] = a1.s[5];
    s[10] = a1.s[6];
    s[11] = a1.s[7];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  short16(const short4& a0, const short8& a1, const short4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a1.s[4];
    s[9] = a1.s[5];
    s[10] = a1.s[6];
    s[11] = a1.s[7];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a2.s[2];
    s[15] = a2.s[3];
  }

  short16(const short8& a0, const short2& a1, const short2& a2, const short2& a3, const short2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  short16(const short8& a0, const short2& a1, const short2& a2, const short4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  short16(const short8& a0, const short2& a1, const short3& a2, const short3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  short16(const short8& a0, const short2& a1, const short4& a2, const short2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a2.s[3];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  short16(const short8& a0, const short3& a1, const short2& a2, const short3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  short16(const short8& a0, const short3& a1, const short3& a2, const short2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a2.s[2];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  short16(const short8& a0, const short4& a1, const short2& a2, const short2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  short16(const short8& a0, const short4& a1, const short4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a2.s[2];
    s[15] = a2.s[3];
  }

  short16(const short8& a0, const short8& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a1.s[4];
    s[13] = a1.s[5];
    s[14] = a1.s[6];
    s[15] = a1.s[7];
  }

  short16(const short16& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a0.s[8];
    s[9] = a0.s[9];
    s[10] = a0.s[10];
    s[11] = a0.s[11];
    s[12] = a0.s[12];
    s[13] = a0.s[13];
    s[14] = a0.s[14];
    s[15] = a0.s[15];
  }

  short16& operator+=(const short16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  short16& operator+=(cl_short rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  short16& operator-=(const short16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  short16& operator-=(cl_short rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  short16& operator*=(const short16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  short16& operator*=(cl_short rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  short16& operator/=(const short16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  short16& operator/=(cl_short rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  short16& operator%=(const short16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  short16& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  short16& operator&=(const short16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  short16& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  short16& operator|=(const short16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  short16& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  short16& operator^=(const short16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  short16& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  short16& operator<<=(const short16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  short16& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  short16& operator>>=(const short16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  short16& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline short16 operator+(short16 lhs, const short16& rhs)
{
  lhs += rhs;
  return lhs;
}

inline short16 operator+(short16 lhs, cl_short rhs)
{
  lhs += rhs;
  return lhs;
}

inline short16 operator+(cl_short lhs, const short16& rhs)
{
  auto tmp = short16(lhs);
  return tmp + rhs;
}

inline short16 operator-(short16 lhs, const short16& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline short16 operator-(short16 lhs, cl_short rhs)
{
  lhs -= rhs;
  return lhs;
}

inline short16 operator-(cl_short lhs, const short16& rhs)
{
  auto tmp = short16(lhs);
  return tmp - rhs;
}

inline short16 operator*(short16 lhs, const short16& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline short16 operator*(short16 lhs, cl_short rhs)
{
  lhs *= rhs;
  return lhs;
}

inline short16 operator*(cl_short lhs, const short16& rhs)
{
  auto tmp = short16(lhs);
  return tmp * rhs;
}

inline short16 operator/(short16 lhs, const short16& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline short16 operator/(short16 lhs, cl_short rhs)
{
  lhs /= rhs;
  return lhs;
}

inline short16 operator/(cl_short lhs, const short16& rhs)
{
  auto tmp = short16(lhs);
  return tmp / rhs;
}

inline short16 operator%(short16 lhs, const short16& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline short16 operator%(short16 lhs, cl_short rhs)
{
  lhs %= rhs;
  return lhs;
}

inline short16 operator%(cl_short lhs, const short16& rhs)
{
  auto tmp = short16(lhs);
  return tmp % rhs;
}

inline short16 operator&(short16 lhs, const short16& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline short16 operator&(short16 lhs, cl_short rhs)
{
  lhs &= rhs;
  return lhs;
}

inline short16 operator&(cl_short lhs, const short16& rhs)
{
  auto tmp = short16(lhs);
  return tmp & rhs;
}

inline short16 operator|(short16 lhs, const short16& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline short16 operator|(short16 lhs, cl_short rhs)
{
  lhs |= rhs;
  return lhs;
}

inline short16 operator|(cl_short lhs, const short16& rhs)
{
  auto tmp = short16(lhs);
  return tmp | rhs;
}

inline short16 operator^(short16 lhs, const short16& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline short16 operator^(short16 lhs, cl_short rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline short16 operator^(cl_short lhs, const short16& rhs)
{
  auto tmp = short16(lhs);
  return tmp ^ rhs;
}

inline short16 operator<<(short16 lhs, const short16& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline short16 operator<<(short16 lhs, cl_short rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline short16 operator<<(cl_short lhs, const short16& rhs)
{
  auto tmp = short16(lhs);
  return tmp << rhs;
}

inline short16 operator>>(short16 lhs, const short16& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline short16 operator>>(short16 lhs, cl_short rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline short16 operator>>(cl_short lhs, const short16& rhs)
{
  auto tmp = short16(lhs);
  return tmp >> rhs;
}

inline bool operator==(const short16& lhs, const short16& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]) &&
  (lhs.s[4] == rhs.s[4]) &&
  (lhs.s[5] == rhs.s[5]) &&
  (lhs.s[6] == rhs.s[6]) &&
  (lhs.s[7] == rhs.s[7]) &&
  (lhs.s[8] == rhs.s[8]) &&
  (lhs.s[9] == rhs.s[9]) &&
  (lhs.s[10] == rhs.s[10]) &&
  (lhs.s[11] == rhs.s[11]) &&
  (lhs.s[12] == rhs.s[12]) &&
  (lhs.s[13] == rhs.s[13]) &&
  (lhs.s[14] == rhs.s[14]) &&
  (lhs.s[15] == rhs.s[15]);
}

inline bool operator==(const short16& lhs, const cl_short& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs) &&
  (lhs.s[4] == rhs) &&
  (lhs.s[5] == rhs) &&
  (lhs.s[6] == rhs) &&
  (lhs.s[7] == rhs) &&
  (lhs.s[8] == rhs) &&
  (lhs.s[9] == rhs) &&
  (lhs.s[10] == rhs) &&
  (lhs.s[11] == rhs) &&
  (lhs.s[12] == rhs) &&
  (lhs.s[13] == rhs) &&
  (lhs.s[14] == rhs) &&
  (lhs.s[15] == rhs);
}

inline bool operator<(const short16& lhs, const short16& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const short16& lhs, const cl_short& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const short16& lhs, const short16& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const short16& lhs, const cl_short& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const short16& lhs, const short16& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const short16& lhs, const cl_short& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const short16& lhs, const short16& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const short16& lhs, const cl_short& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const short16& lhs, const short16& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const short16& lhs, const cl_short& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline short16 vload16(size_t offset, const short16* p)
{
  return *(p + offset);
}

inline void vstore16(short16 data, size_t offset, short16* p)
{
  *(p + offset) = data;
}


class ushort2{
public:
  static const size_t N = 2; // Number of elements in vector
  union {
	 cl_ushort CL_ALIGNED(4) s[2];
#if defined(__GNUC__)
    __extension__ struct { cl_ushort x, y; };
    __extension__ struct { cl_ushort s0, s1; };
#endif
  };

  ushort2(){}

  ushort2(cl_ushort i0,cl_ushort i1){
    s[0] = i0;
    s[1] = i1;

  }

  ushort2(cl_ushort i0) {
  for (int i = 0; i < 2; ++i) {
		s[i] = i0;
	 }
  }

  ushort2(const ushort2& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
  }

  ushort2& operator+=(const ushort2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  ushort2& operator+=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  ushort2& operator-=(const ushort2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  ushort2& operator-=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  ushort2& operator*=(const ushort2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  ushort2& operator*=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  ushort2& operator/=(const ushort2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  ushort2& operator/=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  ushort2& operator%=(const ushort2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  ushort2& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  ushort2& operator&=(const ushort2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  ushort2& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  ushort2& operator|=(const ushort2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  ushort2& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  ushort2& operator^=(const ushort2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  ushort2& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  ushort2& operator<<=(const ushort2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  ushort2& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  ushort2& operator>>=(const ushort2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  ushort2& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline ushort2 operator+(ushort2 lhs, const ushort2& rhs)
{
  lhs += rhs;
  return lhs;
}

inline ushort2 operator+(ushort2 lhs, cl_ushort rhs)
{
  lhs += rhs;
  return lhs;
}

inline ushort2 operator+(cl_ushort lhs, const ushort2& rhs)
{
  auto tmp = ushort2(lhs);
  return tmp + rhs;
}

inline ushort2 operator-(ushort2 lhs, const ushort2& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline ushort2 operator-(ushort2 lhs, cl_ushort rhs)
{
  lhs -= rhs;
  return lhs;
}

inline ushort2 operator-(cl_ushort lhs, const ushort2& rhs)
{
  auto tmp = ushort2(lhs);
  return tmp - rhs;
}

inline ushort2 operator*(ushort2 lhs, const ushort2& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline ushort2 operator*(ushort2 lhs, cl_ushort rhs)
{
  lhs *= rhs;
  return lhs;
}

inline ushort2 operator*(cl_ushort lhs, const ushort2& rhs)
{
  auto tmp = ushort2(lhs);
  return tmp * rhs;
}

inline ushort2 operator/(ushort2 lhs, const ushort2& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline ushort2 operator/(ushort2 lhs, cl_ushort rhs)
{
  lhs /= rhs;
  return lhs;
}

inline ushort2 operator/(cl_ushort lhs, const ushort2& rhs)
{
  auto tmp = ushort2(lhs);
  return tmp / rhs;
}

inline ushort2 operator%(ushort2 lhs, const ushort2& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline ushort2 operator%(ushort2 lhs, cl_ushort rhs)
{
  lhs %= rhs;
  return lhs;
}

inline ushort2 operator%(cl_ushort lhs, const ushort2& rhs)
{
  auto tmp = ushort2(lhs);
  return tmp % rhs;
}

inline ushort2 operator&(ushort2 lhs, const ushort2& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline ushort2 operator&(ushort2 lhs, cl_ushort rhs)
{
  lhs &= rhs;
  return lhs;
}

inline ushort2 operator&(cl_ushort lhs, const ushort2& rhs)
{
  auto tmp = ushort2(lhs);
  return tmp & rhs;
}

inline ushort2 operator|(ushort2 lhs, const ushort2& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline ushort2 operator|(ushort2 lhs, cl_ushort rhs)
{
  lhs |= rhs;
  return lhs;
}

inline ushort2 operator|(cl_ushort lhs, const ushort2& rhs)
{
  auto tmp = ushort2(lhs);
  return tmp | rhs;
}

inline ushort2 operator^(ushort2 lhs, const ushort2& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline ushort2 operator^(ushort2 lhs, cl_ushort rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline ushort2 operator^(cl_ushort lhs, const ushort2& rhs)
{
  auto tmp = ushort2(lhs);
  return tmp ^ rhs;
}

inline ushort2 operator<<(ushort2 lhs, const ushort2& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline ushort2 operator<<(ushort2 lhs, cl_ushort rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline ushort2 operator<<(cl_ushort lhs, const ushort2& rhs)
{
  auto tmp = ushort2(lhs);
  return tmp << rhs;
}

inline ushort2 operator>>(ushort2 lhs, const ushort2& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline ushort2 operator>>(ushort2 lhs, cl_ushort rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline ushort2 operator>>(cl_ushort lhs, const ushort2& rhs)
{
  auto tmp = ushort2(lhs);
  return tmp >> rhs;
}

inline bool operator==(const ushort2& lhs, const ushort2& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]);
}

inline bool operator==(const ushort2& lhs, const cl_ushort& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs);
}

inline bool operator<(const ushort2& lhs, const ushort2& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const ushort2& lhs, const cl_ushort& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const ushort2& lhs, const ushort2& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const ushort2& lhs, const cl_ushort& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const ushort2& lhs, const ushort2& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const ushort2& lhs, const cl_ushort& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const ushort2& lhs, const ushort2& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const ushort2& lhs, const cl_ushort& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const ushort2& lhs, const ushort2& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const ushort2& lhs, const cl_ushort& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline ushort2 vload2(size_t offset, const ushort2* p)
{
  return *(p + offset);
}

inline void vstore2(ushort2 data, size_t offset, ushort2* p)
{
  *(p + offset) = data;
}


class ushort3{
public:
  static const size_t N = 4; // Number of elements in vector
  union {
	 cl_ushort CL_ALIGNED(8) s[4];
#if defined(__GNUC__)
    __extension__ struct { cl_ushort x, y, z, w; };
    __extension__ struct { cl_ushort s0, s1, s2, s3; };
#endif
  };

  ushort3(){}

  ushort3(cl_ushort i0,cl_ushort i1,cl_ushort i2){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;

  }

  ushort3(cl_ushort i0) {
  for (int i = 0; i < 3; ++i) {
		s[i] = i0;
	 }
  }

  ushort3(const ushort3& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
  }

  ushort3& operator+=(const ushort3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  ushort3& operator+=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  ushort3& operator-=(const ushort3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  ushort3& operator-=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  ushort3& operator*=(const ushort3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  ushort3& operator*=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  ushort3& operator/=(const ushort3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  ushort3& operator/=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  ushort3& operator%=(const ushort3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  ushort3& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  ushort3& operator&=(const ushort3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  ushort3& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  ushort3& operator|=(const ushort3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  ushort3& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  ushort3& operator^=(const ushort3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  ushort3& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  ushort3& operator<<=(const ushort3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  ushort3& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  ushort3& operator>>=(const ushort3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  ushort3& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline ushort3 operator+(ushort3 lhs, const ushort3& rhs)
{
  lhs += rhs;
  return lhs;
}

inline ushort3 operator+(ushort3 lhs, cl_ushort rhs)
{
  lhs += rhs;
  return lhs;
}

inline ushort3 operator+(cl_ushort lhs, const ushort3& rhs)
{
  auto tmp = ushort3(lhs);
  return tmp + rhs;
}

inline ushort3 operator-(ushort3 lhs, const ushort3& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline ushort3 operator-(ushort3 lhs, cl_ushort rhs)
{
  lhs -= rhs;
  return lhs;
}

inline ushort3 operator-(cl_ushort lhs, const ushort3& rhs)
{
  auto tmp = ushort3(lhs);
  return tmp - rhs;
}

inline ushort3 operator*(ushort3 lhs, const ushort3& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline ushort3 operator*(ushort3 lhs, cl_ushort rhs)
{
  lhs *= rhs;
  return lhs;
}

inline ushort3 operator*(cl_ushort lhs, const ushort3& rhs)
{
  auto tmp = ushort3(lhs);
  return tmp * rhs;
}

inline ushort3 operator/(ushort3 lhs, const ushort3& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline ushort3 operator/(ushort3 lhs, cl_ushort rhs)
{
  lhs /= rhs;
  return lhs;
}

inline ushort3 operator/(cl_ushort lhs, const ushort3& rhs)
{
  auto tmp = ushort3(lhs);
  return tmp / rhs;
}

inline ushort3 operator%(ushort3 lhs, const ushort3& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline ushort3 operator%(ushort3 lhs, cl_ushort rhs)
{
  lhs %= rhs;
  return lhs;
}

inline ushort3 operator%(cl_ushort lhs, const ushort3& rhs)
{
  auto tmp = ushort3(lhs);
  return tmp % rhs;
}

inline ushort3 operator&(ushort3 lhs, const ushort3& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline ushort3 operator&(ushort3 lhs, cl_ushort rhs)
{
  lhs &= rhs;
  return lhs;
}

inline ushort3 operator&(cl_ushort lhs, const ushort3& rhs)
{
  auto tmp = ushort3(lhs);
  return tmp & rhs;
}

inline ushort3 operator|(ushort3 lhs, const ushort3& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline ushort3 operator|(ushort3 lhs, cl_ushort rhs)
{
  lhs |= rhs;
  return lhs;
}

inline ushort3 operator|(cl_ushort lhs, const ushort3& rhs)
{
  auto tmp = ushort3(lhs);
  return tmp | rhs;
}

inline ushort3 operator^(ushort3 lhs, const ushort3& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline ushort3 operator^(ushort3 lhs, cl_ushort rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline ushort3 operator^(cl_ushort lhs, const ushort3& rhs)
{
  auto tmp = ushort3(lhs);
  return tmp ^ rhs;
}

inline ushort3 operator<<(ushort3 lhs, const ushort3& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline ushort3 operator<<(ushort3 lhs, cl_ushort rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline ushort3 operator<<(cl_ushort lhs, const ushort3& rhs)
{
  auto tmp = ushort3(lhs);
  return tmp << rhs;
}

inline ushort3 operator>>(ushort3 lhs, const ushort3& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline ushort3 operator>>(ushort3 lhs, cl_ushort rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline ushort3 operator>>(cl_ushort lhs, const ushort3& rhs)
{
  auto tmp = ushort3(lhs);
  return tmp >> rhs;
}

inline bool operator==(const ushort3& lhs, const ushort3& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]);
}

inline bool operator==(const ushort3& lhs, const cl_ushort& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs);
}

inline bool operator<(const ushort3& lhs, const ushort3& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const ushort3& lhs, const cl_ushort& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const ushort3& lhs, const ushort3& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const ushort3& lhs, const cl_ushort& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const ushort3& lhs, const ushort3& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const ushort3& lhs, const cl_ushort& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const ushort3& lhs, const ushort3& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const ushort3& lhs, const cl_ushort& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const ushort3& lhs, const ushort3& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const ushort3& lhs, const cl_ushort& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline ushort3 vload3(size_t offset, const ushort3* p)
{
  return *(p + offset);
}

inline void vstore3(ushort3 data, size_t offset, ushort3* p)
{
  *(p + offset) = data;
}


class ushort4{
public:
  static const size_t N = 4; // Number of elements in vector
  union {
	 cl_ushort CL_ALIGNED(8) s[4];
#if defined(__GNUC__)
    __extension__ struct { cl_ushort x, y, z, w; };
    __extension__ struct { cl_ushort s0, s1, s2, s3; };
#endif
  };

  ushort4(){}

  ushort4(cl_ushort i0,cl_ushort i1,cl_ushort i2,cl_ushort i3
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;

  }

  ushort4(cl_ushort i0) {
  for (int i = 0; i < 4; ++i) {
		s[i] = i0;
	 }
  }

  ushort4(const ushort2& a0, const ushort2& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
  }

  ushort4(const ushort4& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
  }

  ushort4& operator+=(const ushort4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  ushort4& operator+=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  ushort4& operator-=(const ushort4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  ushort4& operator-=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  ushort4& operator*=(const ushort4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  ushort4& operator*=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  ushort4& operator/=(const ushort4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  ushort4& operator/=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  ushort4& operator%=(const ushort4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  ushort4& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  ushort4& operator&=(const ushort4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  ushort4& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  ushort4& operator|=(const ushort4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  ushort4& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  ushort4& operator^=(const ushort4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  ushort4& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  ushort4& operator<<=(const ushort4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  ushort4& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  ushort4& operator>>=(const ushort4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  ushort4& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline ushort4 operator+(ushort4 lhs, const ushort4& rhs)
{
  lhs += rhs;
  return lhs;
}

inline ushort4 operator+(ushort4 lhs, cl_ushort rhs)
{
  lhs += rhs;
  return lhs;
}

inline ushort4 operator+(cl_ushort lhs, const ushort4& rhs)
{
  auto tmp = ushort4(lhs);
  return tmp + rhs;
}

inline ushort4 operator-(ushort4 lhs, const ushort4& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline ushort4 operator-(ushort4 lhs, cl_ushort rhs)
{
  lhs -= rhs;
  return lhs;
}

inline ushort4 operator-(cl_ushort lhs, const ushort4& rhs)
{
  auto tmp = ushort4(lhs);
  return tmp - rhs;
}

inline ushort4 operator*(ushort4 lhs, const ushort4& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline ushort4 operator*(ushort4 lhs, cl_ushort rhs)
{
  lhs *= rhs;
  return lhs;
}

inline ushort4 operator*(cl_ushort lhs, const ushort4& rhs)
{
  auto tmp = ushort4(lhs);
  return tmp * rhs;
}

inline ushort4 operator/(ushort4 lhs, const ushort4& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline ushort4 operator/(ushort4 lhs, cl_ushort rhs)
{
  lhs /= rhs;
  return lhs;
}

inline ushort4 operator/(cl_ushort lhs, const ushort4& rhs)
{
  auto tmp = ushort4(lhs);
  return tmp / rhs;
}

inline ushort4 operator%(ushort4 lhs, const ushort4& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline ushort4 operator%(ushort4 lhs, cl_ushort rhs)
{
  lhs %= rhs;
  return lhs;
}

inline ushort4 operator%(cl_ushort lhs, const ushort4& rhs)
{
  auto tmp = ushort4(lhs);
  return tmp % rhs;
}

inline ushort4 operator&(ushort4 lhs, const ushort4& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline ushort4 operator&(ushort4 lhs, cl_ushort rhs)
{
  lhs &= rhs;
  return lhs;
}

inline ushort4 operator&(cl_ushort lhs, const ushort4& rhs)
{
  auto tmp = ushort4(lhs);
  return tmp & rhs;
}

inline ushort4 operator|(ushort4 lhs, const ushort4& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline ushort4 operator|(ushort4 lhs, cl_ushort rhs)
{
  lhs |= rhs;
  return lhs;
}

inline ushort4 operator|(cl_ushort lhs, const ushort4& rhs)
{
  auto tmp = ushort4(lhs);
  return tmp | rhs;
}

inline ushort4 operator^(ushort4 lhs, const ushort4& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline ushort4 operator^(ushort4 lhs, cl_ushort rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline ushort4 operator^(cl_ushort lhs, const ushort4& rhs)
{
  auto tmp = ushort4(lhs);
  return tmp ^ rhs;
}

inline ushort4 operator<<(ushort4 lhs, const ushort4& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline ushort4 operator<<(ushort4 lhs, cl_ushort rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline ushort4 operator<<(cl_ushort lhs, const ushort4& rhs)
{
  auto tmp = ushort4(lhs);
  return tmp << rhs;
}

inline ushort4 operator>>(ushort4 lhs, const ushort4& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline ushort4 operator>>(ushort4 lhs, cl_ushort rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline ushort4 operator>>(cl_ushort lhs, const ushort4& rhs)
{
  auto tmp = ushort4(lhs);
  return tmp >> rhs;
}

inline bool operator==(const ushort4& lhs, const ushort4& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]);
}

inline bool operator==(const ushort4& lhs, const cl_ushort& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs);
}

inline bool operator<(const ushort4& lhs, const ushort4& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const ushort4& lhs, const cl_ushort& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const ushort4& lhs, const ushort4& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const ushort4& lhs, const cl_ushort& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const ushort4& lhs, const ushort4& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const ushort4& lhs, const cl_ushort& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const ushort4& lhs, const ushort4& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const ushort4& lhs, const cl_ushort& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const ushort4& lhs, const ushort4& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const ushort4& lhs, const cl_ushort& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline ushort4 vload4(size_t offset, const ushort4* p)
{
  return *(p + offset);
}

inline void vstore4(ushort4 data, size_t offset, ushort4* p)
{
  *(p + offset) = data;
}


class ushort8{
public:
  static const size_t N = 8; // Number of elements in vector
  union {
	 cl_ushort CL_ALIGNED(16) s[8];
#if defined(__GNUC__)
    __extension__ struct { cl_ushort x, y, z, w; };
    __extension__ struct { cl_ushort s0, s1, s2, s3, s4, s5, s6, s7; };
#endif
  };

  ushort8(){}

  ushort8(cl_ushort i0,cl_ushort i1,cl_ushort i2,cl_ushort i3,
          cl_ushort i4,cl_ushort i5,cl_ushort i6,cl_ushort i7
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;
    s[4] = i4;
    s[5] = i5;
    s[6] = i6;
    s[7] = i7;

  }

  ushort8(cl_ushort i0) {
  for (int i = 0; i < 8; ++i) {
		s[i] = i0;
	 }
  }

  ushort8(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
  }

  ushort8(const ushort2& a0, const ushort2& a1, const ushort4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
  }

  ushort8(const ushort2& a0, const ushort3& a1, const ushort3& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
  }

  ushort8(const ushort2& a0, const ushort4& a1, const ushort2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  ushort8(const ushort3& a0, const ushort2& a1, const ushort3& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
  }

  ushort8(const ushort3& a0, const ushort3& a1, const ushort2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  ushort8(const ushort4& a0, const ushort2& a1, const ushort2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  ushort8(const ushort4& a0, const ushort4& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
  }

  ushort8(const ushort8& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
  }

  ushort8& operator+=(const ushort8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  ushort8& operator+=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  ushort8& operator-=(const ushort8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  ushort8& operator-=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  ushort8& operator*=(const ushort8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  ushort8& operator*=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  ushort8& operator/=(const ushort8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  ushort8& operator/=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  ushort8& operator%=(const ushort8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  ushort8& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  ushort8& operator&=(const ushort8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  ushort8& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  ushort8& operator|=(const ushort8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  ushort8& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  ushort8& operator^=(const ushort8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  ushort8& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  ushort8& operator<<=(const ushort8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  ushort8& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  ushort8& operator>>=(const ushort8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  ushort8& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline ushort8 operator+(ushort8 lhs, const ushort8& rhs)
{
  lhs += rhs;
  return lhs;
}

inline ushort8 operator+(ushort8 lhs, cl_ushort rhs)
{
  lhs += rhs;
  return lhs;
}

inline ushort8 operator+(cl_ushort lhs, const ushort8& rhs)
{
  auto tmp = ushort8(lhs);
  return tmp + rhs;
}

inline ushort8 operator-(ushort8 lhs, const ushort8& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline ushort8 operator-(ushort8 lhs, cl_ushort rhs)
{
  lhs -= rhs;
  return lhs;
}

inline ushort8 operator-(cl_ushort lhs, const ushort8& rhs)
{
  auto tmp = ushort8(lhs);
  return tmp - rhs;
}

inline ushort8 operator*(ushort8 lhs, const ushort8& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline ushort8 operator*(ushort8 lhs, cl_ushort rhs)
{
  lhs *= rhs;
  return lhs;
}

inline ushort8 operator*(cl_ushort lhs, const ushort8& rhs)
{
  auto tmp = ushort8(lhs);
  return tmp * rhs;
}

inline ushort8 operator/(ushort8 lhs, const ushort8& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline ushort8 operator/(ushort8 lhs, cl_ushort rhs)
{
  lhs /= rhs;
  return lhs;
}

inline ushort8 operator/(cl_ushort lhs, const ushort8& rhs)
{
  auto tmp = ushort8(lhs);
  return tmp / rhs;
}

inline ushort8 operator%(ushort8 lhs, const ushort8& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline ushort8 operator%(ushort8 lhs, cl_ushort rhs)
{
  lhs %= rhs;
  return lhs;
}

inline ushort8 operator%(cl_ushort lhs, const ushort8& rhs)
{
  auto tmp = ushort8(lhs);
  return tmp % rhs;
}

inline ushort8 operator&(ushort8 lhs, const ushort8& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline ushort8 operator&(ushort8 lhs, cl_ushort rhs)
{
  lhs &= rhs;
  return lhs;
}

inline ushort8 operator&(cl_ushort lhs, const ushort8& rhs)
{
  auto tmp = ushort8(lhs);
  return tmp & rhs;
}

inline ushort8 operator|(ushort8 lhs, const ushort8& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline ushort8 operator|(ushort8 lhs, cl_ushort rhs)
{
  lhs |= rhs;
  return lhs;
}

inline ushort8 operator|(cl_ushort lhs, const ushort8& rhs)
{
  auto tmp = ushort8(lhs);
  return tmp | rhs;
}

inline ushort8 operator^(ushort8 lhs, const ushort8& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline ushort8 operator^(ushort8 lhs, cl_ushort rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline ushort8 operator^(cl_ushort lhs, const ushort8& rhs)
{
  auto tmp = ushort8(lhs);
  return tmp ^ rhs;
}

inline ushort8 operator<<(ushort8 lhs, const ushort8& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline ushort8 operator<<(ushort8 lhs, cl_ushort rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline ushort8 operator<<(cl_ushort lhs, const ushort8& rhs)
{
  auto tmp = ushort8(lhs);
  return tmp << rhs;
}

inline ushort8 operator>>(ushort8 lhs, const ushort8& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline ushort8 operator>>(ushort8 lhs, cl_ushort rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline ushort8 operator>>(cl_ushort lhs, const ushort8& rhs)
{
  auto tmp = ushort8(lhs);
  return tmp >> rhs;
}

inline bool operator==(const ushort8& lhs, const ushort8& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]) &&
  (lhs.s[4] == rhs.s[4]) &&
  (lhs.s[5] == rhs.s[5]) &&
  (lhs.s[6] == rhs.s[6]) &&
  (lhs.s[7] == rhs.s[7]);
}

inline bool operator==(const ushort8& lhs, const cl_ushort& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs) &&
  (lhs.s[4] == rhs) &&
  (lhs.s[5] == rhs) &&
  (lhs.s[6] == rhs) &&
  (lhs.s[7] == rhs);
}

inline bool operator<(const ushort8& lhs, const ushort8& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const ushort8& lhs, const cl_ushort& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const ushort8& lhs, const ushort8& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const ushort8& lhs, const cl_ushort& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const ushort8& lhs, const ushort8& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const ushort8& lhs, const cl_ushort& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const ushort8& lhs, const ushort8& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const ushort8& lhs, const cl_ushort& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const ushort8& lhs, const ushort8& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const ushort8& lhs, const cl_ushort& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline ushort8 vload8(size_t offset, const ushort8* p)
{
  return *(p + offset);
}

inline void vstore8(ushort8 data, size_t offset, ushort8* p)
{
  *(p + offset) = data;
}


class ushort16{
public:
  static const size_t N = 16; // Number of elements in vector
  union {
	 cl_ushort CL_ALIGNED(32) s[16];
#if defined(__GNUC__)
    __extension__ struct { cl_ushort x, y, z, w; };
    __extension__ struct { cl_ushort s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15; };
#endif
  };

  ushort16(){}

  ushort16(cl_ushort i0,cl_ushort i1,cl_ushort i2,cl_ushort i3,
          cl_ushort i4,cl_ushort i5,cl_ushort i6,cl_ushort i7,
          cl_ushort i8,cl_ushort i9,cl_ushort i10,cl_ushort i11,
          cl_ushort i12,cl_ushort i13,cl_ushort i14,cl_ushort i15
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;
    s[4] = i4;
    s[5] = i5;
    s[6] = i6;
    s[7] = i7;
    s[8] = i8;
    s[9] = i9;
    s[10] = i10;
    s[11] = i11;
    s[12] = i12;
    s[13] = i13;
    s[14] = i14;
    s[15] = i15;

  }

  ushort16(cl_ushort i0) {
  for (int i = 0; i < 16; ++i) {
		s[i] = i0;
	 }
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3, const ushort2& a4, const ushort2& a5, const ushort2& a6, const ushort2& a7) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a6.s[0];
    s[13] = a6.s[1];
    s[14] = a7.s[0];
    s[15] = a7.s[1];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3, const ushort2& a4, const ushort2& a5, const ushort4& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a6.s[0];
    s[13] = a6.s[1];
    s[14] = a6.s[2];
    s[15] = a6.s[3];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3, const ushort2& a4, const ushort3& a5, const ushort3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a5.s[2];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3, const ushort2& a4, const ushort4& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a5.s[2];
    s[13] = a5.s[3];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3, const ushort3& a4, const ushort2& a5, const ushort3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3, const ushort3& a4, const ushort3& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3, const ushort4& a4, const ushort2& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3, const ushort4& a4, const ushort4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3, const ushort8& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a4.s[4];
    s[13] = a4.s[5];
    s[14] = a4.s[6];
    s[15] = a4.s[7];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort3& a3, const ushort2& a4, const ushort2& a5, const ushort3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort3& a3, const ushort2& a4, const ushort3& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort3& a3, const ushort3& a4, const ushort2& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort3& a3, const ushort3& a4, const ushort4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort3& a3, const ushort4& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort4& a3, const ushort2& a4, const ushort2& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort4& a3, const ushort2& a4, const ushort4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort4& a3, const ushort3& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort4& a3, const ushort4& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort2& a2, const ushort8& a3, const ushort2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a3.s[4];
    s[11] = a3.s[5];
    s[12] = a3.s[6];
    s[13] = a3.s[7];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort3& a2, const ushort2& a3, const ushort2& a4, const ushort2& a5, const ushort3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort3& a2, const ushort2& a3, const ushort2& a4, const ushort3& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort3& a2, const ushort2& a3, const ushort3& a4, const ushort2& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort3& a2, const ushort2& a3, const ushort3& a4, const ushort4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort3& a2, const ushort2& a3, const ushort4& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort3& a2, const ushort3& a3, const ushort2& a4, const ushort2& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort3& a2, const ushort3& a3, const ushort2& a4, const ushort4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort3& a2, const ushort3& a3, const ushort3& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort3& a2, const ushort3& a3, const ushort4& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort3& a2, const ushort4& a3, const ushort2& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort3& a2, const ushort4& a3, const ushort3& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort4& a2, const ushort2& a3, const ushort2& a4, const ushort2& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort4& a2, const ushort2& a3, const ushort2& a4, const ushort4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort4& a2, const ushort2& a3, const ushort3& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort4& a2, const ushort2& a3, const ushort4& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort4& a2, const ushort3& a3, const ushort2& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort4& a2, const ushort3& a3, const ushort3& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort4& a2, const ushort4& a3, const ushort2& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort4& a2, const ushort4& a3, const ushort4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort4& a2, const ushort8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort8& a2, const ushort2& a3, const ushort2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a2.s[4];
    s[9] = a2.s[5];
    s[10] = a2.s[6];
    s[11] = a2.s[7];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  ushort16(const ushort2& a0, const ushort2& a1, const ushort8& a2, const ushort4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a2.s[4];
    s[9] = a2.s[5];
    s[10] = a2.s[6];
    s[11] = a2.s[7];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort2& a2, const ushort2& a3, const ushort2& a4, const ushort2& a5, const ushort3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort2& a2, const ushort2& a3, const ushort2& a4, const ushort3& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort2& a2, const ushort2& a3, const ushort3& a4, const ushort2& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort2& a2, const ushort2& a3, const ushort3& a4, const ushort4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort2& a2, const ushort2& a3, const ushort4& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort2& a2, const ushort3& a3, const ushort2& a4, const ushort2& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort2& a2, const ushort3& a3, const ushort2& a4, const ushort4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort2& a2, const ushort3& a3, const ushort3& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort2& a2, const ushort3& a3, const ushort4& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort2& a2, const ushort4& a3, const ushort2& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort2& a2, const ushort4& a3, const ushort3& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort3& a2, const ushort2& a3, const ushort2& a4, const ushort2& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort3& a2, const ushort2& a3, const ushort2& a4, const ushort4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort3& a2, const ushort2& a3, const ushort3& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort3& a2, const ushort2& a3, const ushort4& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort3& a2, const ushort3& a3, const ushort2& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort3& a2, const ushort3& a3, const ushort3& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort3& a2, const ushort4& a3, const ushort2& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort3& a2, const ushort4& a3, const ushort4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort3& a2, const ushort8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort4& a2, const ushort2& a3, const ushort2& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort4& a2, const ushort2& a3, const ushort3& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort4& a2, const ushort3& a3, const ushort2& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort4& a2, const ushort3& a3, const ushort4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort4& a2, const ushort4& a3, const ushort3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  ushort16(const ushort2& a0, const ushort3& a1, const ushort8& a2, const ushort3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a2.s[4];
    s[10] = a2.s[5];
    s[11] = a2.s[6];
    s[12] = a2.s[7];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  ushort16(const ushort2& a0, const ushort4& a1, const ushort2& a2, const ushort2& a3, const ushort2& a4, const ushort2& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort2& a0, const ushort4& a1, const ushort2& a2, const ushort2& a3, const ushort2& a4, const ushort4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ushort16(const ushort2& a0, const ushort4& a1, const ushort2& a2, const ushort2& a3, const ushort3& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort2& a0, const ushort4& a1, const ushort2& a2, const ushort2& a3, const ushort4& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort2& a0, const ushort4& a1, const ushort2& a2, const ushort3& a3, const ushort2& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort2& a0, const ushort4& a1, const ushort2& a2, const ushort3& a3, const ushort3& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort2& a0, const ushort4& a1, const ushort2& a2, const ushort4& a3, const ushort2& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort2& a0, const ushort4& a1, const ushort2& a2, const ushort4& a3, const ushort4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ushort16(const ushort2& a0, const ushort4& a1, const ushort2& a2, const ushort8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  ushort16(const ushort2& a0, const ushort4& a1, const ushort3& a2, const ushort2& a3, const ushort2& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort2& a0, const ushort4& a1, const ushort3& a2, const ushort2& a3, const ushort3& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort2& a0, const ushort4& a1, const ushort3& a2, const ushort3& a3, const ushort2& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort2& a0, const ushort4& a1, const ushort3& a2, const ushort3& a3, const ushort4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ushort16(const ushort2& a0, const ushort4& a1, const ushort3& a2, const ushort4& a3, const ushort3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  ushort16(const ushort2& a0, const ushort4& a1, const ushort4& a2, const ushort2& a3, const ushort2& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort2& a0, const ushort4& a1, const ushort4& a2, const ushort2& a3, const ushort4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ushort16(const ushort2& a0, const ushort4& a1, const ushort4& a2, const ushort3& a3, const ushort3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  ushort16(const ushort2& a0, const ushort4& a1, const ushort4& a2, const ushort4& a3, const ushort2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  ushort16(const ushort2& a0, const ushort4& a1, const ushort8& a2, const ushort2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  ushort16(const ushort2& a0, const ushort8& a1, const ushort2& a2, const ushort2& a3, const ushort2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  ushort16(const ushort2& a0, const ushort8& a1, const ushort2& a2, const ushort4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  ushort16(const ushort2& a0, const ushort8& a1, const ushort3& a2, const ushort3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  ushort16(const ushort2& a0, const ushort8& a1, const ushort4& a2, const ushort2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a2.s[3];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3, const ushort2& a4, const ushort2& a5, const ushort3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3, const ushort2& a4, const ushort3& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3, const ushort3& a4, const ushort2& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3, const ushort3& a4, const ushort4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3, const ushort4& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort2& a2, const ushort3& a3, const ushort2& a4, const ushort2& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort2& a2, const ushort3& a3, const ushort2& a4, const ushort4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort2& a2, const ushort3& a3, const ushort3& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort2& a2, const ushort3& a3, const ushort4& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort2& a2, const ushort4& a3, const ushort2& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort2& a2, const ushort4& a3, const ushort3& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort3& a2, const ushort2& a3, const ushort2& a4, const ushort2& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort3& a2, const ushort2& a3, const ushort2& a4, const ushort4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort3& a2, const ushort2& a3, const ushort3& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort3& a2, const ushort2& a3, const ushort4& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort3& a2, const ushort3& a3, const ushort2& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort3& a2, const ushort3& a3, const ushort3& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort3& a2, const ushort4& a3, const ushort2& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort3& a2, const ushort4& a3, const ushort4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort3& a2, const ushort8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort4& a2, const ushort2& a3, const ushort2& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort4& a2, const ushort2& a3, const ushort3& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort4& a2, const ushort3& a3, const ushort2& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort4& a2, const ushort3& a3, const ushort4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort4& a2, const ushort4& a3, const ushort3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  ushort16(const ushort3& a0, const ushort2& a1, const ushort8& a2, const ushort3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a2.s[4];
    s[10] = a2.s[5];
    s[11] = a2.s[6];
    s[12] = a2.s[7];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  ushort16(const ushort3& a0, const ushort3& a1, const ushort2& a2, const ushort2& a3, const ushort2& a4, const ushort2& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort3& a0, const ushort3& a1, const ushort2& a2, const ushort2& a3, const ushort2& a4, const ushort4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ushort16(const ushort3& a0, const ushort3& a1, const ushort2& a2, const ushort2& a3, const ushort3& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort3& a0, const ushort3& a1, const ushort2& a2, const ushort2& a3, const ushort4& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort3& a0, const ushort3& a1, const ushort2& a2, const ushort3& a3, const ushort2& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort3& a0, const ushort3& a1, const ushort2& a2, const ushort3& a3, const ushort3& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort3& a0, const ushort3& a1, const ushort2& a2, const ushort4& a3, const ushort2& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort3& a0, const ushort3& a1, const ushort2& a2, const ushort4& a3, const ushort4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ushort16(const ushort3& a0, const ushort3& a1, const ushort2& a2, const ushort8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  ushort16(const ushort3& a0, const ushort3& a1, const ushort3& a2, const ushort2& a3, const ushort2& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort3& a0, const ushort3& a1, const ushort3& a2, const ushort2& a3, const ushort3& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort3& a0, const ushort3& a1, const ushort3& a2, const ushort3& a3, const ushort2& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort3& a0, const ushort3& a1, const ushort3& a2, const ushort3& a3, const ushort4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ushort16(const ushort3& a0, const ushort3& a1, const ushort3& a2, const ushort4& a3, const ushort3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  ushort16(const ushort3& a0, const ushort3& a1, const ushort4& a2, const ushort2& a3, const ushort2& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort3& a0, const ushort3& a1, const ushort4& a2, const ushort2& a3, const ushort4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ushort16(const ushort3& a0, const ushort3& a1, const ushort4& a2, const ushort3& a3, const ushort3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  ushort16(const ushort3& a0, const ushort3& a1, const ushort4& a2, const ushort4& a3, const ushort2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  ushort16(const ushort3& a0, const ushort3& a1, const ushort8& a2, const ushort2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  ushort16(const ushort3& a0, const ushort4& a1, const ushort2& a2, const ushort2& a3, const ushort2& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort3& a0, const ushort4& a1, const ushort2& a2, const ushort2& a3, const ushort3& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort3& a0, const ushort4& a1, const ushort2& a2, const ushort3& a3, const ushort2& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort3& a0, const ushort4& a1, const ushort2& a2, const ushort3& a3, const ushort4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ushort16(const ushort3& a0, const ushort4& a1, const ushort2& a2, const ushort4& a3, const ushort3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  ushort16(const ushort3& a0, const ushort4& a1, const ushort3& a2, const ushort2& a3, const ushort2& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort3& a0, const ushort4& a1, const ushort3& a2, const ushort2& a3, const ushort4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ushort16(const ushort3& a0, const ushort4& a1, const ushort3& a2, const ushort3& a3, const ushort3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  ushort16(const ushort3& a0, const ushort4& a1, const ushort3& a2, const ushort4& a3, const ushort2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  ushort16(const ushort3& a0, const ushort4& a1, const ushort4& a2, const ushort2& a3, const ushort3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  ushort16(const ushort3& a0, const ushort4& a1, const ushort4& a2, const ushort3& a3, const ushort2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  ushort16(const ushort3& a0, const ushort8& a1, const ushort2& a2, const ushort3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a1.s[4];
    s[8] = a1.s[5];
    s[9] = a1.s[6];
    s[10] = a1.s[7];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  ushort16(const ushort3& a0, const ushort8& a1, const ushort3& a2, const ushort2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a1.s[4];
    s[8] = a1.s[5];
    s[9] = a1.s[6];
    s[10] = a1.s[7];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a2.s[2];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  ushort16(const ushort4& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3, const ushort2& a4, const ushort2& a5, const ushort2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ushort16(const ushort4& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3, const ushort2& a4, const ushort4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ushort16(const ushort4& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3, const ushort3& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort4& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3, const ushort4& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort4& a0, const ushort2& a1, const ushort2& a2, const ushort3& a3, const ushort2& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort4& a0, const ushort2& a1, const ushort2& a2, const ushort3& a3, const ushort3& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort4& a0, const ushort2& a1, const ushort2& a2, const ushort4& a3, const ushort2& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort4& a0, const ushort2& a1, const ushort2& a2, const ushort4& a3, const ushort4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ushort16(const ushort4& a0, const ushort2& a1, const ushort2& a2, const ushort8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  ushort16(const ushort4& a0, const ushort2& a1, const ushort3& a2, const ushort2& a3, const ushort2& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort4& a0, const ushort2& a1, const ushort3& a2, const ushort2& a3, const ushort3& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort4& a0, const ushort2& a1, const ushort3& a2, const ushort3& a3, const ushort2& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort4& a0, const ushort2& a1, const ushort3& a2, const ushort3& a3, const ushort4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ushort16(const ushort4& a0, const ushort2& a1, const ushort3& a2, const ushort4& a3, const ushort3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  ushort16(const ushort4& a0, const ushort2& a1, const ushort4& a2, const ushort2& a3, const ushort2& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort4& a0, const ushort2& a1, const ushort4& a2, const ushort2& a3, const ushort4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ushort16(const ushort4& a0, const ushort2& a1, const ushort4& a2, const ushort3& a3, const ushort3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  ushort16(const ushort4& a0, const ushort2& a1, const ushort4& a2, const ushort4& a3, const ushort2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  ushort16(const ushort4& a0, const ushort2& a1, const ushort8& a2, const ushort2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  ushort16(const ushort4& a0, const ushort3& a1, const ushort2& a2, const ushort2& a3, const ushort2& a4, const ushort3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ushort16(const ushort4& a0, const ushort3& a1, const ushort2& a2, const ushort2& a3, const ushort3& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort4& a0, const ushort3& a1, const ushort2& a2, const ushort3& a3, const ushort2& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort4& a0, const ushort3& a1, const ushort2& a2, const ushort3& a3, const ushort4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ushort16(const ushort4& a0, const ushort3& a1, const ushort2& a2, const ushort4& a3, const ushort3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  ushort16(const ushort4& a0, const ushort3& a1, const ushort3& a2, const ushort2& a3, const ushort2& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort4& a0, const ushort3& a1, const ushort3& a2, const ushort2& a3, const ushort4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ushort16(const ushort4& a0, const ushort3& a1, const ushort3& a2, const ushort3& a3, const ushort3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  ushort16(const ushort4& a0, const ushort3& a1, const ushort3& a2, const ushort4& a3, const ushort2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  ushort16(const ushort4& a0, const ushort3& a1, const ushort4& a2, const ushort2& a3, const ushort3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  ushort16(const ushort4& a0, const ushort3& a1, const ushort4& a2, const ushort3& a3, const ushort2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  ushort16(const ushort4& a0, const ushort4& a1, const ushort2& a2, const ushort2& a3, const ushort2& a4, const ushort2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ushort16(const ushort4& a0, const ushort4& a1, const ushort2& a2, const ushort2& a3, const ushort4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ushort16(const ushort4& a0, const ushort4& a1, const ushort2& a2, const ushort3& a3, const ushort3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  ushort16(const ushort4& a0, const ushort4& a1, const ushort2& a2, const ushort4& a3, const ushort2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  ushort16(const ushort4& a0, const ushort4& a1, const ushort3& a2, const ushort2& a3, const ushort3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  ushort16(const ushort4& a0, const ushort4& a1, const ushort3& a2, const ushort3& a3, const ushort2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  ushort16(const ushort4& a0, const ushort4& a1, const ushort4& a2, const ushort2& a3, const ushort2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  ushort16(const ushort4& a0, const ushort4& a1, const ushort4& a2, const ushort4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  ushort16(const ushort4& a0, const ushort4& a1, const ushort8& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a2.s[4];
    s[13] = a2.s[5];
    s[14] = a2.s[6];
    s[15] = a2.s[7];
  }

  ushort16(const ushort4& a0, const ushort8& a1, const ushort2& a2, const ushort2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a1.s[4];
    s[9] = a1.s[5];
    s[10] = a1.s[6];
    s[11] = a1.s[7];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  ushort16(const ushort4& a0, const ushort8& a1, const ushort4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a1.s[4];
    s[9] = a1.s[5];
    s[10] = a1.s[6];
    s[11] = a1.s[7];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a2.s[2];
    s[15] = a2.s[3];
  }

  ushort16(const ushort8& a0, const ushort2& a1, const ushort2& a2, const ushort2& a3, const ushort2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  ushort16(const ushort8& a0, const ushort2& a1, const ushort2& a2, const ushort4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  ushort16(const ushort8& a0, const ushort2& a1, const ushort3& a2, const ushort3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  ushort16(const ushort8& a0, const ushort2& a1, const ushort4& a2, const ushort2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a2.s[3];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  ushort16(const ushort8& a0, const ushort3& a1, const ushort2& a2, const ushort3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  ushort16(const ushort8& a0, const ushort3& a1, const ushort3& a2, const ushort2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a2.s[2];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  ushort16(const ushort8& a0, const ushort4& a1, const ushort2& a2, const ushort2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  ushort16(const ushort8& a0, const ushort4& a1, const ushort4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a2.s[2];
    s[15] = a2.s[3];
  }

  ushort16(const ushort8& a0, const ushort8& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a1.s[4];
    s[13] = a1.s[5];
    s[14] = a1.s[6];
    s[15] = a1.s[7];
  }

  ushort16(const ushort16& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a0.s[8];
    s[9] = a0.s[9];
    s[10] = a0.s[10];
    s[11] = a0.s[11];
    s[12] = a0.s[12];
    s[13] = a0.s[13];
    s[14] = a0.s[14];
    s[15] = a0.s[15];
  }

  ushort16& operator+=(const ushort16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  ushort16& operator+=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  ushort16& operator-=(const ushort16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  ushort16& operator-=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  ushort16& operator*=(const ushort16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  ushort16& operator*=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  ushort16& operator/=(const ushort16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  ushort16& operator/=(cl_ushort rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  ushort16& operator%=(const ushort16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  ushort16& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  ushort16& operator&=(const ushort16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  ushort16& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  ushort16& operator|=(const ushort16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  ushort16& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  ushort16& operator^=(const ushort16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  ushort16& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  ushort16& operator<<=(const ushort16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  ushort16& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  ushort16& operator>>=(const ushort16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  ushort16& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline ushort16 operator+(ushort16 lhs, const ushort16& rhs)
{
  lhs += rhs;
  return lhs;
}

inline ushort16 operator+(ushort16 lhs, cl_ushort rhs)
{
  lhs += rhs;
  return lhs;
}

inline ushort16 operator+(cl_ushort lhs, const ushort16& rhs)
{
  auto tmp = ushort16(lhs);
  return tmp + rhs;
}

inline ushort16 operator-(ushort16 lhs, const ushort16& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline ushort16 operator-(ushort16 lhs, cl_ushort rhs)
{
  lhs -= rhs;
  return lhs;
}

inline ushort16 operator-(cl_ushort lhs, const ushort16& rhs)
{
  auto tmp = ushort16(lhs);
  return tmp - rhs;
}

inline ushort16 operator*(ushort16 lhs, const ushort16& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline ushort16 operator*(ushort16 lhs, cl_ushort rhs)
{
  lhs *= rhs;
  return lhs;
}

inline ushort16 operator*(cl_ushort lhs, const ushort16& rhs)
{
  auto tmp = ushort16(lhs);
  return tmp * rhs;
}

inline ushort16 operator/(ushort16 lhs, const ushort16& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline ushort16 operator/(ushort16 lhs, cl_ushort rhs)
{
  lhs /= rhs;
  return lhs;
}

inline ushort16 operator/(cl_ushort lhs, const ushort16& rhs)
{
  auto tmp = ushort16(lhs);
  return tmp / rhs;
}

inline ushort16 operator%(ushort16 lhs, const ushort16& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline ushort16 operator%(ushort16 lhs, cl_ushort rhs)
{
  lhs %= rhs;
  return lhs;
}

inline ushort16 operator%(cl_ushort lhs, const ushort16& rhs)
{
  auto tmp = ushort16(lhs);
  return tmp % rhs;
}

inline ushort16 operator&(ushort16 lhs, const ushort16& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline ushort16 operator&(ushort16 lhs, cl_ushort rhs)
{
  lhs &= rhs;
  return lhs;
}

inline ushort16 operator&(cl_ushort lhs, const ushort16& rhs)
{
  auto tmp = ushort16(lhs);
  return tmp & rhs;
}

inline ushort16 operator|(ushort16 lhs, const ushort16& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline ushort16 operator|(ushort16 lhs, cl_ushort rhs)
{
  lhs |= rhs;
  return lhs;
}

inline ushort16 operator|(cl_ushort lhs, const ushort16& rhs)
{
  auto tmp = ushort16(lhs);
  return tmp | rhs;
}

inline ushort16 operator^(ushort16 lhs, const ushort16& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline ushort16 operator^(ushort16 lhs, cl_ushort rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline ushort16 operator^(cl_ushort lhs, const ushort16& rhs)
{
  auto tmp = ushort16(lhs);
  return tmp ^ rhs;
}

inline ushort16 operator<<(ushort16 lhs, const ushort16& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline ushort16 operator<<(ushort16 lhs, cl_ushort rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline ushort16 operator<<(cl_ushort lhs, const ushort16& rhs)
{
  auto tmp = ushort16(lhs);
  return tmp << rhs;
}

inline ushort16 operator>>(ushort16 lhs, const ushort16& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline ushort16 operator>>(ushort16 lhs, cl_ushort rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline ushort16 operator>>(cl_ushort lhs, const ushort16& rhs)
{
  auto tmp = ushort16(lhs);
  return tmp >> rhs;
}

inline bool operator==(const ushort16& lhs, const ushort16& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]) &&
  (lhs.s[4] == rhs.s[4]) &&
  (lhs.s[5] == rhs.s[5]) &&
  (lhs.s[6] == rhs.s[6]) &&
  (lhs.s[7] == rhs.s[7]) &&
  (lhs.s[8] == rhs.s[8]) &&
  (lhs.s[9] == rhs.s[9]) &&
  (lhs.s[10] == rhs.s[10]) &&
  (lhs.s[11] == rhs.s[11]) &&
  (lhs.s[12] == rhs.s[12]) &&
  (lhs.s[13] == rhs.s[13]) &&
  (lhs.s[14] == rhs.s[14]) &&
  (lhs.s[15] == rhs.s[15]);
}

inline bool operator==(const ushort16& lhs, const cl_ushort& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs) &&
  (lhs.s[4] == rhs) &&
  (lhs.s[5] == rhs) &&
  (lhs.s[6] == rhs) &&
  (lhs.s[7] == rhs) &&
  (lhs.s[8] == rhs) &&
  (lhs.s[9] == rhs) &&
  (lhs.s[10] == rhs) &&
  (lhs.s[11] == rhs) &&
  (lhs.s[12] == rhs) &&
  (lhs.s[13] == rhs) &&
  (lhs.s[14] == rhs) &&
  (lhs.s[15] == rhs);
}

inline bool operator<(const ushort16& lhs, const ushort16& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const ushort16& lhs, const cl_ushort& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const ushort16& lhs, const ushort16& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const ushort16& lhs, const cl_ushort& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const ushort16& lhs, const ushort16& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const ushort16& lhs, const cl_ushort& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const ushort16& lhs, const ushort16& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const ushort16& lhs, const cl_ushort& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const ushort16& lhs, const ushort16& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const ushort16& lhs, const cl_ushort& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline ushort16 vload16(size_t offset, const ushort16* p)
{
  return *(p + offset);
}

inline void vstore16(ushort16 data, size_t offset, ushort16* p)
{
  *(p + offset) = data;
}


class int2{
public:
  static const size_t N = 2; // Number of elements in vector
  union {
	 cl_int CL_ALIGNED(8) s[2];
#if defined(__GNUC__)
    __extension__ struct { cl_int x, y; };
    __extension__ struct { cl_int s0, s1; };
#endif
  };

  int2(){}

  int2(cl_int i0,cl_int i1){
    s[0] = i0;
    s[1] = i1;

  }

  int2(cl_int i0) {
  for (int i = 0; i < 2; ++i) {
		s[i] = i0;
	 }
  }

  int2(const int2& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
  }

  int2& operator+=(const int2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  int2& operator+=(cl_int rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  int2& operator-=(const int2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  int2& operator-=(cl_int rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  int2& operator*=(const int2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  int2& operator*=(cl_int rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  int2& operator/=(const int2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  int2& operator/=(cl_int rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  int2& operator%=(const int2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  int2& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  int2& operator&=(const int2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  int2& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  int2& operator|=(const int2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  int2& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  int2& operator^=(const int2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  int2& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  int2& operator<<=(const int2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  int2& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  int2& operator>>=(const int2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  int2& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline int2 operator+(int2 lhs, const int2& rhs)
{
  lhs += rhs;
  return lhs;
}

inline int2 operator+(int2 lhs, cl_int rhs)
{
  lhs += rhs;
  return lhs;
}

inline int2 operator+(cl_int lhs, const int2& rhs)
{
  auto tmp = int2(lhs);
  return tmp + rhs;
}

inline int2 operator-(int2 lhs, const int2& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline int2 operator-(int2 lhs, cl_int rhs)
{
  lhs -= rhs;
  return lhs;
}

inline int2 operator-(cl_int lhs, const int2& rhs)
{
  auto tmp = int2(lhs);
  return tmp - rhs;
}

inline int2 operator*(int2 lhs, const int2& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline int2 operator*(int2 lhs, cl_int rhs)
{
  lhs *= rhs;
  return lhs;
}

inline int2 operator*(cl_int lhs, const int2& rhs)
{
  auto tmp = int2(lhs);
  return tmp * rhs;
}

inline int2 operator/(int2 lhs, const int2& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline int2 operator/(int2 lhs, cl_int rhs)
{
  lhs /= rhs;
  return lhs;
}

inline int2 operator/(cl_int lhs, const int2& rhs)
{
  auto tmp = int2(lhs);
  return tmp / rhs;
}

inline int2 operator%(int2 lhs, const int2& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline int2 operator%(int2 lhs, cl_int rhs)
{
  lhs %= rhs;
  return lhs;
}

inline int2 operator%(cl_int lhs, const int2& rhs)
{
  auto tmp = int2(lhs);
  return tmp % rhs;
}

inline int2 operator&(int2 lhs, const int2& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline int2 operator&(int2 lhs, cl_int rhs)
{
  lhs &= rhs;
  return lhs;
}

inline int2 operator&(cl_int lhs, const int2& rhs)
{
  auto tmp = int2(lhs);
  return tmp & rhs;
}

inline int2 operator|(int2 lhs, const int2& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline int2 operator|(int2 lhs, cl_int rhs)
{
  lhs |= rhs;
  return lhs;
}

inline int2 operator|(cl_int lhs, const int2& rhs)
{
  auto tmp = int2(lhs);
  return tmp | rhs;
}

inline int2 operator^(int2 lhs, const int2& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline int2 operator^(int2 lhs, cl_int rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline int2 operator^(cl_int lhs, const int2& rhs)
{
  auto tmp = int2(lhs);
  return tmp ^ rhs;
}

inline int2 operator<<(int2 lhs, const int2& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline int2 operator<<(int2 lhs, cl_int rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline int2 operator<<(cl_int lhs, const int2& rhs)
{
  auto tmp = int2(lhs);
  return tmp << rhs;
}

inline int2 operator>>(int2 lhs, const int2& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline int2 operator>>(int2 lhs, cl_int rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline int2 operator>>(cl_int lhs, const int2& rhs)
{
  auto tmp = int2(lhs);
  return tmp >> rhs;
}

inline bool operator==(const int2& lhs, const int2& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]);
}

inline bool operator==(const int2& lhs, const cl_int& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs);
}

inline bool operator<(const int2& lhs, const int2& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const int2& lhs, const cl_int& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const int2& lhs, const int2& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const int2& lhs, const cl_int& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const int2& lhs, const int2& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const int2& lhs, const cl_int& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const int2& lhs, const int2& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const int2& lhs, const cl_int& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const int2& lhs, const int2& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const int2& lhs, const cl_int& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline int2 vload2(size_t offset, const int2* p)
{
  return *(p + offset);
}

inline void vstore2(int2 data, size_t offset, int2* p)
{
  *(p + offset) = data;
}


class int3{
public:
  static const size_t N = 4; // Number of elements in vector
  union {
	 cl_int CL_ALIGNED(16) s[4];
#if defined(__GNUC__)
    __extension__ struct { cl_int x, y, z, w; };
    __extension__ struct { cl_int s0, s1, s2, s3; };
#endif
  };

  int3(){}

  int3(cl_int i0,cl_int i1,cl_int i2){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;

  }

  int3(cl_int i0) {
  for (int i = 0; i < 3; ++i) {
		s[i] = i0;
	 }
  }

  int3(const int3& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
  }

  int3& operator+=(const int3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  int3& operator+=(cl_int rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  int3& operator-=(const int3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  int3& operator-=(cl_int rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  int3& operator*=(const int3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  int3& operator*=(cl_int rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  int3& operator/=(const int3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  int3& operator/=(cl_int rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  int3& operator%=(const int3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  int3& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  int3& operator&=(const int3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  int3& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  int3& operator|=(const int3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  int3& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  int3& operator^=(const int3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  int3& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  int3& operator<<=(const int3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  int3& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  int3& operator>>=(const int3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  int3& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline int3 operator+(int3 lhs, const int3& rhs)
{
  lhs += rhs;
  return lhs;
}

inline int3 operator+(int3 lhs, cl_int rhs)
{
  lhs += rhs;
  return lhs;
}

inline int3 operator+(cl_int lhs, const int3& rhs)
{
  auto tmp = int3(lhs);
  return tmp + rhs;
}

inline int3 operator-(int3 lhs, const int3& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline int3 operator-(int3 lhs, cl_int rhs)
{
  lhs -= rhs;
  return lhs;
}

inline int3 operator-(cl_int lhs, const int3& rhs)
{
  auto tmp = int3(lhs);
  return tmp - rhs;
}

inline int3 operator*(int3 lhs, const int3& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline int3 operator*(int3 lhs, cl_int rhs)
{
  lhs *= rhs;
  return lhs;
}

inline int3 operator*(cl_int lhs, const int3& rhs)
{
  auto tmp = int3(lhs);
  return tmp * rhs;
}

inline int3 operator/(int3 lhs, const int3& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline int3 operator/(int3 lhs, cl_int rhs)
{
  lhs /= rhs;
  return lhs;
}

inline int3 operator/(cl_int lhs, const int3& rhs)
{
  auto tmp = int3(lhs);
  return tmp / rhs;
}

inline int3 operator%(int3 lhs, const int3& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline int3 operator%(int3 lhs, cl_int rhs)
{
  lhs %= rhs;
  return lhs;
}

inline int3 operator%(cl_int lhs, const int3& rhs)
{
  auto tmp = int3(lhs);
  return tmp % rhs;
}

inline int3 operator&(int3 lhs, const int3& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline int3 operator&(int3 lhs, cl_int rhs)
{
  lhs &= rhs;
  return lhs;
}

inline int3 operator&(cl_int lhs, const int3& rhs)
{
  auto tmp = int3(lhs);
  return tmp & rhs;
}

inline int3 operator|(int3 lhs, const int3& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline int3 operator|(int3 lhs, cl_int rhs)
{
  lhs |= rhs;
  return lhs;
}

inline int3 operator|(cl_int lhs, const int3& rhs)
{
  auto tmp = int3(lhs);
  return tmp | rhs;
}

inline int3 operator^(int3 lhs, const int3& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline int3 operator^(int3 lhs, cl_int rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline int3 operator^(cl_int lhs, const int3& rhs)
{
  auto tmp = int3(lhs);
  return tmp ^ rhs;
}

inline int3 operator<<(int3 lhs, const int3& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline int3 operator<<(int3 lhs, cl_int rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline int3 operator<<(cl_int lhs, const int3& rhs)
{
  auto tmp = int3(lhs);
  return tmp << rhs;
}

inline int3 operator>>(int3 lhs, const int3& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline int3 operator>>(int3 lhs, cl_int rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline int3 operator>>(cl_int lhs, const int3& rhs)
{
  auto tmp = int3(lhs);
  return tmp >> rhs;
}

inline bool operator==(const int3& lhs, const int3& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]);
}

inline bool operator==(const int3& lhs, const cl_int& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs);
}

inline bool operator<(const int3& lhs, const int3& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const int3& lhs, const cl_int& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const int3& lhs, const int3& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const int3& lhs, const cl_int& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const int3& lhs, const int3& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const int3& lhs, const cl_int& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const int3& lhs, const int3& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const int3& lhs, const cl_int& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const int3& lhs, const int3& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const int3& lhs, const cl_int& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline int3 vload3(size_t offset, const int3* p)
{
  return *(p + offset);
}

inline void vstore3(int3 data, size_t offset, int3* p)
{
  *(p + offset) = data;
}


class int4{
public:
  static const size_t N = 4; // Number of elements in vector
  union {
	 cl_int CL_ALIGNED(16) s[4];
#if defined(__GNUC__)
    __extension__ struct { cl_int x, y, z, w; };
    __extension__ struct { cl_int s0, s1, s2, s3; };
#endif
  };

  int4(){}

  int4(cl_int i0,cl_int i1,cl_int i2,cl_int i3
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;

  }

  int4(cl_int i0) {
  for (int i = 0; i < 4; ++i) {
		s[i] = i0;
	 }
  }

  int4(const int2& a0, const int2& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
  }

  int4(const int4& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
  }

  int4& operator+=(const int4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  int4& operator+=(cl_int rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  int4& operator-=(const int4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  int4& operator-=(cl_int rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  int4& operator*=(const int4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  int4& operator*=(cl_int rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  int4& operator/=(const int4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  int4& operator/=(cl_int rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  int4& operator%=(const int4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  int4& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  int4& operator&=(const int4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  int4& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  int4& operator|=(const int4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  int4& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  int4& operator^=(const int4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  int4& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  int4& operator<<=(const int4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  int4& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  int4& operator>>=(const int4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  int4& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline int4 operator+(int4 lhs, const int4& rhs)
{
  lhs += rhs;
  return lhs;
}

inline int4 operator+(int4 lhs, cl_int rhs)
{
  lhs += rhs;
  return lhs;
}

inline int4 operator+(cl_int lhs, const int4& rhs)
{
  auto tmp = int4(lhs);
  return tmp + rhs;
}

inline int4 operator-(int4 lhs, const int4& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline int4 operator-(int4 lhs, cl_int rhs)
{
  lhs -= rhs;
  return lhs;
}

inline int4 operator-(cl_int lhs, const int4& rhs)
{
  auto tmp = int4(lhs);
  return tmp - rhs;
}

inline int4 operator*(int4 lhs, const int4& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline int4 operator*(int4 lhs, cl_int rhs)
{
  lhs *= rhs;
  return lhs;
}

inline int4 operator*(cl_int lhs, const int4& rhs)
{
  auto tmp = int4(lhs);
  return tmp * rhs;
}

inline int4 operator/(int4 lhs, const int4& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline int4 operator/(int4 lhs, cl_int rhs)
{
  lhs /= rhs;
  return lhs;
}

inline int4 operator/(cl_int lhs, const int4& rhs)
{
  auto tmp = int4(lhs);
  return tmp / rhs;
}

inline int4 operator%(int4 lhs, const int4& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline int4 operator%(int4 lhs, cl_int rhs)
{
  lhs %= rhs;
  return lhs;
}

inline int4 operator%(cl_int lhs, const int4& rhs)
{
  auto tmp = int4(lhs);
  return tmp % rhs;
}

inline int4 operator&(int4 lhs, const int4& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline int4 operator&(int4 lhs, cl_int rhs)
{
  lhs &= rhs;
  return lhs;
}

inline int4 operator&(cl_int lhs, const int4& rhs)
{
  auto tmp = int4(lhs);
  return tmp & rhs;
}

inline int4 operator|(int4 lhs, const int4& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline int4 operator|(int4 lhs, cl_int rhs)
{
  lhs |= rhs;
  return lhs;
}

inline int4 operator|(cl_int lhs, const int4& rhs)
{
  auto tmp = int4(lhs);
  return tmp | rhs;
}

inline int4 operator^(int4 lhs, const int4& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline int4 operator^(int4 lhs, cl_int rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline int4 operator^(cl_int lhs, const int4& rhs)
{
  auto tmp = int4(lhs);
  return tmp ^ rhs;
}

inline int4 operator<<(int4 lhs, const int4& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline int4 operator<<(int4 lhs, cl_int rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline int4 operator<<(cl_int lhs, const int4& rhs)
{
  auto tmp = int4(lhs);
  return tmp << rhs;
}

inline int4 operator>>(int4 lhs, const int4& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline int4 operator>>(int4 lhs, cl_int rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline int4 operator>>(cl_int lhs, const int4& rhs)
{
  auto tmp = int4(lhs);
  return tmp >> rhs;
}

inline bool operator==(const int4& lhs, const int4& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]);
}

inline bool operator==(const int4& lhs, const cl_int& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs);
}

inline bool operator<(const int4& lhs, const int4& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const int4& lhs, const cl_int& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const int4& lhs, const int4& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const int4& lhs, const cl_int& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const int4& lhs, const int4& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const int4& lhs, const cl_int& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const int4& lhs, const int4& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const int4& lhs, const cl_int& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const int4& lhs, const int4& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const int4& lhs, const cl_int& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline int4 vload4(size_t offset, const int4* p)
{
  return *(p + offset);
}

inline void vstore4(int4 data, size_t offset, int4* p)
{
  *(p + offset) = data;
}


class int8{
public:
  static const size_t N = 8; // Number of elements in vector
  union {
	 cl_int CL_ALIGNED(32) s[8];
#if defined(__GNUC__)
    __extension__ struct { cl_int x, y, z, w; };
    __extension__ struct { cl_int s0, s1, s2, s3, s4, s5, s6, s7; };
#endif
  };

  int8(){}

  int8(cl_int i0,cl_int i1,cl_int i2,cl_int i3,
          cl_int i4,cl_int i5,cl_int i6,cl_int i7
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;
    s[4] = i4;
    s[5] = i5;
    s[6] = i6;
    s[7] = i7;

  }

  int8(cl_int i0) {
  for (int i = 0; i < 8; ++i) {
		s[i] = i0;
	 }
  }

  int8(const int2& a0, const int2& a1, const int2& a2, const int2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
  }

  int8(const int2& a0, const int2& a1, const int4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
  }

  int8(const int2& a0, const int3& a1, const int3& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
  }

  int8(const int2& a0, const int4& a1, const int2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  int8(const int3& a0, const int2& a1, const int3& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
  }

  int8(const int3& a0, const int3& a1, const int2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  int8(const int4& a0, const int2& a1, const int2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  int8(const int4& a0, const int4& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
  }

  int8(const int8& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
  }

  int8& operator+=(const int8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  int8& operator+=(cl_int rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  int8& operator-=(const int8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  int8& operator-=(cl_int rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  int8& operator*=(const int8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  int8& operator*=(cl_int rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  int8& operator/=(const int8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  int8& operator/=(cl_int rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  int8& operator%=(const int8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  int8& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  int8& operator&=(const int8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  int8& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  int8& operator|=(const int8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  int8& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  int8& operator^=(const int8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  int8& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  int8& operator<<=(const int8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  int8& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  int8& operator>>=(const int8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  int8& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline int8 operator+(int8 lhs, const int8& rhs)
{
  lhs += rhs;
  return lhs;
}

inline int8 operator+(int8 lhs, cl_int rhs)
{
  lhs += rhs;
  return lhs;
}

inline int8 operator+(cl_int lhs, const int8& rhs)
{
  auto tmp = int8(lhs);
  return tmp + rhs;
}

inline int8 operator-(int8 lhs, const int8& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline int8 operator-(int8 lhs, cl_int rhs)
{
  lhs -= rhs;
  return lhs;
}

inline int8 operator-(cl_int lhs, const int8& rhs)
{
  auto tmp = int8(lhs);
  return tmp - rhs;
}

inline int8 operator*(int8 lhs, const int8& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline int8 operator*(int8 lhs, cl_int rhs)
{
  lhs *= rhs;
  return lhs;
}

inline int8 operator*(cl_int lhs, const int8& rhs)
{
  auto tmp = int8(lhs);
  return tmp * rhs;
}

inline int8 operator/(int8 lhs, const int8& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline int8 operator/(int8 lhs, cl_int rhs)
{
  lhs /= rhs;
  return lhs;
}

inline int8 operator/(cl_int lhs, const int8& rhs)
{
  auto tmp = int8(lhs);
  return tmp / rhs;
}

inline int8 operator%(int8 lhs, const int8& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline int8 operator%(int8 lhs, cl_int rhs)
{
  lhs %= rhs;
  return lhs;
}

inline int8 operator%(cl_int lhs, const int8& rhs)
{
  auto tmp = int8(lhs);
  return tmp % rhs;
}

inline int8 operator&(int8 lhs, const int8& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline int8 operator&(int8 lhs, cl_int rhs)
{
  lhs &= rhs;
  return lhs;
}

inline int8 operator&(cl_int lhs, const int8& rhs)
{
  auto tmp = int8(lhs);
  return tmp & rhs;
}

inline int8 operator|(int8 lhs, const int8& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline int8 operator|(int8 lhs, cl_int rhs)
{
  lhs |= rhs;
  return lhs;
}

inline int8 operator|(cl_int lhs, const int8& rhs)
{
  auto tmp = int8(lhs);
  return tmp | rhs;
}

inline int8 operator^(int8 lhs, const int8& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline int8 operator^(int8 lhs, cl_int rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline int8 operator^(cl_int lhs, const int8& rhs)
{
  auto tmp = int8(lhs);
  return tmp ^ rhs;
}

inline int8 operator<<(int8 lhs, const int8& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline int8 operator<<(int8 lhs, cl_int rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline int8 operator<<(cl_int lhs, const int8& rhs)
{
  auto tmp = int8(lhs);
  return tmp << rhs;
}

inline int8 operator>>(int8 lhs, const int8& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline int8 operator>>(int8 lhs, cl_int rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline int8 operator>>(cl_int lhs, const int8& rhs)
{
  auto tmp = int8(lhs);
  return tmp >> rhs;
}

inline bool operator==(const int8& lhs, const int8& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]) &&
  (lhs.s[4] == rhs.s[4]) &&
  (lhs.s[5] == rhs.s[5]) &&
  (lhs.s[6] == rhs.s[6]) &&
  (lhs.s[7] == rhs.s[7]);
}

inline bool operator==(const int8& lhs, const cl_int& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs) &&
  (lhs.s[4] == rhs) &&
  (lhs.s[5] == rhs) &&
  (lhs.s[6] == rhs) &&
  (lhs.s[7] == rhs);
}

inline bool operator<(const int8& lhs, const int8& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const int8& lhs, const cl_int& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const int8& lhs, const int8& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const int8& lhs, const cl_int& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const int8& lhs, const int8& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const int8& lhs, const cl_int& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const int8& lhs, const int8& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const int8& lhs, const cl_int& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const int8& lhs, const int8& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const int8& lhs, const cl_int& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline int8 vload8(size_t offset, const int8* p)
{
  return *(p + offset);
}

inline void vstore8(int8 data, size_t offset, int8* p)
{
  *(p + offset) = data;
}


class int16{
public:
  static const size_t N = 16; // Number of elements in vector
  union {
	 cl_int CL_ALIGNED(64) s[16];
#if defined(__GNUC__)
    __extension__ struct { cl_int x, y, z, w; };
    __extension__ struct { cl_int s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15; };
#endif
  };

  int16(){}

  int16(cl_int i0,cl_int i1,cl_int i2,cl_int i3,
          cl_int i4,cl_int i5,cl_int i6,cl_int i7,
          cl_int i8,cl_int i9,cl_int i10,cl_int i11,
          cl_int i12,cl_int i13,cl_int i14,cl_int i15
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;
    s[4] = i4;
    s[5] = i5;
    s[6] = i6;
    s[7] = i7;
    s[8] = i8;
    s[9] = i9;
    s[10] = i10;
    s[11] = i11;
    s[12] = i12;
    s[13] = i13;
    s[14] = i14;
    s[15] = i15;

  }

  int16(cl_int i0) {
  for (int i = 0; i < 16; ++i) {
		s[i] = i0;
	 }
  }

  int16(const int2& a0, const int2& a1, const int2& a2, const int2& a3, const int2& a4, const int2& a5, const int2& a6, const int2& a7) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a6.s[0];
    s[13] = a6.s[1];
    s[14] = a7.s[0];
    s[15] = a7.s[1];
  }

  int16(const int2& a0, const int2& a1, const int2& a2, const int2& a3, const int2& a4, const int2& a5, const int4& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a6.s[0];
    s[13] = a6.s[1];
    s[14] = a6.s[2];
    s[15] = a6.s[3];
  }

  int16(const int2& a0, const int2& a1, const int2& a2, const int2& a3, const int2& a4, const int3& a5, const int3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a5.s[2];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  int16(const int2& a0, const int2& a1, const int2& a2, const int2& a3, const int2& a4, const int4& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a5.s[2];
    s[13] = a5.s[3];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int2& a0, const int2& a1, const int2& a2, const int2& a3, const int3& a4, const int2& a5, const int3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  int16(const int2& a0, const int2& a1, const int2& a2, const int2& a3, const int3& a4, const int3& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int2& a0, const int2& a1, const int2& a2, const int2& a3, const int4& a4, const int2& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int2& a0, const int2& a1, const int2& a2, const int2& a3, const int4& a4, const int4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  int16(const int2& a0, const int2& a1, const int2& a2, const int2& a3, const int8& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a4.s[4];
    s[13] = a4.s[5];
    s[14] = a4.s[6];
    s[15] = a4.s[7];
  }

  int16(const int2& a0, const int2& a1, const int2& a2, const int3& a3, const int2& a4, const int2& a5, const int3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  int16(const int2& a0, const int2& a1, const int2& a2, const int3& a3, const int2& a4, const int3& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int2& a0, const int2& a1, const int2& a2, const int3& a3, const int3& a4, const int2& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int2& a0, const int2& a1, const int2& a2, const int3& a3, const int3& a4, const int4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  int16(const int2& a0, const int2& a1, const int2& a2, const int3& a3, const int4& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int2& a0, const int2& a1, const int2& a2, const int4& a3, const int2& a4, const int2& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int2& a0, const int2& a1, const int2& a2, const int4& a3, const int2& a4, const int4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  int16(const int2& a0, const int2& a1, const int2& a2, const int4& a3, const int3& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int2& a0, const int2& a1, const int2& a2, const int4& a3, const int4& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int2& a0, const int2& a1, const int2& a2, const int8& a3, const int2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a3.s[4];
    s[11] = a3.s[5];
    s[12] = a3.s[6];
    s[13] = a3.s[7];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  int16(const int2& a0, const int2& a1, const int3& a2, const int2& a3, const int2& a4, const int2& a5, const int3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  int16(const int2& a0, const int2& a1, const int3& a2, const int2& a3, const int2& a4, const int3& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int2& a0, const int2& a1, const int3& a2, const int2& a3, const int3& a4, const int2& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int2& a0, const int2& a1, const int3& a2, const int2& a3, const int3& a4, const int4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  int16(const int2& a0, const int2& a1, const int3& a2, const int2& a3, const int4& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int2& a0, const int2& a1, const int3& a2, const int3& a3, const int2& a4, const int2& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int2& a0, const int2& a1, const int3& a2, const int3& a3, const int2& a4, const int4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  int16(const int2& a0, const int2& a1, const int3& a2, const int3& a3, const int3& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int2& a0, const int2& a1, const int3& a2, const int3& a3, const int4& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int2& a0, const int2& a1, const int3& a2, const int4& a3, const int2& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int2& a0, const int2& a1, const int3& a2, const int4& a3, const int3& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int2& a0, const int2& a1, const int4& a2, const int2& a3, const int2& a4, const int2& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int2& a0, const int2& a1, const int4& a2, const int2& a3, const int2& a4, const int4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  int16(const int2& a0, const int2& a1, const int4& a2, const int2& a3, const int3& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int2& a0, const int2& a1, const int4& a2, const int2& a3, const int4& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int2& a0, const int2& a1, const int4& a2, const int3& a3, const int2& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int2& a0, const int2& a1, const int4& a2, const int3& a3, const int3& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int2& a0, const int2& a1, const int4& a2, const int4& a3, const int2& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int2& a0, const int2& a1, const int4& a2, const int4& a3, const int4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  int16(const int2& a0, const int2& a1, const int4& a2, const int8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  int16(const int2& a0, const int2& a1, const int8& a2, const int2& a3, const int2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a2.s[4];
    s[9] = a2.s[5];
    s[10] = a2.s[6];
    s[11] = a2.s[7];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  int16(const int2& a0, const int2& a1, const int8& a2, const int4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a2.s[4];
    s[9] = a2.s[5];
    s[10] = a2.s[6];
    s[11] = a2.s[7];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  int16(const int2& a0, const int3& a1, const int2& a2, const int2& a3, const int2& a4, const int2& a5, const int3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  int16(const int2& a0, const int3& a1, const int2& a2, const int2& a3, const int2& a4, const int3& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int2& a0, const int3& a1, const int2& a2, const int2& a3, const int3& a4, const int2& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int2& a0, const int3& a1, const int2& a2, const int2& a3, const int3& a4, const int4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  int16(const int2& a0, const int3& a1, const int2& a2, const int2& a3, const int4& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int2& a0, const int3& a1, const int2& a2, const int3& a3, const int2& a4, const int2& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int2& a0, const int3& a1, const int2& a2, const int3& a3, const int2& a4, const int4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  int16(const int2& a0, const int3& a1, const int2& a2, const int3& a3, const int3& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int2& a0, const int3& a1, const int2& a2, const int3& a3, const int4& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int2& a0, const int3& a1, const int2& a2, const int4& a3, const int2& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int2& a0, const int3& a1, const int2& a2, const int4& a3, const int3& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int2& a0, const int3& a1, const int3& a2, const int2& a3, const int2& a4, const int2& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int2& a0, const int3& a1, const int3& a2, const int2& a3, const int2& a4, const int4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  int16(const int2& a0, const int3& a1, const int3& a2, const int2& a3, const int3& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int2& a0, const int3& a1, const int3& a2, const int2& a3, const int4& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int2& a0, const int3& a1, const int3& a2, const int3& a3, const int2& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int2& a0, const int3& a1, const int3& a2, const int3& a3, const int3& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int2& a0, const int3& a1, const int3& a2, const int4& a3, const int2& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int2& a0, const int3& a1, const int3& a2, const int4& a3, const int4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  int16(const int2& a0, const int3& a1, const int3& a2, const int8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  int16(const int2& a0, const int3& a1, const int4& a2, const int2& a3, const int2& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int2& a0, const int3& a1, const int4& a2, const int2& a3, const int3& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int2& a0, const int3& a1, const int4& a2, const int3& a3, const int2& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int2& a0, const int3& a1, const int4& a2, const int3& a3, const int4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  int16(const int2& a0, const int3& a1, const int4& a2, const int4& a3, const int3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  int16(const int2& a0, const int3& a1, const int8& a2, const int3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a2.s[4];
    s[10] = a2.s[5];
    s[11] = a2.s[6];
    s[12] = a2.s[7];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  int16(const int2& a0, const int4& a1, const int2& a2, const int2& a3, const int2& a4, const int2& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int2& a0, const int4& a1, const int2& a2, const int2& a3, const int2& a4, const int4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  int16(const int2& a0, const int4& a1, const int2& a2, const int2& a3, const int3& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int2& a0, const int4& a1, const int2& a2, const int2& a3, const int4& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int2& a0, const int4& a1, const int2& a2, const int3& a3, const int2& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int2& a0, const int4& a1, const int2& a2, const int3& a3, const int3& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int2& a0, const int4& a1, const int2& a2, const int4& a3, const int2& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int2& a0, const int4& a1, const int2& a2, const int4& a3, const int4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  int16(const int2& a0, const int4& a1, const int2& a2, const int8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  int16(const int2& a0, const int4& a1, const int3& a2, const int2& a3, const int2& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int2& a0, const int4& a1, const int3& a2, const int2& a3, const int3& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int2& a0, const int4& a1, const int3& a2, const int3& a3, const int2& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int2& a0, const int4& a1, const int3& a2, const int3& a3, const int4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  int16(const int2& a0, const int4& a1, const int3& a2, const int4& a3, const int3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  int16(const int2& a0, const int4& a1, const int4& a2, const int2& a3, const int2& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int2& a0, const int4& a1, const int4& a2, const int2& a3, const int4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  int16(const int2& a0, const int4& a1, const int4& a2, const int3& a3, const int3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  int16(const int2& a0, const int4& a1, const int4& a2, const int4& a3, const int2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  int16(const int2& a0, const int4& a1, const int8& a2, const int2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  int16(const int2& a0, const int8& a1, const int2& a2, const int2& a3, const int2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  int16(const int2& a0, const int8& a1, const int2& a2, const int4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  int16(const int2& a0, const int8& a1, const int3& a2, const int3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  int16(const int2& a0, const int8& a1, const int4& a2, const int2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a2.s[3];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  int16(const int3& a0, const int2& a1, const int2& a2, const int2& a3, const int2& a4, const int2& a5, const int3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  int16(const int3& a0, const int2& a1, const int2& a2, const int2& a3, const int2& a4, const int3& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int3& a0, const int2& a1, const int2& a2, const int2& a3, const int3& a4, const int2& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int3& a0, const int2& a1, const int2& a2, const int2& a3, const int3& a4, const int4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  int16(const int3& a0, const int2& a1, const int2& a2, const int2& a3, const int4& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int3& a0, const int2& a1, const int2& a2, const int3& a3, const int2& a4, const int2& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int3& a0, const int2& a1, const int2& a2, const int3& a3, const int2& a4, const int4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  int16(const int3& a0, const int2& a1, const int2& a2, const int3& a3, const int3& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int3& a0, const int2& a1, const int2& a2, const int3& a3, const int4& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int3& a0, const int2& a1, const int2& a2, const int4& a3, const int2& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int3& a0, const int2& a1, const int2& a2, const int4& a3, const int3& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int3& a0, const int2& a1, const int3& a2, const int2& a3, const int2& a4, const int2& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int3& a0, const int2& a1, const int3& a2, const int2& a3, const int2& a4, const int4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  int16(const int3& a0, const int2& a1, const int3& a2, const int2& a3, const int3& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int3& a0, const int2& a1, const int3& a2, const int2& a3, const int4& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int3& a0, const int2& a1, const int3& a2, const int3& a3, const int2& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int3& a0, const int2& a1, const int3& a2, const int3& a3, const int3& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int3& a0, const int2& a1, const int3& a2, const int4& a3, const int2& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int3& a0, const int2& a1, const int3& a2, const int4& a3, const int4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  int16(const int3& a0, const int2& a1, const int3& a2, const int8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  int16(const int3& a0, const int2& a1, const int4& a2, const int2& a3, const int2& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int3& a0, const int2& a1, const int4& a2, const int2& a3, const int3& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int3& a0, const int2& a1, const int4& a2, const int3& a3, const int2& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int3& a0, const int2& a1, const int4& a2, const int3& a3, const int4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  int16(const int3& a0, const int2& a1, const int4& a2, const int4& a3, const int3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  int16(const int3& a0, const int2& a1, const int8& a2, const int3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a2.s[4];
    s[10] = a2.s[5];
    s[11] = a2.s[6];
    s[12] = a2.s[7];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  int16(const int3& a0, const int3& a1, const int2& a2, const int2& a3, const int2& a4, const int2& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int3& a0, const int3& a1, const int2& a2, const int2& a3, const int2& a4, const int4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  int16(const int3& a0, const int3& a1, const int2& a2, const int2& a3, const int3& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int3& a0, const int3& a1, const int2& a2, const int2& a3, const int4& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int3& a0, const int3& a1, const int2& a2, const int3& a3, const int2& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int3& a0, const int3& a1, const int2& a2, const int3& a3, const int3& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int3& a0, const int3& a1, const int2& a2, const int4& a3, const int2& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int3& a0, const int3& a1, const int2& a2, const int4& a3, const int4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  int16(const int3& a0, const int3& a1, const int2& a2, const int8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  int16(const int3& a0, const int3& a1, const int3& a2, const int2& a3, const int2& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int3& a0, const int3& a1, const int3& a2, const int2& a3, const int3& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int3& a0, const int3& a1, const int3& a2, const int3& a3, const int2& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int3& a0, const int3& a1, const int3& a2, const int3& a3, const int4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  int16(const int3& a0, const int3& a1, const int3& a2, const int4& a3, const int3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  int16(const int3& a0, const int3& a1, const int4& a2, const int2& a3, const int2& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int3& a0, const int3& a1, const int4& a2, const int2& a3, const int4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  int16(const int3& a0, const int3& a1, const int4& a2, const int3& a3, const int3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  int16(const int3& a0, const int3& a1, const int4& a2, const int4& a3, const int2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  int16(const int3& a0, const int3& a1, const int8& a2, const int2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  int16(const int3& a0, const int4& a1, const int2& a2, const int2& a3, const int2& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int3& a0, const int4& a1, const int2& a2, const int2& a3, const int3& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int3& a0, const int4& a1, const int2& a2, const int3& a3, const int2& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int3& a0, const int4& a1, const int2& a2, const int3& a3, const int4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  int16(const int3& a0, const int4& a1, const int2& a2, const int4& a3, const int3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  int16(const int3& a0, const int4& a1, const int3& a2, const int2& a3, const int2& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int3& a0, const int4& a1, const int3& a2, const int2& a3, const int4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  int16(const int3& a0, const int4& a1, const int3& a2, const int3& a3, const int3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  int16(const int3& a0, const int4& a1, const int3& a2, const int4& a3, const int2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  int16(const int3& a0, const int4& a1, const int4& a2, const int2& a3, const int3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  int16(const int3& a0, const int4& a1, const int4& a2, const int3& a3, const int2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  int16(const int3& a0, const int8& a1, const int2& a2, const int3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a1.s[4];
    s[8] = a1.s[5];
    s[9] = a1.s[6];
    s[10] = a1.s[7];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  int16(const int3& a0, const int8& a1, const int3& a2, const int2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a1.s[4];
    s[8] = a1.s[5];
    s[9] = a1.s[6];
    s[10] = a1.s[7];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a2.s[2];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  int16(const int4& a0, const int2& a1, const int2& a2, const int2& a3, const int2& a4, const int2& a5, const int2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  int16(const int4& a0, const int2& a1, const int2& a2, const int2& a3, const int2& a4, const int4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  int16(const int4& a0, const int2& a1, const int2& a2, const int2& a3, const int3& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int4& a0, const int2& a1, const int2& a2, const int2& a3, const int4& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int4& a0, const int2& a1, const int2& a2, const int3& a3, const int2& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int4& a0, const int2& a1, const int2& a2, const int3& a3, const int3& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int4& a0, const int2& a1, const int2& a2, const int4& a3, const int2& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int4& a0, const int2& a1, const int2& a2, const int4& a3, const int4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  int16(const int4& a0, const int2& a1, const int2& a2, const int8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  int16(const int4& a0, const int2& a1, const int3& a2, const int2& a3, const int2& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int4& a0, const int2& a1, const int3& a2, const int2& a3, const int3& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int4& a0, const int2& a1, const int3& a2, const int3& a3, const int2& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int4& a0, const int2& a1, const int3& a2, const int3& a3, const int4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  int16(const int4& a0, const int2& a1, const int3& a2, const int4& a3, const int3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  int16(const int4& a0, const int2& a1, const int4& a2, const int2& a3, const int2& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int4& a0, const int2& a1, const int4& a2, const int2& a3, const int4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  int16(const int4& a0, const int2& a1, const int4& a2, const int3& a3, const int3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  int16(const int4& a0, const int2& a1, const int4& a2, const int4& a3, const int2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  int16(const int4& a0, const int2& a1, const int8& a2, const int2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  int16(const int4& a0, const int3& a1, const int2& a2, const int2& a3, const int2& a4, const int3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  int16(const int4& a0, const int3& a1, const int2& a2, const int2& a3, const int3& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int4& a0, const int3& a1, const int2& a2, const int3& a3, const int2& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int4& a0, const int3& a1, const int2& a2, const int3& a3, const int4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  int16(const int4& a0, const int3& a1, const int2& a2, const int4& a3, const int3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  int16(const int4& a0, const int3& a1, const int3& a2, const int2& a3, const int2& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int4& a0, const int3& a1, const int3& a2, const int2& a3, const int4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  int16(const int4& a0, const int3& a1, const int3& a2, const int3& a3, const int3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  int16(const int4& a0, const int3& a1, const int3& a2, const int4& a3, const int2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  int16(const int4& a0, const int3& a1, const int4& a2, const int2& a3, const int3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  int16(const int4& a0, const int3& a1, const int4& a2, const int3& a3, const int2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  int16(const int4& a0, const int4& a1, const int2& a2, const int2& a3, const int2& a4, const int2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  int16(const int4& a0, const int4& a1, const int2& a2, const int2& a3, const int4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  int16(const int4& a0, const int4& a1, const int2& a2, const int3& a3, const int3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  int16(const int4& a0, const int4& a1, const int2& a2, const int4& a3, const int2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  int16(const int4& a0, const int4& a1, const int3& a2, const int2& a3, const int3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  int16(const int4& a0, const int4& a1, const int3& a2, const int3& a3, const int2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  int16(const int4& a0, const int4& a1, const int4& a2, const int2& a3, const int2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  int16(const int4& a0, const int4& a1, const int4& a2, const int4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  int16(const int4& a0, const int4& a1, const int8& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a2.s[4];
    s[13] = a2.s[5];
    s[14] = a2.s[6];
    s[15] = a2.s[7];
  }

  int16(const int4& a0, const int8& a1, const int2& a2, const int2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a1.s[4];
    s[9] = a1.s[5];
    s[10] = a1.s[6];
    s[11] = a1.s[7];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  int16(const int4& a0, const int8& a1, const int4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a1.s[4];
    s[9] = a1.s[5];
    s[10] = a1.s[6];
    s[11] = a1.s[7];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a2.s[2];
    s[15] = a2.s[3];
  }

  int16(const int8& a0, const int2& a1, const int2& a2, const int2& a3, const int2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  int16(const int8& a0, const int2& a1, const int2& a2, const int4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  int16(const int8& a0, const int2& a1, const int3& a2, const int3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  int16(const int8& a0, const int2& a1, const int4& a2, const int2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a2.s[3];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  int16(const int8& a0, const int3& a1, const int2& a2, const int3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  int16(const int8& a0, const int3& a1, const int3& a2, const int2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a2.s[2];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  int16(const int8& a0, const int4& a1, const int2& a2, const int2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  int16(const int8& a0, const int4& a1, const int4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a2.s[2];
    s[15] = a2.s[3];
  }

  int16(const int8& a0, const int8& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a1.s[4];
    s[13] = a1.s[5];
    s[14] = a1.s[6];
    s[15] = a1.s[7];
  }

  int16(const int16& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a0.s[8];
    s[9] = a0.s[9];
    s[10] = a0.s[10];
    s[11] = a0.s[11];
    s[12] = a0.s[12];
    s[13] = a0.s[13];
    s[14] = a0.s[14];
    s[15] = a0.s[15];
  }

  int16& operator+=(const int16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  int16& operator+=(cl_int rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  int16& operator-=(const int16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  int16& operator-=(cl_int rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  int16& operator*=(const int16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  int16& operator*=(cl_int rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  int16& operator/=(const int16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  int16& operator/=(cl_int rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  int16& operator%=(const int16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  int16& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  int16& operator&=(const int16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  int16& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  int16& operator|=(const int16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  int16& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  int16& operator^=(const int16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  int16& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  int16& operator<<=(const int16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  int16& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  int16& operator>>=(const int16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  int16& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline int16 operator+(int16 lhs, const int16& rhs)
{
  lhs += rhs;
  return lhs;
}

inline int16 operator+(int16 lhs, cl_int rhs)
{
  lhs += rhs;
  return lhs;
}

inline int16 operator+(cl_int lhs, const int16& rhs)
{
  auto tmp = int16(lhs);
  return tmp + rhs;
}

inline int16 operator-(int16 lhs, const int16& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline int16 operator-(int16 lhs, cl_int rhs)
{
  lhs -= rhs;
  return lhs;
}

inline int16 operator-(cl_int lhs, const int16& rhs)
{
  auto tmp = int16(lhs);
  return tmp - rhs;
}

inline int16 operator*(int16 lhs, const int16& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline int16 operator*(int16 lhs, cl_int rhs)
{
  lhs *= rhs;
  return lhs;
}

inline int16 operator*(cl_int lhs, const int16& rhs)
{
  auto tmp = int16(lhs);
  return tmp * rhs;
}

inline int16 operator/(int16 lhs, const int16& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline int16 operator/(int16 lhs, cl_int rhs)
{
  lhs /= rhs;
  return lhs;
}

inline int16 operator/(cl_int lhs, const int16& rhs)
{
  auto tmp = int16(lhs);
  return tmp / rhs;
}

inline int16 operator%(int16 lhs, const int16& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline int16 operator%(int16 lhs, cl_int rhs)
{
  lhs %= rhs;
  return lhs;
}

inline int16 operator%(cl_int lhs, const int16& rhs)
{
  auto tmp = int16(lhs);
  return tmp % rhs;
}

inline int16 operator&(int16 lhs, const int16& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline int16 operator&(int16 lhs, cl_int rhs)
{
  lhs &= rhs;
  return lhs;
}

inline int16 operator&(cl_int lhs, const int16& rhs)
{
  auto tmp = int16(lhs);
  return tmp & rhs;
}

inline int16 operator|(int16 lhs, const int16& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline int16 operator|(int16 lhs, cl_int rhs)
{
  lhs |= rhs;
  return lhs;
}

inline int16 operator|(cl_int lhs, const int16& rhs)
{
  auto tmp = int16(lhs);
  return tmp | rhs;
}

inline int16 operator^(int16 lhs, const int16& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline int16 operator^(int16 lhs, cl_int rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline int16 operator^(cl_int lhs, const int16& rhs)
{
  auto tmp = int16(lhs);
  return tmp ^ rhs;
}

inline int16 operator<<(int16 lhs, const int16& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline int16 operator<<(int16 lhs, cl_int rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline int16 operator<<(cl_int lhs, const int16& rhs)
{
  auto tmp = int16(lhs);
  return tmp << rhs;
}

inline int16 operator>>(int16 lhs, const int16& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline int16 operator>>(int16 lhs, cl_int rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline int16 operator>>(cl_int lhs, const int16& rhs)
{
  auto tmp = int16(lhs);
  return tmp >> rhs;
}

inline bool operator==(const int16& lhs, const int16& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]) &&
  (lhs.s[4] == rhs.s[4]) &&
  (lhs.s[5] == rhs.s[5]) &&
  (lhs.s[6] == rhs.s[6]) &&
  (lhs.s[7] == rhs.s[7]) &&
  (lhs.s[8] == rhs.s[8]) &&
  (lhs.s[9] == rhs.s[9]) &&
  (lhs.s[10] == rhs.s[10]) &&
  (lhs.s[11] == rhs.s[11]) &&
  (lhs.s[12] == rhs.s[12]) &&
  (lhs.s[13] == rhs.s[13]) &&
  (lhs.s[14] == rhs.s[14]) &&
  (lhs.s[15] == rhs.s[15]);
}

inline bool operator==(const int16& lhs, const cl_int& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs) &&
  (lhs.s[4] == rhs) &&
  (lhs.s[5] == rhs) &&
  (lhs.s[6] == rhs) &&
  (lhs.s[7] == rhs) &&
  (lhs.s[8] == rhs) &&
  (lhs.s[9] == rhs) &&
  (lhs.s[10] == rhs) &&
  (lhs.s[11] == rhs) &&
  (lhs.s[12] == rhs) &&
  (lhs.s[13] == rhs) &&
  (lhs.s[14] == rhs) &&
  (lhs.s[15] == rhs);
}

inline bool operator<(const int16& lhs, const int16& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const int16& lhs, const cl_int& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const int16& lhs, const int16& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const int16& lhs, const cl_int& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const int16& lhs, const int16& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const int16& lhs, const cl_int& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const int16& lhs, const int16& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const int16& lhs, const cl_int& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const int16& lhs, const int16& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const int16& lhs, const cl_int& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline int16 vload16(size_t offset, const int16* p)
{
  return *(p + offset);
}

inline void vstore16(int16 data, size_t offset, int16* p)
{
  *(p + offset) = data;
}


class uint2{
public:
  static const size_t N = 2; // Number of elements in vector
  union {
	 cl_uint CL_ALIGNED(8) s[2];
#if defined(__GNUC__)
    __extension__ struct { cl_uint x, y; };
    __extension__ struct { cl_uint s0, s1; };
#endif
  };

  uint2(){}

  uint2(cl_uint i0,cl_uint i1){
    s[0] = i0;
    s[1] = i1;

  }

  uint2(cl_uint i0) {
  for (int i = 0; i < 2; ++i) {
		s[i] = i0;
	 }
  }

  uint2(const uint2& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
  }

  uint2& operator+=(const uint2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  uint2& operator+=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  uint2& operator-=(const uint2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  uint2& operator-=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  uint2& operator*=(const uint2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  uint2& operator*=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  uint2& operator/=(const uint2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  uint2& operator/=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  uint2& operator%=(const uint2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  uint2& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  uint2& operator&=(const uint2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  uint2& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  uint2& operator|=(const uint2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  uint2& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  uint2& operator^=(const uint2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  uint2& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  uint2& operator<<=(const uint2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  uint2& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  uint2& operator>>=(const uint2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  uint2& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline uint2 operator+(uint2 lhs, const uint2& rhs)
{
  lhs += rhs;
  return lhs;
}

inline uint2 operator+(uint2 lhs, cl_uint rhs)
{
  lhs += rhs;
  return lhs;
}

inline uint2 operator+(cl_uint lhs, const uint2& rhs)
{
  auto tmp = uint2(lhs);
  return tmp + rhs;
}

inline uint2 operator-(uint2 lhs, const uint2& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uint2 operator-(uint2 lhs, cl_uint rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uint2 operator-(cl_uint lhs, const uint2& rhs)
{
  auto tmp = uint2(lhs);
  return tmp - rhs;
}

inline uint2 operator*(uint2 lhs, const uint2& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uint2 operator*(uint2 lhs, cl_uint rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uint2 operator*(cl_uint lhs, const uint2& rhs)
{
  auto tmp = uint2(lhs);
  return tmp * rhs;
}

inline uint2 operator/(uint2 lhs, const uint2& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uint2 operator/(uint2 lhs, cl_uint rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uint2 operator/(cl_uint lhs, const uint2& rhs)
{
  auto tmp = uint2(lhs);
  return tmp / rhs;
}

inline uint2 operator%(uint2 lhs, const uint2& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uint2 operator%(uint2 lhs, cl_uint rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uint2 operator%(cl_uint lhs, const uint2& rhs)
{
  auto tmp = uint2(lhs);
  return tmp % rhs;
}

inline uint2 operator&(uint2 lhs, const uint2& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uint2 operator&(uint2 lhs, cl_uint rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uint2 operator&(cl_uint lhs, const uint2& rhs)
{
  auto tmp = uint2(lhs);
  return tmp & rhs;
}

inline uint2 operator|(uint2 lhs, const uint2& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uint2 operator|(uint2 lhs, cl_uint rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uint2 operator|(cl_uint lhs, const uint2& rhs)
{
  auto tmp = uint2(lhs);
  return tmp | rhs;
}

inline uint2 operator^(uint2 lhs, const uint2& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uint2 operator^(uint2 lhs, cl_uint rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uint2 operator^(cl_uint lhs, const uint2& rhs)
{
  auto tmp = uint2(lhs);
  return tmp ^ rhs;
}

inline uint2 operator<<(uint2 lhs, const uint2& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uint2 operator<<(uint2 lhs, cl_uint rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uint2 operator<<(cl_uint lhs, const uint2& rhs)
{
  auto tmp = uint2(lhs);
  return tmp << rhs;
}

inline uint2 operator>>(uint2 lhs, const uint2& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uint2 operator>>(uint2 lhs, cl_uint rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uint2 operator>>(cl_uint lhs, const uint2& rhs)
{
  auto tmp = uint2(lhs);
  return tmp >> rhs;
}

inline bool operator==(const uint2& lhs, const uint2& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]);
}

inline bool operator==(const uint2& lhs, const cl_uint& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs);
}

inline bool operator<(const uint2& lhs, const uint2& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const uint2& lhs, const cl_uint& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uint2& lhs, const uint2& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uint2& lhs, const cl_uint& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const uint2& lhs, const uint2& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const uint2& lhs, const cl_uint& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const uint2& lhs, const uint2& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const uint2& lhs, const cl_uint& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const uint2& lhs, const uint2& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const uint2& lhs, const cl_uint& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline uint2 vload2(size_t offset, const uint2* p)
{
  return *(p + offset);
}

inline void vstore2(uint2 data, size_t offset, uint2* p)
{
  *(p + offset) = data;
}


class uint3{
public:
  static const size_t N = 4; // Number of elements in vector
  union {
	 cl_uint CL_ALIGNED(16) s[4];
#if defined(__GNUC__)
    __extension__ struct { cl_uint x, y, z, w; };
    __extension__ struct { cl_uint s0, s1, s2, s3; };
#endif
  };

  uint3(){}

  uint3(cl_uint i0,cl_uint i1,cl_uint i2){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;

  }

  uint3(cl_uint i0) {
  for (int i = 0; i < 3; ++i) {
		s[i] = i0;
	 }
  }

  uint3(const uint3& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
  }

  uint3& operator+=(const uint3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  uint3& operator+=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  uint3& operator-=(const uint3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  uint3& operator-=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  uint3& operator*=(const uint3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  uint3& operator*=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  uint3& operator/=(const uint3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  uint3& operator/=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  uint3& operator%=(const uint3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  uint3& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  uint3& operator&=(const uint3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  uint3& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  uint3& operator|=(const uint3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  uint3& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  uint3& operator^=(const uint3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  uint3& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  uint3& operator<<=(const uint3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  uint3& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  uint3& operator>>=(const uint3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  uint3& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline uint3 operator+(uint3 lhs, const uint3& rhs)
{
  lhs += rhs;
  return lhs;
}

inline uint3 operator+(uint3 lhs, cl_uint rhs)
{
  lhs += rhs;
  return lhs;
}

inline uint3 operator+(cl_uint lhs, const uint3& rhs)
{
  auto tmp = uint3(lhs);
  return tmp + rhs;
}

inline uint3 operator-(uint3 lhs, const uint3& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uint3 operator-(uint3 lhs, cl_uint rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uint3 operator-(cl_uint lhs, const uint3& rhs)
{
  auto tmp = uint3(lhs);
  return tmp - rhs;
}

inline uint3 operator*(uint3 lhs, const uint3& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uint3 operator*(uint3 lhs, cl_uint rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uint3 operator*(cl_uint lhs, const uint3& rhs)
{
  auto tmp = uint3(lhs);
  return tmp * rhs;
}

inline uint3 operator/(uint3 lhs, const uint3& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uint3 operator/(uint3 lhs, cl_uint rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uint3 operator/(cl_uint lhs, const uint3& rhs)
{
  auto tmp = uint3(lhs);
  return tmp / rhs;
}

inline uint3 operator%(uint3 lhs, const uint3& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uint3 operator%(uint3 lhs, cl_uint rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uint3 operator%(cl_uint lhs, const uint3& rhs)
{
  auto tmp = uint3(lhs);
  return tmp % rhs;
}

inline uint3 operator&(uint3 lhs, const uint3& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uint3 operator&(uint3 lhs, cl_uint rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uint3 operator&(cl_uint lhs, const uint3& rhs)
{
  auto tmp = uint3(lhs);
  return tmp & rhs;
}

inline uint3 operator|(uint3 lhs, const uint3& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uint3 operator|(uint3 lhs, cl_uint rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uint3 operator|(cl_uint lhs, const uint3& rhs)
{
  auto tmp = uint3(lhs);
  return tmp | rhs;
}

inline uint3 operator^(uint3 lhs, const uint3& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uint3 operator^(uint3 lhs, cl_uint rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uint3 operator^(cl_uint lhs, const uint3& rhs)
{
  auto tmp = uint3(lhs);
  return tmp ^ rhs;
}

inline uint3 operator<<(uint3 lhs, const uint3& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uint3 operator<<(uint3 lhs, cl_uint rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uint3 operator<<(cl_uint lhs, const uint3& rhs)
{
  auto tmp = uint3(lhs);
  return tmp << rhs;
}

inline uint3 operator>>(uint3 lhs, const uint3& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uint3 operator>>(uint3 lhs, cl_uint rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uint3 operator>>(cl_uint lhs, const uint3& rhs)
{
  auto tmp = uint3(lhs);
  return tmp >> rhs;
}

inline bool operator==(const uint3& lhs, const uint3& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]);
}

inline bool operator==(const uint3& lhs, const cl_uint& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs);
}

inline bool operator<(const uint3& lhs, const uint3& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const uint3& lhs, const cl_uint& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uint3& lhs, const uint3& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uint3& lhs, const cl_uint& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const uint3& lhs, const uint3& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const uint3& lhs, const cl_uint& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const uint3& lhs, const uint3& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const uint3& lhs, const cl_uint& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const uint3& lhs, const uint3& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const uint3& lhs, const cl_uint& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline uint3 vload3(size_t offset, const uint3* p)
{
  return *(p + offset);
}

inline void vstore3(uint3 data, size_t offset, uint3* p)
{
  *(p + offset) = data;
}


class uint4{
public:
  static const size_t N = 4; // Number of elements in vector
  union {
	 cl_uint CL_ALIGNED(16) s[4];
#if defined(__GNUC__)
    __extension__ struct { cl_uint x, y, z, w; };
    __extension__ struct { cl_uint s0, s1, s2, s3; };
#endif
  };

  uint4(){}

  uint4(cl_uint i0,cl_uint i1,cl_uint i2,cl_uint i3
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;

  }

  uint4(cl_uint i0) {
  for (int i = 0; i < 4; ++i) {
		s[i] = i0;
	 }
  }

  uint4(const uint2& a0, const uint2& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
  }

  uint4(const uint4& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
  }

  uint4& operator+=(const uint4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  uint4& operator+=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  uint4& operator-=(const uint4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  uint4& operator-=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  uint4& operator*=(const uint4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  uint4& operator*=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  uint4& operator/=(const uint4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  uint4& operator/=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  uint4& operator%=(const uint4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  uint4& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  uint4& operator&=(const uint4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  uint4& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  uint4& operator|=(const uint4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  uint4& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  uint4& operator^=(const uint4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  uint4& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  uint4& operator<<=(const uint4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  uint4& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  uint4& operator>>=(const uint4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  uint4& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline uint4 operator+(uint4 lhs, const uint4& rhs)
{
  lhs += rhs;
  return lhs;
}

inline uint4 operator+(uint4 lhs, cl_uint rhs)
{
  lhs += rhs;
  return lhs;
}

inline uint4 operator+(cl_uint lhs, const uint4& rhs)
{
  auto tmp = uint4(lhs);
  return tmp + rhs;
}

inline uint4 operator-(uint4 lhs, const uint4& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uint4 operator-(uint4 lhs, cl_uint rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uint4 operator-(cl_uint lhs, const uint4& rhs)
{
  auto tmp = uint4(lhs);
  return tmp - rhs;
}

inline uint4 operator*(uint4 lhs, const uint4& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uint4 operator*(uint4 lhs, cl_uint rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uint4 operator*(cl_uint lhs, const uint4& rhs)
{
  auto tmp = uint4(lhs);
  return tmp * rhs;
}

inline uint4 operator/(uint4 lhs, const uint4& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uint4 operator/(uint4 lhs, cl_uint rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uint4 operator/(cl_uint lhs, const uint4& rhs)
{
  auto tmp = uint4(lhs);
  return tmp / rhs;
}

inline uint4 operator%(uint4 lhs, const uint4& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uint4 operator%(uint4 lhs, cl_uint rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uint4 operator%(cl_uint lhs, const uint4& rhs)
{
  auto tmp = uint4(lhs);
  return tmp % rhs;
}

inline uint4 operator&(uint4 lhs, const uint4& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uint4 operator&(uint4 lhs, cl_uint rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uint4 operator&(cl_uint lhs, const uint4& rhs)
{
  auto tmp = uint4(lhs);
  return tmp & rhs;
}

inline uint4 operator|(uint4 lhs, const uint4& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uint4 operator|(uint4 lhs, cl_uint rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uint4 operator|(cl_uint lhs, const uint4& rhs)
{
  auto tmp = uint4(lhs);
  return tmp | rhs;
}

inline uint4 operator^(uint4 lhs, const uint4& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uint4 operator^(uint4 lhs, cl_uint rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uint4 operator^(cl_uint lhs, const uint4& rhs)
{
  auto tmp = uint4(lhs);
  return tmp ^ rhs;
}

inline uint4 operator<<(uint4 lhs, const uint4& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uint4 operator<<(uint4 lhs, cl_uint rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uint4 operator<<(cl_uint lhs, const uint4& rhs)
{
  auto tmp = uint4(lhs);
  return tmp << rhs;
}

inline uint4 operator>>(uint4 lhs, const uint4& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uint4 operator>>(uint4 lhs, cl_uint rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uint4 operator>>(cl_uint lhs, const uint4& rhs)
{
  auto tmp = uint4(lhs);
  return tmp >> rhs;
}

inline bool operator==(const uint4& lhs, const uint4& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]);
}

inline bool operator==(const uint4& lhs, const cl_uint& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs);
}

inline bool operator<(const uint4& lhs, const uint4& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const uint4& lhs, const cl_uint& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uint4& lhs, const uint4& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uint4& lhs, const cl_uint& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const uint4& lhs, const uint4& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const uint4& lhs, const cl_uint& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const uint4& lhs, const uint4& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const uint4& lhs, const cl_uint& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const uint4& lhs, const uint4& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const uint4& lhs, const cl_uint& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline uint4 vload4(size_t offset, const uint4* p)
{
  return *(p + offset);
}

inline void vstore4(uint4 data, size_t offset, uint4* p)
{
  *(p + offset) = data;
}


class uint8{
public:
  static const size_t N = 8; // Number of elements in vector
  union {
	 cl_uint CL_ALIGNED(32) s[8];
#if defined(__GNUC__)
    __extension__ struct { cl_uint x, y, z, w; };
    __extension__ struct { cl_uint s0, s1, s2, s3, s4, s5, s6, s7; };
#endif
  };

  uint8(){}

  uint8(cl_uint i0,cl_uint i1,cl_uint i2,cl_uint i3,
          cl_uint i4,cl_uint i5,cl_uint i6,cl_uint i7
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;
    s[4] = i4;
    s[5] = i5;
    s[6] = i6;
    s[7] = i7;

  }

  uint8(cl_uint i0) {
  for (int i = 0; i < 8; ++i) {
		s[i] = i0;
	 }
  }

  uint8(const uint2& a0, const uint2& a1, const uint2& a2, const uint2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
  }

  uint8(const uint2& a0, const uint2& a1, const uint4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
  }

  uint8(const uint2& a0, const uint3& a1, const uint3& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
  }

  uint8(const uint2& a0, const uint4& a1, const uint2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  uint8(const uint3& a0, const uint2& a1, const uint3& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
  }

  uint8(const uint3& a0, const uint3& a1, const uint2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  uint8(const uint4& a0, const uint2& a1, const uint2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  uint8(const uint4& a0, const uint4& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
  }

  uint8(const uint8& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
  }

  uint8& operator+=(const uint8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  uint8& operator+=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  uint8& operator-=(const uint8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  uint8& operator-=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  uint8& operator*=(const uint8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  uint8& operator*=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  uint8& operator/=(const uint8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  uint8& operator/=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  uint8& operator%=(const uint8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  uint8& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  uint8& operator&=(const uint8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  uint8& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  uint8& operator|=(const uint8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  uint8& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  uint8& operator^=(const uint8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  uint8& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  uint8& operator<<=(const uint8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  uint8& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  uint8& operator>>=(const uint8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  uint8& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline uint8 operator+(uint8 lhs, const uint8& rhs)
{
  lhs += rhs;
  return lhs;
}

inline uint8 operator+(uint8 lhs, cl_uint rhs)
{
  lhs += rhs;
  return lhs;
}

inline uint8 operator+(cl_uint lhs, const uint8& rhs)
{
  auto tmp = uint8(lhs);
  return tmp + rhs;
}

inline uint8 operator-(uint8 lhs, const uint8& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uint8 operator-(uint8 lhs, cl_uint rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uint8 operator-(cl_uint lhs, const uint8& rhs)
{
  auto tmp = uint8(lhs);
  return tmp - rhs;
}

inline uint8 operator*(uint8 lhs, const uint8& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uint8 operator*(uint8 lhs, cl_uint rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uint8 operator*(cl_uint lhs, const uint8& rhs)
{
  auto tmp = uint8(lhs);
  return tmp * rhs;
}

inline uint8 operator/(uint8 lhs, const uint8& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uint8 operator/(uint8 lhs, cl_uint rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uint8 operator/(cl_uint lhs, const uint8& rhs)
{
  auto tmp = uint8(lhs);
  return tmp / rhs;
}

inline uint8 operator%(uint8 lhs, const uint8& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uint8 operator%(uint8 lhs, cl_uint rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uint8 operator%(cl_uint lhs, const uint8& rhs)
{
  auto tmp = uint8(lhs);
  return tmp % rhs;
}

inline uint8 operator&(uint8 lhs, const uint8& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uint8 operator&(uint8 lhs, cl_uint rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uint8 operator&(cl_uint lhs, const uint8& rhs)
{
  auto tmp = uint8(lhs);
  return tmp & rhs;
}

inline uint8 operator|(uint8 lhs, const uint8& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uint8 operator|(uint8 lhs, cl_uint rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uint8 operator|(cl_uint lhs, const uint8& rhs)
{
  auto tmp = uint8(lhs);
  return tmp | rhs;
}

inline uint8 operator^(uint8 lhs, const uint8& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uint8 operator^(uint8 lhs, cl_uint rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uint8 operator^(cl_uint lhs, const uint8& rhs)
{
  auto tmp = uint8(lhs);
  return tmp ^ rhs;
}

inline uint8 operator<<(uint8 lhs, const uint8& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uint8 operator<<(uint8 lhs, cl_uint rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uint8 operator<<(cl_uint lhs, const uint8& rhs)
{
  auto tmp = uint8(lhs);
  return tmp << rhs;
}

inline uint8 operator>>(uint8 lhs, const uint8& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uint8 operator>>(uint8 lhs, cl_uint rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uint8 operator>>(cl_uint lhs, const uint8& rhs)
{
  auto tmp = uint8(lhs);
  return tmp >> rhs;
}

inline bool operator==(const uint8& lhs, const uint8& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]) &&
  (lhs.s[4] == rhs.s[4]) &&
  (lhs.s[5] == rhs.s[5]) &&
  (lhs.s[6] == rhs.s[6]) &&
  (lhs.s[7] == rhs.s[7]);
}

inline bool operator==(const uint8& lhs, const cl_uint& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs) &&
  (lhs.s[4] == rhs) &&
  (lhs.s[5] == rhs) &&
  (lhs.s[6] == rhs) &&
  (lhs.s[7] == rhs);
}

inline bool operator<(const uint8& lhs, const uint8& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const uint8& lhs, const cl_uint& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uint8& lhs, const uint8& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uint8& lhs, const cl_uint& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const uint8& lhs, const uint8& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const uint8& lhs, const cl_uint& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const uint8& lhs, const uint8& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const uint8& lhs, const cl_uint& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const uint8& lhs, const uint8& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const uint8& lhs, const cl_uint& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline uint8 vload8(size_t offset, const uint8* p)
{
  return *(p + offset);
}

inline void vstore8(uint8 data, size_t offset, uint8* p)
{
  *(p + offset) = data;
}


class uint16{
public:
  static const size_t N = 16; // Number of elements in vector
  union {
	 cl_uint CL_ALIGNED(64) s[16];
#if defined(__GNUC__)
    __extension__ struct { cl_uint x, y, z, w; };
    __extension__ struct { cl_uint s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15; };
#endif
  };

  uint16(){}

  uint16(cl_uint i0,cl_uint i1,cl_uint i2,cl_uint i3,
          cl_uint i4,cl_uint i5,cl_uint i6,cl_uint i7,
          cl_uint i8,cl_uint i9,cl_uint i10,cl_uint i11,
          cl_uint i12,cl_uint i13,cl_uint i14,cl_uint i15
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;
    s[4] = i4;
    s[5] = i5;
    s[6] = i6;
    s[7] = i7;
    s[8] = i8;
    s[9] = i9;
    s[10] = i10;
    s[11] = i11;
    s[12] = i12;
    s[13] = i13;
    s[14] = i14;
    s[15] = i15;

  }

  uint16(cl_uint i0) {
  for (int i = 0; i < 16; ++i) {
		s[i] = i0;
	 }
  }

  uint16(const uint2& a0, const uint2& a1, const uint2& a2, const uint2& a3, const uint2& a4, const uint2& a5, const uint2& a6, const uint2& a7) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a6.s[0];
    s[13] = a6.s[1];
    s[14] = a7.s[0];
    s[15] = a7.s[1];
  }

  uint16(const uint2& a0, const uint2& a1, const uint2& a2, const uint2& a3, const uint2& a4, const uint2& a5, const uint4& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a6.s[0];
    s[13] = a6.s[1];
    s[14] = a6.s[2];
    s[15] = a6.s[3];
  }

  uint16(const uint2& a0, const uint2& a1, const uint2& a2, const uint2& a3, const uint2& a4, const uint3& a5, const uint3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a5.s[2];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  uint16(const uint2& a0, const uint2& a1, const uint2& a2, const uint2& a3, const uint2& a4, const uint4& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a5.s[2];
    s[13] = a5.s[3];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint2& a0, const uint2& a1, const uint2& a2, const uint2& a3, const uint3& a4, const uint2& a5, const uint3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  uint16(const uint2& a0, const uint2& a1, const uint2& a2, const uint2& a3, const uint3& a4, const uint3& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint2& a0, const uint2& a1, const uint2& a2, const uint2& a3, const uint4& a4, const uint2& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint2& a0, const uint2& a1, const uint2& a2, const uint2& a3, const uint4& a4, const uint4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uint16(const uint2& a0, const uint2& a1, const uint2& a2, const uint2& a3, const uint8& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a4.s[4];
    s[13] = a4.s[5];
    s[14] = a4.s[6];
    s[15] = a4.s[7];
  }

  uint16(const uint2& a0, const uint2& a1, const uint2& a2, const uint3& a3, const uint2& a4, const uint2& a5, const uint3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  uint16(const uint2& a0, const uint2& a1, const uint2& a2, const uint3& a3, const uint2& a4, const uint3& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint2& a0, const uint2& a1, const uint2& a2, const uint3& a3, const uint3& a4, const uint2& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint2& a0, const uint2& a1, const uint2& a2, const uint3& a3, const uint3& a4, const uint4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uint16(const uint2& a0, const uint2& a1, const uint2& a2, const uint3& a3, const uint4& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint2& a0, const uint2& a1, const uint2& a2, const uint4& a3, const uint2& a4, const uint2& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint2& a0, const uint2& a1, const uint2& a2, const uint4& a3, const uint2& a4, const uint4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uint16(const uint2& a0, const uint2& a1, const uint2& a2, const uint4& a3, const uint3& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint2& a0, const uint2& a1, const uint2& a2, const uint4& a3, const uint4& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint2& a0, const uint2& a1, const uint2& a2, const uint8& a3, const uint2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a3.s[4];
    s[11] = a3.s[5];
    s[12] = a3.s[6];
    s[13] = a3.s[7];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uint16(const uint2& a0, const uint2& a1, const uint3& a2, const uint2& a3, const uint2& a4, const uint2& a5, const uint3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  uint16(const uint2& a0, const uint2& a1, const uint3& a2, const uint2& a3, const uint2& a4, const uint3& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint2& a0, const uint2& a1, const uint3& a2, const uint2& a3, const uint3& a4, const uint2& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint2& a0, const uint2& a1, const uint3& a2, const uint2& a3, const uint3& a4, const uint4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uint16(const uint2& a0, const uint2& a1, const uint3& a2, const uint2& a3, const uint4& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint2& a0, const uint2& a1, const uint3& a2, const uint3& a3, const uint2& a4, const uint2& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint2& a0, const uint2& a1, const uint3& a2, const uint3& a3, const uint2& a4, const uint4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uint16(const uint2& a0, const uint2& a1, const uint3& a2, const uint3& a3, const uint3& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint2& a0, const uint2& a1, const uint3& a2, const uint3& a3, const uint4& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint2& a0, const uint2& a1, const uint3& a2, const uint4& a3, const uint2& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint2& a0, const uint2& a1, const uint3& a2, const uint4& a3, const uint3& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint2& a0, const uint2& a1, const uint4& a2, const uint2& a3, const uint2& a4, const uint2& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint2& a0, const uint2& a1, const uint4& a2, const uint2& a3, const uint2& a4, const uint4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uint16(const uint2& a0, const uint2& a1, const uint4& a2, const uint2& a3, const uint3& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint2& a0, const uint2& a1, const uint4& a2, const uint2& a3, const uint4& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint2& a0, const uint2& a1, const uint4& a2, const uint3& a3, const uint2& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint2& a0, const uint2& a1, const uint4& a2, const uint3& a3, const uint3& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint2& a0, const uint2& a1, const uint4& a2, const uint4& a3, const uint2& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint2& a0, const uint2& a1, const uint4& a2, const uint4& a3, const uint4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uint16(const uint2& a0, const uint2& a1, const uint4& a2, const uint8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  uint16(const uint2& a0, const uint2& a1, const uint8& a2, const uint2& a3, const uint2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a2.s[4];
    s[9] = a2.s[5];
    s[10] = a2.s[6];
    s[11] = a2.s[7];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uint16(const uint2& a0, const uint2& a1, const uint8& a2, const uint4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a2.s[4];
    s[9] = a2.s[5];
    s[10] = a2.s[6];
    s[11] = a2.s[7];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  uint16(const uint2& a0, const uint3& a1, const uint2& a2, const uint2& a3, const uint2& a4, const uint2& a5, const uint3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  uint16(const uint2& a0, const uint3& a1, const uint2& a2, const uint2& a3, const uint2& a4, const uint3& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint2& a0, const uint3& a1, const uint2& a2, const uint2& a3, const uint3& a4, const uint2& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint2& a0, const uint3& a1, const uint2& a2, const uint2& a3, const uint3& a4, const uint4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uint16(const uint2& a0, const uint3& a1, const uint2& a2, const uint2& a3, const uint4& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint2& a0, const uint3& a1, const uint2& a2, const uint3& a3, const uint2& a4, const uint2& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint2& a0, const uint3& a1, const uint2& a2, const uint3& a3, const uint2& a4, const uint4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uint16(const uint2& a0, const uint3& a1, const uint2& a2, const uint3& a3, const uint3& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint2& a0, const uint3& a1, const uint2& a2, const uint3& a3, const uint4& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint2& a0, const uint3& a1, const uint2& a2, const uint4& a3, const uint2& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint2& a0, const uint3& a1, const uint2& a2, const uint4& a3, const uint3& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint2& a0, const uint3& a1, const uint3& a2, const uint2& a3, const uint2& a4, const uint2& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint2& a0, const uint3& a1, const uint3& a2, const uint2& a3, const uint2& a4, const uint4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uint16(const uint2& a0, const uint3& a1, const uint3& a2, const uint2& a3, const uint3& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint2& a0, const uint3& a1, const uint3& a2, const uint2& a3, const uint4& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint2& a0, const uint3& a1, const uint3& a2, const uint3& a3, const uint2& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint2& a0, const uint3& a1, const uint3& a2, const uint3& a3, const uint3& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint2& a0, const uint3& a1, const uint3& a2, const uint4& a3, const uint2& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint2& a0, const uint3& a1, const uint3& a2, const uint4& a3, const uint4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uint16(const uint2& a0, const uint3& a1, const uint3& a2, const uint8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  uint16(const uint2& a0, const uint3& a1, const uint4& a2, const uint2& a3, const uint2& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint2& a0, const uint3& a1, const uint4& a2, const uint2& a3, const uint3& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint2& a0, const uint3& a1, const uint4& a2, const uint3& a3, const uint2& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint2& a0, const uint3& a1, const uint4& a2, const uint3& a3, const uint4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uint16(const uint2& a0, const uint3& a1, const uint4& a2, const uint4& a3, const uint3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uint16(const uint2& a0, const uint3& a1, const uint8& a2, const uint3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a2.s[4];
    s[10] = a2.s[5];
    s[11] = a2.s[6];
    s[12] = a2.s[7];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  uint16(const uint2& a0, const uint4& a1, const uint2& a2, const uint2& a3, const uint2& a4, const uint2& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint2& a0, const uint4& a1, const uint2& a2, const uint2& a3, const uint2& a4, const uint4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uint16(const uint2& a0, const uint4& a1, const uint2& a2, const uint2& a3, const uint3& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint2& a0, const uint4& a1, const uint2& a2, const uint2& a3, const uint4& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint2& a0, const uint4& a1, const uint2& a2, const uint3& a3, const uint2& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint2& a0, const uint4& a1, const uint2& a2, const uint3& a3, const uint3& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint2& a0, const uint4& a1, const uint2& a2, const uint4& a3, const uint2& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint2& a0, const uint4& a1, const uint2& a2, const uint4& a3, const uint4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uint16(const uint2& a0, const uint4& a1, const uint2& a2, const uint8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  uint16(const uint2& a0, const uint4& a1, const uint3& a2, const uint2& a3, const uint2& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint2& a0, const uint4& a1, const uint3& a2, const uint2& a3, const uint3& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint2& a0, const uint4& a1, const uint3& a2, const uint3& a3, const uint2& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint2& a0, const uint4& a1, const uint3& a2, const uint3& a3, const uint4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uint16(const uint2& a0, const uint4& a1, const uint3& a2, const uint4& a3, const uint3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uint16(const uint2& a0, const uint4& a1, const uint4& a2, const uint2& a3, const uint2& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint2& a0, const uint4& a1, const uint4& a2, const uint2& a3, const uint4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uint16(const uint2& a0, const uint4& a1, const uint4& a2, const uint3& a3, const uint3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uint16(const uint2& a0, const uint4& a1, const uint4& a2, const uint4& a3, const uint2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uint16(const uint2& a0, const uint4& a1, const uint8& a2, const uint2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  uint16(const uint2& a0, const uint8& a1, const uint2& a2, const uint2& a3, const uint2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uint16(const uint2& a0, const uint8& a1, const uint2& a2, const uint4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  uint16(const uint2& a0, const uint8& a1, const uint3& a2, const uint3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  uint16(const uint2& a0, const uint8& a1, const uint4& a2, const uint2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a2.s[3];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  uint16(const uint3& a0, const uint2& a1, const uint2& a2, const uint2& a3, const uint2& a4, const uint2& a5, const uint3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  uint16(const uint3& a0, const uint2& a1, const uint2& a2, const uint2& a3, const uint2& a4, const uint3& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint3& a0, const uint2& a1, const uint2& a2, const uint2& a3, const uint3& a4, const uint2& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint3& a0, const uint2& a1, const uint2& a2, const uint2& a3, const uint3& a4, const uint4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uint16(const uint3& a0, const uint2& a1, const uint2& a2, const uint2& a3, const uint4& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint3& a0, const uint2& a1, const uint2& a2, const uint3& a3, const uint2& a4, const uint2& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint3& a0, const uint2& a1, const uint2& a2, const uint3& a3, const uint2& a4, const uint4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uint16(const uint3& a0, const uint2& a1, const uint2& a2, const uint3& a3, const uint3& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint3& a0, const uint2& a1, const uint2& a2, const uint3& a3, const uint4& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint3& a0, const uint2& a1, const uint2& a2, const uint4& a3, const uint2& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint3& a0, const uint2& a1, const uint2& a2, const uint4& a3, const uint3& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint3& a0, const uint2& a1, const uint3& a2, const uint2& a3, const uint2& a4, const uint2& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint3& a0, const uint2& a1, const uint3& a2, const uint2& a3, const uint2& a4, const uint4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uint16(const uint3& a0, const uint2& a1, const uint3& a2, const uint2& a3, const uint3& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint3& a0, const uint2& a1, const uint3& a2, const uint2& a3, const uint4& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint3& a0, const uint2& a1, const uint3& a2, const uint3& a3, const uint2& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint3& a0, const uint2& a1, const uint3& a2, const uint3& a3, const uint3& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint3& a0, const uint2& a1, const uint3& a2, const uint4& a3, const uint2& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint3& a0, const uint2& a1, const uint3& a2, const uint4& a3, const uint4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uint16(const uint3& a0, const uint2& a1, const uint3& a2, const uint8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  uint16(const uint3& a0, const uint2& a1, const uint4& a2, const uint2& a3, const uint2& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint3& a0, const uint2& a1, const uint4& a2, const uint2& a3, const uint3& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint3& a0, const uint2& a1, const uint4& a2, const uint3& a3, const uint2& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint3& a0, const uint2& a1, const uint4& a2, const uint3& a3, const uint4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uint16(const uint3& a0, const uint2& a1, const uint4& a2, const uint4& a3, const uint3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uint16(const uint3& a0, const uint2& a1, const uint8& a2, const uint3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a2.s[4];
    s[10] = a2.s[5];
    s[11] = a2.s[6];
    s[12] = a2.s[7];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  uint16(const uint3& a0, const uint3& a1, const uint2& a2, const uint2& a3, const uint2& a4, const uint2& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint3& a0, const uint3& a1, const uint2& a2, const uint2& a3, const uint2& a4, const uint4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uint16(const uint3& a0, const uint3& a1, const uint2& a2, const uint2& a3, const uint3& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint3& a0, const uint3& a1, const uint2& a2, const uint2& a3, const uint4& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint3& a0, const uint3& a1, const uint2& a2, const uint3& a3, const uint2& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint3& a0, const uint3& a1, const uint2& a2, const uint3& a3, const uint3& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint3& a0, const uint3& a1, const uint2& a2, const uint4& a3, const uint2& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint3& a0, const uint3& a1, const uint2& a2, const uint4& a3, const uint4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uint16(const uint3& a0, const uint3& a1, const uint2& a2, const uint8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  uint16(const uint3& a0, const uint3& a1, const uint3& a2, const uint2& a3, const uint2& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint3& a0, const uint3& a1, const uint3& a2, const uint2& a3, const uint3& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint3& a0, const uint3& a1, const uint3& a2, const uint3& a3, const uint2& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint3& a0, const uint3& a1, const uint3& a2, const uint3& a3, const uint4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uint16(const uint3& a0, const uint3& a1, const uint3& a2, const uint4& a3, const uint3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uint16(const uint3& a0, const uint3& a1, const uint4& a2, const uint2& a3, const uint2& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint3& a0, const uint3& a1, const uint4& a2, const uint2& a3, const uint4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uint16(const uint3& a0, const uint3& a1, const uint4& a2, const uint3& a3, const uint3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uint16(const uint3& a0, const uint3& a1, const uint4& a2, const uint4& a3, const uint2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uint16(const uint3& a0, const uint3& a1, const uint8& a2, const uint2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  uint16(const uint3& a0, const uint4& a1, const uint2& a2, const uint2& a3, const uint2& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint3& a0, const uint4& a1, const uint2& a2, const uint2& a3, const uint3& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint3& a0, const uint4& a1, const uint2& a2, const uint3& a3, const uint2& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint3& a0, const uint4& a1, const uint2& a2, const uint3& a3, const uint4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uint16(const uint3& a0, const uint4& a1, const uint2& a2, const uint4& a3, const uint3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uint16(const uint3& a0, const uint4& a1, const uint3& a2, const uint2& a3, const uint2& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint3& a0, const uint4& a1, const uint3& a2, const uint2& a3, const uint4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uint16(const uint3& a0, const uint4& a1, const uint3& a2, const uint3& a3, const uint3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uint16(const uint3& a0, const uint4& a1, const uint3& a2, const uint4& a3, const uint2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uint16(const uint3& a0, const uint4& a1, const uint4& a2, const uint2& a3, const uint3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uint16(const uint3& a0, const uint4& a1, const uint4& a2, const uint3& a3, const uint2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uint16(const uint3& a0, const uint8& a1, const uint2& a2, const uint3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a1.s[4];
    s[8] = a1.s[5];
    s[9] = a1.s[6];
    s[10] = a1.s[7];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  uint16(const uint3& a0, const uint8& a1, const uint3& a2, const uint2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a1.s[4];
    s[8] = a1.s[5];
    s[9] = a1.s[6];
    s[10] = a1.s[7];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a2.s[2];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  uint16(const uint4& a0, const uint2& a1, const uint2& a2, const uint2& a3, const uint2& a4, const uint2& a5, const uint2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  uint16(const uint4& a0, const uint2& a1, const uint2& a2, const uint2& a3, const uint2& a4, const uint4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  uint16(const uint4& a0, const uint2& a1, const uint2& a2, const uint2& a3, const uint3& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint4& a0, const uint2& a1, const uint2& a2, const uint2& a3, const uint4& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint4& a0, const uint2& a1, const uint2& a2, const uint3& a3, const uint2& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint4& a0, const uint2& a1, const uint2& a2, const uint3& a3, const uint3& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint4& a0, const uint2& a1, const uint2& a2, const uint4& a3, const uint2& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint4& a0, const uint2& a1, const uint2& a2, const uint4& a3, const uint4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uint16(const uint4& a0, const uint2& a1, const uint2& a2, const uint8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  uint16(const uint4& a0, const uint2& a1, const uint3& a2, const uint2& a3, const uint2& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint4& a0, const uint2& a1, const uint3& a2, const uint2& a3, const uint3& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint4& a0, const uint2& a1, const uint3& a2, const uint3& a3, const uint2& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint4& a0, const uint2& a1, const uint3& a2, const uint3& a3, const uint4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uint16(const uint4& a0, const uint2& a1, const uint3& a2, const uint4& a3, const uint3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uint16(const uint4& a0, const uint2& a1, const uint4& a2, const uint2& a3, const uint2& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint4& a0, const uint2& a1, const uint4& a2, const uint2& a3, const uint4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uint16(const uint4& a0, const uint2& a1, const uint4& a2, const uint3& a3, const uint3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uint16(const uint4& a0, const uint2& a1, const uint4& a2, const uint4& a3, const uint2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uint16(const uint4& a0, const uint2& a1, const uint8& a2, const uint2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  uint16(const uint4& a0, const uint3& a1, const uint2& a2, const uint2& a3, const uint2& a4, const uint3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  uint16(const uint4& a0, const uint3& a1, const uint2& a2, const uint2& a3, const uint3& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint4& a0, const uint3& a1, const uint2& a2, const uint3& a3, const uint2& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint4& a0, const uint3& a1, const uint2& a2, const uint3& a3, const uint4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uint16(const uint4& a0, const uint3& a1, const uint2& a2, const uint4& a3, const uint3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uint16(const uint4& a0, const uint3& a1, const uint3& a2, const uint2& a3, const uint2& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint4& a0, const uint3& a1, const uint3& a2, const uint2& a3, const uint4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uint16(const uint4& a0, const uint3& a1, const uint3& a2, const uint3& a3, const uint3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uint16(const uint4& a0, const uint3& a1, const uint3& a2, const uint4& a3, const uint2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uint16(const uint4& a0, const uint3& a1, const uint4& a2, const uint2& a3, const uint3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uint16(const uint4& a0, const uint3& a1, const uint4& a2, const uint3& a3, const uint2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uint16(const uint4& a0, const uint4& a1, const uint2& a2, const uint2& a3, const uint2& a4, const uint2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  uint16(const uint4& a0, const uint4& a1, const uint2& a2, const uint2& a3, const uint4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  uint16(const uint4& a0, const uint4& a1, const uint2& a2, const uint3& a3, const uint3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uint16(const uint4& a0, const uint4& a1, const uint2& a2, const uint4& a3, const uint2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uint16(const uint4& a0, const uint4& a1, const uint3& a2, const uint2& a3, const uint3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  uint16(const uint4& a0, const uint4& a1, const uint3& a2, const uint3& a3, const uint2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uint16(const uint4& a0, const uint4& a1, const uint4& a2, const uint2& a3, const uint2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uint16(const uint4& a0, const uint4& a1, const uint4& a2, const uint4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  uint16(const uint4& a0, const uint4& a1, const uint8& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a2.s[4];
    s[13] = a2.s[5];
    s[14] = a2.s[6];
    s[15] = a2.s[7];
  }

  uint16(const uint4& a0, const uint8& a1, const uint2& a2, const uint2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a1.s[4];
    s[9] = a1.s[5];
    s[10] = a1.s[6];
    s[11] = a1.s[7];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  uint16(const uint4& a0, const uint8& a1, const uint4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a1.s[4];
    s[9] = a1.s[5];
    s[10] = a1.s[6];
    s[11] = a1.s[7];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a2.s[2];
    s[15] = a2.s[3];
  }

  uint16(const uint8& a0, const uint2& a1, const uint2& a2, const uint2& a3, const uint2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  uint16(const uint8& a0, const uint2& a1, const uint2& a2, const uint4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  uint16(const uint8& a0, const uint2& a1, const uint3& a2, const uint3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  uint16(const uint8& a0, const uint2& a1, const uint4& a2, const uint2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a2.s[3];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  uint16(const uint8& a0, const uint3& a1, const uint2& a2, const uint3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  uint16(const uint8& a0, const uint3& a1, const uint3& a2, const uint2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a2.s[2];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  uint16(const uint8& a0, const uint4& a1, const uint2& a2, const uint2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  uint16(const uint8& a0, const uint4& a1, const uint4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a2.s[2];
    s[15] = a2.s[3];
  }

  uint16(const uint8& a0, const uint8& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a1.s[4];
    s[13] = a1.s[5];
    s[14] = a1.s[6];
    s[15] = a1.s[7];
  }

  uint16(const uint16& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a0.s[8];
    s[9] = a0.s[9];
    s[10] = a0.s[10];
    s[11] = a0.s[11];
    s[12] = a0.s[12];
    s[13] = a0.s[13];
    s[14] = a0.s[14];
    s[15] = a0.s[15];
  }

  uint16& operator+=(const uint16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  uint16& operator+=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  uint16& operator-=(const uint16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  uint16& operator-=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  uint16& operator*=(const uint16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  uint16& operator*=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  uint16& operator/=(const uint16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  uint16& operator/=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  uint16& operator%=(const uint16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  uint16& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  uint16& operator&=(const uint16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  uint16& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  uint16& operator|=(const uint16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  uint16& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  uint16& operator^=(const uint16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  uint16& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  uint16& operator<<=(const uint16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  uint16& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  uint16& operator>>=(const uint16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  uint16& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline uint16 operator+(uint16 lhs, const uint16& rhs)
{
  lhs += rhs;
  return lhs;
}

inline uint16 operator+(uint16 lhs, cl_uint rhs)
{
  lhs += rhs;
  return lhs;
}

inline uint16 operator+(cl_uint lhs, const uint16& rhs)
{
  auto tmp = uint16(lhs);
  return tmp + rhs;
}

inline uint16 operator-(uint16 lhs, const uint16& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uint16 operator-(uint16 lhs, cl_uint rhs)
{
  lhs -= rhs;
  return lhs;
}

inline uint16 operator-(cl_uint lhs, const uint16& rhs)
{
  auto tmp = uint16(lhs);
  return tmp - rhs;
}

inline uint16 operator*(uint16 lhs, const uint16& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uint16 operator*(uint16 lhs, cl_uint rhs)
{
  lhs *= rhs;
  return lhs;
}

inline uint16 operator*(cl_uint lhs, const uint16& rhs)
{
  auto tmp = uint16(lhs);
  return tmp * rhs;
}

inline uint16 operator/(uint16 lhs, const uint16& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uint16 operator/(uint16 lhs, cl_uint rhs)
{
  lhs /= rhs;
  return lhs;
}

inline uint16 operator/(cl_uint lhs, const uint16& rhs)
{
  auto tmp = uint16(lhs);
  return tmp / rhs;
}

inline uint16 operator%(uint16 lhs, const uint16& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uint16 operator%(uint16 lhs, cl_uint rhs)
{
  lhs %= rhs;
  return lhs;
}

inline uint16 operator%(cl_uint lhs, const uint16& rhs)
{
  auto tmp = uint16(lhs);
  return tmp % rhs;
}

inline uint16 operator&(uint16 lhs, const uint16& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uint16 operator&(uint16 lhs, cl_uint rhs)
{
  lhs &= rhs;
  return lhs;
}

inline uint16 operator&(cl_uint lhs, const uint16& rhs)
{
  auto tmp = uint16(lhs);
  return tmp & rhs;
}

inline uint16 operator|(uint16 lhs, const uint16& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uint16 operator|(uint16 lhs, cl_uint rhs)
{
  lhs |= rhs;
  return lhs;
}

inline uint16 operator|(cl_uint lhs, const uint16& rhs)
{
  auto tmp = uint16(lhs);
  return tmp | rhs;
}

inline uint16 operator^(uint16 lhs, const uint16& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uint16 operator^(uint16 lhs, cl_uint rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline uint16 operator^(cl_uint lhs, const uint16& rhs)
{
  auto tmp = uint16(lhs);
  return tmp ^ rhs;
}

inline uint16 operator<<(uint16 lhs, const uint16& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uint16 operator<<(uint16 lhs, cl_uint rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline uint16 operator<<(cl_uint lhs, const uint16& rhs)
{
  auto tmp = uint16(lhs);
  return tmp << rhs;
}

inline uint16 operator>>(uint16 lhs, const uint16& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uint16 operator>>(uint16 lhs, cl_uint rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline uint16 operator>>(cl_uint lhs, const uint16& rhs)
{
  auto tmp = uint16(lhs);
  return tmp >> rhs;
}

inline bool operator==(const uint16& lhs, const uint16& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]) &&
  (lhs.s[4] == rhs.s[4]) &&
  (lhs.s[5] == rhs.s[5]) &&
  (lhs.s[6] == rhs.s[6]) &&
  (lhs.s[7] == rhs.s[7]) &&
  (lhs.s[8] == rhs.s[8]) &&
  (lhs.s[9] == rhs.s[9]) &&
  (lhs.s[10] == rhs.s[10]) &&
  (lhs.s[11] == rhs.s[11]) &&
  (lhs.s[12] == rhs.s[12]) &&
  (lhs.s[13] == rhs.s[13]) &&
  (lhs.s[14] == rhs.s[14]) &&
  (lhs.s[15] == rhs.s[15]);
}

inline bool operator==(const uint16& lhs, const cl_uint& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs) &&
  (lhs.s[4] == rhs) &&
  (lhs.s[5] == rhs) &&
  (lhs.s[6] == rhs) &&
  (lhs.s[7] == rhs) &&
  (lhs.s[8] == rhs) &&
  (lhs.s[9] == rhs) &&
  (lhs.s[10] == rhs) &&
  (lhs.s[11] == rhs) &&
  (lhs.s[12] == rhs) &&
  (lhs.s[13] == rhs) &&
  (lhs.s[14] == rhs) &&
  (lhs.s[15] == rhs);
}

inline bool operator<(const uint16& lhs, const uint16& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const uint16& lhs, const cl_uint& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uint16& lhs, const uint16& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const uint16& lhs, const cl_uint& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const uint16& lhs, const uint16& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const uint16& lhs, const cl_uint& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const uint16& lhs, const uint16& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const uint16& lhs, const cl_uint& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const uint16& lhs, const uint16& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const uint16& lhs, const cl_uint& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline uint16 vload16(size_t offset, const uint16* p)
{
  return *(p + offset);
}

inline void vstore16(uint16 data, size_t offset, uint16* p)
{
  *(p + offset) = data;
}


class long2{
public:
  static const size_t N = 2; // Number of elements in vector
  union {
	 cl_long CL_ALIGNED(16) s[2];
#if defined(__GNUC__)
    __extension__ struct { cl_long x, y; };
    __extension__ struct { cl_long s0, s1; };
#endif
  };

  long2(){}

  long2(cl_long i0,cl_long i1){
    s[0] = i0;
    s[1] = i1;

  }

  long2(cl_long i0) {
  for (int i = 0; i < 2; ++i) {
		s[i] = i0;
	 }
  }

  long2(const long2& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
  }

  long2& operator+=(const long2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  long2& operator+=(cl_long rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  long2& operator-=(const long2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  long2& operator-=(cl_long rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  long2& operator*=(const long2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  long2& operator*=(cl_long rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  long2& operator/=(const long2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  long2& operator/=(cl_long rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  long2& operator%=(const long2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  long2& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  long2& operator&=(const long2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  long2& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  long2& operator|=(const long2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  long2& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  long2& operator^=(const long2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  long2& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  long2& operator<<=(const long2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  long2& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  long2& operator>>=(const long2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  long2& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline long2 operator+(long2 lhs, const long2& rhs)
{
  lhs += rhs;
  return lhs;
}

inline long2 operator+(long2 lhs, cl_long rhs)
{
  lhs += rhs;
  return lhs;
}

inline long2 operator+(cl_long lhs, const long2& rhs)
{
  auto tmp = long2(lhs);
  return tmp + rhs;
}

inline long2 operator-(long2 lhs, const long2& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline long2 operator-(long2 lhs, cl_long rhs)
{
  lhs -= rhs;
  return lhs;
}

inline long2 operator-(cl_long lhs, const long2& rhs)
{
  auto tmp = long2(lhs);
  return tmp - rhs;
}

inline long2 operator*(long2 lhs, const long2& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline long2 operator*(long2 lhs, cl_long rhs)
{
  lhs *= rhs;
  return lhs;
}

inline long2 operator*(cl_long lhs, const long2& rhs)
{
  auto tmp = long2(lhs);
  return tmp * rhs;
}

inline long2 operator/(long2 lhs, const long2& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline long2 operator/(long2 lhs, cl_long rhs)
{
  lhs /= rhs;
  return lhs;
}

inline long2 operator/(cl_long lhs, const long2& rhs)
{
  auto tmp = long2(lhs);
  return tmp / rhs;
}

inline long2 operator%(long2 lhs, const long2& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline long2 operator%(long2 lhs, cl_long rhs)
{
  lhs %= rhs;
  return lhs;
}

inline long2 operator%(cl_long lhs, const long2& rhs)
{
  auto tmp = long2(lhs);
  return tmp % rhs;
}

inline long2 operator&(long2 lhs, const long2& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline long2 operator&(long2 lhs, cl_long rhs)
{
  lhs &= rhs;
  return lhs;
}

inline long2 operator&(cl_long lhs, const long2& rhs)
{
  auto tmp = long2(lhs);
  return tmp & rhs;
}

inline long2 operator|(long2 lhs, const long2& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline long2 operator|(long2 lhs, cl_long rhs)
{
  lhs |= rhs;
  return lhs;
}

inline long2 operator|(cl_long lhs, const long2& rhs)
{
  auto tmp = long2(lhs);
  return tmp | rhs;
}

inline long2 operator^(long2 lhs, const long2& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline long2 operator^(long2 lhs, cl_long rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline long2 operator^(cl_long lhs, const long2& rhs)
{
  auto tmp = long2(lhs);
  return tmp ^ rhs;
}

inline long2 operator<<(long2 lhs, const long2& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline long2 operator<<(long2 lhs, cl_long rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline long2 operator<<(cl_long lhs, const long2& rhs)
{
  auto tmp = long2(lhs);
  return tmp << rhs;
}

inline long2 operator>>(long2 lhs, const long2& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline long2 operator>>(long2 lhs, cl_long rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline long2 operator>>(cl_long lhs, const long2& rhs)
{
  auto tmp = long2(lhs);
  return tmp >> rhs;
}

inline bool operator==(const long2& lhs, const long2& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]);
}

inline bool operator==(const long2& lhs, const cl_long& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs);
}

inline bool operator<(const long2& lhs, const long2& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const long2& lhs, const cl_long& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const long2& lhs, const long2& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const long2& lhs, const cl_long& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const long2& lhs, const long2& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const long2& lhs, const cl_long& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const long2& lhs, const long2& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const long2& lhs, const cl_long& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const long2& lhs, const long2& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const long2& lhs, const cl_long& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline long2 vload2(size_t offset, const long2* p)
{
  return *(p + offset);
}

inline void vstore2(long2 data, size_t offset, long2* p)
{
  *(p + offset) = data;
}


class long3{
public:
  static const size_t N = 4; // Number of elements in vector
  union {
	 cl_long CL_ALIGNED(32) s[4];
#if defined(__GNUC__)
    __extension__ struct { cl_long x, y, z, w; };
    __extension__ struct { cl_long s0, s1, s2, s3; };
#endif
  };

  long3(){}

  long3(cl_long i0,cl_long i1,cl_long i2){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;

  }

  long3(cl_long i0) {
  for (int i = 0; i < 3; ++i) {
		s[i] = i0;
	 }
  }

  long3(const long3& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
  }

  long3& operator+=(const long3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  long3& operator+=(cl_long rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  long3& operator-=(const long3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  long3& operator-=(cl_long rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  long3& operator*=(const long3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  long3& operator*=(cl_long rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  long3& operator/=(const long3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  long3& operator/=(cl_long rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  long3& operator%=(const long3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  long3& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  long3& operator&=(const long3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  long3& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  long3& operator|=(const long3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  long3& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  long3& operator^=(const long3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  long3& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  long3& operator<<=(const long3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  long3& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  long3& operator>>=(const long3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  long3& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline long3 operator+(long3 lhs, const long3& rhs)
{
  lhs += rhs;
  return lhs;
}

inline long3 operator+(long3 lhs, cl_long rhs)
{
  lhs += rhs;
  return lhs;
}

inline long3 operator+(cl_long lhs, const long3& rhs)
{
  auto tmp = long3(lhs);
  return tmp + rhs;
}

inline long3 operator-(long3 lhs, const long3& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline long3 operator-(long3 lhs, cl_long rhs)
{
  lhs -= rhs;
  return lhs;
}

inline long3 operator-(cl_long lhs, const long3& rhs)
{
  auto tmp = long3(lhs);
  return tmp - rhs;
}

inline long3 operator*(long3 lhs, const long3& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline long3 operator*(long3 lhs, cl_long rhs)
{
  lhs *= rhs;
  return lhs;
}

inline long3 operator*(cl_long lhs, const long3& rhs)
{
  auto tmp = long3(lhs);
  return tmp * rhs;
}

inline long3 operator/(long3 lhs, const long3& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline long3 operator/(long3 lhs, cl_long rhs)
{
  lhs /= rhs;
  return lhs;
}

inline long3 operator/(cl_long lhs, const long3& rhs)
{
  auto tmp = long3(lhs);
  return tmp / rhs;
}

inline long3 operator%(long3 lhs, const long3& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline long3 operator%(long3 lhs, cl_long rhs)
{
  lhs %= rhs;
  return lhs;
}

inline long3 operator%(cl_long lhs, const long3& rhs)
{
  auto tmp = long3(lhs);
  return tmp % rhs;
}

inline long3 operator&(long3 lhs, const long3& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline long3 operator&(long3 lhs, cl_long rhs)
{
  lhs &= rhs;
  return lhs;
}

inline long3 operator&(cl_long lhs, const long3& rhs)
{
  auto tmp = long3(lhs);
  return tmp & rhs;
}

inline long3 operator|(long3 lhs, const long3& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline long3 operator|(long3 lhs, cl_long rhs)
{
  lhs |= rhs;
  return lhs;
}

inline long3 operator|(cl_long lhs, const long3& rhs)
{
  auto tmp = long3(lhs);
  return tmp | rhs;
}

inline long3 operator^(long3 lhs, const long3& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline long3 operator^(long3 lhs, cl_long rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline long3 operator^(cl_long lhs, const long3& rhs)
{
  auto tmp = long3(lhs);
  return tmp ^ rhs;
}

inline long3 operator<<(long3 lhs, const long3& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline long3 operator<<(long3 lhs, cl_long rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline long3 operator<<(cl_long lhs, const long3& rhs)
{
  auto tmp = long3(lhs);
  return tmp << rhs;
}

inline long3 operator>>(long3 lhs, const long3& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline long3 operator>>(long3 lhs, cl_long rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline long3 operator>>(cl_long lhs, const long3& rhs)
{
  auto tmp = long3(lhs);
  return tmp >> rhs;
}

inline bool operator==(const long3& lhs, const long3& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]);
}

inline bool operator==(const long3& lhs, const cl_long& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs);
}

inline bool operator<(const long3& lhs, const long3& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const long3& lhs, const cl_long& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const long3& lhs, const long3& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const long3& lhs, const cl_long& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const long3& lhs, const long3& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const long3& lhs, const cl_long& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const long3& lhs, const long3& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const long3& lhs, const cl_long& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const long3& lhs, const long3& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const long3& lhs, const cl_long& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline long3 vload3(size_t offset, const long3* p)
{
  return *(p + offset);
}

inline void vstore3(long3 data, size_t offset, long3* p)
{
  *(p + offset) = data;
}


class long4{
public:
  static const size_t N = 4; // Number of elements in vector
  union {
	 cl_long CL_ALIGNED(32) s[4];
#if defined(__GNUC__)
    __extension__ struct { cl_long x, y, z, w; };
    __extension__ struct { cl_long s0, s1, s2, s3; };
#endif
  };

  long4(){}

  long4(cl_long i0,cl_long i1,cl_long i2,cl_long i3
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;

  }

  long4(cl_long i0) {
  for (int i = 0; i < 4; ++i) {
		s[i] = i0;
	 }
  }

  long4(const long2& a0, const long2& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
  }

  long4(const long4& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
  }

  long4& operator+=(const long4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  long4& operator+=(cl_long rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  long4& operator-=(const long4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  long4& operator-=(cl_long rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  long4& operator*=(const long4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  long4& operator*=(cl_long rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  long4& operator/=(const long4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  long4& operator/=(cl_long rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  long4& operator%=(const long4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  long4& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  long4& operator&=(const long4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  long4& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  long4& operator|=(const long4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  long4& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  long4& operator^=(const long4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  long4& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  long4& operator<<=(const long4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  long4& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  long4& operator>>=(const long4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  long4& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline long4 operator+(long4 lhs, const long4& rhs)
{
  lhs += rhs;
  return lhs;
}

inline long4 operator+(long4 lhs, cl_long rhs)
{
  lhs += rhs;
  return lhs;
}

inline long4 operator+(cl_long lhs, const long4& rhs)
{
  auto tmp = long4(lhs);
  return tmp + rhs;
}

inline long4 operator-(long4 lhs, const long4& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline long4 operator-(long4 lhs, cl_long rhs)
{
  lhs -= rhs;
  return lhs;
}

inline long4 operator-(cl_long lhs, const long4& rhs)
{
  auto tmp = long4(lhs);
  return tmp - rhs;
}

inline long4 operator*(long4 lhs, const long4& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline long4 operator*(long4 lhs, cl_long rhs)
{
  lhs *= rhs;
  return lhs;
}

inline long4 operator*(cl_long lhs, const long4& rhs)
{
  auto tmp = long4(lhs);
  return tmp * rhs;
}

inline long4 operator/(long4 lhs, const long4& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline long4 operator/(long4 lhs, cl_long rhs)
{
  lhs /= rhs;
  return lhs;
}

inline long4 operator/(cl_long lhs, const long4& rhs)
{
  auto tmp = long4(lhs);
  return tmp / rhs;
}

inline long4 operator%(long4 lhs, const long4& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline long4 operator%(long4 lhs, cl_long rhs)
{
  lhs %= rhs;
  return lhs;
}

inline long4 operator%(cl_long lhs, const long4& rhs)
{
  auto tmp = long4(lhs);
  return tmp % rhs;
}

inline long4 operator&(long4 lhs, const long4& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline long4 operator&(long4 lhs, cl_long rhs)
{
  lhs &= rhs;
  return lhs;
}

inline long4 operator&(cl_long lhs, const long4& rhs)
{
  auto tmp = long4(lhs);
  return tmp & rhs;
}

inline long4 operator|(long4 lhs, const long4& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline long4 operator|(long4 lhs, cl_long rhs)
{
  lhs |= rhs;
  return lhs;
}

inline long4 operator|(cl_long lhs, const long4& rhs)
{
  auto tmp = long4(lhs);
  return tmp | rhs;
}

inline long4 operator^(long4 lhs, const long4& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline long4 operator^(long4 lhs, cl_long rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline long4 operator^(cl_long lhs, const long4& rhs)
{
  auto tmp = long4(lhs);
  return tmp ^ rhs;
}

inline long4 operator<<(long4 lhs, const long4& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline long4 operator<<(long4 lhs, cl_long rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline long4 operator<<(cl_long lhs, const long4& rhs)
{
  auto tmp = long4(lhs);
  return tmp << rhs;
}

inline long4 operator>>(long4 lhs, const long4& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline long4 operator>>(long4 lhs, cl_long rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline long4 operator>>(cl_long lhs, const long4& rhs)
{
  auto tmp = long4(lhs);
  return tmp >> rhs;
}

inline bool operator==(const long4& lhs, const long4& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]);
}

inline bool operator==(const long4& lhs, const cl_long& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs);
}

inline bool operator<(const long4& lhs, const long4& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const long4& lhs, const cl_long& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const long4& lhs, const long4& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const long4& lhs, const cl_long& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const long4& lhs, const long4& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const long4& lhs, const cl_long& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const long4& lhs, const long4& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const long4& lhs, const cl_long& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const long4& lhs, const long4& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const long4& lhs, const cl_long& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline long4 vload4(size_t offset, const long4* p)
{
  return *(p + offset);
}

inline void vstore4(long4 data, size_t offset, long4* p)
{
  *(p + offset) = data;
}


class long8{
public:
  static const size_t N = 8; // Number of elements in vector
  union {
	 cl_long CL_ALIGNED(64) s[8];
#if defined(__GNUC__)
    __extension__ struct { cl_long x, y, z, w; };
    __extension__ struct { cl_long s0, s1, s2, s3, s4, s5, s6, s7; };
#endif
  };

  long8(){}

  long8(cl_long i0,cl_long i1,cl_long i2,cl_long i3,
          cl_long i4,cl_long i5,cl_long i6,cl_long i7
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;
    s[4] = i4;
    s[5] = i5;
    s[6] = i6;
    s[7] = i7;

  }

  long8(cl_long i0) {
  for (int i = 0; i < 8; ++i) {
		s[i] = i0;
	 }
  }

  long8(const long2& a0, const long2& a1, const long2& a2, const long2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
  }

  long8(const long2& a0, const long2& a1, const long4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
  }

  long8(const long2& a0, const long3& a1, const long3& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
  }

  long8(const long2& a0, const long4& a1, const long2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  long8(const long3& a0, const long2& a1, const long3& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
  }

  long8(const long3& a0, const long3& a1, const long2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  long8(const long4& a0, const long2& a1, const long2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  long8(const long4& a0, const long4& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
  }

  long8(const long8& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
  }

  long8& operator+=(const long8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  long8& operator+=(cl_long rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  long8& operator-=(const long8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  long8& operator-=(cl_long rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  long8& operator*=(const long8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  long8& operator*=(cl_long rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  long8& operator/=(const long8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  long8& operator/=(cl_long rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  long8& operator%=(const long8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  long8& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  long8& operator&=(const long8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  long8& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  long8& operator|=(const long8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  long8& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  long8& operator^=(const long8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  long8& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  long8& operator<<=(const long8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  long8& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  long8& operator>>=(const long8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  long8& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline long8 operator+(long8 lhs, const long8& rhs)
{
  lhs += rhs;
  return lhs;
}

inline long8 operator+(long8 lhs, cl_long rhs)
{
  lhs += rhs;
  return lhs;
}

inline long8 operator+(cl_long lhs, const long8& rhs)
{
  auto tmp = long8(lhs);
  return tmp + rhs;
}

inline long8 operator-(long8 lhs, const long8& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline long8 operator-(long8 lhs, cl_long rhs)
{
  lhs -= rhs;
  return lhs;
}

inline long8 operator-(cl_long lhs, const long8& rhs)
{
  auto tmp = long8(lhs);
  return tmp - rhs;
}

inline long8 operator*(long8 lhs, const long8& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline long8 operator*(long8 lhs, cl_long rhs)
{
  lhs *= rhs;
  return lhs;
}

inline long8 operator*(cl_long lhs, const long8& rhs)
{
  auto tmp = long8(lhs);
  return tmp * rhs;
}

inline long8 operator/(long8 lhs, const long8& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline long8 operator/(long8 lhs, cl_long rhs)
{
  lhs /= rhs;
  return lhs;
}

inline long8 operator/(cl_long lhs, const long8& rhs)
{
  auto tmp = long8(lhs);
  return tmp / rhs;
}

inline long8 operator%(long8 lhs, const long8& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline long8 operator%(long8 lhs, cl_long rhs)
{
  lhs %= rhs;
  return lhs;
}

inline long8 operator%(cl_long lhs, const long8& rhs)
{
  auto tmp = long8(lhs);
  return tmp % rhs;
}

inline long8 operator&(long8 lhs, const long8& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline long8 operator&(long8 lhs, cl_long rhs)
{
  lhs &= rhs;
  return lhs;
}

inline long8 operator&(cl_long lhs, const long8& rhs)
{
  auto tmp = long8(lhs);
  return tmp & rhs;
}

inline long8 operator|(long8 lhs, const long8& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline long8 operator|(long8 lhs, cl_long rhs)
{
  lhs |= rhs;
  return lhs;
}

inline long8 operator|(cl_long lhs, const long8& rhs)
{
  auto tmp = long8(lhs);
  return tmp | rhs;
}

inline long8 operator^(long8 lhs, const long8& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline long8 operator^(long8 lhs, cl_long rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline long8 operator^(cl_long lhs, const long8& rhs)
{
  auto tmp = long8(lhs);
  return tmp ^ rhs;
}

inline long8 operator<<(long8 lhs, const long8& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline long8 operator<<(long8 lhs, cl_long rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline long8 operator<<(cl_long lhs, const long8& rhs)
{
  auto tmp = long8(lhs);
  return tmp << rhs;
}

inline long8 operator>>(long8 lhs, const long8& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline long8 operator>>(long8 lhs, cl_long rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline long8 operator>>(cl_long lhs, const long8& rhs)
{
  auto tmp = long8(lhs);
  return tmp >> rhs;
}

inline bool operator==(const long8& lhs, const long8& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]) &&
  (lhs.s[4] == rhs.s[4]) &&
  (lhs.s[5] == rhs.s[5]) &&
  (lhs.s[6] == rhs.s[6]) &&
  (lhs.s[7] == rhs.s[7]);
}

inline bool operator==(const long8& lhs, const cl_long& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs) &&
  (lhs.s[4] == rhs) &&
  (lhs.s[5] == rhs) &&
  (lhs.s[6] == rhs) &&
  (lhs.s[7] == rhs);
}

inline bool operator<(const long8& lhs, const long8& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const long8& lhs, const cl_long& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const long8& lhs, const long8& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const long8& lhs, const cl_long& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const long8& lhs, const long8& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const long8& lhs, const cl_long& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const long8& lhs, const long8& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const long8& lhs, const cl_long& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const long8& lhs, const long8& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const long8& lhs, const cl_long& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline long8 vload8(size_t offset, const long8* p)
{
  return *(p + offset);
}

inline void vstore8(long8 data, size_t offset, long8* p)
{
  *(p + offset) = data;
}


class long16{
public:
  static const size_t N = 16; // Number of elements in vector
  union {
	 cl_long CL_ALIGNED(128) s[16];
#if defined(__GNUC__)
    __extension__ struct { cl_long x, y, z, w; };
    __extension__ struct { cl_long s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15; };
#endif
  };

  long16(){}

  long16(cl_long i0,cl_long i1,cl_long i2,cl_long i3,
          cl_long i4,cl_long i5,cl_long i6,cl_long i7,
          cl_long i8,cl_long i9,cl_long i10,cl_long i11,
          cl_long i12,cl_long i13,cl_long i14,cl_long i15
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;
    s[4] = i4;
    s[5] = i5;
    s[6] = i6;
    s[7] = i7;
    s[8] = i8;
    s[9] = i9;
    s[10] = i10;
    s[11] = i11;
    s[12] = i12;
    s[13] = i13;
    s[14] = i14;
    s[15] = i15;

  }

  long16(cl_long i0) {
  for (int i = 0; i < 16; ++i) {
		s[i] = i0;
	 }
  }

  long16(const long2& a0, const long2& a1, const long2& a2, const long2& a3, const long2& a4, const long2& a5, const long2& a6, const long2& a7) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a6.s[0];
    s[13] = a6.s[1];
    s[14] = a7.s[0];
    s[15] = a7.s[1];
  }

  long16(const long2& a0, const long2& a1, const long2& a2, const long2& a3, const long2& a4, const long2& a5, const long4& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a6.s[0];
    s[13] = a6.s[1];
    s[14] = a6.s[2];
    s[15] = a6.s[3];
  }

  long16(const long2& a0, const long2& a1, const long2& a2, const long2& a3, const long2& a4, const long3& a5, const long3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a5.s[2];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  long16(const long2& a0, const long2& a1, const long2& a2, const long2& a3, const long2& a4, const long4& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a5.s[2];
    s[13] = a5.s[3];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long2& a0, const long2& a1, const long2& a2, const long2& a3, const long3& a4, const long2& a5, const long3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  long16(const long2& a0, const long2& a1, const long2& a2, const long2& a3, const long3& a4, const long3& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long2& a0, const long2& a1, const long2& a2, const long2& a3, const long4& a4, const long2& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long2& a0, const long2& a1, const long2& a2, const long2& a3, const long4& a4, const long4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  long16(const long2& a0, const long2& a1, const long2& a2, const long2& a3, const long8& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a4.s[4];
    s[13] = a4.s[5];
    s[14] = a4.s[6];
    s[15] = a4.s[7];
  }

  long16(const long2& a0, const long2& a1, const long2& a2, const long3& a3, const long2& a4, const long2& a5, const long3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  long16(const long2& a0, const long2& a1, const long2& a2, const long3& a3, const long2& a4, const long3& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long2& a0, const long2& a1, const long2& a2, const long3& a3, const long3& a4, const long2& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long2& a0, const long2& a1, const long2& a2, const long3& a3, const long3& a4, const long4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  long16(const long2& a0, const long2& a1, const long2& a2, const long3& a3, const long4& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long2& a0, const long2& a1, const long2& a2, const long4& a3, const long2& a4, const long2& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long2& a0, const long2& a1, const long2& a2, const long4& a3, const long2& a4, const long4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  long16(const long2& a0, const long2& a1, const long2& a2, const long4& a3, const long3& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long2& a0, const long2& a1, const long2& a2, const long4& a3, const long4& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long2& a0, const long2& a1, const long2& a2, const long8& a3, const long2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a3.s[4];
    s[11] = a3.s[5];
    s[12] = a3.s[6];
    s[13] = a3.s[7];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  long16(const long2& a0, const long2& a1, const long3& a2, const long2& a3, const long2& a4, const long2& a5, const long3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  long16(const long2& a0, const long2& a1, const long3& a2, const long2& a3, const long2& a4, const long3& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long2& a0, const long2& a1, const long3& a2, const long2& a3, const long3& a4, const long2& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long2& a0, const long2& a1, const long3& a2, const long2& a3, const long3& a4, const long4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  long16(const long2& a0, const long2& a1, const long3& a2, const long2& a3, const long4& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long2& a0, const long2& a1, const long3& a2, const long3& a3, const long2& a4, const long2& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long2& a0, const long2& a1, const long3& a2, const long3& a3, const long2& a4, const long4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  long16(const long2& a0, const long2& a1, const long3& a2, const long3& a3, const long3& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long2& a0, const long2& a1, const long3& a2, const long3& a3, const long4& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long2& a0, const long2& a1, const long3& a2, const long4& a3, const long2& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long2& a0, const long2& a1, const long3& a2, const long4& a3, const long3& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long2& a0, const long2& a1, const long4& a2, const long2& a3, const long2& a4, const long2& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long2& a0, const long2& a1, const long4& a2, const long2& a3, const long2& a4, const long4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  long16(const long2& a0, const long2& a1, const long4& a2, const long2& a3, const long3& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long2& a0, const long2& a1, const long4& a2, const long2& a3, const long4& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long2& a0, const long2& a1, const long4& a2, const long3& a3, const long2& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long2& a0, const long2& a1, const long4& a2, const long3& a3, const long3& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long2& a0, const long2& a1, const long4& a2, const long4& a3, const long2& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long2& a0, const long2& a1, const long4& a2, const long4& a3, const long4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  long16(const long2& a0, const long2& a1, const long4& a2, const long8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  long16(const long2& a0, const long2& a1, const long8& a2, const long2& a3, const long2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a2.s[4];
    s[9] = a2.s[5];
    s[10] = a2.s[6];
    s[11] = a2.s[7];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  long16(const long2& a0, const long2& a1, const long8& a2, const long4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a2.s[4];
    s[9] = a2.s[5];
    s[10] = a2.s[6];
    s[11] = a2.s[7];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  long16(const long2& a0, const long3& a1, const long2& a2, const long2& a3, const long2& a4, const long2& a5, const long3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  long16(const long2& a0, const long3& a1, const long2& a2, const long2& a3, const long2& a4, const long3& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long2& a0, const long3& a1, const long2& a2, const long2& a3, const long3& a4, const long2& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long2& a0, const long3& a1, const long2& a2, const long2& a3, const long3& a4, const long4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  long16(const long2& a0, const long3& a1, const long2& a2, const long2& a3, const long4& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long2& a0, const long3& a1, const long2& a2, const long3& a3, const long2& a4, const long2& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long2& a0, const long3& a1, const long2& a2, const long3& a3, const long2& a4, const long4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  long16(const long2& a0, const long3& a1, const long2& a2, const long3& a3, const long3& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long2& a0, const long3& a1, const long2& a2, const long3& a3, const long4& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long2& a0, const long3& a1, const long2& a2, const long4& a3, const long2& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long2& a0, const long3& a1, const long2& a2, const long4& a3, const long3& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long2& a0, const long3& a1, const long3& a2, const long2& a3, const long2& a4, const long2& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long2& a0, const long3& a1, const long3& a2, const long2& a3, const long2& a4, const long4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  long16(const long2& a0, const long3& a1, const long3& a2, const long2& a3, const long3& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long2& a0, const long3& a1, const long3& a2, const long2& a3, const long4& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long2& a0, const long3& a1, const long3& a2, const long3& a3, const long2& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long2& a0, const long3& a1, const long3& a2, const long3& a3, const long3& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long2& a0, const long3& a1, const long3& a2, const long4& a3, const long2& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long2& a0, const long3& a1, const long3& a2, const long4& a3, const long4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  long16(const long2& a0, const long3& a1, const long3& a2, const long8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  long16(const long2& a0, const long3& a1, const long4& a2, const long2& a3, const long2& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long2& a0, const long3& a1, const long4& a2, const long2& a3, const long3& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long2& a0, const long3& a1, const long4& a2, const long3& a3, const long2& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long2& a0, const long3& a1, const long4& a2, const long3& a3, const long4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  long16(const long2& a0, const long3& a1, const long4& a2, const long4& a3, const long3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  long16(const long2& a0, const long3& a1, const long8& a2, const long3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a2.s[4];
    s[10] = a2.s[5];
    s[11] = a2.s[6];
    s[12] = a2.s[7];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  long16(const long2& a0, const long4& a1, const long2& a2, const long2& a3, const long2& a4, const long2& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long2& a0, const long4& a1, const long2& a2, const long2& a3, const long2& a4, const long4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  long16(const long2& a0, const long4& a1, const long2& a2, const long2& a3, const long3& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long2& a0, const long4& a1, const long2& a2, const long2& a3, const long4& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long2& a0, const long4& a1, const long2& a2, const long3& a3, const long2& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long2& a0, const long4& a1, const long2& a2, const long3& a3, const long3& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long2& a0, const long4& a1, const long2& a2, const long4& a3, const long2& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long2& a0, const long4& a1, const long2& a2, const long4& a3, const long4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  long16(const long2& a0, const long4& a1, const long2& a2, const long8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  long16(const long2& a0, const long4& a1, const long3& a2, const long2& a3, const long2& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long2& a0, const long4& a1, const long3& a2, const long2& a3, const long3& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long2& a0, const long4& a1, const long3& a2, const long3& a3, const long2& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long2& a0, const long4& a1, const long3& a2, const long3& a3, const long4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  long16(const long2& a0, const long4& a1, const long3& a2, const long4& a3, const long3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  long16(const long2& a0, const long4& a1, const long4& a2, const long2& a3, const long2& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long2& a0, const long4& a1, const long4& a2, const long2& a3, const long4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  long16(const long2& a0, const long4& a1, const long4& a2, const long3& a3, const long3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  long16(const long2& a0, const long4& a1, const long4& a2, const long4& a3, const long2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  long16(const long2& a0, const long4& a1, const long8& a2, const long2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  long16(const long2& a0, const long8& a1, const long2& a2, const long2& a3, const long2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  long16(const long2& a0, const long8& a1, const long2& a2, const long4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  long16(const long2& a0, const long8& a1, const long3& a2, const long3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  long16(const long2& a0, const long8& a1, const long4& a2, const long2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a2.s[3];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  long16(const long3& a0, const long2& a1, const long2& a2, const long2& a3, const long2& a4, const long2& a5, const long3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  long16(const long3& a0, const long2& a1, const long2& a2, const long2& a3, const long2& a4, const long3& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long3& a0, const long2& a1, const long2& a2, const long2& a3, const long3& a4, const long2& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long3& a0, const long2& a1, const long2& a2, const long2& a3, const long3& a4, const long4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  long16(const long3& a0, const long2& a1, const long2& a2, const long2& a3, const long4& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long3& a0, const long2& a1, const long2& a2, const long3& a3, const long2& a4, const long2& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long3& a0, const long2& a1, const long2& a2, const long3& a3, const long2& a4, const long4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  long16(const long3& a0, const long2& a1, const long2& a2, const long3& a3, const long3& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long3& a0, const long2& a1, const long2& a2, const long3& a3, const long4& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long3& a0, const long2& a1, const long2& a2, const long4& a3, const long2& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long3& a0, const long2& a1, const long2& a2, const long4& a3, const long3& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long3& a0, const long2& a1, const long3& a2, const long2& a3, const long2& a4, const long2& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long3& a0, const long2& a1, const long3& a2, const long2& a3, const long2& a4, const long4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  long16(const long3& a0, const long2& a1, const long3& a2, const long2& a3, const long3& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long3& a0, const long2& a1, const long3& a2, const long2& a3, const long4& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long3& a0, const long2& a1, const long3& a2, const long3& a3, const long2& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long3& a0, const long2& a1, const long3& a2, const long3& a3, const long3& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long3& a0, const long2& a1, const long3& a2, const long4& a3, const long2& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long3& a0, const long2& a1, const long3& a2, const long4& a3, const long4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  long16(const long3& a0, const long2& a1, const long3& a2, const long8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  long16(const long3& a0, const long2& a1, const long4& a2, const long2& a3, const long2& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long3& a0, const long2& a1, const long4& a2, const long2& a3, const long3& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long3& a0, const long2& a1, const long4& a2, const long3& a3, const long2& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long3& a0, const long2& a1, const long4& a2, const long3& a3, const long4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  long16(const long3& a0, const long2& a1, const long4& a2, const long4& a3, const long3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  long16(const long3& a0, const long2& a1, const long8& a2, const long3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a2.s[4];
    s[10] = a2.s[5];
    s[11] = a2.s[6];
    s[12] = a2.s[7];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  long16(const long3& a0, const long3& a1, const long2& a2, const long2& a3, const long2& a4, const long2& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long3& a0, const long3& a1, const long2& a2, const long2& a3, const long2& a4, const long4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  long16(const long3& a0, const long3& a1, const long2& a2, const long2& a3, const long3& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long3& a0, const long3& a1, const long2& a2, const long2& a3, const long4& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long3& a0, const long3& a1, const long2& a2, const long3& a3, const long2& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long3& a0, const long3& a1, const long2& a2, const long3& a3, const long3& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long3& a0, const long3& a1, const long2& a2, const long4& a3, const long2& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long3& a0, const long3& a1, const long2& a2, const long4& a3, const long4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  long16(const long3& a0, const long3& a1, const long2& a2, const long8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  long16(const long3& a0, const long3& a1, const long3& a2, const long2& a3, const long2& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long3& a0, const long3& a1, const long3& a2, const long2& a3, const long3& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long3& a0, const long3& a1, const long3& a2, const long3& a3, const long2& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long3& a0, const long3& a1, const long3& a2, const long3& a3, const long4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  long16(const long3& a0, const long3& a1, const long3& a2, const long4& a3, const long3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  long16(const long3& a0, const long3& a1, const long4& a2, const long2& a3, const long2& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long3& a0, const long3& a1, const long4& a2, const long2& a3, const long4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  long16(const long3& a0, const long3& a1, const long4& a2, const long3& a3, const long3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  long16(const long3& a0, const long3& a1, const long4& a2, const long4& a3, const long2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  long16(const long3& a0, const long3& a1, const long8& a2, const long2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  long16(const long3& a0, const long4& a1, const long2& a2, const long2& a3, const long2& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long3& a0, const long4& a1, const long2& a2, const long2& a3, const long3& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long3& a0, const long4& a1, const long2& a2, const long3& a3, const long2& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long3& a0, const long4& a1, const long2& a2, const long3& a3, const long4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  long16(const long3& a0, const long4& a1, const long2& a2, const long4& a3, const long3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  long16(const long3& a0, const long4& a1, const long3& a2, const long2& a3, const long2& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long3& a0, const long4& a1, const long3& a2, const long2& a3, const long4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  long16(const long3& a0, const long4& a1, const long3& a2, const long3& a3, const long3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  long16(const long3& a0, const long4& a1, const long3& a2, const long4& a3, const long2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  long16(const long3& a0, const long4& a1, const long4& a2, const long2& a3, const long3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  long16(const long3& a0, const long4& a1, const long4& a2, const long3& a3, const long2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  long16(const long3& a0, const long8& a1, const long2& a2, const long3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a1.s[4];
    s[8] = a1.s[5];
    s[9] = a1.s[6];
    s[10] = a1.s[7];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  long16(const long3& a0, const long8& a1, const long3& a2, const long2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a1.s[3];
    s[7] = a1.s[4];
    s[8] = a1.s[5];
    s[9] = a1.s[6];
    s[10] = a1.s[7];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a2.s[2];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  long16(const long4& a0, const long2& a1, const long2& a2, const long2& a3, const long2& a4, const long2& a5, const long2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  long16(const long4& a0, const long2& a1, const long2& a2, const long2& a3, const long2& a4, const long4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  long16(const long4& a0, const long2& a1, const long2& a2, const long2& a3, const long3& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long4& a0, const long2& a1, const long2& a2, const long2& a3, const long4& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long4& a0, const long2& a1, const long2& a2, const long3& a3, const long2& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long4& a0, const long2& a1, const long2& a2, const long3& a3, const long3& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long4& a0, const long2& a1, const long2& a2, const long4& a3, const long2& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long4& a0, const long2& a1, const long2& a2, const long4& a3, const long4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  long16(const long4& a0, const long2& a1, const long2& a2, const long8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  long16(const long4& a0, const long2& a1, const long3& a2, const long2& a3, const long2& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long4& a0, const long2& a1, const long3& a2, const long2& a3, const long3& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long4& a0, const long2& a1, const long3& a2, const long3& a3, const long2& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long4& a0, const long2& a1, const long3& a2, const long3& a3, const long4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  long16(const long4& a0, const long2& a1, const long3& a2, const long4& a3, const long3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  long16(const long4& a0, const long2& a1, const long4& a2, const long2& a3, const long2& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long4& a0, const long2& a1, const long4& a2, const long2& a3, const long4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  long16(const long4& a0, const long2& a1, const long4& a2, const long3& a3, const long3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  long16(const long4& a0, const long2& a1, const long4& a2, const long4& a3, const long2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  long16(const long4& a0, const long2& a1, const long8& a2, const long2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  long16(const long4& a0, const long3& a1, const long2& a2, const long2& a3, const long2& a4, const long3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  long16(const long4& a0, const long3& a1, const long2& a2, const long2& a3, const long3& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long4& a0, const long3& a1, const long2& a2, const long3& a3, const long2& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long4& a0, const long3& a1, const long2& a2, const long3& a3, const long4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  long16(const long4& a0, const long3& a1, const long2& a2, const long4& a3, const long3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  long16(const long4& a0, const long3& a1, const long3& a2, const long2& a3, const long2& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long4& a0, const long3& a1, const long3& a2, const long2& a3, const long4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  long16(const long4& a0, const long3& a1, const long3& a2, const long3& a3, const long3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  long16(const long4& a0, const long3& a1, const long3& a2, const long4& a3, const long2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  long16(const long4& a0, const long3& a1, const long4& a2, const long2& a3, const long3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  long16(const long4& a0, const long3& a1, const long4& a2, const long3& a3, const long2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a2.s[0];
    s[8] = a2.s[1];
    s[9] = a2.s[2];
    s[10] = a2.s[3];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  long16(const long4& a0, const long4& a1, const long2& a2, const long2& a3, const long2& a4, const long2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  long16(const long4& a0, const long4& a1, const long2& a2, const long2& a3, const long4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  long16(const long4& a0, const long4& a1, const long2& a2, const long3& a3, const long3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  long16(const long4& a0, const long4& a1, const long2& a2, const long4& a3, const long2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  long16(const long4& a0, const long4& a1, const long3& a2, const long2& a3, const long3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  long16(const long4& a0, const long4& a1, const long3& a2, const long3& a3, const long2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a3.s[0];
    s[12] = a3.s[1];
    s[13] = a3.s[2];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  long16(const long4& a0, const long4& a1, const long4& a2, const long2& a3, const long2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  long16(const long4& a0, const long4& a1, const long4& a2, const long4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  long16(const long4& a0, const long4& a1, const long8& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a2.s[0];
    s[9] = a2.s[1];
    s[10] = a2.s[2];
    s[11] = a2.s[3];
    s[12] = a2.s[4];
    s[13] = a2.s[5];
    s[14] = a2.s[6];
    s[15] = a2.s[7];
  }

  long16(const long4& a0, const long8& a1, const long2& a2, const long2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a1.s[4];
    s[9] = a1.s[5];
    s[10] = a1.s[6];
    s[11] = a1.s[7];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  long16(const long4& a0, const long8& a1, const long4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
    s[8] = a1.s[4];
    s[9] = a1.s[5];
    s[10] = a1.s[6];
    s[11] = a1.s[7];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a2.s[2];
    s[15] = a2.s[3];
  }

  long16(const long8& a0, const long2& a1, const long2& a2, const long2& a3, const long2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  long16(const long8& a0, const long2& a1, const long2& a2, const long4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  long16(const long8& a0, const long2& a1, const long3& a2, const long3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  long16(const long8& a0, const long2& a1, const long4& a2, const long2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a2.s[3];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  long16(const long8& a0, const long3& a1, const long2& a2, const long3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  long16(const long8& a0, const long3& a1, const long3& a2, const long2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a2.s[0];
    s[12] = a2.s[1];
    s[13] = a2.s[2];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  long16(const long8& a0, const long4& a1, const long2& a2, const long2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  long16(const long8& a0, const long4& a1, const long4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a2.s[0];
    s[13] = a2.s[1];
    s[14] = a2.s[2];
    s[15] = a2.s[3];
  }

  long16(const long8& a0, const long8& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a1.s[0];
    s[9] = a1.s[1];
    s[10] = a1.s[2];
    s[11] = a1.s[3];
    s[12] = a1.s[4];
    s[13] = a1.s[5];
    s[14] = a1.s[6];
    s[15] = a1.s[7];
  }

  long16(const long16& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
    s[8] = a0.s[8];
    s[9] = a0.s[9];
    s[10] = a0.s[10];
    s[11] = a0.s[11];
    s[12] = a0.s[12];
    s[13] = a0.s[13];
    s[14] = a0.s[14];
    s[15] = a0.s[15];
  }

  long16& operator+=(const long16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  long16& operator+=(cl_long rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  long16& operator-=(const long16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  long16& operator-=(cl_long rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  long16& operator*=(const long16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  long16& operator*=(cl_long rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  long16& operator/=(const long16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  long16& operator/=(cl_long rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  long16& operator%=(const long16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  long16& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  long16& operator&=(const long16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  long16& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  long16& operator|=(const long16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  long16& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  long16& operator^=(const long16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  long16& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  long16& operator<<=(const long16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  long16& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  long16& operator>>=(const long16& rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  long16& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 16; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline long16 operator+(long16 lhs, const long16& rhs)
{
  lhs += rhs;
  return lhs;
}

inline long16 operator+(long16 lhs, cl_long rhs)
{
  lhs += rhs;
  return lhs;
}

inline long16 operator+(cl_long lhs, const long16& rhs)
{
  auto tmp = long16(lhs);
  return tmp + rhs;
}

inline long16 operator-(long16 lhs, const long16& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline long16 operator-(long16 lhs, cl_long rhs)
{
  lhs -= rhs;
  return lhs;
}

inline long16 operator-(cl_long lhs, const long16& rhs)
{
  auto tmp = long16(lhs);
  return tmp - rhs;
}

inline long16 operator*(long16 lhs, const long16& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline long16 operator*(long16 lhs, cl_long rhs)
{
  lhs *= rhs;
  return lhs;
}

inline long16 operator*(cl_long lhs, const long16& rhs)
{
  auto tmp = long16(lhs);
  return tmp * rhs;
}

inline long16 operator/(long16 lhs, const long16& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline long16 operator/(long16 lhs, cl_long rhs)
{
  lhs /= rhs;
  return lhs;
}

inline long16 operator/(cl_long lhs, const long16& rhs)
{
  auto tmp = long16(lhs);
  return tmp / rhs;
}

inline long16 operator%(long16 lhs, const long16& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline long16 operator%(long16 lhs, cl_long rhs)
{
  lhs %= rhs;
  return lhs;
}

inline long16 operator%(cl_long lhs, const long16& rhs)
{
  auto tmp = long16(lhs);
  return tmp % rhs;
}

inline long16 operator&(long16 lhs, const long16& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline long16 operator&(long16 lhs, cl_long rhs)
{
  lhs &= rhs;
  return lhs;
}

inline long16 operator&(cl_long lhs, const long16& rhs)
{
  auto tmp = long16(lhs);
  return tmp & rhs;
}

inline long16 operator|(long16 lhs, const long16& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline long16 operator|(long16 lhs, cl_long rhs)
{
  lhs |= rhs;
  return lhs;
}

inline long16 operator|(cl_long lhs, const long16& rhs)
{
  auto tmp = long16(lhs);
  return tmp | rhs;
}

inline long16 operator^(long16 lhs, const long16& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline long16 operator^(long16 lhs, cl_long rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline long16 operator^(cl_long lhs, const long16& rhs)
{
  auto tmp = long16(lhs);
  return tmp ^ rhs;
}

inline long16 operator<<(long16 lhs, const long16& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline long16 operator<<(long16 lhs, cl_long rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline long16 operator<<(cl_long lhs, const long16& rhs)
{
  auto tmp = long16(lhs);
  return tmp << rhs;
}

inline long16 operator>>(long16 lhs, const long16& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline long16 operator>>(long16 lhs, cl_long rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline long16 operator>>(cl_long lhs, const long16& rhs)
{
  auto tmp = long16(lhs);
  return tmp >> rhs;
}

inline bool operator==(const long16& lhs, const long16& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]) &&
  (lhs.s[4] == rhs.s[4]) &&
  (lhs.s[5] == rhs.s[5]) &&
  (lhs.s[6] == rhs.s[6]) &&
  (lhs.s[7] == rhs.s[7]) &&
  (lhs.s[8] == rhs.s[8]) &&
  (lhs.s[9] == rhs.s[9]) &&
  (lhs.s[10] == rhs.s[10]) &&
  (lhs.s[11] == rhs.s[11]) &&
  (lhs.s[12] == rhs.s[12]) &&
  (lhs.s[13] == rhs.s[13]) &&
  (lhs.s[14] == rhs.s[14]) &&
  (lhs.s[15] == rhs.s[15]);
}

inline bool operator==(const long16& lhs, const cl_long& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs) &&
  (lhs.s[4] == rhs) &&
  (lhs.s[5] == rhs) &&
  (lhs.s[6] == rhs) &&
  (lhs.s[7] == rhs) &&
  (lhs.s[8] == rhs) &&
  (lhs.s[9] == rhs) &&
  (lhs.s[10] == rhs) &&
  (lhs.s[11] == rhs) &&
  (lhs.s[12] == rhs) &&
  (lhs.s[13] == rhs) &&
  (lhs.s[14] == rhs) &&
  (lhs.s[15] == rhs);
}

inline bool operator<(const long16& lhs, const long16& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const long16& lhs, const cl_long& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const long16& lhs, const long16& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const long16& lhs, const cl_long& rhs)
{
  for (size_t i = 0; i < 16; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const long16& lhs, const long16& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const long16& lhs, const cl_long& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const long16& lhs, const long16& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const long16& lhs, const cl_long& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const long16& lhs, const long16& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const long16& lhs, const cl_long& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline long16 vload16(size_t offset, const long16* p)
{
  return *(p + offset);
}

inline void vstore16(long16 data, size_t offset, long16* p)
{
  *(p + offset) = data;
}


class ulong2{
public:
  static const size_t N = 2; // Number of elements in vector
  union {
	 cl_ulong CL_ALIGNED(16) s[2];
#if defined(__GNUC__)
    __extension__ struct { cl_ulong x, y; };
    __extension__ struct { cl_ulong s0, s1; };
#endif
  };

  ulong2(){}

  ulong2(cl_ulong i0,cl_ulong i1){
    s[0] = i0;
    s[1] = i1;

  }

  ulong2(cl_ulong i0) {
  for (int i = 0; i < 2; ++i) {
		s[i] = i0;
	 }
  }

  ulong2(const ulong2& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
  }

  ulong2& operator+=(const ulong2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  ulong2& operator+=(cl_ulong rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  ulong2& operator-=(const ulong2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  ulong2& operator-=(cl_ulong rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  ulong2& operator*=(const ulong2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  ulong2& operator*=(cl_ulong rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  ulong2& operator/=(const ulong2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  ulong2& operator/=(cl_ulong rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  ulong2& operator%=(const ulong2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  ulong2& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  ulong2& operator&=(const ulong2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  ulong2& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  ulong2& operator|=(const ulong2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  ulong2& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  ulong2& operator^=(const ulong2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  ulong2& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  ulong2& operator<<=(const ulong2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  ulong2& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  ulong2& operator>>=(const ulong2& rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  ulong2& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 2; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline ulong2 operator+(ulong2 lhs, const ulong2& rhs)
{
  lhs += rhs;
  return lhs;
}

inline ulong2 operator+(ulong2 lhs, cl_ulong rhs)
{
  lhs += rhs;
  return lhs;
}

inline ulong2 operator+(cl_ulong lhs, const ulong2& rhs)
{
  auto tmp = ulong2(lhs);
  return tmp + rhs;
}

inline ulong2 operator-(ulong2 lhs, const ulong2& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline ulong2 operator-(ulong2 lhs, cl_ulong rhs)
{
  lhs -= rhs;
  return lhs;
}

inline ulong2 operator-(cl_ulong lhs, const ulong2& rhs)
{
  auto tmp = ulong2(lhs);
  return tmp - rhs;
}

inline ulong2 operator*(ulong2 lhs, const ulong2& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline ulong2 operator*(ulong2 lhs, cl_ulong rhs)
{
  lhs *= rhs;
  return lhs;
}

inline ulong2 operator*(cl_ulong lhs, const ulong2& rhs)
{
  auto tmp = ulong2(lhs);
  return tmp * rhs;
}

inline ulong2 operator/(ulong2 lhs, const ulong2& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline ulong2 operator/(ulong2 lhs, cl_ulong rhs)
{
  lhs /= rhs;
  return lhs;
}

inline ulong2 operator/(cl_ulong lhs, const ulong2& rhs)
{
  auto tmp = ulong2(lhs);
  return tmp / rhs;
}

inline ulong2 operator%(ulong2 lhs, const ulong2& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline ulong2 operator%(ulong2 lhs, cl_ulong rhs)
{
  lhs %= rhs;
  return lhs;
}

inline ulong2 operator%(cl_ulong lhs, const ulong2& rhs)
{
  auto tmp = ulong2(lhs);
  return tmp % rhs;
}

inline ulong2 operator&(ulong2 lhs, const ulong2& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline ulong2 operator&(ulong2 lhs, cl_ulong rhs)
{
  lhs &= rhs;
  return lhs;
}

inline ulong2 operator&(cl_ulong lhs, const ulong2& rhs)
{
  auto tmp = ulong2(lhs);
  return tmp & rhs;
}

inline ulong2 operator|(ulong2 lhs, const ulong2& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline ulong2 operator|(ulong2 lhs, cl_ulong rhs)
{
  lhs |= rhs;
  return lhs;
}

inline ulong2 operator|(cl_ulong lhs, const ulong2& rhs)
{
  auto tmp = ulong2(lhs);
  return tmp | rhs;
}

inline ulong2 operator^(ulong2 lhs, const ulong2& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline ulong2 operator^(ulong2 lhs, cl_ulong rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline ulong2 operator^(cl_ulong lhs, const ulong2& rhs)
{
  auto tmp = ulong2(lhs);
  return tmp ^ rhs;
}

inline ulong2 operator<<(ulong2 lhs, const ulong2& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline ulong2 operator<<(ulong2 lhs, cl_ulong rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline ulong2 operator<<(cl_ulong lhs, const ulong2& rhs)
{
  auto tmp = ulong2(lhs);
  return tmp << rhs;
}

inline ulong2 operator>>(ulong2 lhs, const ulong2& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline ulong2 operator>>(ulong2 lhs, cl_ulong rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline ulong2 operator>>(cl_ulong lhs, const ulong2& rhs)
{
  auto tmp = ulong2(lhs);
  return tmp >> rhs;
}

inline bool operator==(const ulong2& lhs, const ulong2& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]);
}

inline bool operator==(const ulong2& lhs, const cl_ulong& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs);
}

inline bool operator<(const ulong2& lhs, const ulong2& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const ulong2& lhs, const cl_ulong& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const ulong2& lhs, const ulong2& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const ulong2& lhs, const cl_ulong& rhs)
{
  for (size_t i = 0; i < 2; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const ulong2& lhs, const ulong2& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const ulong2& lhs, const cl_ulong& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const ulong2& lhs, const ulong2& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const ulong2& lhs, const cl_ulong& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const ulong2& lhs, const ulong2& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const ulong2& lhs, const cl_ulong& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline ulong2 vload2(size_t offset, const ulong2* p)
{
  return *(p + offset);
}

inline void vstore2(ulong2 data, size_t offset, ulong2* p)
{
  *(p + offset) = data;
}


class ulong3{
public:
  static const size_t N = 4; // Number of elements in vector
  union {
	 cl_ulong CL_ALIGNED(32) s[4];
#if defined(__GNUC__)
    __extension__ struct { cl_ulong x, y, z, w; };
    __extension__ struct { cl_ulong s0, s1, s2, s3; };
#endif
  };

  ulong3(){}

  ulong3(cl_ulong i0,cl_ulong i1,cl_ulong i2){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;

  }

  ulong3(cl_ulong i0) {
  for (int i = 0; i < 3; ++i) {
		s[i] = i0;
	 }
  }

  ulong3(const ulong3& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
  }

  ulong3& operator+=(const ulong3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  ulong3& operator+=(cl_ulong rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  ulong3& operator-=(const ulong3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  ulong3& operator-=(cl_ulong rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  ulong3& operator*=(const ulong3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  ulong3& operator*=(cl_ulong rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  ulong3& operator/=(const ulong3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  ulong3& operator/=(cl_ulong rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  ulong3& operator%=(const ulong3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  ulong3& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  ulong3& operator&=(const ulong3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  ulong3& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  ulong3& operator|=(const ulong3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  ulong3& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  ulong3& operator^=(const ulong3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  ulong3& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  ulong3& operator<<=(const ulong3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  ulong3& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  ulong3& operator>>=(const ulong3& rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  ulong3& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 3; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline ulong3 operator+(ulong3 lhs, const ulong3& rhs)
{
  lhs += rhs;
  return lhs;
}

inline ulong3 operator+(ulong3 lhs, cl_ulong rhs)
{
  lhs += rhs;
  return lhs;
}

inline ulong3 operator+(cl_ulong lhs, const ulong3& rhs)
{
  auto tmp = ulong3(lhs);
  return tmp + rhs;
}

inline ulong3 operator-(ulong3 lhs, const ulong3& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline ulong3 operator-(ulong3 lhs, cl_ulong rhs)
{
  lhs -= rhs;
  return lhs;
}

inline ulong3 operator-(cl_ulong lhs, const ulong3& rhs)
{
  auto tmp = ulong3(lhs);
  return tmp - rhs;
}

inline ulong3 operator*(ulong3 lhs, const ulong3& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline ulong3 operator*(ulong3 lhs, cl_ulong rhs)
{
  lhs *= rhs;
  return lhs;
}

inline ulong3 operator*(cl_ulong lhs, const ulong3& rhs)
{
  auto tmp = ulong3(lhs);
  return tmp * rhs;
}

inline ulong3 operator/(ulong3 lhs, const ulong3& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline ulong3 operator/(ulong3 lhs, cl_ulong rhs)
{
  lhs /= rhs;
  return lhs;
}

inline ulong3 operator/(cl_ulong lhs, const ulong3& rhs)
{
  auto tmp = ulong3(lhs);
  return tmp / rhs;
}

inline ulong3 operator%(ulong3 lhs, const ulong3& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline ulong3 operator%(ulong3 lhs, cl_ulong rhs)
{
  lhs %= rhs;
  return lhs;
}

inline ulong3 operator%(cl_ulong lhs, const ulong3& rhs)
{
  auto tmp = ulong3(lhs);
  return tmp % rhs;
}

inline ulong3 operator&(ulong3 lhs, const ulong3& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline ulong3 operator&(ulong3 lhs, cl_ulong rhs)
{
  lhs &= rhs;
  return lhs;
}

inline ulong3 operator&(cl_ulong lhs, const ulong3& rhs)
{
  auto tmp = ulong3(lhs);
  return tmp & rhs;
}

inline ulong3 operator|(ulong3 lhs, const ulong3& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline ulong3 operator|(ulong3 lhs, cl_ulong rhs)
{
  lhs |= rhs;
  return lhs;
}

inline ulong3 operator|(cl_ulong lhs, const ulong3& rhs)
{
  auto tmp = ulong3(lhs);
  return tmp | rhs;
}

inline ulong3 operator^(ulong3 lhs, const ulong3& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline ulong3 operator^(ulong3 lhs, cl_ulong rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline ulong3 operator^(cl_ulong lhs, const ulong3& rhs)
{
  auto tmp = ulong3(lhs);
  return tmp ^ rhs;
}

inline ulong3 operator<<(ulong3 lhs, const ulong3& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline ulong3 operator<<(ulong3 lhs, cl_ulong rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline ulong3 operator<<(cl_ulong lhs, const ulong3& rhs)
{
  auto tmp = ulong3(lhs);
  return tmp << rhs;
}

inline ulong3 operator>>(ulong3 lhs, const ulong3& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline ulong3 operator>>(ulong3 lhs, cl_ulong rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline ulong3 operator>>(cl_ulong lhs, const ulong3& rhs)
{
  auto tmp = ulong3(lhs);
  return tmp >> rhs;
}

inline bool operator==(const ulong3& lhs, const ulong3& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]);
}

inline bool operator==(const ulong3& lhs, const cl_ulong& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs);
}

inline bool operator<(const ulong3& lhs, const ulong3& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const ulong3& lhs, const cl_ulong& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const ulong3& lhs, const ulong3& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const ulong3& lhs, const cl_ulong& rhs)
{
  for (size_t i = 0; i < 3; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const ulong3& lhs, const ulong3& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const ulong3& lhs, const cl_ulong& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const ulong3& lhs, const ulong3& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const ulong3& lhs, const cl_ulong& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const ulong3& lhs, const ulong3& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const ulong3& lhs, const cl_ulong& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline ulong3 vload3(size_t offset, const ulong3* p)
{
  return *(p + offset);
}

inline void vstore3(ulong3 data, size_t offset, ulong3* p)
{
  *(p + offset) = data;
}


class ulong4{
public:
  static const size_t N = 4; // Number of elements in vector
  union {
	 cl_ulong CL_ALIGNED(32) s[4];
#if defined(__GNUC__)
    __extension__ struct { cl_ulong x, y, z, w; };
    __extension__ struct { cl_ulong s0, s1, s2, s3; };
#endif
  };

  ulong4(){}

  ulong4(cl_ulong i0,cl_ulong i1,cl_ulong i2,cl_ulong i3
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;

  }

  ulong4(cl_ulong i0) {
  for (int i = 0; i < 4; ++i) {
		s[i] = i0;
	 }
  }

  ulong4(const ulong2& a0, const ulong2& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
  }

  ulong4(const ulong4& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
  }

  ulong4& operator+=(const ulong4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  ulong4& operator+=(cl_ulong rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  ulong4& operator-=(const ulong4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  ulong4& operator-=(cl_ulong rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  ulong4& operator*=(const ulong4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  ulong4& operator*=(cl_ulong rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  ulong4& operator/=(const ulong4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  ulong4& operator/=(cl_ulong rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  ulong4& operator%=(const ulong4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  ulong4& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  ulong4& operator&=(const ulong4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  ulong4& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  ulong4& operator|=(const ulong4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  ulong4& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  ulong4& operator^=(const ulong4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  ulong4& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  ulong4& operator<<=(const ulong4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  ulong4& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  ulong4& operator>>=(const ulong4& rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  ulong4& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 4; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline ulong4 operator+(ulong4 lhs, const ulong4& rhs)
{
  lhs += rhs;
  return lhs;
}

inline ulong4 operator+(ulong4 lhs, cl_ulong rhs)
{
  lhs += rhs;
  return lhs;
}

inline ulong4 operator+(cl_ulong lhs, const ulong4& rhs)
{
  auto tmp = ulong4(lhs);
  return tmp + rhs;
}

inline ulong4 operator-(ulong4 lhs, const ulong4& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline ulong4 operator-(ulong4 lhs, cl_ulong rhs)
{
  lhs -= rhs;
  return lhs;
}

inline ulong4 operator-(cl_ulong lhs, const ulong4& rhs)
{
  auto tmp = ulong4(lhs);
  return tmp - rhs;
}

inline ulong4 operator*(ulong4 lhs, const ulong4& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline ulong4 operator*(ulong4 lhs, cl_ulong rhs)
{
  lhs *= rhs;
  return lhs;
}

inline ulong4 operator*(cl_ulong lhs, const ulong4& rhs)
{
  auto tmp = ulong4(lhs);
  return tmp * rhs;
}

inline ulong4 operator/(ulong4 lhs, const ulong4& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline ulong4 operator/(ulong4 lhs, cl_ulong rhs)
{
  lhs /= rhs;
  return lhs;
}

inline ulong4 operator/(cl_ulong lhs, const ulong4& rhs)
{
  auto tmp = ulong4(lhs);
  return tmp / rhs;
}

inline ulong4 operator%(ulong4 lhs, const ulong4& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline ulong4 operator%(ulong4 lhs, cl_ulong rhs)
{
  lhs %= rhs;
  return lhs;
}

inline ulong4 operator%(cl_ulong lhs, const ulong4& rhs)
{
  auto tmp = ulong4(lhs);
  return tmp % rhs;
}

inline ulong4 operator&(ulong4 lhs, const ulong4& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline ulong4 operator&(ulong4 lhs, cl_ulong rhs)
{
  lhs &= rhs;
  return lhs;
}

inline ulong4 operator&(cl_ulong lhs, const ulong4& rhs)
{
  auto tmp = ulong4(lhs);
  return tmp & rhs;
}

inline ulong4 operator|(ulong4 lhs, const ulong4& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline ulong4 operator|(ulong4 lhs, cl_ulong rhs)
{
  lhs |= rhs;
  return lhs;
}

inline ulong4 operator|(cl_ulong lhs, const ulong4& rhs)
{
  auto tmp = ulong4(lhs);
  return tmp | rhs;
}

inline ulong4 operator^(ulong4 lhs, const ulong4& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline ulong4 operator^(ulong4 lhs, cl_ulong rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline ulong4 operator^(cl_ulong lhs, const ulong4& rhs)
{
  auto tmp = ulong4(lhs);
  return tmp ^ rhs;
}

inline ulong4 operator<<(ulong4 lhs, const ulong4& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline ulong4 operator<<(ulong4 lhs, cl_ulong rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline ulong4 operator<<(cl_ulong lhs, const ulong4& rhs)
{
  auto tmp = ulong4(lhs);
  return tmp << rhs;
}

inline ulong4 operator>>(ulong4 lhs, const ulong4& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline ulong4 operator>>(ulong4 lhs, cl_ulong rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline ulong4 operator>>(cl_ulong lhs, const ulong4& rhs)
{
  auto tmp = ulong4(lhs);
  return tmp >> rhs;
}

inline bool operator==(const ulong4& lhs, const ulong4& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]);
}

inline bool operator==(const ulong4& lhs, const cl_ulong& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs);
}

inline bool operator<(const ulong4& lhs, const ulong4& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const ulong4& lhs, const cl_ulong& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const ulong4& lhs, const ulong4& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const ulong4& lhs, const cl_ulong& rhs)
{
  for (size_t i = 0; i < 4; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const ulong4& lhs, const ulong4& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const ulong4& lhs, const cl_ulong& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const ulong4& lhs, const ulong4& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const ulong4& lhs, const cl_ulong& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const ulong4& lhs, const ulong4& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const ulong4& lhs, const cl_ulong& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline ulong4 vload4(size_t offset, const ulong4* p)
{
  return *(p + offset);
}

inline void vstore4(ulong4 data, size_t offset, ulong4* p)
{
  *(p + offset) = data;
}


class ulong8{
public:
  static const size_t N = 8; // Number of elements in vector
  union {
	 cl_ulong CL_ALIGNED(64) s[8];
#if defined(__GNUC__)
    __extension__ struct { cl_ulong x, y, z, w; };
    __extension__ struct { cl_ulong s0, s1, s2, s3, s4, s5, s6, s7; };
#endif
  };

  ulong8(){}

  ulong8(cl_ulong i0,cl_ulong i1,cl_ulong i2,cl_ulong i3,
          cl_ulong i4,cl_ulong i5,cl_ulong i6,cl_ulong i7
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;
    s[4] = i4;
    s[5] = i5;
    s[6] = i6;
    s[7] = i7;

  }

  ulong8(cl_ulong i0) {
  for (int i = 0; i < 8; ++i) {
		s[i] = i0;
	 }
  }

  ulong8(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
  }

  ulong8(const ulong2& a0, const ulong2& a1, const ulong4& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
  }

  ulong8(const ulong2& a0, const ulong3& a1, const ulong3& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
  }

  ulong8(const ulong2& a0, const ulong4& a1, const ulong2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  ulong8(const ulong3& a0, const ulong2& a1, const ulong3& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
  }

  ulong8(const ulong3& a0, const ulong3& a1, const ulong2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a1.s[2];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  ulong8(const ulong4& a0, const ulong2& a1, const ulong2& a2) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
  }

  ulong8(const ulong4& a0, const ulong4& a1) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a1.s[0];
    s[5] = a1.s[1];
    s[6] = a1.s[2];
    s[7] = a1.s[3];
  }

  ulong8(const ulong8& a0) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a0.s[3];
    s[4] = a0.s[4];
    s[5] = a0.s[5];
    s[6] = a0.s[6];
    s[7] = a0.s[7];
  }

  ulong8& operator+=(const ulong8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] += rhs.s[i];
    }
    return *this;
  }

  ulong8& operator+=(cl_ulong rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] += rhs;
    }
    return *this;
  }

  ulong8& operator-=(const ulong8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] -= rhs.s[i];
    }
    return *this;
  }

  ulong8& operator-=(cl_ulong rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] -= rhs;
    }
    return *this;
  }

  ulong8& operator*=(const ulong8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] *= rhs.s[i];
    }
    return *this;
  }

  ulong8& operator*=(cl_ulong rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] *= rhs;
    }
    return *this;
  }

  ulong8& operator/=(const ulong8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] /= rhs.s[i];
    }
    return *this;
  }

  ulong8& operator/=(cl_ulong rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] /= rhs;
    }
    return *this;
  }

  ulong8& operator%=(const ulong8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] %= rhs.s[i];
    }
    return *this;
  }

  ulong8& operator%=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] %= rhs;
    }
    return *this;
  }

  ulong8& operator&=(const ulong8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] &= rhs.s[i];
    }
    return *this;
  }

  ulong8& operator&=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] &= rhs;
    }
    return *this;
  }

  ulong8& operator|=(const ulong8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] |= rhs.s[i];
    }
    return *this;
  }

  ulong8& operator|=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] |= rhs;
    }
    return *this;
  }

  ulong8& operator^=(const ulong8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] ^= rhs.s[i];
    }
    return *this;
  }

  ulong8& operator^=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] ^= rhs;
    }
    return *this;
  }

  ulong8& operator<<=(const ulong8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] <<= rhs.s[i];
    }
    return *this;
  }

  ulong8& operator<<=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] <<= rhs;
    }
    return *this;
  }

  ulong8& operator>>=(const ulong8& rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] >>= rhs.s[i];
    }
    return *this;
  }

  ulong8& operator>>=(cl_uint rhs)
  {
    for (size_t i = 0; i < 8; ++i) {
      s[i] >>= rhs;
    }
    return *this;
  }

};

inline ulong8 operator+(ulong8 lhs, const ulong8& rhs)
{
  lhs += rhs;
  return lhs;
}

inline ulong8 operator+(ulong8 lhs, cl_ulong rhs)
{
  lhs += rhs;
  return lhs;
}

inline ulong8 operator+(cl_ulong lhs, const ulong8& rhs)
{
  auto tmp = ulong8(lhs);
  return tmp + rhs;
}

inline ulong8 operator-(ulong8 lhs, const ulong8& rhs)
{
  lhs -= rhs;
  return lhs;
}

inline ulong8 operator-(ulong8 lhs, cl_ulong rhs)
{
  lhs -= rhs;
  return lhs;
}

inline ulong8 operator-(cl_ulong lhs, const ulong8& rhs)
{
  auto tmp = ulong8(lhs);
  return tmp - rhs;
}

inline ulong8 operator*(ulong8 lhs, const ulong8& rhs)
{
  lhs *= rhs;
  return lhs;
}

inline ulong8 operator*(ulong8 lhs, cl_ulong rhs)
{
  lhs *= rhs;
  return lhs;
}

inline ulong8 operator*(cl_ulong lhs, const ulong8& rhs)
{
  auto tmp = ulong8(lhs);
  return tmp * rhs;
}

inline ulong8 operator/(ulong8 lhs, const ulong8& rhs)
{
  lhs /= rhs;
  return lhs;
}

inline ulong8 operator/(ulong8 lhs, cl_ulong rhs)
{
  lhs /= rhs;
  return lhs;
}

inline ulong8 operator/(cl_ulong lhs, const ulong8& rhs)
{
  auto tmp = ulong8(lhs);
  return tmp / rhs;
}

inline ulong8 operator%(ulong8 lhs, const ulong8& rhs)
{
  lhs %= rhs;
  return lhs;
}

inline ulong8 operator%(ulong8 lhs, cl_ulong rhs)
{
  lhs %= rhs;
  return lhs;
}

inline ulong8 operator%(cl_ulong lhs, const ulong8& rhs)
{
  auto tmp = ulong8(lhs);
  return tmp % rhs;
}

inline ulong8 operator&(ulong8 lhs, const ulong8& rhs)
{
  lhs &= rhs;
  return lhs;
}

inline ulong8 operator&(ulong8 lhs, cl_ulong rhs)
{
  lhs &= rhs;
  return lhs;
}

inline ulong8 operator&(cl_ulong lhs, const ulong8& rhs)
{
  auto tmp = ulong8(lhs);
  return tmp & rhs;
}

inline ulong8 operator|(ulong8 lhs, const ulong8& rhs)
{
  lhs |= rhs;
  return lhs;
}

inline ulong8 operator|(ulong8 lhs, cl_ulong rhs)
{
  lhs |= rhs;
  return lhs;
}

inline ulong8 operator|(cl_ulong lhs, const ulong8& rhs)
{
  auto tmp = ulong8(lhs);
  return tmp | rhs;
}

inline ulong8 operator^(ulong8 lhs, const ulong8& rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline ulong8 operator^(ulong8 lhs, cl_ulong rhs)
{
  lhs ^= rhs;
  return lhs;
}

inline ulong8 operator^(cl_ulong lhs, const ulong8& rhs)
{
  auto tmp = ulong8(lhs);
  return tmp ^ rhs;
}

inline ulong8 operator<<(ulong8 lhs, const ulong8& rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline ulong8 operator<<(ulong8 lhs, cl_ulong rhs)
{
  lhs <<= rhs;
  return lhs;
}

inline ulong8 operator<<(cl_ulong lhs, const ulong8& rhs)
{
  auto tmp = ulong8(lhs);
  return tmp << rhs;
}

inline ulong8 operator>>(ulong8 lhs, const ulong8& rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline ulong8 operator>>(ulong8 lhs, cl_ulong rhs)
{
  lhs >>= rhs;
  return lhs;
}

inline ulong8 operator>>(cl_ulong lhs, const ulong8& rhs)
{
  auto tmp = ulong8(lhs);
  return tmp >> rhs;
}

inline bool operator==(const ulong8& lhs, const ulong8& rhs)
{
  return  (lhs.s[0] == rhs.s[0]) &&
  (lhs.s[1] == rhs.s[1]) &&
  (lhs.s[2] == rhs.s[2]) &&
  (lhs.s[3] == rhs.s[3]) &&
  (lhs.s[4] == rhs.s[4]) &&
  (lhs.s[5] == rhs.s[5]) &&
  (lhs.s[6] == rhs.s[6]) &&
  (lhs.s[7] == rhs.s[7]);
}

inline bool operator==(const ulong8& lhs, const cl_ulong& rhs)
{
  return  (lhs.s[0] == rhs) &&
  (lhs.s[1] == rhs) &&
  (lhs.s[2] == rhs) &&
  (lhs.s[3] == rhs) &&
  (lhs.s[4] == rhs) &&
  (lhs.s[5] == rhs) &&
  (lhs.s[6] == rhs) &&
  (lhs.s[7] == rhs);
}

inline bool operator<(const ulong8& lhs, const ulong8& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] < rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator<(const ulong8& lhs, const cl_ulong& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] < rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator>(const ulong8& lhs, const ulong8& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] > rhs.s[i]) return true;
    else if (lhs.s[i] == rhs.s[i]) continue;
    else break;
  }
  return false;
}

inline bool operator>(const ulong8& lhs, const cl_ulong& rhs)
{
  for (size_t i = 0; i < 8; ++i) {
    if (lhs.s[i] > rhs) return true;
    else if (lhs.s[i] == rhs) continue;
    else break;
  }
  return false;
}

inline bool operator!=(const ulong8& lhs, const ulong8& rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const ulong8& lhs, const cl_ulong& rhs)
{
  return !(lhs == rhs);
}

inline bool operator<=(const ulong8& lhs, const ulong8& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator<=(const ulong8& lhs, const cl_ulong& rhs)
{
  return (lhs < rhs) || (lhs == rhs);
}

inline bool operator>=(const ulong8& lhs, const ulong8& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline bool operator>=(const ulong8& lhs, const cl_ulong& rhs)
{
  return (lhs > rhs) || (lhs == rhs);
}

inline ulong8 vload8(size_t offset, const ulong8* p)
{
  return *(p + offset);
}

inline void vstore8(ulong8 data, size_t offset, ulong8* p)
{
  *(p + offset) = data;
}


class ulong16{
public:
  static const size_t N = 16; // Number of elements in vector
  union {
	 cl_ulong CL_ALIGNED(128) s[16];
#if defined(__GNUC__)
    __extension__ struct { cl_ulong x, y, z, w; };
    __extension__ struct { cl_ulong s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15; };
#endif
  };

  ulong16(){}

  ulong16(cl_ulong i0,cl_ulong i1,cl_ulong i2,cl_ulong i3,
          cl_ulong i4,cl_ulong i5,cl_ulong i6,cl_ulong i7,
          cl_ulong i8,cl_ulong i9,cl_ulong i10,cl_ulong i11,
          cl_ulong i12,cl_ulong i13,cl_ulong i14,cl_ulong i15
          ){
    s[0] = i0;
    s[1] = i1;
    s[2] = i2;
    s[3] = i3;
    s[4] = i4;
    s[5] = i5;
    s[6] = i6;
    s[7] = i7;
    s[8] = i8;
    s[9] = i9;
    s[10] = i10;
    s[11] = i11;
    s[12] = i12;
    s[13] = i13;
    s[14] = i14;
    s[15] = i15;

  }

  ulong16(cl_ulong i0) {
  for (int i = 0; i < 16; ++i) {
		s[i] = i0;
	 }
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong2& a3, const ulong2& a4, const ulong2& a5, const ulong2& a6, const ulong2& a7) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a6.s[0];
    s[13] = a6.s[1];
    s[14] = a7.s[0];
    s[15] = a7.s[1];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong2& a3, const ulong2& a4, const ulong2& a5, const ulong4& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a6.s[0];
    s[13] = a6.s[1];
    s[14] = a6.s[2];
    s[15] = a6.s[3];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong2& a3, const ulong2& a4, const ulong3& a5, const ulong3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a5.s[2];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong2& a3, const ulong2& a4, const ulong4& a5, const ulong2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a5.s[0];
    s[11] = a5.s[1];
    s[12] = a5.s[2];
    s[13] = a5.s[3];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong2& a3, const ulong3& a4, const ulong2& a5, const ulong3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong2& a3, const ulong3& a4, const ulong3& a5, const ulong2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong2& a3, const ulong4& a4, const ulong2& a5, const ulong2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong2& a3, const ulong4& a4, const ulong4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong2& a3, const ulong8& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a4.s[0];
    s[9] = a4.s[1];
    s[10] = a4.s[2];
    s[11] = a4.s[3];
    s[12] = a4.s[4];
    s[13] = a4.s[5];
    s[14] = a4.s[6];
    s[15] = a4.s[7];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong3& a3, const ulong2& a4, const ulong2& a5, const ulong3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong3& a3, const ulong2& a4, const ulong3& a5, const ulong2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong3& a3, const ulong3& a4, const ulong2& a5, const ulong2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong3& a3, const ulong3& a4, const ulong4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong3& a3, const ulong4& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong4& a3, const ulong2& a4, const ulong2& a5, const ulong2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong4& a3, const ulong2& a4, const ulong4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong4& a3, const ulong3& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong4& a3, const ulong4& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong2& a2, const ulong8& a3, const ulong2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a3.s[0];
    s[7] = a3.s[1];
    s[8] = a3.s[2];
    s[9] = a3.s[3];
    s[10] = a3.s[4];
    s[11] = a3.s[5];
    s[12] = a3.s[6];
    s[13] = a3.s[7];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong3& a2, const ulong2& a3, const ulong2& a4, const ulong2& a5, const ulong3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong3& a2, const ulong2& a3, const ulong2& a4, const ulong3& a5, const ulong2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong3& a2, const ulong2& a3, const ulong3& a4, const ulong2& a5, const ulong2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong3& a2, const ulong2& a3, const ulong3& a4, const ulong4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong3& a2, const ulong2& a3, const ulong4& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong3& a2, const ulong3& a3, const ulong2& a4, const ulong2& a5, const ulong2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong3& a2, const ulong3& a3, const ulong2& a4, const ulong4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong3& a2, const ulong3& a3, const ulong3& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong3& a2, const ulong3& a3, const ulong4& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong3& a2, const ulong4& a3, const ulong2& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong3& a2, const ulong4& a3, const ulong3& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong4& a2, const ulong2& a3, const ulong2& a4, const ulong2& a5, const ulong2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong4& a2, const ulong2& a3, const ulong2& a4, const ulong4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong4& a2, const ulong2& a3, const ulong3& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong4& a2, const ulong2& a3, const ulong4& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong4& a2, const ulong3& a3, const ulong2& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong4& a2, const ulong3& a3, const ulong3& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong4& a2, const ulong4& a3, const ulong2& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong4& a2, const ulong4& a3, const ulong4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong4& a2, const ulong8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong8& a2, const ulong2& a3, const ulong2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a2.s[4];
    s[9] = a2.s[5];
    s[10] = a2.s[6];
    s[11] = a2.s[7];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  ulong16(const ulong2& a0, const ulong2& a1, const ulong8& a2, const ulong4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a2.s[0];
    s[5] = a2.s[1];
    s[6] = a2.s[2];
    s[7] = a2.s[3];
    s[8] = a2.s[4];
    s[9] = a2.s[5];
    s[10] = a2.s[6];
    s[11] = a2.s[7];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong2& a2, const ulong2& a3, const ulong2& a4, const ulong2& a5, const ulong3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong2& a2, const ulong2& a3, const ulong2& a4, const ulong3& a5, const ulong2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong2& a2, const ulong2& a3, const ulong3& a4, const ulong2& a5, const ulong2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong2& a2, const ulong2& a3, const ulong3& a4, const ulong4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong2& a2, const ulong2& a3, const ulong4& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong2& a2, const ulong3& a3, const ulong2& a4, const ulong2& a5, const ulong2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong2& a2, const ulong3& a3, const ulong2& a4, const ulong4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong2& a2, const ulong3& a3, const ulong3& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong2& a2, const ulong3& a3, const ulong4& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong2& a2, const ulong4& a3, const ulong2& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong2& a2, const ulong4& a3, const ulong3& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong3& a2, const ulong2& a3, const ulong2& a4, const ulong2& a5, const ulong2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong3& a2, const ulong2& a3, const ulong2& a4, const ulong4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong3& a2, const ulong2& a3, const ulong3& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong3& a2, const ulong2& a3, const ulong4& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong3& a2, const ulong3& a3, const ulong2& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong3& a2, const ulong3& a3, const ulong3& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong3& a2, const ulong4& a3, const ulong2& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong3& a2, const ulong4& a3, const ulong4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong3& a2, const ulong8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong4& a2, const ulong2& a3, const ulong2& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong4& a2, const ulong2& a3, const ulong3& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong4& a2, const ulong3& a3, const ulong2& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong4& a2, const ulong3& a3, const ulong4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong4& a2, const ulong4& a3, const ulong3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  ulong16(const ulong2& a0, const ulong3& a1, const ulong8& a2, const ulong3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a2.s[4];
    s[10] = a2.s[5];
    s[11] = a2.s[6];
    s[12] = a2.s[7];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  ulong16(const ulong2& a0, const ulong4& a1, const ulong2& a2, const ulong2& a3, const ulong2& a4, const ulong2& a5, const ulong2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ulong16(const ulong2& a0, const ulong4& a1, const ulong2& a2, const ulong2& a3, const ulong2& a4, const ulong4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ulong16(const ulong2& a0, const ulong4& a1, const ulong2& a2, const ulong2& a3, const ulong3& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong2& a0, const ulong4& a1, const ulong2& a2, const ulong2& a3, const ulong4& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong2& a0, const ulong4& a1, const ulong2& a2, const ulong3& a3, const ulong2& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong2& a0, const ulong4& a1, const ulong2& a2, const ulong3& a3, const ulong3& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong2& a0, const ulong4& a1, const ulong2& a2, const ulong4& a3, const ulong2& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong2& a0, const ulong4& a1, const ulong2& a2, const ulong4& a3, const ulong4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ulong16(const ulong2& a0, const ulong4& a1, const ulong2& a2, const ulong8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  ulong16(const ulong2& a0, const ulong4& a1, const ulong3& a2, const ulong2& a3, const ulong2& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong2& a0, const ulong4& a1, const ulong3& a2, const ulong2& a3, const ulong3& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong2& a0, const ulong4& a1, const ulong3& a2, const ulong3& a3, const ulong2& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong2& a0, const ulong4& a1, const ulong3& a2, const ulong3& a3, const ulong4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ulong16(const ulong2& a0, const ulong4& a1, const ulong3& a2, const ulong4& a3, const ulong3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a3.s[3];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  ulong16(const ulong2& a0, const ulong4& a1, const ulong4& a2, const ulong2& a3, const ulong2& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong2& a0, const ulong4& a1, const ulong4& a2, const ulong2& a3, const ulong4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ulong16(const ulong2& a0, const ulong4& a1, const ulong4& a2, const ulong3& a3, const ulong3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a4.s[0];
    s[14] = a4.s[1];
    s[15] = a4.s[2];
  }

  ulong16(const ulong2& a0, const ulong4& a1, const ulong4& a2, const ulong4& a3, const ulong2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a3.s[0];
    s[11] = a3.s[1];
    s[12] = a3.s[2];
    s[13] = a3.s[3];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  ulong16(const ulong2& a0, const ulong4& a1, const ulong8& a2, const ulong2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a2.s[0];
    s[7] = a2.s[1];
    s[8] = a2.s[2];
    s[9] = a2.s[3];
    s[10] = a2.s[4];
    s[11] = a2.s[5];
    s[12] = a2.s[6];
    s[13] = a2.s[7];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  ulong16(const ulong2& a0, const ulong8& a1, const ulong2& a2, const ulong2& a3, const ulong2& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a4.s[0];
    s[15] = a4.s[1];
  }

  ulong16(const ulong2& a0, const ulong8& a1, const ulong2& a2, const ulong4& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a3.s[0];
    s[13] = a3.s[1];
    s[14] = a3.s[2];
    s[15] = a3.s[3];
  }

  ulong16(const ulong2& a0, const ulong8& a1, const ulong3& a2, const ulong3& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a3.s[0];
    s[14] = a3.s[1];
    s[15] = a3.s[2];
  }

  ulong16(const ulong2& a0, const ulong8& a1, const ulong4& a2, const ulong2& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a1.s[0];
    s[3] = a1.s[1];
    s[4] = a1.s[2];
    s[5] = a1.s[3];
    s[6] = a1.s[4];
    s[7] = a1.s[5];
    s[8] = a1.s[6];
    s[9] = a1.s[7];
    s[10] = a2.s[0];
    s[11] = a2.s[1];
    s[12] = a2.s[2];
    s[13] = a2.s[3];
    s[14] = a3.s[0];
    s[15] = a3.s[1];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong2& a2, const ulong2& a3, const ulong2& a4, const ulong2& a5, const ulong3& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a6.s[0];
    s[14] = a6.s[1];
    s[15] = a6.s[2];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong2& a2, const ulong2& a3, const ulong2& a4, const ulong3& a5, const ulong2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a5.s[0];
    s[12] = a5.s[1];
    s[13] = a5.s[2];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong2& a2, const ulong2& a3, const ulong3& a4, const ulong2& a5, const ulong2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong2& a2, const ulong2& a3, const ulong3& a4, const ulong4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong2& a2, const ulong2& a3, const ulong4& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a4.s[0];
    s[10] = a4.s[1];
    s[11] = a4.s[2];
    s[12] = a4.s[3];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong2& a2, const ulong3& a3, const ulong2& a4, const ulong2& a5, const ulong2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong2& a2, const ulong3& a3, const ulong2& a4, const ulong4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong2& a2, const ulong3& a3, const ulong3& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong2& a2, const ulong3& a3, const ulong4& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong2& a2, const ulong4& a3, const ulong2& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong2& a2, const ulong4& a3, const ulong3& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a3.s[0];
    s[8] = a3.s[1];
    s[9] = a3.s[2];
    s[10] = a3.s[3];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong3& a2, const ulong2& a3, const ulong2& a4, const ulong2& a5, const ulong2& a6) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a6.s[0];
    s[15] = a6.s[1];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong3& a2, const ulong2& a3, const ulong2& a4, const ulong4& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a5.s[0];
    s[13] = a5.s[1];
    s[14] = a5.s[2];
    s[15] = a5.s[3];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong3& a2, const ulong2& a3, const ulong3& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong3& a2, const ulong2& a3, const ulong4& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a4.s[0];
    s[11] = a4.s[1];
    s[12] = a4.s[2];
    s[13] = a4.s[3];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong3& a2, const ulong3& a3, const ulong2& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong3& a2, const ulong3& a3, const ulong3& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong3& a2, const ulong4& a3, const ulong2& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong3& a2, const ulong4& a3, const ulong4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong3& a2, const ulong8& a3) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a3.s[0];
    s[9] = a3.s[1];
    s[10] = a3.s[2];
    s[11] = a3.s[3];
    s[12] = a3.s[4];
    s[13] = a3.s[5];
    s[14] = a3.s[6];
    s[15] = a3.s[7];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong4& a2, const ulong2& a3, const ulong2& a4, const ulong3& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a5.s[0];
    s[14] = a5.s[1];
    s[15] = a5.s[2];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong4& a2, const ulong2& a3, const ulong3& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a4.s[0];
    s[12] = a4.s[1];
    s[13] = a4.s[2];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong4& a2, const ulong3& a3, const ulong2& a4, const ulong2& a5) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a5.s[0];
    s[15] = a5.s[1];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong4& a2, const ulong3& a3, const ulong4& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
    s[8] = a2.s[3];
    s[9] = a3.s[0];
    s[10] = a3.s[1];
    s[11] = a3.s[2];
    s[12] = a4.s[0];
    s[13] = a4.s[1];
    s[14] = a4.s[2];
    s[15] = a4.s[3];
  }

  ulong16(const ulong3& a0, const ulong2& a1, const ulong4& a2, const ulong4& a3, const ulong3& a4) {
    s[0] = a0.s[0];
    s[1] = a0.s[1];
    s[2] = a0.s[2];
    s[3] = a1.s[0];
    s[4] = a1.s[1];
    s[5] = a2.s[0];
    s[6] = a2.s[1];
    s[7] = a2.s[2];
  }

  }

