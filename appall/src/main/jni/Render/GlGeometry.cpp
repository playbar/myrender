
#include "GlGeometry.h"
#include "MojingRenderBase.h"
#include "../MojingSDKStatus.h"
#include "../MojingManager.h"
#include "../Distortion/MojingDistortion.h"

namespace Baofeng
{
#define CELLS_COUNT 32
	namespace Mojing
	{

		/*======================================================

		类: GlGeometry

		=======================================================*/
		GlGeometry::GlGeometry() :
			m_VertexBuffer(0),
			m_IndexBuffer(0),
			m_VertexArrayObject(0),
			m_VertexCount(0),
			m_IndexCount(0)
		{}

		GlGeometry::~GlGeometry()
		{
			ClearBuffers();
		}

	
		/*------------------------------------------------------------------
		2015/08/17: 绑定缓冲区
		------------------------------------------------------------------*/
		void GlGeometry::BindVBO()
		{
			const int attribCount = 10;
			glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_POSITION);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_POSITION, 2, GL_FLOAT, false, attribCount * sizeof(float), (void *)(0 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_R);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_R, 2, GL_FLOAT, false, attribCount * sizeof(float), (void *)(2 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_G);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_G, 2, GL_FLOAT, false, attribCount * sizeof(float), (void *)(4 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_B);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_B, 2, GL_FLOAT, false, attribCount * sizeof(float), (void *)(6 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_SLICE_INFO);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_SLICE_INFO, 2, GL_FLOAT, false, attribCount * sizeof(float), (void *)(8 * sizeof(float)));

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		}

		/*------------------------------------------------------------------
		2015/08/17: 解除绑定缓冲区
		------------------------------------------------------------------*/
		void GlGeometry::UnBindVBO()
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		/*------------------------------------------------------------------
		2015/08/17: 清理缓冲区
		------------------------------------------------------------------*/
		void GlGeometry::ClearBuffers()
		{
			//============================ 释放顶点 ============================//

			if (m_VertexBuffer != 0 && glIsBuffer(m_VertexBuffer))
			{
				glDeleteBuffers(1, &m_VertexBuffer);
			}
			m_VertexBuffer = 0;
			m_VertexCount = 0;

			//============================ 释放索引 ============================//

			if (m_IndexBuffer != 0 && glIsBuffer(m_IndexBuffer))
			{
				glDeleteBuffers(1, &m_IndexBuffer);
			}
			m_IndexBuffer = 0;
			m_IndexCount = 0;

			m_VertexArrayObject = 0;
		}

		/*------------------------------------------------------------------
		2015/08/17: 判断缓冲区是否已经成功建立
		------------------------------------------------------------------*/
		bool GlGeometry::IsAvailable()
		{
			return m_VertexBuffer != 0 && glIsBuffer(m_VertexBuffer);
		}

		bool GlGeometry::BuildGeometry(int iWidth /*= 0*/, int iHeight /*= 0*/, void *pBuffer /*= NULL*/)
		{
			ClearBuffers();
			return true;
		}

		/*======================================================

		类: GlGeometryTriangles

		=======================================================*/
		/*------------------------------------------------------------------
		2015/08/17:  绘制图元 参数：左右眼
		------------------------------------------------------------------*/
		void GlGeometryTriangles::DrawElements(int eye)
		{
			const int indexCount = m_IndexCount / 2;
			const int indexOffset = eye * indexCount;
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, (void*)(indexOffset * 2));
		}
		void GlGeometryTriangles::DrawElementsRange(int eye)
		{
			const int indexCount = m_IndexCount / 2;
			const int indexOffset = eye * indexCount;
			glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_SHORT, (void*)(indexOffset * 2));
		}
		/*------------------------------------------------------------------
		2015/08/17: 建立顶点与索引缓冲区
		------------------------------------------------------------------*/

		bool GlGeometryTriangles::BuildGeometry(int iWidth /*= 0*/, int iHeight /*= 0*/ , void *pBuffer /* = NULL*/)
		{
			GlGeometry::BuildGeometry(0, 0); // just ClearBuffer

			const int NUM_SLICES_PER_EYE = 1;
			const float fovScale = 1.0f;
			bool bNeedFreeBuffer = false;
			if (pBuffer == NULL)
			{
				bNeedFreeBuffer = true;
#if USING_NEW_MESH
				Mesh_820 mesh;
				pBuffer = Manager::GetMojingManager()->GetDistortion()->BuildDistortionBuffer_V2(mesh, CELLS_COUNT, CELLS_COUNT);
#else
				pBuffer = (iWidth == 0 && iHeight == 0) ? Manager::GetMojingManager()->GetDistortion()->BuildDistortionBuffer(CELLS_COUNT, CELLS_COUNT) :
					Manager::GetMojingManager()->GetDistortion()->BuildDistortionBufferOverlay(CELLS_COUNT, CELLS_COUNT, iWidth, iHeight);
#endif
			}


			//const int magic = ((int *)pBuffer)[0];
			const int tesselationsX = ((int *)pBuffer)[1];
			const int tesselationsY = ((int *)pBuffer)[2];

			//const int vertexBytes = 12 + 2 * (tesselationsX + 1) * (tesselationsY + 1) * 6 * sizeof(float);
			const float * bufferVerts = &((float *)pBuffer)[3];

			const int attribCount = 10;
			//const int sliceTess = tesselationsX / NUM_SLICES_PER_EYE;

			this->m_VertexCount = 2 * NUM_SLICES_PER_EYE*(tesselationsX + 1)*(tesselationsY + 1);
			const int floatCount = m_VertexCount * attribCount;
			float* pTessVertices = new float[floatCount];
			//  0   1   2   3   4   5   6   7   8   9   
			// [X   Y   Rx  Ry  Gx  Gy  Bx  By  A   0]
			int	verts = 0;
			for (int eye = 0; eye < 2; eye++)
			{
				//for (int slice = 0; slice < NUM_SLICES_PER_EYE; slice++)
				//{
				//	LOGE("--madi-- slice = %d", slice);
					const int vertBase = verts;
					for (int y = 0; y <= tesselationsY; y++)
					{
						const float	yf = (float)y / (float)tesselationsY;
						for (int x = 0; x <= tesselationsX; x++)
						{
							const float	xf = (float)x / (float)tesselationsX;
							float * v = &pTessVertices[attribCount * (vertBase + y * (tesselationsX + 1) + x)];
#if USING_NEW_MESH
							const float * vSrc = bufferVerts+((y*(tesselationsX + 1) * 2 + x + eye * (tesselationsX + 1)) * UNREAL_DISTORTION_PARAMETES_COUNT );

							v[0] = vSrc[7];
							v[1] = vSrc[8];

							v[2] = vSrc[0];
							v[3] = vSrc[1];

							v[4] = vSrc[2];
							v[5] = vSrc[3];

							v[6] = vSrc[4];
							v[7] = vSrc[5];

							v[8] = (float)x / tesselationsX;
							v[9] = 1;

							if (eye == 0)
							{
								m_VertexLeft_Qualcomm.push_back(v[0]);
								m_VertexLeft_Qualcomm.push_back(v[1]); // x,y

								m_VertexLeft_Qualcomm.push_back(v[2]);
								m_VertexLeft_Qualcomm.push_back(v[3]); // R

								m_VertexLeft_Qualcomm.push_back(v[4]);
								m_VertexLeft_Qualcomm.push_back(v[5]); // G

								m_VertexLeft_Qualcomm.push_back(v[6]);
								m_VertexLeft_Qualcomm.push_back(v[7]); // B

								//LOGE("left vertex[%d]: %f,%f,%f,%f,%f,%f,%f,%f", vertNum, v[7], v[8], v[0], v[1], v[2], v[3], v[4], v[5]);
							}
							else
							{
								m_VertexRight_Qualcomm.push_back(v[0]);
								m_VertexRight_Qualcomm.push_back(v[1]); // x,y

								m_VertexRight_Qualcomm.push_back(v[2]);
								m_VertexRight_Qualcomm.push_back(v[3]); // R

								m_VertexRight_Qualcomm.push_back(v[4]);
								m_VertexRight_Qualcomm.push_back(v[5]); // G

								m_VertexRight_Qualcomm.push_back(v[6]);
								m_VertexRight_Qualcomm.push_back(v[7]); // B

								//LOGE("right vertex[%d]: %f,%f,%f,%f,%f,%f,%f,%f", vertNum, v[7], v[8], v[0], v[1], v[2], v[3], v[4], v[5]);
						}

#else
							v[0] = -1.0 + eye + xf;	// 目标双眼[-1,+1]区间的X坐标,左眼固定于[-1 ， 0]，右眼固定于[0 ， 1]
							v[1] = yf*2.0f - 1.0f;	// 目标双眼[-1,+1]区间的Y坐标

							// Copy the offsets from the file
							// 下标2到7依次为Rx,Ry,Gx,Gy,Bx,By
							for (int i = 0; i < 6; i++)
							{
								v[2 + i] = fovScale * bufferVerts
									[(y*(tesselationsX + 1) * 2 + sx + eye * (tesselationsX + 1)) * DISTORTION_PARAMETES_COUNT + i];
							}

							// 下标8是X坐标在条带内的位置比值，越靠近条带右边界数值越大，越靠近条带左边界数值越小
							v[8] = (float)x / sliceTess;
							// Enable this to allow fading at the edges.
							// Samsung recommends not doing this, because it could cause
							// visible differences in pixel wear on the screen over long
							// periods of time.

							// 下标9恒定为1
							if (0 && (y == 0 || y == tesselationsY || sx == 0 || sx == tesselationsX))
							{
								v[9] = 0.0f;	// fade to black at edge
							}
							else
							{
								//v[9] = 1.0f;
								v[9] = bufferVerts[(y*(tesselationsX + 1) * 2 + sx + eye * (tesselationsX + 1)) * DISTORTION_PARAMETES_COUNT + 6];
							}
#endif
						}
					}
					verts += (tesselationsY + 1)*(tesselationsX + 1);
				//}
			}
			if (bNeedFreeBuffer)
				free(pBuffer);

			m_IndexCount = 2 * tesselationsX * tesselationsY * 6;
			unsigned short* pTessIndices = new unsigned short[m_IndexCount];

			int	index = 0;
			verts = 0;
			for (int eye = 0; eye < 2; eye++)
			{
				//for (int slice = 0; slice < NUM_SLICES_PER_EYE; slice++)
				//{
					const int vertBase = verts;
					// The order of triangles doesn't matter for tiled rendering,
					// but when we can do direct rendering to the screen, we want the
					// order to follow the raster order to minimize the chance
					// of tear lines.
					//
					// This can be checked by quartering the number of indexes, and
					// making sure that the drawn pixels are the first pixels that
					// the raster will scan.
					// 下面的代码用于绘制各个矩形，使用六个顶点将一个矩形切分成两个三角形；
					// 并且分别采用了从左上到右下和从右上到左下两种分割矩形的方式分割不同象限中的矩形。
					// 以下注释所描述的三角形方向和象限均是以左上角为原点坐标
					for (int y = 0; y < tesselationsY; y++)
					{
						for (int x = 0; x < tesselationsX; x++)
						{
							// flip the triangulation in opposite corners
							if (( x < tesselationsX / 2) ^ (y < (tesselationsY / 2)))
							{
								// 第一三象限，斜边从左上角到右下角,0与3、2与4重叠
								/*
								03 - - -1
								| \     |
								|   \   |
								|     \ |
								5 - - - 24
								*/
								pTessIndices[index + 0] = vertBase + y * (tesselationsX + 1) + x;
								pTessIndices[index + 1] = vertBase + y * (tesselationsX + 1) + x + 1;
								pTessIndices[index + 2] = vertBase + (y + 1) * (tesselationsX + 1) + x + 1;

								pTessIndices[index + 3] = vertBase + y * (tesselationsX + 1) + x;
								pTessIndices[index + 4] = vertBase + (y + 1) * (tesselationsX + 1) + x + 1;
								pTessIndices[index + 5] = vertBase + (y + 1) * (tesselationsX + 1) + x;
							}
							else
							{
								// 第二四象限，斜边从右上角到左下角,1与4、2与3重叠
								/*
								0  - - -14
								|     / |
								|   /   |
								| /     |
								23- - - 5
								*/
								pTessIndices[index + 0] = vertBase + y * (tesselationsX + 1) + x;
								pTessIndices[index + 1] = vertBase + y * (tesselationsX + 1) + x + 1;
								pTessIndices[index + 2] = vertBase + (y + 1) * (tesselationsX + 1) + x;

								pTessIndices[index + 3] = vertBase + (y + 1) * (tesselationsX + 1) + x;
								pTessIndices[index + 4] = vertBase + y * (tesselationsX + 1) + x + 1;
								pTessIndices[index + 5] = vertBase + (y + 1) * (tesselationsX + 1) + x + 1;
							}

							if (eye == 0)
							{
								m_IndexLeft_Qualcomm.push_back(pTessIndices[index + 0]);
								m_IndexLeft_Qualcomm.push_back(pTessIndices[index + 1]);
								m_IndexLeft_Qualcomm.push_back(pTessIndices[index + 2]);
								m_IndexLeft_Qualcomm.push_back(pTessIndices[index + 3]);
								m_IndexLeft_Qualcomm.push_back(pTessIndices[index + 4]);
								m_IndexLeft_Qualcomm.push_back(pTessIndices[index + 5]);
							}
							else
							{
								m_IndexRight_Qualcomm.push_back(pTessIndices[index + 0]);
								m_IndexRight_Qualcomm.push_back(pTessIndices[index + 1]);
								m_IndexRight_Qualcomm.push_back(pTessIndices[index + 2]);
								m_IndexRight_Qualcomm.push_back(pTessIndices[index + 3]);
								m_IndexRight_Qualcomm.push_back(pTessIndices[index + 4]);
								m_IndexRight_Qualcomm.push_back(pTessIndices[index + 5]);
							}
							index += 6;
						}
					}
					verts += (tesselationsY + 1)*(tesselationsX + 1);
				//}
			}

			if (0 == this->m_VertexBuffer || !glIsBuffer(this->m_VertexBuffer))
			{
				glGenBuffers(1, &this->m_VertexBuffer);
			}
			glBindBuffer(GL_ARRAY_BUFFER, this->m_VertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, floatCount * sizeof(*pTessVertices), (void*)pTessVertices, GL_STATIC_DRAW);
			delete[] pTessVertices;

			if (0 == m_IndexBuffer || !glIsBuffer(m_IndexBuffer))
			{
				glGenBuffers(1, &m_IndexBuffer);
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexCount * sizeof(*pTessIndices), (void*)pTessIndices, GL_STATIC_DRAW);
			delete[] pTessIndices;

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_POSITION);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_POSITION, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(0 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_R);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_R, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(2 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_G);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_G, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(4 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_B);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_B, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(6 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_SLICE_INFO);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_SLICE_INFO, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(8 * sizeof(float)));

			return true;
		}

		/*======================================================

		类: GlGeometryTriangleStrip

		=======================================================*/
		GlGeometryTriangleStrip::GlGeometryTriangleStrip() : GlGeometry(),
			m_TesselationsX(32), m_TesselationsY(32)
		{}

		/*------------------------------------------------------------------
		2015/08/17: 建立顶点与索引缓冲区
		------------------------------------------------------------------*/
		bool GlGeometryTriangleStrip::BuildGeometry(int iWidth /*= 0*/, int iHeight /*= 0*/, void *pBuffer /*= NULL*/)
		{
			GlGeometry::BuildGeometry(iWidth /*= 0*/, iHeight /*= 0*/);

			const int NUM_SLICES_PER_EYE = 1;
			const float fovScale = 1.0f;
			bool bNeedFreeBuffer = false;
			if (pBuffer == NULL)
			{
				bNeedFreeBuffer = true;
				pBuffer = (iWidth == 0 && iHeight == 0) ? Manager::GetMojingManager()->GetDistortion()->BuildDistortionBuffer(CELLS_COUNT, CELLS_COUNT) :
					Manager::GetMojingManager()->GetDistortion()->BuildDistortionBufferOverlay(CELLS_COUNT, CELLS_COUNT, iWidth, iHeight);
			}

			const int magic = ((int *)pBuffer)[0];
			const int tesselationsX = ((int *)pBuffer)[1];
			const int tesselationsY = ((int *)pBuffer)[2];

			//============================ 计算顶点数据 ============================//

			const int vertexBytes = 12 + 2 * (tesselationsX + 1) * (tesselationsY + 1) * 6 * sizeof(float);
			const float * bufferVerts = &((float *)pBuffer)[3];

			const int attribCount = 10;
			const int sliceTess = tesselationsX / NUM_SLICES_PER_EYE;

			this->m_VertexCount = 2 * NUM_SLICES_PER_EYE*(sliceTess + 1)*(tesselationsY + 1);
			const int floatCount = m_VertexCount * attribCount;
			float* pTessVertices = new float[floatCount];

			int	verts = 0;
			for (int eye = 0; eye < 2; eye++)
			{
				for (int slice = 0; slice < NUM_SLICES_PER_EYE; slice++)
				{
					const int vertBase = verts;
					for (int y = 0; y <= tesselationsY; y++)
					{
						const float	yf = (float)y / (float)tesselationsY;
						for (int x = 0; x <= sliceTess; x++)
						{
							const int sx = slice * sliceTess + x;
							const float	xf = (float)sx / (float)tesselationsX;
							float * v = &pTessVertices[attribCount * (vertBase + y * (sliceTess + 1) + x)];
							v[0] = -1.0 + eye + xf;	// 目标双眼[-1,+1]区间的X坐标,左眼固定于[-1 ， 0]，右眼固定于[0 ， 1]
							v[1] = yf*2.0f - 1.0f;	// 目标双眼[-1,+1]区间的Y坐标

							// Copy the offsets from the file
							// 下标2到7依次为Rx,Ry,Gx,Gy,Bx,By
							for (int i = 0; i < 6; i++)
							{
								v[2 + i] = fovScale * bufferVerts
									[(y*(tesselationsX + 1) * 2 + sx + eye * (tesselationsX + 1)) * DISTORTION_PARAMETES_COUNT + i];
							}
							// 下标8是X坐标在条带内的位置比值，越靠近条带右边界数值越大，越靠近条带左边界数值越小
							v[8] = (float)x / sliceTess;
							// Enable this to allow fading at the edges.
							// Samsung recommends not doing this, because it could cause
							// visible differences in pixel wear on the screen over long
							// periods of time.

							// 下标9恒定为1
							if (0 && (y == 0 || y == tesselationsY || sx == 0 || sx == tesselationsX))
							{
								v[9] = 0.0f;	// fade to black at edge
							}
							else
							{
								//v[9] = 1.0f;
								v[9] = bufferVerts[(y*(tesselationsX + 1) * 2 + sx + eye * (tesselationsX + 1)) * DISTORTION_PARAMETES_COUNT + 6];
							}
						}
					}
					verts += (tesselationsY + 1)*(sliceTess + 1);
				}
			}
			if (bNeedFreeBuffer)
				free(pBuffer);

			//============================ 计算索引数据 ============================//

			m_IndexCount = 2/*Two eyes*/ * (tesselationsX + 1) * tesselationsY * 2/*Two points*/;
			unsigned short* pTessIndices = new unsigned short[m_IndexCount];

			int	index = 0;
			for (int eye = 0; eye < 2; eye++)
			{
				const int vertBase = ((tesselationsY + 1)*(sliceTess + 1)) * eye;
				for (int y = 0; y < tesselationsY; y++)
				{
					for (int x = 0; x <= sliceTess; x++)
					{
						pTessIndices[index + 0] = vertBase + y * (sliceTess + 1) + x;
						pTessIndices[index + 1] = vertBase + (y + 1) * (sliceTess + 1) + x;
						index += 2;
					}
				}
			}

			//============================ 建立顶点缓冲 ============================//

			if (0 == m_VertexBuffer || !glIsBuffer(m_VertexBuffer))
			{
				glGenBuffers(1, &m_VertexBuffer);
			}
			glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, floatCount * sizeof(*pTessVertices), (void*)pTessVertices, GL_STATIC_DRAW);
			delete[] pTessVertices;

			//============================ 建立索引缓冲 ============================//

			if (0 == m_IndexBuffer || !glIsBuffer(m_IndexBuffer))
			{
				glGenBuffers(1, &m_IndexBuffer);
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexCount * sizeof(*pTessIndices), (void*)pTessIndices, GL_STATIC_DRAW);
			delete[] pTessIndices;

			//============================ 定位属性偏移 ============================//

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_POSITION);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_POSITION, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(0 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_R);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_R, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(2 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_G);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_G, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(4 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_B);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_B, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(6 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_SLICE_INFO);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_SLICE_INFO, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(8 * sizeof(float)));

			return true;
		}

		/*------------------------------------------------------------------
		2015/08/17: 绘制元素
		------------------------------------------------------------------*/
		void GlGeometryTriangleStrip::DrawElements(int eye)
		{
			const int indexCount = m_IndexCount / 2;
			const int indexOffset = eye * indexCount;
			const int nBiasUnit = (m_TesselationsX + 1) * 2;
			for (int i = 0; i < m_TesselationsY; i++)
			{
				glDrawElements(GL_TRIANGLE_STRIP, nBiasUnit,
					GL_UNSIGNED_SHORT, (void *)(indexOffset * 2 + nBiasUnit * i * 2));
			}
		}
		void GlGeometryTriangleStrip::DrawElementsRange(int eye)
		{
			const int indexCount = m_IndexCount / 2;
			const int indexOffset = eye * indexCount;
			glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_SHORT, (void*)(indexOffset * 2));
		}

		/*******************************************************************************
		//设置区域
		TopLeft------------
		|			  |
		|	LeftEye	  |Offset.y
		|			  |
		|--------------|
		Offset.x
		TopLeft:初始值（0，0，0），Offset初始值：（1， 1， 0）

		********************************************************************************/
