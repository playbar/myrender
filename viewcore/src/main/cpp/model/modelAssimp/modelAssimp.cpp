/*
 *    Copyright 2016 Anand Muralidhar
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "../common/myShader.h"
#include "modelAssimp.h"


//#include "assimp/Importer.hpp"
//#include <opencv2/opencv.hpp>
//#include "../common/myJNIHelper.h"

/**
 * Class constructor
 */
ModelAssimp::ModelAssimp() {

    MyLOGD("ModelAssimp::ModelAssimp");

    modelObject = NULL;
}

ModelAssimp::~ModelAssimp() {

    MyLOGD("ModelAssimp::ModelAssimpssimp");
    if (modelObject) {
        delete modelObject;
    }
}

void ModelAssimp::Render(float* mvpMatrix) {
    glm::mat4 mvpMat = glm::mat4(1.0f);
    if( mvpMatrix != NULL) {
        mvpMat = glm::mat4(mvpMatrix[0], mvpMatrix[1], mvpMatrix[2], mvpMatrix[3],
                           mvpMatrix[4], mvpMatrix[5], mvpMatrix[6], mvpMatrix[7],
                           mvpMatrix[8], mvpMatrix[9], mvpMatrix[10], mvpMatrix[11],
                           mvpMatrix[12], mvpMatrix[13], mvpMatrix[14], mvpMatrix[15]);
    }

    if(modelObject != NULL)
    {
        modelObject->Render3DModel(&mvpMat);
//        MyLOGI("modelAssimp", "Render 3D Model");
    }

    CheckGLError("ModelAssimp::Render");
}

void ModelAssimp::LoadModel(const char * modelName) {

    MyLOGD("ModelAssimp::LoadModel %s", modelName);

    modelObject = new AssimpLoader();
    modelObject->Load3DModel(modelName);

    MyLOGE("ModelAssimp::LoadModel Done");
}

