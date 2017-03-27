#include "RectangleModel.h"
#include "../../Profile/AtlasProfile.h"
#include "../../Base/MojingLog.h"

#include <algorithm>
#include <utility>

namespace Baofeng
{
	namespace Mojing
	{
		RectangleModel::RectangleModel(int textureType, const char* atlasJson) : Model(textureType),
			m_v3TopLeft(-1 , 1 , -1),
			m_v3TopRight(1, 1, -1),
			m_v3BottomLeft(-1, -1, -1),
			m_v3BottomRight(1, -1, -1)
		{
			SetClassName(__FUNCTION__);

			AtlasProfile atlas;
			LOGE("atlasJson: %s", atlasJson);

			atlas.Update(atlasJson);
			LOGE("map size: %d", atlas.spriteUVs.size());
			SetSpriteUVs(atlas.spriteUVs);
		}
		RectangleModel::~RectangleModel()
		{

		}

		void RectangleModel::Init()
		{
			prepareData();
			Setup();
		}

		void RectangleModel::prepareData()
		{
			data.vertexShader =
				"#version 300 es\n"

				"uniform mat4 modelViewProjection;\n"
				"uniform mat4 model;\n"

				"in vec4 vertexPosition;\n"
				"in vec3 vertexNormal;\n"
				"in vec2 vertexUV;\n"
				"out vec2 textureCoord;\n"

				"void main()\n"
				"{\n"
				"   gl_Position = modelViewProjection * vertexPosition;\n"
				"   textureCoord = vertexUV;\n"
				"}\n";

			data.fragmentShader = 
				"#version 300 es\n"
				"precision mediump float;\n"
				"uniform sampler2D sTexture;\n"
				"uniform int layerIndex;\n"
				"in vec2 textureCoord;\n"
				"out vec4 finalColor;\n"
				"void main()\n"
				"{\n"
				"   finalColor = texture(sTexture, textureCoord);\n"
				"}\n";

			data.vertices = {
				-7.0f, 1.0f, -10.0f,
				-7.0f, -1.0f, -10.0f,
				// USB
				-5.0f, 1.0f, -10.0f,
				-5.0f, -1.0f, -10.0f,
				-5.0f, 1.0f, -10.0f,
				-5.0f, -1.0f, -10.0f,
				// Joystick
				-3.0f, 1.0f, -10.0f,
				-3.0f, -1.0f, -10.0f,
				-3.0f, 1.0f, -10.0f,
				-3.0f, -1.0f, -10.0f,
				// bluetooth
				-1.0f, 1.0f, -10.0f,
				-1.0f, -1.0f, -10.0f,
				-1.0f, 1.0f, -10.0f,
				-1.0f, -1.0f, -10.0f,
				// wifi
				1.0f, 1.0f, -10.0f,
				1.0f, -1.0f, -10.0f,
				1.0f, 1.0f, -10.0f,
				1.0f, -1.0f, -10.0f,
				// time
				
				1.4f, 1.0f, -10.0f,
				1.4f, -1.0f, -10.0f,
				1.4f, 1.0f, -10.0f,
				1.4f, -1.0f, -10.0f, // ten hour
				1.8f, 1.0f, -10.0f,
				1.8f, -1.0f, -10.0f,
				1.8f, 1.0f, -10.0f,
				1.8f, -1.0f, -10.0f, // hour
				2.2f, 1.0f, -10.0f,
				2.2f, -1.0f, -10.0f,
				2.2f, 1.0f, -10.0f,
				2.2f, -1.0f, -10.0f, // :
				2.6f, 1.0f, -10.0f,
				2.6f, -1.0f, -10.0f,
				2.6f, 1.0f, -10.0f,
				2.6f, -1.0f, -10.0f, // ten minutes
				3.0f, 1.0f, -10.0f,
				3.0f, -1.0f, -10.0f,
				3.0f, 1.0f, -10.0f,
				3.0f, -1.0f, -10.0f, // minutes
				// batteray
				5.0f, 1.0f, -10.0f,
				5.0f, -1.0f, -10.0f,
				5.0f, 1.0f, -10.0f,
				5.0f, -1.0f, -10.0f,
				// batteray status
				5.5f, 1.0f, -10.0f,
				5.5f, -1.0f, -10.0f,
				5.5f, 1.0f, -10.0f,
				5.5f, -1.0f, -10.0f, // number
				6.0f, 1.0f, -10.0f,
				6.0f, -1.0f, -10.0f,
				6.0f, 1.0f, -10.0f,
				6.0f, -1.0f, -10.0f, // number
				6.5f, 1.0f, -10.0f, // %
				6.5f, -1.0f, -10.0f,
			};

			data.indices = {
				1, 2, 0,
				1, 3, 2,
				3, 4, 2,
				3, 5, 4,
				5, 6, 4,
				5, 7, 6,
				7, 8, 6,
				7, 9, 8,
				9, 10, 8,
				9, 11, 10,
				11, 12, 10,
				11, 13, 12,
				13, 14, 12,
				13, 15, 14,
				15, 16, 14,
				15, 17, 16,
				17, 18, 16,
				17, 19, 18,
				19, 20, 18,
				19, 21, 20,
				21, 22, 20,
				21, 23, 22,
				23, 24, 22,
				23, 25, 24,
				25, 26, 24,
				25, 27, 26,
				27, 28, 26,
				27, 29, 28,
				29, 30, 28,
				29, 31, 30,
				31, 32, 30,
				31, 33, 32,
				33, 34, 32,
				33, 35, 34,
				35, 36, 34,
				35, 37, 36,
				37, 38, 36,
				37, 39, 38,
				39, 40, 38,
				39, 41, 40,
				41, 42, 40,
				41, 43, 42,
				43, 44, 42,
				43, 45, 44,
				45, 46, 44,
				45, 47, 46,
				47, 48, 46,
				47, 49, 48,
				49, 50, 48,
				49, 51, 50,
			};

			std::vector<float> tempUVs;

			// all sprites in settings
			const int SPRITE_COUNT = 13;

			const char* sprite_names[SPRITE_COUNT] = {
				"111_usb_06.png",
				"222_yaokong_06.png",
				"333_lanya_06.png",
				"444_wifi1_13.png",
				// time
				"1.png",
				"8.png",
				"555_colon.png",
				"0.png",
				"0.png",
				// battery
				"666_dc1_16.png",
				// battery statue
				"9.png",
				"8.png",
				"777_percent.png",
			};

			for (int i = 0; i < SPRITE_COUNT; i++)
			{
				/* we need flip the UV upside down, because the bitmap was loaded upside down */

				// left down
				data.uvs.push_back(m_SpriteUVs[sprite_names[i]].x);
				data.uvs.push_back(m_SpriteUVs[sprite_names[i]].y);
				// left up
				data.uvs.push_back(m_SpriteUVs[sprite_names[i]].x);
				data.uvs.push_back(m_SpriteUVs[sprite_names[i]].w);
				
				// right down
				data.uvs.push_back(m_SpriteUVs[sprite_names[i]].z);
				data.uvs.push_back(m_SpriteUVs[sprite_names[i]].y);
				// right up
				data.uvs.push_back(m_SpriteUVs[sprite_names[i]].z);
				data.uvs.push_back(m_SpriteUVs[sprite_names[i]].w);
			}
			
		}
	}
}
