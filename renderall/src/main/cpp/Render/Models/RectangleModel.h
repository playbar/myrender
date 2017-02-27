#ifndef MOJING_RENDER_MODEL_RECTANGLE
#define MOJING_RENDER_MODEL_RECTANGLE

#include "Model.h"
#include <map>

namespace Baofeng
{
	namespace Mojing
	{
		class RectangleModel : public Model
		{
			typedef std::map<String, Vector4f> SpriteUV_t;
		public:
			RectangleModel(int textureType, const char* atlasJson);
			virtual ~RectangleModel();

			virtual void Init();
			CLASS_MEMBER(Vector3f, m_v3, TopLeft);
			CLASS_MEMBER(Vector3f, m_v3, BottomRight);
			CLASS_MEMBER(Vector3f, m_v3, TopRight);
			CLASS_MEMBER(Vector3f, m_v3, BottomLeft);
			CLASS_MEMBER(SpriteUV_t, m_, SpriteUVs);
		protected:
			virtual void prepareData();

		};

	}
}

#endif // MOJING_RENDER_MODEL_RECTANGLE