#if 0
		void MojingRenderBase::SetPositionVector(Vector3f& TopLeft, Vector3f& Offset)
		{
			GLint view[4];
			glGetIntegerv(GL_VIEWPORT, view); //viewport
			if (TopLeft.x > 1.0 || TopLeft.x < 0.0)
			{
				TopLeft.x = 0.0;
			}
			if (TopLeft.y > 0.0 || TopLeft.y < 2.0)
			{
				TopLeft.y = 0.0;
			}
			if (m_Offset.x < 0.0 || m_Offset.x > 1.0)
			{
				m_Offset.x = 0.0;

			}
			if (m_Offset.y < 0.0 || m_Offset.y > 2.0)
			{
				m_Offset.y = 0.0;
			}
			m_Point = TopLeft + Vector3f(-1.0, 1.0, 0.0);
			m_Offset = Offset;

			m_QuadDraw.SetPositionVector(m_Point, m_Offset);
		}
#endif


		bool GlGeometryQuad::BuildGeometry(int iWidth/* = 0*/, int iHeight/* = 0*/, void *pBuffer /*= NULL*/)
		{
			GlGeometry::BuildGeometry(iWidth, iHeight);

			const int NUM_SLICES_PER_EYE = 1;
			const float fovScale = 1.0f;
			bool bNeedFreeBuffer = false;
			if (pBuffer == NULL)
			{
				bNeedFreeBuffer = true;
				pBuffer = (iWidth == 0 && iHeight == 0) ? Manager::GetMojingManager()->GetDistortion()->BuildDistortionBuffer(CELLS_COUNT, CELLS_COUNT) :
					Manager::GetMojingManager()->GetDistortion()->BuildDistortionBufferOverlay(CELLS_COUNT, CELLS_COUNT, iWidth, iHeight);
			}

			//const int magic = ((int *)pBuffer)[0];
			const int tesselationsX = ((int *)pBuffer)[1];
			const int tesselationsY = ((int *)pBuffer)[2];

			//const int vertexBytes = 12 + 2 * (tesselationsX + 1) * (tesselationsY + 1) * 6 * sizeof(float);
			const float * bufferVerts = &((float *)pBuffer)[3];

			const int attribCount = 10;
			const int sliceTess = tesselationsX / NUM_SLICES_PER_EYE;

			this->m_VertexCount = 2 * NUM_SLICES_PER_EYE*(sliceTess + 1)*(tesselationsY + 1);
			const int floatCount = m_VertexCount * attribCount;
			float* pTessVertices = new float[floatCount];

			int	verts = 0;
			for (int eye = 0; eye < 2; eye++)
			{
				for (int slice = 0; slice < NUM_SLICES_PER_EYE; slice++)
				{
					const int vertBase = verts;
					for (int y = 0; y <= tesselationsY; y++)
					{
						const float	yf = (float)y / (float)tesselationsY;
						for (int x = 0; x <= sliceTess; x++)
						{
							const int sx = slice * sliceTess + x;
							const float	xf = (float)sx / (float)tesselationsX;
							float * v = &pTessVertices[attribCount * (vertBase + y * (sliceTess + 1) + x)];
							v[0] = -1.0 + eye + xf;	// 目标双眼[-1,+1]区间的X坐标,左眼固定于[-1 ， 0]，右眼固定于[0 ， 1]
							v[1] = yf*2.0f - 1.0f;	// 目标双眼[-1,+1]区间的Y坐标

							// Copy the offsets from the file
							// 下标2到7依次为Rx,Ry,Gx,Gy,Bx,By
							for (int i = 0; i < 6; i++)
							{
								v[2 + i] = fovScale * bufferVerts
									[(y*(tesselationsX + 1) * 2 + sx + eye * (tesselationsX + 1)) * DISTORTION_PARAMETES_COUNT + i];
							}

							// 下标8是X坐标在条带内的位置比值，越靠近条带右边界数值越大，越靠近条带左边界数值越小
							v[8] = (float)x / sliceTess;
							// Enable this to allow fading at the edges.
							// Samsung recommends not doing this, because it could cause
							// visible differences in pixel wear on the screen over long
							// periods of time.

							// 下标9恒定为1
							if (0 && (y == 0 || y == tesselationsY || sx == 0 || sx == tesselationsX))
							{
								v[9] = 0.0f;	// fade to black at edge
							}
							else
							{
								//v[9] = 1.0f;
								v[9] = bufferVerts[(y*(tesselationsX + 1) * 2 + sx + eye * (tesselationsX + 1)) * DISTORTION_PARAMETES_COUNT + 6];
							}
						}
					}
					verts += (tesselationsY + 1)*(sliceTess + 1);
				}
			}
			if (bNeedFreeBuffer)
				free(pBuffer);

			m_IndexCount = 2 * tesselationsX * tesselationsY * 6;
			unsigned short* pTessIndices = new unsigned short[m_IndexCount];

			int	index = 0;
			verts = 0;
			for (int eye = 0; eye < 2; eye++)
			{
				for (int slice = 0; slice < NUM_SLICES_PER_EYE; slice++)
				{
					const int vertBase = verts;
					// The order of triangles doesn't matter for tiled rendering,
					// but when we can do direct rendering to the screen, we want the
					// order to follow the raster order to minimize the chance
					// of tear lines.
					//
					// This can be checked by quartering the number of indexes, and
					// making sure that the drawn pixels are the first pixels that
					// the raster will scan.
					// 下面的代码用于绘制各个矩形，使用六个顶点将一个矩形切分成两个三角形；
					// 并且分别采用了从左上到右下和从右上到左下两种分割矩形的方式分割不同象限中的矩形。
					// 以下注释所描述的三角形方向和象限均是以左上角为原点坐标
					for (int x = 0; x < sliceTess; x++)
					{
						for (int y = 0; y < tesselationsY; y++)
						{
							// flip the triangulation in opposite corners
							if ((slice*sliceTess + x < tesselationsX / 2) ^ (y < (tesselationsY / 2)))
							{
								// 第一三象限，斜边从左上角到右下角,0与3、2与4重叠
								/*
								03 - - -1
								| \     |
								|   \   |
								|     \ |
								5 - - - 24
								*/
								pTessIndices[index + 0] = vertBase + y * (sliceTess + 1) + x;
								pTessIndices[index + 1] = vertBase + y * (sliceTess + 1) + x + 1;
								pTessIndices[index + 2] = vertBase + (y + 1) * (sliceTess + 1) + x + 1;

								pTessIndices[index + 3] = vertBase + y * (sliceTess + 1) + x;
								pTessIndices[index + 4] = vertBase + (y + 1) * (sliceTess + 1) + x + 1;
								pTessIndices[index + 5] = vertBase + (y + 1) * (sliceTess + 1) + x;
							}
							else
							{
								// 第二四象限，斜边从右上角到左下角,1与4、2与3重叠
								/*
								0  - - -14
								|     / |
								|   /   |
								| /     |
								23- - - 5
								*/
								pTessIndices[index + 0] = vertBase + y * (sliceTess + 1) + x;
								pTessIndices[index + 1] = vertBase + y * (sliceTess + 1) + x + 1;
								pTessIndices[index + 2] = vertBase + (y + 1) * (sliceTess + 1) + x;

								pTessIndices[index + 3] = vertBase + (y + 1) * (sliceTess + 1) + x;
								pTessIndices[index + 4] = vertBase + y * (sliceTess + 1) + x + 1;
								pTessIndices[index + 5] = vertBase + (y + 1) * (sliceTess + 1) + x + 1;
							}
							index += 6;
						}
					}
					verts += (tesselationsY + 1)*(sliceTess + 1);
				}
			}

			if (0 == this->m_VertexBuffer || !glIsBuffer(this->m_VertexBuffer))
			{
				glGenBuffers(1, &this->m_VertexBuffer);
			}
			glBindBuffer(GL_ARRAY_BUFFER, this->m_VertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, floatCount * sizeof(*pTessVertices), (void*)pTessVertices, GL_STATIC_DRAW);
			delete[] pTessVertices;

			if (0 == m_IndexBuffer || !glIsBuffer(m_IndexBuffer))
			{
				glGenBuffers(1, &m_IndexBuffer);
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexCount * sizeof(*pTessIndices), (void*)pTessIndices, GL_STATIC_DRAW);
			delete[] pTessIndices;

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION_POSITION);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION_POSITION, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(0 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_R);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_R, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(2 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_G);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_G, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(4 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_B);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_B, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(6 * sizeof(float)));

			glEnableVertexAttribArray(VERTEX_ATTRIBUTE_DISTORTION_SLICE_INFO);
			glVertexAttribPointer(VERTEX_ATTRIBUTE_DISTORTION_SLICE_INFO, 2, GL_FLOAT,
				false, attribCount * sizeof(float), (void *)(8 * sizeof(float)));

			return true;
		}

		void GlGeometryQuad::DrawElements(int eye)
		{
			const int indexCount = m_IndexCount / 2;
			const int indexOffset = eye * indexCount;
			glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, (void*)(indexOffset * 2));
		}
		void GlGeometryQuad::DrawElementsRange(int eye)
		{
			const int indexCount = m_IndexCount / 2;
			const int indexOffset = eye * indexCount;
			glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_SHORT, (void*)(indexOffset * 2));
		}

	}
}
