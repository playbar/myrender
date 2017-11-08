
#ifndef SKYBOX_MODEL_H
#define SKYBOX_MODEL_H

#include "Model.h"

namespace Baofeng
{
    namespace Mojing
    {
        class SkyboxModel : public Model
        {
        public:
			SkyboxModel(int textureType);
			virtual void Init() ;
            virtual ~SkyboxModel();
        protected:
            void prepareData() override;
		};
    }
}


#endif /* CUBE_MODEL_H */