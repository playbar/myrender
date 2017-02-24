#ifndef SPHERE_MODEL_H
#define SPHERE_MODEL_H

#include "Model.h"
namespace Baofeng
{
	namespace Mojing
	{

		class SphereModel : public Model
		{
		public:
			SphereModel(int modelType, int textureType);
			virtual void Init() ;
			virtual ~SphereModel();
		protected:
			/*
			0: skybox
			1: sphere
			2: sphere 上下分屏
			3: sphere 左右分屏
			*/
			int _modelType = 0;

			const double NV_PI = 3.14159265358979323846;
			void prepareData() override;
			void createSphere(float radius);
		};
	}
}
#endif /* SPHERE_MODEL_H */