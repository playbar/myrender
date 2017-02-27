#ifndef _BASE32_H_
#define _BASE32_H_
int Buffer2Base32(const unsigned char* pInBuffer, int iLength, char* pOutBuffer);
int Base322Buffer(const char* pInBuffer, unsigned char* pOutBuffer);
#endif