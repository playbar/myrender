#ifndef _BASE64_H_
#define _BASE64_H_
#include "MojingString.h"
/**
* Base64 编码/解码
* 不同于Base32.h中采用的编码字典，这里使用Base64编码使用的标准字典
*/
namespace Baofeng
{
	namespace Mojing
	{
		class Base64 
		{
		private:
			String _base64_table;
			static const char base64_pad = '=';
		public:
			Base64()
			{
				_base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; /*这是Base64编码使用的标准字典*/
			}
			int Decode(const char *pInBuffer, char *pOutBuffer);
		};
	}
}
#endif
