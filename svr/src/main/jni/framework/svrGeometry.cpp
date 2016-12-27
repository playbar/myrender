//=============================================================================
// FILE: svrGeometry.cpp
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================
#include <GLES3/gl3.h>

#include "tiny_obj_loader.h"
#include <vector>
#include <string>

#include "svrGeometry.h"
#include "svrShader.h"
#include "svrUtil.h"

namespace Svr
{

SvrGeometry::SvrGeometry()
    : mVbId(0)
    , mIbId(0)
    , mVaoId(0)
    , mVertexCount(0)
    , mIndexCount(0)
{

}

void SvrGeometry::Initialize(SvrProgramAttribute* pAttribs, int nAttribs,
                             unsigned int* pIndices, int nIndices,
                             const void* pVertexData, int bufferSize, int nVertices)
{
    //Create the VBO
    GL(glGenBuffers( 1, &mVbId));
    GL(glBindBuffer( GL_ARRAY_BUFFER, mVbId ));
	GL(glBufferData( GL_ARRAY_BUFFER, bufferSize, pVertexData, GL_STATIC_DRAW));
	GL(glBindBuffer( GL_ARRAY_BUFFER, 0));

    //Create the Index Buffer
	GL(glGenBuffers( 1, &mIbId));
	GL(glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIbId ));
	GL(glBufferData( GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(unsigned int), pIndices, GL_STATIC_DRAW));
	GL(glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0));

    //Create the VAO
    GL(glGenVertexArrays( 1, &mVaoId ));

	GL(glBindVertexArray( mVaoId ));

	GL(glBindBuffer( GL_ARRAY_BUFFER, mVbId ));

	for ( int i = 0; i < nAttribs; i++ )
	{
        GL(glEnableVertexAttribArray( pAttribs[i].index ));
		GL(glVertexAttribPointer(pAttribs[i].index, pAttribs[i].size,
			pAttribs[i].type, pAttribs[i].normalized,
			pAttribs[i].stride, (void*)pAttribs[i].offset));
	}

	GL(glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, mIbId ));

	GL(glBindVertexArray( 0 ));

    mVertexCount = nVertices;
    mIndexCount = nIndices;
}

void SvrGeometry::Destroy()
{
    GL(glDeleteBuffers( 1, &mIbId ));
	GL(glDeleteBuffers( 1, &mVbId ));
}

void SvrGeometry::Submit()
{
    GL( glBindVertexArray( mVaoId ) );
    GL( glDrawElements(GL_TRIANGLES, mIndexCount, GL_UNSIGNED_INT, NULL) );
    GL( glBindVertexArray( 0 ) );
}

void SvrGeometry::CreateFromObjFile(const char* pObjFilePath, SvrGeometry** pOutGeometry, int& outNumGeometry)
{
    std::vector<tinyobj::shape_t>       shapes;
    std::vector<tinyobj::material_t>    materials;
    std::string err = tinyobj::LoadObj(shapes, materials, pObjFilePath, NULL);
    if (!err.empty())
    {
        LOGE("CreateFromObjFile : %s", err.c_str());
        *pOutGeometry = NULL;
        outNumGeometry = 0;
        return;
    }

    SvrProgramAttribute attribs[3];

    *pOutGeometry = new SvrGeometry[shapes.size()];
    LOGV("Found %d shapes", shapes.size());

    for (size_t i = 0; i < shapes.size(); i++)
    {
        int vertexSize = 0;
        int nAttribs = 0;
       
        if (shapes[i].mesh.positions.size() == 0 ||
            shapes[i].mesh.normals.size() == 0)
        {
            LOGE("OBJ must contain at least positions and normals");
            *pOutGeometry = NULL;
            outNumGeometry = 0;
            return;
        }

        nAttribs = 3;
        vertexSize = 8 * sizeof(float);

        attribs[0].index = kPosition;
        attribs[0].size = 3;
        attribs[0].type = GL_FLOAT;
        attribs[0].normalized = false;
        attribs[0].stride = vertexSize;
        attribs[0].offset = 0;

        attribs[1].index = kNormal;
        attribs[1].size = 3;
        attribs[1].type = GL_FLOAT;
        attribs[1].normalized = false;
        attribs[1].stride = vertexSize;
        attribs[1].offset = 3 * sizeof(float);

		attribs[2].index = kTexcoord0;
		attribs[2].size = 2;
		attribs[2].type = GL_FLOAT;
		attribs[2].normalized = false;
		attribs[2].stride = vertexSize;
		attribs[2].offset = 6 * sizeof(float);

        float* pVbData = new float[vertexSize * shapes[i].mesh.positions.size()];
        int vbIndex = 0;
        for (size_t j = 0; j < shapes[i].mesh.positions.size() / 3; j++)
        {
            pVbData[vbIndex++] = shapes[i].mesh.positions[3 * j + 0];
            pVbData[vbIndex++] = shapes[i].mesh.positions[3 * j + 1];
            pVbData[vbIndex++] = shapes[i].mesh.positions[3 * j + 2];

            pVbData[vbIndex++] = shapes[i].mesh.normals[3 * j + 0];
            pVbData[vbIndex++] = shapes[i].mesh.normals[3 * j + 1];
            pVbData[vbIndex++] = shapes[i].mesh.normals[3 * j + 2];

			pVbData[vbIndex++] = shapes[i].mesh.texcoords[2 * j + 0];
			pVbData[vbIndex++] = shapes[i].mesh.texcoords[2 * j + 1];
        }
      
        pOutGeometry[i]->Initialize(&attribs[0], nAttribs,
            &shapes[i].mesh.indices[0], shapes[i].mesh.indices.size(),
            (const void*)pVbData, vertexSize * shapes[i].mesh.positions.size(), shapes[i].mesh.positions.size());

        LOGV("OBJ Geom Initialized, idx count:%d, pos count:%d", shapes[i].mesh.indices.size(), shapes[i].mesh.positions.size());
        delete[] pVbData;
    }

    outNumGeometry = shapes.size();
}

}
