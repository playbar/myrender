//=============================================================================
// FILE: svrApiConfig.h
//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//==============================================================================
#ifndef _SVR_API_CONFIG_H_
#define _SVR_API_CONFIG_H_

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "svrContainers.h"

void LoadCommandLineVariables(char* argv[], int argc);
void LoadVariableFile(const char* filename);
void LoadVariableBuffer(const char* buffer);
void WriteVariableFile(const char* filename);
void LoadVariable(const char* text);

inline bool CompareTextCaseless(const char* s1, const char* s2)
{
	for (int a = 0;; a++) {
		unsigned long x = *reinterpret_cast<const unsigned char*>(s1 + a);
		unsigned long y = *reinterpret_cast<const unsigned char*>(s2 + a);
		if (x - 65 < 26UL) x += 32;
		if (y - 65 < 26UL) y += 32;
		if (x != y) return false;
		if (x == 0) break;
	}

	return true;
}

inline bool CompareTextLessThan(const char* s1, const char* s2)
{
	for (int a = 0;; a++) {
		unsigned long x = *reinterpret_cast<const unsigned char*>(s1 + a);
		unsigned long y = *reinterpret_cast<const unsigned char*>(s2 + a);
		if (x - 'a' < 26UL) x -= 32;
		if (y - 'a' < 26UL) y -= 32;
		if ((x != y) || (x == 0)) return (x < y);
	}
}

inline bool CompareTextLessEqual(const char* s1, const char* s2)
{
	for (int a = 0;; a++) {
		unsigned long x = *reinterpret_cast<const unsigned char*>(s1 + a);
		unsigned long y = *reinterpret_cast<const unsigned char*>(s2 + a);
		if (x - 'a' < 26UL) x -= 32;
		if (y - 'a' < 26UL) y -= 32;
		if ((x != y) || (x == 0)) return (x <= y);
	}
}

inline long StringToInteger(const char* text)
{
	long value = 0;
	bool negative = false;

	for (;;) {
		unsigned long x = *text++;
		if (x == 0) break;

		if (x == '-') {
			negative = true;
		}
		else {
			x -= 48;
			if (x < 10) value = value * 10 + x;
		}
	}

	if (negative) value = -value;
	return value;
}

inline float StringToFloat(const char* text)
{
	float value = 0.0F;
	float expon = 0.0F;
	float decplace = 0.1F;

	bool negative = false;
	bool exponent = false;
	bool exponNeg = false;
	bool decimal = false;

	for (;;) {
		unsigned long x = *text++;
		if (x == 0) break;

		if (x == '-') {
			if (exponent) exponNeg = true;
			else negative = true;
		}
		else if (x == '.') {
			decimal = true;
		}
		else if ((x == 'e') || (x == 'E')) {
			exponent = true;
		}
		else {
			x -= 48;
			if (x < 10) {
				if (exponent) {
					expon = expon * 10.0F + x;
				}
				else {
					if (decimal) {
						value += x * decplace;
						decplace *= 0.1F;
					}
					else {
						value = value * 10.0F + x;
					}
				}
			}
		}
	}

	if (exponent) {
		if (exponNeg) expon = -expon;
		value *= pow(10.0F, expon);
	}

	if (negative) value = -value;
	return value;
}

inline bool StringToBool(const char* text)
{
	if (CompareTextCaseless(text, "yes") || CompareTextCaseless(text, "true") || CompareTextCaseless(text, "on")) {
		return true;
	}
	else
		if (CompareTextCaseless(text, "no") || CompareTextCaseless(text, "false") || CompareTextCaseless(text, "off")) {
			return false;
		}
		else {
			int i = atoi(text);
			return i != 0;
		}
}

inline long IntegerToString(long num, char* text, long max)
{
	char c[16];

	bool negative = (num < 0);
	num = abs(num) & 0x7FFFFFFF;

	int length = 0;
	do {
		long p = num % 10;
		c[length++] = (char)(p + 48);
		num /= 10;
	} while (num != 0);

	int a = -1;
	if (negative) {
		if (++a < max) {
			text[a] = '-';
		}
		else {
			text[a] = 0;
			return (a);
		}
	}

	do {
		if (++a < max) {
			text[a] = c[--length];
		}
		else {
			text[a] = 0;
			return (a);
		}
	} while (length != 0);

	text[++a] = 0;
	return (a);
}

