#pragma once
#include "../Parameters/MojingJsonElement.h"
#include "../Base/MojingMath.h"
#include <map>
#include <vector>

namespace Baofeng
{
	namespace Mojing
	{
		class AtlasSize : public MojingJsonElement
		{
		public:
			AtlasSize(){};
			virtual ~AtlasSize() {};
			virtual JSON* ToJson()
			{
				JSON* pJson = JSON::CreateObject();

				wToJson(pJson);
				hToJson(pJson);
				return pJson;
			}
			virtual bool FromJson(JSON* pJson)
			{
				return
					wFromJson(pJson) &&
					hFromJson(pJson);
			}
			CLASS_MEMBER_J(int, m_i, w);
			CLASS_MEMBER_J(int, m_i, h);
		};
		class AtlasRect :public MojingJsonElement
		{
		public:
			AtlasRect() {};
			virtual ~AtlasRect() {};
			virtual JSON* ToJson()
			{
				JSON* pJson = JSON::CreateObject();
				xToJson(pJson);
				yToJson(pJson);
				wToJson(pJson);
				hToJson(pJson);
				return pJson;
			}
			virtual bool FromJson(JSON* pJson)
			{
				return xFromJson(pJson) &&
					yFromJson(pJson) &&
					wFromJson(pJson) &&
					hFromJson(pJson);
			}
			CLASS_MEMBER_J(int, m_i, x);
			CLASS_MEMBER_J(int, m_i, y);
			CLASS_MEMBER_J(int, m_i, w);
			CLASS_MEMBER_J(int, m_i, h);
		};

		class AtlasSprite :public MojingJsonElement
		{
		public:
			AtlasSprite() {};
			virtual ~AtlasSprite(){};
			virtual JSON* ToJson()
			{
				JSON* pJson = JSON::CreateObject();
				// TODO:
				return pJson;
			}
			virtual bool FromJson(JSON* pJson)
			{
				JSON* pFrameNode = pJson->GetItemByName("frame");
				if (pFrameNode && pFrameNode->Type == JSON_Object)
				{
					return filenameFromJson(pJson) && m_Frame.FromJson(pFrameNode);
					
				}
				return true;
			}
			CLASS_MEMBER_STR_J(String, m_sz, filename);
			const AtlasRect GetFrame()const{ return m_Frame; };
		private:
			AtlasRect m_Frame;

		};

		class AtlasMeta : public MojingJsonElement
		{
		public:
			AtlasMeta() {};
			virtual ~AtlasMeta(){};

			virtual JSON* ToJson()
			{
				JSON* pJson = JSON::CreateObject();
				// TODO:
				return pJson;
			}

			virtual bool FromJson(JSON* pJson)
			{
				JSON* pSizeNode = pJson->GetItemByName("size");
				if (pSizeNode->Type == JSON_Object)
				{
					return m_Size.FromJson(pSizeNode);

				}

				return false;
			}
			const AtlasSize & GetSize()const{ return m_Size; };
		private:
			AtlasSize m_Size;
		};

		class AtlasProfile
		{
		public:
			std::map<String, Vector4f> spriteUVs;
			int atlasWidth;
			int atlasHeight;

			bool Update(const char* buffer)
			{
				const char* szError = nullptr;
				JSON*  pJsonRoot = JSON::Parse(buffer, &szError);
				if (pJsonRoot != nullptr)
				{
					
					JSON* pJsonMeta = pJsonRoot->GetItemByName("meta");
					if (pJsonMeta != nullptr)
					{
						AtlasMeta* pAtlasMeta = new AtlasMeta();
						pAtlasMeta->FromJson(pJsonMeta);
						atlasWidth = pAtlasMeta->GetSize().Getw();
						atlasHeight = pAtlasMeta->GetSize().Geth();
						delete pAtlasMeta;
					}
					else
					{
						return false;
					}
					// Sprite array in atlas
					JSON* pJsonArray = pJsonRoot->GetItemByName("frames");
					if ((pJsonArray != nullptr) &&
						(pJsonArray->Type == JSON_Array) &&
						pJsonArray->GetArraySize()
						)
					{
						int size = pJsonArray->GetArraySize();

						for (int i = 0; i < size; i++)
						{
							JSON* item = pJsonArray->GetItemByIndex(i);
							AtlasSprite* pAtlasSprite = new AtlasSprite;
							if (pAtlasSprite->FromJson(item))
							{
								int spriteWidth = pAtlasSprite->GetFrame().Getw();
								int spriteHeight = pAtlasSprite->GetFrame().Geth();
								int iU = pAtlasSprite->GetFrame().Getx();
								int iV = pAtlasSprite->GetFrame().Gety();
								float fU0 = (float)iU / (float)atlasWidth;
								float fV0 = (float)iV / (float)atlasHeight;
								float fU1 = (float)(iU + spriteWidth) / (float)atlasWidth;
								float fV1 = (float)(iV + spriteHeight) / (float)atlasHeight;

								spriteUVs.insert({ String(pAtlasSprite->Getfilename()), Vector4f(fU0, fV0, fU1, fV1) });

								delete pAtlasSprite;
							}
							else
							{
								delete pAtlasSprite;
								return false;
							}
						}
					}
				}
				return true;
			}
		}; // AtlasProfile

	}
}
