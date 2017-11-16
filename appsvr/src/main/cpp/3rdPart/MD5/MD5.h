///////////////////////////////
// http://mingcn.cnblogs.com //
//  xelz CopyRight (c) 2010  //
///////////////////////////////


#if !defined(AFX_AES_H__6BDD3760_BDE8_4C42_85EE_6F7A434B81C4__INCLUDED_)
#define AFX_AES_H__6BDD3760_BDE8_4C42_85EE_6F7A434B81C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/* Type define */
typedef unsigned char byte;
typedef unsigned int uint32;

#include "../../Base/MojingString.h"
using namespace::Baofeng::Mojing;

/* MD5 declaration. */
class MD5  
{

	public:
		MD5();
		MD5(const void* input, size_t length);
		void update(const void* input, size_t length);
		
		const byte* digest();
		String toString();
		void reset();

	private:
		void update(const byte* input, size_t length);
		void final();
		void transform(const byte block[64]);
		void encode(const uint32* input, byte* output, size_t length);
		void decode(const byte* input, uint32* output, size_t length);
		byte* bytesToHexString(const byte* input, size_t length);

		/* class uncopyable */
		MD5(const MD5&);
		MD5& operator=(const MD5&);

	private:
		uint32 _state[4]; /* state (ABCD) */
		uint32 _count[2]; /* number of bits, modulo 2^64 (low-order word first) */
		byte _buffer[64]; /* input buffer */
		byte _digest[16]; /* message digest */
		bool _finished;   /* calculate finished ? */

		static const byte PADDING[64]; /* padding for calculate */
		static const char HEX[16];
		enum { BUFFER_SIZE = 1024 };
};

#endif // !defined(AFX_AES_H__6BDD3760_BDE8_4C42_85EE_6F7A434B81C4__INCLUDED_)