inline long FloatToString(float num, char* text, long max)
{
	if (max < 1) {
		text[0] = 0;
		return (0);
	}

	long binary = *reinterpret_cast<long*>(&num);
	long exponent = (binary >> 23) & 0xFF;

	if (exponent == 0) {
		if (max >= 3) {
			text[0] = '0';
			text[1] = '.';
			text[2] = '0';
			text[3] = 0;
			return (3);
		}

		text[0] = '0';
		text[1] = 0;
		return (1);
	}

	long mantissa = binary & 0x007FFFFF;

	if (exponent == 0xFF) {
		if (max >= 4) {
			bool b = (binary < 0);
			if (b) *text++ = '-';

			if (mantissa == 0) {
				text[0] = 'I';
				text[1] = 'N';
				text[2] = 'F';
				text[3] = 0;
			}
			else {
				text[0] = 'N';
				text[1] = 'A';
				text[2] = 'N';
				text[3] = 0;
			}

			return (3 + b);
		}

		text[0] = 0;
		return (0);
	}

	long power = 0;
	float absolute = fabs(num);
	if ((!(absolute > 1.0e-4F)) || (!(absolute < 1.0e5F))) {
		float f = floor(log10(absolute));
		absolute /= pow(10.0F, f);
		power = (long)f;

		binary = *reinterpret_cast<long *>(&absolute);
		exponent = (binary >> 23) & 0xFF;
		mantissa = binary & 0x007FFFFF;
	}

	exponent -= 0x7F;
	mantissa |= 0x00800000;

	int len = 0;
	if (num < 0.0F) {
		text[0] = '-';
		len = 1;
	}

	if (exponent >= 0) {
		long whole = mantissa >> (23 - exponent);
		mantissa = (mantissa << exponent) & 0x007FFFFF;

		len += IntegerToString(whole, &text[len], max - len);
		if (len < max) text[len++] = '.';
		if (len == max) goto end;
	}
	else {
		if (len + 2 <= max) {
			text[len++] = '0';
			text[len++] = '.';
			if (len == max) goto end;
		}
		else {
			if (len < max) text[len++] = '0';
			goto end;
		}

		mantissa >>= -exponent;
	}

	for (int a = 0, zeroCount = 0, nineCount = 0; (a < 7) && (len < max); a++) {
		mantissa *= 10;
		long n = (mantissa >> 23) + 48;
		text[len++] = (char)n;

		if (n == '0') {
			if ((++zeroCount >= 4) && (a >= 4)) break;
		}
		else if (n == '9') {
			if ((++nineCount >= 4) && (a >= 4)) break;
		}

		mantissa &= 0x007FFFFF;
		if (mantissa < 2) break;
	}

	if ((text[len - 1] == '9') && (text[len - 2] == '9')) {
		for (int a = len - 3;; a--) {
			char c = text[a];
			if (c != '9') {
				if (c != '.') {
					text[a] = c + 1;
					len = a + 1;
				}

				break;
			}
		}
	}
	else {
		while (text[len - 1] == '0') len--;
		if (text[len - 1] == '.') text[len++] = '0';
	}

	if ((power != 0) && (len < max)) {
		text[len++] = 'e';
		return (IntegerToString(power, &text[len], max - len));
	}

end:
	text[len] = 0;
	return (len);
}


struct ConstCharPtr {
private:
	const char	*ptr;
public:
	ConstCharPtr() {}

	ConstCharPtr(const char* c)
	{
		ptr = c;
	}

	operator const char*() const
	{
		return ptr;
	}

	ConstCharPtr& operator =(const char* c)
	{
		ptr = c;
		return *this;
	}

	bool operator ==(const char* c) const
	{
		return CompareTextCaseless(ptr, c);
	}

	bool operator !=(const char* c) const
	{
		return !CompareTextCaseless(ptr, c);
	}

	bool operator <(const char* c) const
	{
		return CompareTextLessThan(ptr, c);
	}

	bool operator <=(const char* c) const
	{
		return CompareTextLessEqual(ptr, c);
	}

	bool operator >(const char* c) const
	{
		return !CompareTextLessEqual(ptr, c);
	}

	bool operator >=(const char* c) const
	{
		return !CompareTextLessThan(ptr, c);
	}
};

