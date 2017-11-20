#include "MojingProfileKey.h"
#include "../Base/CRC.h"
#include "../Base/Base32.h"
#include "../3rdPart/AES/AES.h"
namespace Baofeng
{
	namespace Mojing
	{
		extern unsigned char g_EncKey[16];
#if defined (MJ_OS_ANDROID)
#define PlatformID 1
#elif defined (MJ_OS_IPHONE)
#define PlatformID 2
#elif defined (MJ_OS_WIN32)
#define PlatformID 3
#endif

		MojingProfileKey::MojingProfileKey():
			m_uiManufacturerID(0),
			m_uiProductID(0),
			m_uiGlassID(0),
			m_uiPlatformID(0),
			m_uiAppID(0)
		{
		}

		MojingProfileKey::~MojingProfileKey()
		{
		}
		void MojingProfileKey::Reset()
		{
			m_uiManufacturerID = 0;
			m_uiPlatformID = 0;
			m_uiProductID = 0;
			m_uiGlassID = 0;
			m_uiAppID = 0;

			m_uiPlatformID = PlatformID;
		}
		void MojingProfileKey::ToBuffer(unsigned char Buffer[16])const
		{
#ifdef _DEBUG
			Buffer[0] = 0;
#else
			while (!Buffer[0])
				Buffer[0] = rand() & 0xFF;
#endif
			//Buffer[0] |= m_uiPlatformID<<6;
			Buffer[1] = m_uiPlatformID;
			// 厂商ID
			Buffer[2] = (m_uiManufacturerID >> 16) & 0xFF;
			Buffer[3] = (m_uiManufacturerID >> 8) & 0xFF;
			Buffer[4] = m_uiManufacturerID & 0xFF;

			// 产品ID
			Buffer[5] = (m_uiProductID >> 16) & 0xFF;
			Buffer[6] = (m_uiProductID >> 8) & 0xFF;
			Buffer[7] = m_uiProductID & 0xFF;

			// 镜片ID
			Buffer[8] = (m_uiGlassID >> 24) & 0xFF;
			Buffer[9] = (m_uiGlassID >> 16) & 0xFF;
			Buffer[10] = (m_uiGlassID >> 8) & 0xFF;
			Buffer[11] = m_uiGlassID & 0xFF;

			// AppID
			Buffer[12] = (m_uiAppID >> 24) & 0xFF;
			Buffer[13] = (m_uiAppID >> 16) & 0xFF;
			Buffer[14] = (m_uiAppID >> 8) & 0xFF;
			Buffer[15] = m_uiAppID & 0xFF;
		}

