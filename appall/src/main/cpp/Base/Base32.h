#ifndef _BASE32_H_
#define _BASE32_H_
int BufferToBase32(const unsigned char* pInBuffer, int iLength, char* pOutBuffer);
int Base32ToBuffer(const char* pInBuffer, unsigned char* pOutBuffer);

int BufferToBase64(const unsigned char* pInBuffer, int iLength, char* pOutBuffer);
int Base64ToBuffer(const char* pInBuffer, unsigned char* pOutBuffer);
#endif