enum {
	kVariableNonpersistent = 1 << 0,
	kVariablePermanent = 1 << 1
};

enum {
	kMaxVariableNameLength = 31,
	kMaxVariableValueLength = 255
};

class VariableBase : public MapElement<VariableBase> {
private:
	unsigned long mFlags;
	char          mName[kMaxVariableNameLength];
protected:
	VariableBase(const char* name, unsigned long flags = 0)
	{
		mFlags = flags;
		strncpy(mName, name, kMaxVariableNameLength);
	}
	virtual ~VariableBase() {}
public:
	typedef ConstCharPtr KeyType;

	unsigned long GetFlags() const
	{
		return mFlags;
	}

	void SetFlags(unsigned long flags)
	{
		mFlags = flags;
	}

	KeyType GetKey() const
	{
		return mName;
	}

	const char* GetName() const
	{
		return mName;
	}

	virtual void SetValue(const char* const text) = 0;
	virtual void GetValue(char* text, long max) = 0;
};

template <class Type>
void ReadFromText(Type*, const char* const)
{
	//assert(false); // this type is unsupported
}

template <>
inline void ReadFromText<char>(char* val, const char* const text)
{
	*val = *text;
}

template <>
inline void ReadFromText<char*>(char** val, const char* const text)
{
	*val = new char[strlen(text) + 1];
	strcpy(*val, text);
}

template <>
inline void ReadFromText<bool>(bool* val, const char* const text)
{
	*val = StringToBool(text);
}

template <>
inline void ReadFromText<float>(float* val, const char* const text)
{
	*val = StringToFloat(text);
}

template <>
inline void ReadFromText<double>(double* val, const char* const text)
{
	*val = StringToFloat(text);
}

template <>
inline void ReadFromText<unsigned long>(unsigned long* val, const char* const text)
{
	*val = StringToInteger(text);
}


template <>
inline void ReadFromText<long>(long* val, const char* const text)
{
	*val = StringToInteger(text);
}

template <>
inline void ReadFromText<unsigned int>(unsigned int* val, const char* const text)
{
	*val = StringToInteger(text);
}

template <>
inline void ReadFromText<int>(int* val, const char* const text)
{
	*val = StringToInteger(text);
}

template <class Type>
void WriteToText(const Type&, char*, long)
{
	//assert(false); // this type is unsupported
}

template <>
inline void WriteToText<char>(const char& val, char* text, long max)
{
	snprintf(text, max, "%s", &val);
}

template <>
inline void WriteToText<bool>(const bool& val, char* text, long max)
{
	snprintf(text, max, "%s", val ? "yes" : "no");
}

template <>
inline void WriteToText<float>(const float& val, char* text, long max)
{
	snprintf(text, max, "%.6g", val);
}

template <>
inline void WriteToText<double>(const double& val, char* text, long max)
{
	snprintf(text, max, "%.8g", val);
}

template <>
inline void WriteToText<int>(const int& val, char* text, long max)
{
	snprintf(text, max, "%d", val);
}

template <class Type>
class Variable : public VariableBase {
private:
	Type*            mPtr;
public:

	explicit Variable(const char* name, Type* ptr, unsigned long flags = 0, void* data = 0)
		: VariableBase(name, flags)
	{
		mPtr = ptr;
	}

	virtual ~Variable() {}

	virtual void SetValue(const char* const text)
	{
		ReadFromText(mPtr, text);
	}

	virtual void GetValue(char* text, long max)
	{
		WriteToText(*mPtr, text, max);
	}
};

extern VariableBase* GetVariable(const char* name);
extern bool AddVariable(VariableBase* variable);

template <typename Type>
Type RegisterVariable(const char* name, Type* ptr, Type init, unsigned long flags)
{
	VariableBase* variable = GetVariable(name);
	if (!variable) {
		variable = new Variable<Type>(name, ptr, flags, 0);
		AddVariable(variable);
	}
	else {
		variable->SetFlags(flags);
	}

	return init;
}

#define VAR(type, var, val, flags) \
    extern "C" { type var = (type)(val); } \
    static type var##_Var = RegisterVariable<type>(#var, (type*)&var, (type)(val), flags)

#define EXTERN_VAR(type, var) \
    extern "C" { extern type var; }

#endif //_SVR_API_CONFIG_H_