		const char* MojingProfileKey::GetGlassKeyIDString(const char* szKey)
		{
			
			MojingProfileKey Key;
			if (Key.SetString(szKey))
			{	
				return Key.GetGlassKeyIDString();
			}
			return "UNKNOWN";
		}
		const char* MojingProfileKey::GetGlassKeyIDString()const
		{
			static char szRet[64];
			sprintf(szRet, "A=%d M=%d P=%d G=%d", GetAppID(), GetManufacturerID(), GetProductID(), GetGlassID());
			return szRet;
		}
		void MojingProfileKey::UnionKey(const MojingProfileKey OtherKey)
		{
			if (GetPlatformID() == 0 && OtherKey.GetPlatformID() != 0)
			{
				SetPlatformID(OtherKey.GetPlatformID());
			}
			if (GetAppID() == 0 && OtherKey.GetAppID() != 0)
			{
				SetAppID(OtherKey.GetAppID());
			}

			if (GetManufacturerID() == 0 && OtherKey.GetManufacturerID() != 0)
			{
				SetManufacturerID(OtherKey.GetManufacturerID());
			}

			if (GetProductID() == 0 && OtherKey.GetProductID() != 0)
			{
				SetProductID(OtherKey.GetProductID());
			}
			if (GetGlassID() == 0 && OtherKey.GetGlassID() != 0)
			{
				SetGlassID(OtherKey.GetGlassID());
			}
		}

#define MAKE_DWORD(A , B , C , D)   ((((unsigned int)A)<<24)|(((unsigned int)B) << 16)|(((unsigned int)C) << 8) | D)
		void MojingProfileKey::FromBuffer(unsigned char Buffer[16])
		{
			m_uiPlatformID = Buffer[1];
			///////////////////////////////////////////
			m_uiManufacturerID = MAKE_DWORD(0 , Buffer[2], Buffer[3], Buffer[4]);
			///////////////////////////////////////////
			m_uiProductID = MAKE_DWORD(0, Buffer[5], Buffer[6], Buffer[7]);
			///////////////////////////////////////////
			m_uiGlassID = MAKE_DWORD(Buffer[8], Buffer[9], Buffer[10], Buffer[11]);
			///////////////////////////////////////////
			m_uiAppID = MAKE_DWORD(Buffer[12], Buffer[13], Buffer[14], Buffer[15]);
		}
		String MojingProfileKey::GetString()const
		{
			String Ret = "";
			/*| 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B | C | D | E | F |
			  | 乱| 版 |   厂商ID   |   产品ID  |  镜片ID(非0)  |     APPID     |  <-- 密文存储
			*/
			unsigned char Buffer[18];
			memset(Buffer, 0, 18);
			
			ToBuffer(Buffer);
			AES A(g_EncKey);
			A.Cipher(Buffer, 16);
			
			unsigned short CRC = GetCrc16(0, Buffer, 16);
			memcpy(Buffer + 16, &CRC, 2);
			if (0 == GetCrc16(0, Buffer, 18))
			{
				int iBufferSizeOfBase32 = BufferToBase32(Buffer, 18 , NULL);
				char *Base32 = new char[iBufferSizeOfBase32 + 1];
				BufferToBase32(Buffer, 18, Base32);
				Ret = Base32;
				Ret = Ret.Insert("-", 30);
				Ret = Ret.Insert("-", 24);
				Ret = Ret.Insert("-", 18);
				Ret = Ret.Insert("-", 12);
				Ret = Ret.Insert("-", 6);
				delete[] Base32;
			}
			return Ret;
		}

		bool MojingProfileKey::SetString(String strKey)
		{
			bool bRet = false;
			// 1 去掉Key中间的'-'
			const char *pKey= strKey.ToCStr();
			char *pKeyBuffer = new char[strlen(pKey) + 1];
			char *pPos = pKeyBuffer;
			while (*pKey)
			{
				if (*pKey != '-')
				{
					*pPos++ = *pKey;
					*pPos = 0;
				}
				pKey++;
			}

			// 2 反Base32
			int iBufferSize = Base32ToBuffer(pKeyBuffer, NULL);
			if (iBufferSize == 18)// 解码后应该是18字节
			{
				unsigned char *pBuffer = new unsigned char[iBufferSize + 1];
				Base32ToBuffer(pKeyBuffer, pBuffer);
				// CRC 检查
				if (0 == GetCrc16(0, pBuffer, iBufferSize))
				{
					AES A(g_EncKey);
					A.InvCipher(pBuffer, iBufferSize - 2);
					FromBuffer(pBuffer);
					bRet = true;
				}
				delete[] pBuffer;
			}
			delete[] pKeyBuffer;

			return bRet;
		}


		const char* MojingProfileKey::GetStringWithID(unsigned int uiManufacturerID, unsigned int uiProductID, unsigned int uiGlassID)
		{
			static char szRet[128];
			*szRet = 0;

			MojingProfileKey Key;
			Key.SetManufacturerID(uiManufacturerID);
			Key.SetProductID(uiProductID);
			Key.SetGlassID(uiGlassID);
			strcpy(szRet , Key.GetString().ToCStr());

			return szRet;
		}
	}
}
