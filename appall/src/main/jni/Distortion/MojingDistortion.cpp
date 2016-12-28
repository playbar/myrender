#include "MojingDistortion.h"
#include <math.h>
#include "../MojingManager.h"
#include "../MojingAPI.h"
#include "../Parameters/MojingParameters.h"
#include "../Render/MojingRenderBase.h"
#include "../MojingSDKStatus.h"
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

namespace Baofeng
{
	namespace Mojing
	{
		extern unsigned char g_EncKey[16];
		Distortion::Distortion()
		{
			SetMetersPerTanAngleAtCenter(0.037f);
			SetDistortionParamet(0,true , NULL, NULL, NULL);
			SetLensSeparation(0.0605);
			SetYOffset(0);
			SetFOV(95);
		}

		Distortion::~Distortion()
		{
		}

		DistortionVertexBuffer* Distortion::BuildDistortionVertexBuffer(int eyeBlocksWide /* = 32*/, int eyeBlocksHigh/* = 32*/)
		{
			Parameters* pParameters = Manager::GetMojingManager()->GetParameters();
			MojingDisplayParameters * pDisplay = pParameters->GetDisplayParameters();
			pDisplay->UpdatePPIFromUserSetting();
			hmdInfoInternal_t HMDI;
			HMDI.heightPixels = fmin(pDisplay->GetScreenHeight() ,pDisplay->GetScreenWidth());
			HMDI.heightMeters = fmin(pDisplay->GetScreenHeightMeter(), pDisplay->GetScreenWidthMeter());
			
			HMDI.widthPixels = fmax(pDisplay->GetScreenHeight(), pDisplay->GetScreenWidth());
			HMDI.widthMeters = fmax(pDisplay->GetScreenHeightMeter(), pDisplay->GetScreenWidthMeter());

			HMDI.MetersPerTanAngleAtCenter = GetMetersPerTanAngleAtCenter();
			HMDI.lensSeparation = GetLensSeparation();
			void * pBuffer = BuildDistortionBuffer_V2(HMDI , eyeBlocksWide, eyeBlocksHigh);
			//const int magic = ((int *)pBuffer)[0];
			const int tesselationsX = ((int *)pBuffer)[1];
			const int tesselationsY = ((int *)pBuffer)[2];

			//const int vertexBytes = 12 + 2 * (tesselationsX + 1) * (tesselationsY + 1) * 6 * sizeof(float);
			const float * bufferVerts = &((float *)pBuffer)[3];

			DistortionVertexBuffer *pRet = new DistortionVertexBuffer(eyeBlocksWide + 1, eyeBlocksHigh + 1);

			//单眼屏幕像素宽高比
			const float aspect = HMDI.widthPixels * 0.5 / HMDI.heightPixels;
			//图像中心-光学中心，测算误差 horizontalShiftMeters>1 镜头大 画面小 ；反之 镜头小 画面大
			const float	horizontalShiftMeters = (HMDI.lensSeparation / 2) - (HMDI.widthMeters / 4); // 瞳距/2 - 宽度(米)/4，测算出显示半幅单眼画面时的尺寸偏差
			const float	horizontalShiftView = horizontalShiftMeters / HMDI.MetersPerTanAngleAtCenter; //  2 * aspect * horizontalShiftMeters / HMDI.widthMeters;// 此处以屏幕长边的整宽度为1

			int	verts = 0;
			float fYOffset = GetYOffset() / HMDI.heightMeters; 
			MOJING_TRACE(g_APIlogger, "Unreadl YOffset = " << fYOffset);
			for (int eye = 0; eye < 2; eye++)
			{
				// 因为是反算，所以移动方向也要反过来
				// theta[1] -= GetYOffset() / GetMetersPerTanAngleAtCenter();
				// 处理Y轴高度调节数据,归一化到以屏幕高度为1，正数为向上调整，负数为向下调整
				

				float fHorizontalShiftView = eye ? horizontalShiftView : -horizontalShiftView;
				for (int y = 0; y <= tesselationsY; y++)
				{
					for (int x = 0; x <= tesselationsX; x++)
					{
						DistortionVertexNode* pNode = eye == 0 ? pRet->GetLeftVertexNode(x, y) : pRet->GetRightVertexNode(x, y);
						// Copy the offsets from the file
						// 下标2到7依次为Rx,Ry,Gx,Gy,Bx,By
						/*	坐标系以左上角为原点（0，0），X轴向右增长，Y轴向下增长
							目标左眼颜色：X : [0 , 0.5] , Y : [0 : 1] , 输入[-1 ， 1]
							目标右眼颜色：X : [0.5 , 1] , Y : [0 : 1] , 输入[-1 ， 1]		
							处理Y轴高度调节数据

						*/
						const float *pColor = &(bufferVerts[(y*(tesselationsX + 1) * 2 + x + eye * (tesselationsX + 1)) * UNREAL_DISTORTION_PARAMETES_COUNT]);

						pNode->m_fRX = 0.25 + 0.25*(*pColor++) + eye * 0.5;
						pNode->m_fRY = 0.5 - 0.5 *(*pColor++);

						pNode->m_fGX = 0.25 + 0.25*(*pColor++) + eye * 0.5;
 						pNode->m_fGY = 0.5 - 0.5 *(*pColor++);

						pNode->m_fBX = 0.25 + 0.25*(*pColor++) + eye * 0.5;
						pNode->m_fBY = 0.5 - 0.5 *(*pColor++);
						// 跳过 亮度 
						pColor++;
						// 原始坐标 ==> 屏幕 坐标 
						// 这是设置ViewPort的时候
						pNode->m_fX = (*pColor++) + fHorizontalShiftView;
						pNode->m_fY = (*pColor++) - fYOffset;
					}
				}// end of y
			}// end of eye
			// 制作Index
			int	index = 0;
			verts = 0;
			for (int eye = 0; eye < 2; eye++)
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
					for (int y = 0; y < tesselationsY; y++)
					{
						for (int x = 0; x < tesselationsX; x++)
						{
							// flip the triangulation in opposite corners
							if (( x <  tesselationsX / 2) ^ (y < (tesselationsY / 2)))
							{
								// 第一三象限，斜边从左上角到右下角,0与3、2与4重叠
								/*
								03 - - -1
								| \     |
								|   \   |
								|     \ |
								5 - - - 24
								*/
								
								pRet->m_pDistortionVertexIndex[index + 0] = vertBase + y * (tesselationsX + 1) + x;
								pRet->m_pDistortionVertexIndex[index + 1] = vertBase + y * (tesselationsX + 1) + x + 1;
								pRet->m_pDistortionVertexIndex[index + 2] = vertBase + (y + 1) * (tesselationsX + 1) + x + 1;

								pRet->m_pDistortionVertexIndex[index + 3] = vertBase + y * (tesselationsX + 1) + x;
								pRet->m_pDistortionVertexIndex[index + 4] = vertBase + (y + 1) * (tesselationsX + 1) + x + 1;
								pRet->m_pDistortionVertexIndex[index + 5] = vertBase + (y + 1) * (tesselationsX + 1) + x;
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
								pRet->m_pDistortionVertexIndex[index + 0] = vertBase + y * (tesselationsX + 1) + x;
								pRet->m_pDistortionVertexIndex[index + 1] = vertBase + y * (tesselationsX + 1) + x + 1;
								pRet->m_pDistortionVertexIndex[index + 2] = vertBase + (y + 1) * (tesselationsX + 1) + x;

								pRet->m_pDistortionVertexIndex[index + 3] = vertBase + (y + 1) * (tesselationsX + 1) + x;
								pRet->m_pDistortionVertexIndex[index + 4] = vertBase + y * (tesselationsX + 1) + x + 1;
								pRet->m_pDistortionVertexIndex[index + 5] = vertBase + (y + 1) * (tesselationsX + 1) + x + 1;
							}
							index += 6;
						}
					}
					verts += (tesselationsY + 1)*(tesselationsX + 1);
				
			}
			free(pBuffer);
			return pRet;
		}

		void * Distortion::BuildDistortionBuffer_V2(Mesh_820& mesh_820,int eyeBlocksWide/* = 32*/, int eyeBlocksHigh/* = 32*/)
		{
			/*全新流程，Mesh表只覆盖需要显示的区域*/
			DistortionVertexBuffer* pUnrealVertex = BuildDistortionVertexBuffer(eyeBlocksWide, eyeBlocksHigh);
			const int vertexCount = 2 * (eyeBlocksWide + 1) * (eyeBlocksHigh + 1);
			const int iBufferSize = 12 + 4 * vertexCount * UNREAL_DISTORTION_PARAMETES_COUNT;
			void* buf = malloc(iBufferSize);
			// 冲算垂直误差
			Parameters* pParameters = Manager::GetMojingManager()->GetParameters();
			MojingDisplayParameters * pDisplay = pParameters->GetDisplayParameters();
			float heightMeters = fmin(pDisplay->GetScreenHeightMeter(), pDisplay->GetScreenWidthMeter());
			float fYOffset = GetYOffset() / heightMeters;
			
			((int *)buf)[0] = (*((long*)"MJDB"));//DISTORTION_BUFFER_MAGIC;
			((int *)buf)[1] = eyeBlocksWide;
			((int *)buf)[2] = eyeBlocksHigh;

			for (int eye = 0; eye < 2; eye++)
			{
				for (int y = 0; y <= eyeBlocksHigh; y++)
				{
					for (int x = 0; x <= eyeBlocksWide; x++)
					{
						int	vertNum = y * (eyeBlocksWide + 1) * 2 + eye * (eyeBlocksWide + 1) + x;
						float * v = &((float *)buf)[3 + vertNum * UNREAL_DISTORTION_PARAMETES_COUNT];
						DistortionVertexNode* pNode = 0 == eye ? pUnrealVertex->GetLeftVertexNode(x, y) : pUnrealVertex->GetRightVertexNode(x, y);
						//0.25 + 0.25*(*pColor++) + eye * 0.5;
						v[0] = (pNode->m_fRX - 0.25 - eye * 0.5) / 0.25;
						// 0.5 - 0.5 *(*pColor++);
						v[1] = 1 - pNode->m_fRY * 2;
						v[2] = (pNode->m_fGX - 0.25 - eye * 0.5) / 0.25;
						v[3] = 1 - pNode->m_fGY * 2;
						v[4] = (pNode->m_fBX - 0.25 - eye * 0.5) / 0.25;
						v[5] = 1 - pNode->m_fBY * 2;
						v[6] = pNode->m_fOne;

						v[7] = pNode->m_fX / 2.0 - 0.5 + 1 * eye;
						// 注意：Android的Y方向和Unreal的Y方向是反的
						v[8] = pNode->m_fY + fYOffset * 2;
						if (eye == 0)
						{
							mesh_820.vertices_left.push_back(v[7]);
							mesh_820.vertices_left.push_back(v[8]);
							mesh_820.vertices_left.push_back(v[0]);
							mesh_820.vertices_left.push_back(v[1]);
							mesh_820.vertices_left.push_back(v[2]);
							mesh_820.vertices_left.push_back(v[3]);
							mesh_820.vertices_left.push_back(v[4]);
							mesh_820.vertices_left.push_back(v[5]);
						}
						else
						{
							mesh_820.vertices_right.push_back(v[7]);
							mesh_820.vertices_right.push_back(v[8]);
							mesh_820.vertices_right.push_back(v[0]);
							mesh_820.vertices_right.push_back(v[1]);
							mesh_820.vertices_right.push_back(v[2]);
							mesh_820.vertices_right.push_back(v[3]);
							mesh_820.vertices_right.push_back(v[4]);
							mesh_820.vertices_right.push_back(v[5]);
						}
						
					}
				}
			}

			// Index
			int m_IndexCount = 2 * eyeBlocksWide * eyeBlocksHigh * 6;
			uint32_t* pTessIndices = new uint32_t[m_IndexCount];

			int	index = 0;
			int verts = 0;
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
				for (int y = 0; y < eyeBlocksHigh; y++)
				{
					for (int x = 0; x < eyeBlocksWide; x++)
					{
						// flip the triangulation in opposite corners
						if ((x < eyeBlocksWide / 2) ^ (y < (eyeBlocksHigh / 2)))
						{
							// 第一三象限，斜边从左上角到右下角,0与3、2与4重叠
							/*
							03 - - -1
							| \     |
							|   \   |
							|     \ |
							5 - - - 24
							*/
							pTessIndices[index + 0] = vertBase + y * (eyeBlocksWide + 1) + x;
							pTessIndices[index + 1] = vertBase + y * (eyeBlocksWide + 1) + x + 1;
							pTessIndices[index + 2] = vertBase + (y + 1) * (eyeBlocksWide + 1) + x + 1;

							pTessIndices[index + 3] = vertBase + y * (eyeBlocksWide + 1) + x;
							pTessIndices[index + 4] = vertBase + (y + 1) * (eyeBlocksWide + 1) + x + 1;
							pTessIndices[index + 5] = vertBase + (y + 1) * (eyeBlocksWide + 1) + x;
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
							pTessIndices[index + 0] = vertBase + y * (eyeBlocksWide + 1) + x;
							pTessIndices[index + 1] = vertBase + y * (eyeBlocksWide + 1) + x + 1;
							pTessIndices[index + 2] = vertBase + (y + 1) * (eyeBlocksWide + 1) + x;

							pTessIndices[index + 3] = vertBase + (y + 1) * (eyeBlocksWide + 1) + x;
							pTessIndices[index + 4] = vertBase + y * (eyeBlocksWide + 1) + x + 1;
							pTessIndices[index + 5] = vertBase + (y + 1) * (eyeBlocksWide + 1) + x + 1;
						}

						if (eye == 0)
						{
							mesh_820.indices.push_back(pTessIndices[index + 0]);
							mesh_820.indices.push_back(pTessIndices[index + 1]);
							mesh_820.indices.push_back(pTessIndices[index + 2]);
							mesh_820.indices.push_back(pTessIndices[index + 3]);
							mesh_820.indices.push_back(pTessIndices[index + 4]);
							mesh_820.indices.push_back(pTessIndices[index + 5]);
						}
						index += 6;
					}
				}
				verts += (eyeBlocksHigh + 1)*(eyeBlocksWide + 1);
				//}
			}
			delete[] pTessIndices;
			// end of Indices

			delete pUnrealVertex;

// #ifdef _DEBUG
// 			FILE* pFile = fopen("/sdcard/MojingSDK/DDD.dat" , "wb");
// 			fwrite(buf, 1, iBufferSize, pFile);
// 			fflush(pFile);
// 			fclose(pFile);
// #endif
			return buf;
		}
		void * Distortion::BuildDistortionBuffer(int eyeBlocksWide /* = 32*/, int eyeBlocksHigh/* = 32*/)
		{
			/*旧有流程，Mesh表覆盖整个屏幕窗口*/
			Parameters* pParameters = Manager::GetMojingManager()->GetParameters();
			MojingDisplayParameters * pDisplay = pParameters->GetDisplayParameters();
			pDisplay->UpdatePPIFromUserSetting();
			hmdInfoInternal_t HMDI;
			HMDI.heightPixels = fmin(pDisplay->GetScreenHeight(), pDisplay->GetScreenWidth());
			HMDI.heightMeters = fmin(pDisplay->GetScreenHeightMeter(), pDisplay->GetScreenWidthMeter());

			HMDI.widthPixels = fmax(pDisplay->GetScreenHeight(), pDisplay->GetScreenWidth());
			HMDI.widthMeters = fmax(pDisplay->GetScreenHeightMeter(), pDisplay->GetScreenWidthMeter());
			 
			HMDI.MetersPerTanAngleAtCenter = GetMetersPerTanAngleAtCenter();
			HMDI.lensSeparation = GetLensSeparation();
			//////////////////////////////////////////////////////////////////////////////////
			return BuildDistortionBuffer(HMDI , eyeBlocksWide, eyeBlocksHigh);
		}

		void * Distortion::BuildDistortionBufferOverlay(int eyeBlocksWide/* = 32*/, int eyeBlocksHigh/* = 32*/, unsigned int OverlayWidth, unsigned int OverlayHeight)
		{
			Parameters* pParameters = Manager::GetMojingManager()->GetParameters();
			MojingDisplayParameters * pDisplay = pParameters->GetDisplayParameters();
			pDisplay->UpdatePPIFromUserSetting();
			hmdInfoInternal_t HMDI;
			HMDI.heightPixels = OverlayHeight;
			HMDI.heightMeters = pDisplay->Pix2Meter(OverlayHeight);

			HMDI.widthPixels = OverlayWidth;
			HMDI.widthMeters = pDisplay->Pix2Meter(OverlayWidth);

			HMDI.MetersPerTanAngleAtCenter = GetMetersPerTanAngleAtCenter();
			HMDI.lensSeparation = GetLensSeparation();
			//////////////////////////////////////////////////////////////////////////////////
			return BuildDistortionBuffer(HMDI, eyeBlocksWide, eyeBlocksHigh);

		}

		int Distortion::UNREAL_BuildDistortionMesh(int eyeBlocksWide /* = 40*/, int eyeBlocksHigh/* = 40*/, void* pVerts, void* pIndices)
		{
			DistortionVertexBuffer* pDistortionVertexBuffer = BuildDistortionVertexBuffer(eyeBlocksWide, eyeBlocksHigh);
			if (pDistortionVertexBuffer)
			{
				for (int iVertex = 0; iVertex < pDistortionVertexBuffer->m_iVertexCount; iVertex++)
				{
					pDistortionVertexBuffer->m_pDistortionVertex[iVertex].m_fY /= 0.9;
				}
				memcpy(pVerts, pDistortionVertexBuffer->m_pDistortionVertex, sizeof(DistortionVertexNode)* pDistortionVertexBuffer->m_iVertexCount);
				memcpy(pIndices, pDistortionVertexBuffer->m_pDistortionVertexIndex, sizeof(short)* pDistortionVertexBuffer->m_iIndexCount);
				delete pDistortionVertexBuffer;
				return 2 * (eyeBlocksWide + 1) * (eyeBlocksHigh + 1);
			}
			return 0;
		}
		int Distortion::UNITY_BuildDistortionMesh(int eyeBlocksWide /* = 40*/, int eyeBlocksHigh/* = 40*/, void* fppVertex, void* fppUV, void* fppIndex)
		{
			MojingSDKStatus *pStatus = MojingSDKStatus::GetSDKStatus(); 
			Parameters* pParameters = Manager::GetMojingManager()->GetParameters();
			MojingDisplayParameters * pDisplay = pParameters->GetDisplayParameters();
			
			const float aspect = pDisplay->GetScreenWidth() * 0.5 / pDisplay->GetScreenHeight();
			float fYOffset = GetYOffset() / pDisplay->GetScreenHeightMeter();
			// 注意：为了满足的DayDream的计算要求和格式，这里要减1
			DistortionVertexBuffer* pUnrealBuffer = BuildDistortionVertexBuffer(eyeBlocksWide - 1, eyeBlocksHigh - 1);
			if (pUnrealBuffer == NULL)
				return 0;

			//const int magic = ((int *)pBuffer)[0];
			const int tesselationsX = eyeBlocksWide;
			const int tesselationsY = eyeBlocksHigh;

			//const int vertexBytes = 12 + 2 * (tesselationsX + 1) * (tesselationsY + 1) * 6 * sizeof(float);
			
			const int sliceTess = tesselationsX ;

			int iVertexCount = 2 * (sliceTess)*(tesselationsY);
			if (fppVertex == NULL || fppUV == NULL || fppIndex == NULL)
			{
				return iVertexCount;
			}
			float* fpVertex = (float*)fppVertex;
			float* fpUV = (float*)fppUV;
			int* fpIndex = (int*)fppIndex;

			for (int i = 0; i < pUnrealBuffer->m_iIndexCount; i++)
			{
				fpIndex[i] = pUnrealBuffer->m_pDistortionVertexIndex[i];
			}

			int	verts = 0;
// #ifdef _DEBUG
// 			float fMiniX = 0, fMiniY = 0;
// 			float fMaxX = -1, fMaxY = -1;
// 			FILE *pFile = fopen("/sdcard/MojingSDK/DDD.txt", "w");
// 			if (pFile)
// 			{
// 				fprintf(pFile, "/* Distortion for DayDream , %d x %d , Vertex = %d*/\n", eyeBlocksWide, eyeBlocksHigh, iVertexCount);
// 				fprintf(pFile, "/* Screen Size = %f x %f , Pixs = %d x %d*/\n", 
// 					pDisplay->GetScreenWidthMeter() , pDisplay->GetScreenHeightMeter() ,
// 					pDisplay->GetScreenWidth(), pDisplay->GetScreenHeight()
// 					);
// 				fprintf(pFile, "float DistortionMesh[] = {/* for each Y ,for each X , for each Point: 3 * Vertex , 2* UV */\n");
// 			}
// #endif
			for (int eye = 0; eye < 2; eye++)
			{
// #ifdef _DEBUG
// 				if (pFile)
// 				{
// 					if (eye == 0)
// 						fputs("/*Left Eye*/\n", pFile);
// 					else
// 						fputs("\n/*Right Eye*/\n", pFile);
// 				}
// #endif			
				float *pOutVertex = fpVertex + eye * (sliceTess)*(tesselationsY) * 3;
				float *pOutUV = fpUV + eye * (sliceTess )*(tesselationsY ) * 2;
				float fEyeOffsetX = eye - 0.5;
				const int vertBase = verts;
				for (int y = 0; y < tesselationsY; y++)
				{
// #ifdef _DEBUG
// 					if (pFile)
// 					{
// 						fprintf(pFile , "\n/*%02d*/\t", y);
// 						fflush(pFile);
// 					}
// #endif
					const float	yf = (float)y / (float)(tesselationsY-1);
					for (int x = 0; x < sliceTess; x++)
					{
						const float	xf = (float)x / (float)(sliceTess-1);

						float fS = xf;
						float fT = yf;

						fS = fmin(1 - 1e-5 , fS);
						fS = fmax(1e-5, fS);

						fT = fmin(1 - 1e-5, fT);
						fT = fmax(1e-5, fT);

						DistortionVertexNode* pNode = eye == 0 ? pUnrealBuffer->GetLeftVertexNode(x, y) : pUnrealBuffer->GetRightVertexNode(x, y);

						float fX = pNode->m_fX;
						float fY = pNode->m_fY;
						pOutVertex[y * (tesselationsX) * 3 + x * 3 + 0] = fX / 2 + fEyeOffsetX;
						pOutVertex[y * (tesselationsX)* 3 + x * 3 + 0] *= 0.9;
						pOutVertex[y * (tesselationsX)* 3 + x * 3 + 1] = fY / 2 / 0.9;
						pOutVertex[y * (tesselationsX) * 3 + x * 3 + 2] = 1.0f;

						// 重新算 UV
						pOutUV[y * (tesselationsX)* 2 + x * 2 + 0] = 0.5 * fS + eye * 0.5;//pNode->m_fGX ;
						pOutUV[y * (tesselationsX)* 2 + x * 2 + 1] = fT;//1.0f - pNode->m_fGY;
// #ifdef _DEBUG
// 						if (eye == 0)
// 						{
// 							fMiniX = fmin(fX / 2 + fEyeOffsetX, fMiniX);
// 							fMiniY = fmin(fY / 2, fMiniY);
// 
// 							fMaxX = fmax(fX / 2 + fEyeOffsetX, fMaxX);
// 							fMaxY = fmax(fY / 2, fMaxY);
// 						}
// 
// 						if (pFile)
// 						{
// 							fprintf(pFile, "%2.4ff , %2.4ff, %2.4ff, %2.4ff, %2.4ff ", 
// 								pOutVertex[y * (tesselationsX) * 3 + x * 3 + 0], 
// 								pOutVertex[y * (tesselationsX) * 3 + x * 3 + 1], 
// 								pOutVertex[y * (tesselationsX) * 3 + x * 3 + 2],
// 								pOutUV[y * (tesselationsX) * 2 + x * 2 + 0],
// 								pOutUV[y * (tesselationsX) * 2 + x * 2 + 1]
// 								);
// 							if (x != sliceTess || y != tesselationsY)
// 								fputs(" , ", pFile);
// 						}
// #endif
					}// end of x
				}// end of y
				/////////////////////////////////////////////////
			}// end of eye

// #ifdef _DEBUG
// 			if (pFile)
// 			{
// 				fputs("\n};\n" , pFile);
// 				fprintf(pFile, "/*MiniX = %f , MaxX = %f , width = %f ( %f )*/\n", fMiniX, fMaxX, fMaxX - fMiniX, pDisplay->GetScreenWidthMeter() / 2.0f * fabs(fMaxX - fMiniX));
// 				fprintf(pFile, "/*MiniY = %f , MaxY = %f , height = %f ( %f )*/\n", fMiniY, fMaxY, fMaxY - fMiniY, pDisplay->GetScreenHeightMeter() * fabs(fMaxY - fMiniY));
// 				fflush(pFile);
// 				fclose(pFile);
// 			}
// 
// 			FILE * pFile2 = fopen("/sdcard/MojingSDK/DDD2.txt" , "w");
// 			if (pFile2)
// 			{
// 				fprintf(pFile2, "/* Distortion for DayDream , %d x %d , Vertex = %d*/\n", eyeBlocksWide, eyeBlocksHigh, iVertexCount);
// 				fprintf(pFile2, "float DistortionMesh[] = {/* for each Y ,for each X , for each Point: 3 * Vertex , 2* UV */\n");
// 				for (int eye = 0; eye < 2; eye++)
// 				{
// 					float *pOutVertex = fpVertex + eye * (sliceTess)*(tesselationsY)* 3;
// 					float *pOutUV = fpUV + eye * (sliceTess)*(tesselationsY)* 2;
// 
// 					if (pFile2)
// 					{
// 						if (eye == 0)
// 							fputs("/*Left Eye*/\n", pFile2);
// 						else
// 							fputs("\n/*Right Eye*/\n", pFile2);
// 					}
// 
// 
// 					for (int y = 0; y < tesselationsY; y++)
// 					{
// 
// 						if (pFile2)
// 						{
// 							fprintf(pFile2, "\n/*%02d*/\t", y);
// 							fflush(pFile2);
// 						}
// 
// 						const float	yf = (float)y / (float)tesselationsY;
// 						for (int x = 0; x < sliceTess; x++)
// 						{
// 							if (pFile2)
// 							{
// 								fprintf(pFile2, "%2.4ff , %2.4ff, %2.4ff ",
// 									pOutVertex[y * (tesselationsX)* 3 + x * 3 + 0],
// 									pOutVertex[y * (tesselationsX)* 3 + x * 3 + 1],
// 									pOutVertex[y * (tesselationsX)* 3 + x * 3 + 2]
// 									);
// 								if (x != sliceTess || y != tesselationsY)
// 									fputs(" , ", pFile2);
// 							}
// 						}
// 					}
// 				}
// 
// 				fputs("\n};\n", pFile2);
// 				fflush(pFile2);
// 				fclose(pFile2);
// 
// 			}
// 
// 			FILE * pFile3 = fopen("/sdcard/MojingSDK/DDD3.txt", "w");
// 			if(pFile3)
// 			{
// 				for (int i = 0, j = 0, k = 0; i < 40 * 40 * 2; i++, j += 3, k += 2)
// 				{
// 					if (i == 0)
// 					{
// 						fprintf(pFile3, "/*Left Eye*/\n");
// 					}
// 					if (i == 1600)
// 					{
// 						fprintf(pFile3, "\n/*Right Eye*/\n");
// 					}
// 					if (i % 40 == 0)
// 					{
// 						if (i < 1600)
// 						{
// 							fprintf(pFile3 , "\n/*%02d*/", i / 40);
// 						}
// 						else
// 						{
// 							fprintf(pFile3, "\n/*%02d*/", i / 40 - 40);
// 						}
// 						fflush(pFile3);
// 					}
// 
// 					fprintf(pFile3, "%2.4ff , %2.4ff , %2.4ff , ", fpVertex[j], fpVertex[j + 1], fpVertex[j + 2]);
// 				}
// 
// 				fflush(pFile3);
// 				fclose(pFile3);
// 			}
// #endif
			
			delete pUnrealBuffer;

			return iVertexCount;
		}

#define	SCREEN_EDGE			(0.0025f)					
#define	SCREEN_LEFT_EYE_LEFT_EDGE			(-1 + SCREEN_EDGE)
#define	SCREEN_LEFT_EYE_RIGHT_EDGE			(0 - SCREEN_EDGE)

#define	SCREEN_RIGHT_EYE_LEFT_EDGE			(0 + SCREEN_EDGE)
#define	SCREEN_RIGHT_EYE_RIGHT_EDGE			(1 - SCREEN_EDGE)

#define	SCREEN_BOTTOM		(-1 + SCREEN_EDGE)
#define	SCREEN_TOP			(+1 - SCREEN_EDGE)

		bool BuildUnrealDistortionBuffer_ClipX(float &fX, const int& eye, const float &scaleOne, const float &ImageInScreenWidth, const float &fHorizontalShiftView ,const Vector3f &scale)
		{// 模拟 修正X不要超过眼睛的视界和 纹理的边界
			float fXR = fX * scale.x;
			float fTXR = fXR / scaleOne * ImageInScreenWidth;
			float fScreenXR = fTXR *0.5 - 0.5 + eye + fHorizontalShiftView;
			bool bRebuildX = false;
			if (eye == 0)
			{
				if (fScreenXR > SCREEN_LEFT_EYE_RIGHT_EDGE)
				{
					fScreenXR = SCREEN_LEFT_EYE_RIGHT_EDGE;
					bRebuildX = true;

				}
				else if (fScreenXR < SCREEN_LEFT_EYE_LEFT_EDGE)
				{
					fScreenXR = SCREEN_LEFT_EYE_LEFT_EDGE;
					bRebuildX = true;
				}
			}
			else
			{
				if (fScreenXR < SCREEN_RIGHT_EYE_LEFT_EDGE)
				{
					fScreenXR = SCREEN_RIGHT_EYE_LEFT_EDGE;
					bRebuildX = true;
				}
				else if (fScreenXR > SCREEN_RIGHT_EYE_RIGHT_EDGE)
				{
					fScreenXR = SCREEN_RIGHT_EYE_RIGHT_EDGE;
					bRebuildX = true;
				}
			}

			if (bRebuildX)
			{
				fTXR = (fScreenXR + 0.5 - eye - fHorizontalShiftView) / 0.5;
				fXR = fTXR * scaleOne / ImageInScreenWidth;
				fX = fXR / scale.x;
			}
			return bRebuildX;
		}
		bool BuildUnrealDistortionBuffer_ClipY(float &fY, const float &scaleOne, const float &ImageInScreenWidth, const float & aspect , const Vector3f &scale)
		{
			float fYR = fY * scale.x;
			float fTYR = fYR / scaleOne * ImageInScreenWidth * aspect;
			bool bRebuildY = false;
			if (fTYR > SCREEN_TOP)
			{
				fTYR = SCREEN_RIGHT_EYE_RIGHT_EDGE;
				bRebuildY = true;
			}
			else if (fTYR < SCREEN_BOTTOM)
			{
				fTYR = SCREEN_BOTTOM;
				bRebuildY = true;
			}
			if (bRebuildY)
			{
				fYR = fTYR * scaleOne / ImageInScreenWidth / aspect;
				fY = fYR / scale.x;
			}
			return bRebuildY;
		}

		void * Distortion::BuildDistortionBuffer_V2(const hmdInfoInternal_t &HMDI, int eyeBlocksWide /* = 32*/, int eyeBlocksHigh/* = 32*/)
		{
			const int vertexCount = 2 * (eyeBlocksWide + 1) * (eyeBlocksHigh + 1);
			//12：
			//4*vertexCount:
			//6:
			void* buf = malloc(12 + 4 * vertexCount * UNREAL_DISTORTION_PARAMETES_COUNT);
			//
			((int *)buf)[0] = 0x56347805;//DISTORTION_BUFFER_MAGIC;
			((int *)buf)[1] = eyeBlocksWide;
			((int *)buf)[2] = eyeBlocksHigh;

			// the centers are offset horizontal in each eye
			//单眼屏幕像素宽高比
			const float aspect = HMDI.widthPixels * 0.5 / HMDI.heightPixels;

			//图像中心-光学中心，测算误差 horizontalShiftMeters>1 镜头大 画面小 ；反之 镜头小 画面大
			const float	horizontalShiftMeters = (HMDI.lensSeparation / 2) - (HMDI.widthMeters / 4); // 瞳距/2 - 宽度(米)/4，测算出显示半幅单眼画面时的尺寸偏差

			const float	horizontalShiftView = 2 * aspect * horizontalShiftMeters / HMDI.widthMeters;// 此处以屏幕长边的整宽度为1

			const float ImageInScreenWidth = HMDI.MetersPerTanAngleAtCenter / (HMDI.widthMeters / 4);
			
#if 1
			Vector3f scaleOne = DistortionFnScaleRadiusSquared_Unreal(1);
			float fModifyOffset = 1.0f / eyeBlocksWide / 25;
			for (int eye = 0; eye < 2; eye++)
			{
				// 因为是反算，所以移动方向也要反过来
				float fHorizontalShiftView = eye ? horizontalShiftView : -horizontalShiftView;
//				float fMinTextureX = eye == 0 ? 0 : 0.5;
//				float fMaxTextureX = eye == 0 ? 0.5 : 1;
//				float fMinTextureY = 0;
//				float fMaxTextureY = 1;

				for (int y = 0; y <= eyeBlocksHigh; y++)
				{
					const float	yf = (float)y / (float)eyeBlocksHigh;
					
					for (int x = 0; x <= eyeBlocksWide; x++)
					{
						//序数，数组中的位置，为了将来从数组中取数据
						int	vertNum = y * (eyeBlocksWide + 1) * 2 + eye * (eyeBlocksWide + 1) + x;
						float * v = &((float *)buf)[3 + vertNum * UNREAL_DISTORTION_PARAMETES_COUNT];

						float xf = (float)x / (float)eyeBlocksWide;
						float fModifyStepX = 0;
						float fModifyStepY = 0;
						bool bCheckOK = true;
						do
						{
							bCheckOK = true;
							float fTargetX = 2 * xf - 1 + fModifyStepX * fModifyOffset;
							float fTargetY = 2 * yf - 1 + fModifyStepY * fModifyOffset;

							float R = (fTargetX*fTargetX + fTargetY*fTargetY);
							Vector3f scale = DistortionFnScaleRadiusSquared_Unreal(R);
							// 对 XY进行不超出屏幕的边界剪裁
							float fX = fTargetX;
							BuildUnrealDistortionBuffer_ClipX(fX , eye, scaleOne.x, ImageInScreenWidth, fHorizontalShiftView, scale);
							float fY = fTargetY;
//							BuildUnrealDistortionBuffer_ClipY(fY, scaleOne.x, ImageInScreenWidth, aspect ,scale);
							fX *= scale.y;
							fY *= scale.y;

							// 对XY进行不超过纹理可视范围的剪裁
							Vector4f chromaScale = DistortionFnScaleRadiusSquared_V4(fX*fX + fY*fY);
//							float fRX = 0.25 + 0.25*(fX*chromaScale.z) + eye * 0.5;
//							float fRY = 0.5 - 0.5 *(fY*chromaScale.z);
// 							if (fRX < fMinTextureX)
// 							{
// 								bCheckOK = false;
// 								fModifyStepX++;
// 							}
// 							else if (fRX > fMaxTextureX)
// 							{
// 								bCheckOK = false;
// 								fModifyStepX--;
// 							}
// 							else if (fRY < fMinTextureY)
// 							{
// 								bCheckOK = false;
// 								fModifyStepY++;
// 							}
// 							else if (fRY > fMaxTextureY)
// 							{
// 								bCheckOK = false;
// 								fModifyStepY--;
// 							}
// 							else
							{
								v[0] = fX * chromaScale.x;
								v[1] = fY * chromaScale.x;
								v[2] = fX * chromaScale.y;
								v[3] = fY * chromaScale.y;
								v[4] = fX * chromaScale.z;
								v[5] = fY * chromaScale.z;
								v[6] = 1;
								v[7] = fX / scaleOne.y * ImageInScreenWidth /*+ fHorizontalShiftView*/;// 瞳距平移
								v[8] = fY / scaleOne.y * ImageInScreenWidth * aspect; // 长宽比缩放

								//<---------检查--------->//
//								float FX = v[2] * 2 - 1;
//								float FY = v[3] * 2 - 1;
//								float R2 = (FX * FX + FY * FY);
//								float R_Offset = R2 - R;
							}
						} while (0);
						
						/*
						// 程序执行到这里，算出来了G的屏幕坐标，是以纹理坐标为参考的,下面把他归一化到屏幕坐标
						float fX = (fTargetX * scale.y);// / scaleOne.y * ImageInScreenWidth;
						float fY = (fTargetY * scale.y);// / scaleOne.y * ImageInScreenWidth;
						// 因为畸变跑的最远的是R，所以使用R来测算边界。
						float fXR = fTargetX * scale.x;
						float fYR = fTargetY * scale.x;

						///////////////////////////////////////////////////////////////////////
						// 防溢出处理，以下的代码保证制作出来的网格图不会跑到图像文理外面去。



						float fTXR = fXR / scaleOne.x * ImageInScreenWidth/ * + fHorizontalShiftView* /;
// 
// 						if (fTX > SCREEN_EDGE_X)
// 							fX = (SCREEN_EDGE_X/ * - fHorizontalShiftView* /)* scaleOne.y / ImageInScreenWidth;
// 						else if (fTX < -SCREEN_EDGE_X)
// 							fX = (-SCREEN_EDGE_X/ * - fHorizontalShiftView* /)* scaleOne.y / ImageInScreenWidth;
						float fScreenXR = fTXR *0.5 - 0.5 + eye + fHorizontalShiftView;
						
						
						bool bRebuildY = false;
						float fTYR = fYR / scaleOne.y * ImageInScreenWidth * aspect;
						float fTextureYR = 0.5 - 0.5*fTYR;
						float fScreenYR = fTYR;
						if (fTYR > SCREEN_TOP)
						{
							fScreenYR = SCREEN_RIGHT_EYE_RIGHT_EDGE;
							bRebuildY = true;
						}
						else if (fTYR < SCREEN_BOTTOM)
						{
							fScreenYR = SCREEN_BOTTOM;
							bRebuildY = true;
						}
						if (bRebuildY)
						{
							fYR = fScreenYR * scaleOne.y / ImageInScreenWidth / aspect;
							fY = fYR / scaleOne.x * scaleOne.y;
						}
						///////////////////////////////////////////////////////////////////////
						// 还需要转换成物理位置后再转化成屏幕坐标
						// 接下来用G的屏幕坐标重新算R,G,B
						Vector4f chromaScale = DistortionFnScaleRadiusSquared_V4(fX*fX + fY*fY);
						v[0] = fX * chromaScale.x;
						v[1] = fY * chromaScale.x;
						v[2] = fX * chromaScale.y;
						v[3] = fY * chromaScale.y;
						v[4] = fX * chromaScale.z;
						v[5] = fY * chromaScale.z;
						v[6] = 1;
						v[7] = fX / scaleOne.y * ImageInScreenWidth / *+ fHorizontalShiftView* /;// 瞳距平移
						v[8] = fY / scaleOne.y * ImageInScreenWidth * aspect ; // 长宽比缩放
						//<---------检查--------->//
						float FX = v[2] * 2 - 1;
						float FY = v[3] * 2 - 1;
						float R2 = (FX * FX + FY * FY);
						float R_Offset = R2 - R;*/
					}
				}
			}
#else

			/*
			着色坐标系以图像中心为原点，标准笛卡尔坐标系
			左眼坐标 X[-1 , 0] , Y[-1 , 1]
			右眼坐标 X[ 0 , 1] , Y[-1 , 1]
			颜色坐标系以左上角为原点（0，0），X轴向右增长，Y轴向下增长
			目标左眼颜色：X : [0 , 0.5] , Y : [0 : 1]
			目标右眼颜色：X : [0.5 , 1] , Y : [0 : 1]
			*/
			float fOffset = 1;
			bool bResetOffset = false;
			do
			{
				fOffset -= 0.005;
				bResetOffset = false;
				for (int eye = 0; eye < 2 && !bResetOffset; eye++)
				{
					for (int y = 0; y <= eyeBlocksHigh && !bResetOffset; y++)
					{
						const float	yf = (float)y / (float)eyeBlocksHigh;
						float fTargetY = 2 * yf - 1;
						fTargetY *= fOffset;// 误差空间 
						for (int x = 0; x <= eyeBlocksWide && !bResetOffset ; x++)
						{
							//序数，数组中的位置，为了将来从数组中取数据
							int	vertNum = y * (eyeBlocksWide + 1) * 2 + eye * (eyeBlocksWide + 1) + x;
							const float	xf = (float)x / (float)eyeBlocksWide;
							float fTargetX = 2 * xf - 1;
							fTargetX *= fOffset;// 5%误差空间 
	
							float * v = &((float *)buf)[3 + vertNum * UNREAL_DISTORTION_PARAMETES_COUNT];
							//输入坐标xf , yf 是在[0...1]闭区间，且统一换算为以高度为参考的倍率关系。
							
							if (fTargetX != 0 && fTargetY != 0)
							{
								bool bXBigger = 1;// abs(fTargetX) > abs(fTargetY);
								float fTheta = fabs(bXBigger ? fTargetY / fTargetX : fTargetX / fTargetY);
								float fTarget = fabs(bXBigger ? fTargetX : fTargetY);
								
	
								/*利用折半逼近算法计算当颜色Y位于特定位置的时候Y的取值*/
								float fCheckValue = 1;
								// QSR' ^2
								float fQSR_T = fTargetY * fTargetY + fTargetX*fTargetX;
								//fQSR_T *= KG_MAX / fTheta;
								// RSQ' -> K'
								Vector3f K_T = DistortionFnScaleRadiusSquared_Unreal(fQSR_T);
								// RSQ' * K' -> RSQ
								float fRSQ = K_T.y * fQSR_T;
								// QSR - >RealX , RealY
								// QSR = RealV^2 * (1+fTheta*fTheta)
								float fRealValue = sqrt(fRSQ / (1 + fTheta*fTheta));
								float fRealX = bXBigger ? fRealValue : fRealValue * fTheta;
								float fRealY = bXBigger ? fRealValue * fTheta : fRealValue;
									
								// fRealY += GetYOffset() / GetMetersPerTanAngleAtCenter();
								/////////////////////////////////////////////////////////
								if (xf < 0.5)
									fRealX = -fRealX;
								if (yf < 0.5)
									fRealY = -fRealY;
								/*
								const float unit = in[i];
								const float ndc = (unit - 0.5f);
								const float pixels = ndc * hmdInfo.heightPixels;
								const float meters = pixels * hmdInfo.widthMeters / hmdInfo.widthPixels;
								const float tanAngle = meters / hmdInfo.MetersPerTanAngleAtCenter;
								theta[i] = tanAngle;
								*/
								fRealX /= (HMDI.heightPixels * HMDI.widthMeters / HMDI.widthPixels / HMDI.MetersPerTanAngleAtCenter);
								fRealY /= (HMDI.heightPixels * HMDI.widthMeters / HMDI.widthPixels / HMDI.MetersPerTanAngleAtCenter);
								
	
								fRealX += 0.5;
								fRealY += 0.5;
								// (eye ? -horizontalShiftView : horizontalShiftView) + xf *aspect + (1.0f - aspect) * 0.5f
								float fShift = (eye ? -horizontalShiftView : horizontalShiftView) + (1.0f - aspect) * 0.5f;
	
								// in[0] = fShift + xf * aspect;
								fRealX = (fRealX - fShift) / aspect;
								v[7] = fRealX; // fmin(fmax(eye * 0.5, fRealX), 1);
								v[8] = fRealY; // fmin(fmax(0, fRealY), 1);
								const float inTex[2] = { (eye ? -horizontalShiftView : horizontalShiftView) +
									fRealX *aspect + (1.0f - aspect) * 0.5f, fRealY };
								WarpTexCoordChroma(HMDI, inTex, &v[0], &v[2], &v[4], &v[6]);
								
								// 测算畸变参数是不是会超出纹理范围。因为ES2.0没有纹理剪裁的能力，所以这里要自行测算

								for (int i = 0 ; i < 6 ; i++)
								{
									if (v[i] < -0.999 || v[i] >= 0.999)
									{
										bResetOffset = true;
										break;
									}
								}

							}
							else// 这里应该不会走到
							{
	
							}
						}// end of x
					}//end of y
				}// end of Eye
			}while (bResetOffset);
#endif
			return buf;
		}
		
		void * Distortion::BuildDistortionBuffer(const hmdInfoInternal_t &HMDI, int eyeBlocksWide /* = 32*/, int eyeBlocksHigh/* = 32*/)
		{
			const int vertexCount = 2 * (eyeBlocksWide + 1) * (eyeBlocksHigh + 1);
			//12：
			//4*vertexCount:
			//6:
			void* buf = malloc(12 + 4 * vertexCount * DISTORTION_PARAMETES_COUNT);
			//
			((int *)buf)[0] = 0x56347805;//DISTORTION_BUFFER_MAGIC;
			((int *)buf)[1] = eyeBlocksWide;
			((int *)buf)[2] = eyeBlocksHigh;

			// the centers are offset horizontal in each eye
			//单眼屏幕像素宽高比
			const float aspect = HMDI.widthPixels * 0.5 / HMDI.heightPixels;

			//图像中心-光学中心，测算误差 horizontalShiftMeters>1 镜头大 画面小 ；反之 镜头小 画面大
			const float	horizontalShiftMeters = (HMDI.lensSeparation / 2) - (HMDI.widthMeters / 4); // 瞳距/2 - 宽度(米)/4，测算出显示半幅单眼画面时的尺寸偏差

			const float	horizontalShiftView = 2 * aspect * horizontalShiftMeters / HMDI.widthMeters;

			for (int eye = 0; eye < 2; eye++)
			{
				for (int y = 0; y <= eyeBlocksHigh; y++)
				{
					const float	yf = (float)y / (float)eyeBlocksHigh;
					for (int x = 0; x <= eyeBlocksWide; x++)
					{
						//序数，数组中的位置，为了将来从数组中取数据
						int	vertNum = y * (eyeBlocksWide + 1) * 2 + eye * (eyeBlocksWide + 1) + x;

						const float	xf = (float)x / (float)eyeBlocksWide;

						
						float * v = &((float *)buf)[3 + vertNum * DISTORTION_PARAMETES_COUNT];
						//输入坐标xf , yf 是在[0...1]闭区间，且统一换算为以高度为参考的倍率关系。
						const float inTex[2] = { (eye ? -horizontalShiftView : horizontalShiftView) +
							xf *aspect + (1.0f - aspect) * 0.5f, yf };
						WarpTexCoordChroma(HMDI, inTex, &v[0], &v[2], &v[4] , &v[6]);
					}
				}
			}
			return buf;
		}

		void Distortion::WarpTexCoordChroma(const hmdInfoInternal_t & hmdInfo, const float in[2],
			float red[2], float green[2], float blue[2] , float light[1]) {
			float theta[2];
			for (int i = 0; i < 2; i++) {
				const float unit = in[i];
				const float ndc = (unit - 0.5f);
				const float pixels = ndc * hmdInfo.heightPixels;
				const float meters = pixels * hmdInfo.widthMeters / hmdInfo.widthPixels;
				const float tanAngle = meters / hmdInfo.MetersPerTanAngleAtCenter;
				theta[i] = tanAngle;
			}
#if 1
			theta[1] -= GetYOffset() / GetMetersPerTanAngleAtCenter();
#endif
			const float rsq = theta[0] * theta[0] + theta[1] * theta[1];
			//卡特摩尔样条插值
			//CatmullRom插值。所谓样条曲线是指给定一组控制点而得到一条曲线，曲线的大致形状由这些点予以控制，
			//一般可分为插值样条和逼近样条两种，插值样条通常用于数字化绘图或动画的设计，
			//逼近样条一般用来构造物体的表面。CatmullRom样条与上一节所讲的B样条很相似，
			//不同在于CatmullRom样条的曲线会经过其每一个控制点。
			// DistortionFnScaleRadiusSquaredChroma(rsq);
			const Vector4f chromaScale = DistortionFnScaleRadiusSquared_V4(rsq);
			const float KG_MAX = m_KG[m_iSegment];
			for (int i = 0; i < 2; i++) {
				red[i] = chromaScale.x * theta[i] ;
				green[i] = chromaScale.y * theta[i] ;
				blue[i] = chromaScale.z * theta[i] ;
				if (m_iSegment > 4)
				{
					red[i] /= KG_MAX;
					green[i] /= KG_MAX;
					blue[i] /= KG_MAX;
				}
			}
			light[0] = chromaScale.w;
		}
		/*
		void Distortion::WarpTexCoordChroma(const hmdInfoInternal_t & hmdInfo, const float eyeCenter[2], const float in[2],
			float red[2], float green[2], float blue[2]) {
			float theta[2];
			for (int i = 0; i < 2; i++) {
				const float unit = in[i];
				const float ndc =(unit - eyeCenter[i]);
				const float pixels = ndc * hmdInfo.heightPixels;
				const float meters = pixels * hmdInfo.widthMeters / hmdInfo.widthPixels;
				const float tanAngle = meters / hmdInfo.MetersPerTanAngleAtCenter;
				theta[i] = tanAngle;
			}
#if 1
			theta[1] += GetYOffset();
#endif
			const float rsq = theta[0] * theta[0] + theta[1] * theta[1];
			//卡特摩尔样条插值
			//CatmullRom插值。所谓样条曲线是指给定一组控制点而得到一条曲线，曲线的大致形状由这些点予以控制，
			//一般可分为插值样条和逼近样条两种，插值样条通常用于数字化绘图或动画的设计，
			//逼近样条一般用来构造物体的表面。CatmullRom样条与上一节所讲的B样条很相似，
			//不同在于CatmullRom样条的曲线会经过其每一个控制点。
			// DistortionFnScaleRadiusSquaredChroma(rsq);
			const Vector3f chromaScale = DistortionFnScaleRadiusSquared_V3(rsq);

			for (int i = 0; i < 2; i++) {
				red[i] = chromaScale[0] * theta[i];
				green[i] = chromaScale[1] * theta[i];
				blue[i] = chromaScale[2] * theta[i];
			}
		}
		*/
		Vector3f Distortion::DistortionFnScaleRadiusSquared_V3(float rsq)const
		{
			Vector3f scale;
			const int NumSegments = GetSegment() + 1;
			const float MaxR = 1.0;
			float scaledRsq = (float)(NumSegments - 1) * rsq / (MaxR * MaxR);

			scale.x = EvalCatmullRomSpline(m_KR, scaledRsq, NumSegments);
			scale.y = EvalCatmullRomSpline(m_KG, scaledRsq, NumSegments);
			scale.z = EvalCatmullRomSpline(m_KB, scaledRsq, NumSegments);
			return scale;
		}

		Vector4f Distortion::DistortionFnScaleRadiusSquared_V4(float rsq)const
		{
			Vector4f scale;
			const int NumSegments = GetSegment() + 1;
			const float MaxR = 1.0;
			float scaledRsq = (float)(NumSegments - 1) * rsq / (MaxR * MaxR);

			scale.x = EvalCatmullRomSpline(m_KR, scaledRsq, NumSegments);
			scale.y = EvalCatmullRomSpline(m_KG, scaledRsq, NumSegments);
			scale.z = EvalCatmullRomSpline(m_KB, scaledRsq, NumSegments);
			scale.w = EvalCatmullRomSpline(m_KL, scaledRsq, NumSegments);
			return scale;
		}
		Vector3f Distortion::DistortionFnScaleRadiusSquared_Unreal(float rsq)const
		{
			Vector3f scale;
			const int NumSegments = GetSegment() + 1;
			const float MaxR = 2.0;// 反向运算的半径值为1^2 + 1^2 = 2 ; 所以R^2 = 2;下面的分母不用再除MaxR * MaxR，直接除MaxR了
			float scaledRsq = (float)(NumSegments - 1) * rsq / (MaxR);

			scale.x = EvalCatmullRomSpline(m_KRT, scaledRsq, NumSegments);
			scale.y = EvalCatmullRomSpline(m_KGT, scaledRsq, NumSegments);
			scale.z = EvalCatmullRomSpline(m_KBT, scaledRsq, NumSegments);
			return scale;
		}
		float Distortion::EvalCatmullRomSpline(float const *K, float scaledVal, int NumSegments)const
		{
			//int const NumSegments = LensConfig::NumCoefficients;
			if (NumSegments < 5)// 保护代码，少于4个畸变引导点(0...4) 的时候不发生缩放变化
				return 1;

			float scaledValFloor = floorf(scaledVal);
            scaledValFloor = fmax(0.0f, fmin((float)(NumSegments - 1), scaledValFloor));

			float t = scaledVal - scaledValFloor;
			int k = (int)scaledValFloor;

			float p0, p1;
			float m0, m1;
			if (k == 0)
			{   // Curve starts at 1.0 with gradient K[1]-K[0]
				p0 = K[0]; //1.0f;
				m0 = (K[1] - K[0]);    // general case would have been (K[1]-K[-1])/2
				p1 = K[1];
				m1 = 0.5f * (K[2] - K[0]);
			}
			else if (k < NumSegments - 2)
			{   // General case
				p0 = K[k];
				m0 = 0.5f * (K[k + 1] - K[k - 1]);
				p1 = K[k + 1];
				m1 = 0.5f * (K[k + 2] - K[k]);
			}
			else if (k == NumSegments - 2)
			{   // Last tangent is just the slope of the last two points.
				p0 = K[NumSegments - 2];
				m0 = 0.5f * (K[NumSegments - 1] - K[NumSegments - 2]);
				p1 = K[NumSegments - 1];
				m1 = K[NumSegments - 1] - K[NumSegments - 2];
			}
			else if (k == NumSegments - 1)
			{   // Beyond the last segment it's just a straight line
				p0 = K[NumSegments - 1];
				m0 = K[NumSegments - 1] - K[NumSegments - 2];
				p1 = p0 + m0;
				m1 = m0;
			}

			float omt = 1.0f - t;
			float res = (p0 * (1.0f + 2.0f *   t) + m0 *   t) * omt * omt
				+ (p1 * (1.0f + 2.0f * omt) - m1 * omt) *   t *   t;

			return res;
		}
		void Distortion::SetDistortionParamet(int iSegment, bool bNoDispersion , float* fKR, float * fKG, float * fKB, float *fKL/* = NULL*/, float * fKRT, float * fKGT, float * fKBT)
		{
			SetSegment(iSegment);
			
			for (int iIndex = 0; iIndex <= 20; iIndex++)
			{
				m_KR[iIndex] = m_KG[iIndex] = m_KB[iIndex] = m_KRT[iIndex] = m_KGT[iIndex] = m_KBT[iIndex] = m_KL[iIndex] = 1.0f;
				
			}

			if (iSegment)
			{
				memcpy(&(m_KR[1]), fKR, iSegment * sizeof(float));
				memcpy(&(m_KG[1]), fKG, iSegment * sizeof(float));
				memcpy(&(m_KB[1]), fKB, iSegment * sizeof(float));
				if (fKL)
				{	// 注意：亮度曲线下标0需要设定，下标iSegment不需要改变
					memcpy(&(m_KL[0]), fKL, (iSegment)* sizeof(float));
				}				
				if (fKRT && fKGT && fKBT)
				{
					memcpy(&(m_KRT[1]), fKRT, iSegment * sizeof(float));
					memcpy(&(m_KGT[1]), fKGT, iSegment * sizeof(float));
					memcpy(&(m_KBT[1]), fKBT, iSegment * sizeof(float));
#ifdef _DEBUG
					float fKT[21];
					*fKT = 1;
					ReCalculationKT(fKT);
					char szLog[1024];
					sprintf(szLog , "============ KT [] = {");
					for (int iIndex = 0; iIndex < 21; iIndex++)
					{
						sprintf(szLog + strlen(szLog), " %2.6f / %2.6f  ", fKT[iIndex], m_KGT[iIndex]);
						if (iIndex != 20)
						{
							strcat(szLog , ",");
						}
					}
					strcat(szLog, " };");
					MOJING_TRACE(g_APIlogger , szLog);
					DistortionVertexBuffer* pBuffer = BuildDistortionVertexBuffer(40, 40);
					delete pBuffer;
#endif
				}
				else
				{// 需要反算KT?
					float fKT[21];
					ReCalculationKT(fKT);
					fKT[0] = 1;
					memcpy(&(m_KRT[1]), fKT + 1, iSegment * sizeof(float));
					memcpy(&(m_KGT[1]), fKT + 1, iSegment * sizeof(float));
					memcpy(&(m_KBT[1]), fKT + 1, iSegment * sizeof(float));
				}
				if (bNoDispersion || (memcmp(fKB, fKR, sizeof(float)* iSegment) == 0 && memcmp(fKG, fKR, sizeof(float)* iSegment) == 0))
				{
					SetNoDispersion(true);
				}
				else
				{
					SetNoDispersion(false);
				}
			}		
			else
			{
				SetNoDispersion(true);
			}
		}
		void Distortion::ReCalculationKT(float* fKT)
		{
			const int NumSegments = GetSegment() + 1;
			const double KG_MAX = m_KG[m_iSegment];
			// K 的索引为R^2 , KT的索引是P^2
			for (int iKT = 1; iKT <= m_iSegment; iKT++)
			{
				double RSQ_P = iKT * 2.0f / m_iSegment;
				double P = sqrt(RSQ_P);
				// 接下来计算P对应的R
				double fRSQMin = 0, fRSQMax = m_KG[GetSegment()] * 1.8;
				double fRSQNow = (fRSQMin + fRSQMax) / 2.0f;
				double fKG = 1;
				double fRNow = sqrt(fRSQNow);
				double fDiff = 0;
				int iSerchCount = 0;
				do 
				{
					iSerchCount++;
					fRSQNow = (fRSQMin + fRSQMax) / 2.0f;
					fRNow = sqrt(fRSQNow);
					double scaledRsq = m_iSegment * fRSQNow;
					fKG = EvalCatmullRomSpline(m_KG, scaledRsq, NumSegments);
					double fP = fKG * fRNow; // 要不要 / KG_MAX？？
					fDiff = RSQ_P - fP*fP;// 判定我们需要的P^2是否已经找到。如果误差<1E-6则认为找到。
					if (iSerchCount <= 30)
					{
						if (fDiff > 0)
						{
							fRSQMin = fRSQNow;
						}
						else
						{
							fRSQMax = fRSQNow;
						}
					}
					else
					{// 几乎没有折半的区间了，不找了。2^-30或1E-7
						break;// 不找了。。。。。
					}
				} while (fabs(fDiff) > 1e-6 );

				fKT[iKT] = 1 / fKG;
			}
		}
		int Distortion::GetDistortionParamet(float* fKR, float * fKG, float * fKB)
		{
			int iRet = GetSegment();
			if (fKR)
			{
				memcpy(fKR, &(m_KR[1]), iRet * sizeof(float));
			}

			if (fKG)
			{
				memcpy(fKG, &(m_KG[1]), iRet * sizeof(float));
			}

			if (fKB)
			{
				memcpy(fKB, &(m_KB[1]), iRet * sizeof(float));
			}

			return iRet;
		}

		bool Distortion::SaveToFile(const char* szFilePathName)const
		{
			JSON *pJson = JSON::CreateObject();
			pJson->AddStringItem("Class", "GlassesConfig");
			time_t now = time(NULL);
			struct tm *pNow = localtime(&now);
			char szToday[16];
			sprintf(szToday, "%04d%02d%02d", pNow->tm_year + 1900, pNow->tm_mon + 1, pNow->tm_mday);
			pJson->AddStringItem("FileVersion", szToday);

			JSON *pGlassesConfigs = pJson->CreateArray();
			JSON *pCurrentGlass = pJson->CreateObject();
			pCurrentGlass->AddStringItem("GlassesName", MojingSDK_GetGlasses());
			pCurrentGlass->AddNumberItem("ClassVersion" , 1);
			//pCurrentGlass->AddStringItem("ReleaseDate", szToday);
			pCurrentGlass->AddNumberItem("ReleaseDate", atoi(szToday));
			JSON *pDistortion = pJson->CreateObject();
			pDistortion->AddNumberItem("Segments", GetSegment());
			
			if (GetSegment() > 1)
			{
				pDistortion->AddNumberItem("MetersPerTanAngleAtCenter", GetMetersPerTanAngleAtCenter());
				pDistortion->AddNumberItem("LensSeparation", GetLensSeparation());
				pDistortion->AddNumberItem("FOV", GetFOV());

				JSON *pKG = pDistortion->CreateArray();
				JSON *pKR = pDistortion->CreateArray();
				JSON *pKB = pDistortion->CreateArray();
				for (int i = 0; i < GetSegment(); i++)
				{
					pKG->AddArrayNumber(m_KG[i+1]);
					pKR->AddArrayNumber(m_KR[i + 1]);
					pKB->AddArrayNumber(m_KB[i + 1]);
				}
				pDistortion->AddItem("KG", pKG);
				pDistortion->AddItem("KR", pKR);
				pDistortion->AddItem("KB", pKB);
			}
			pCurrentGlass->AddItem("Distortion", pDistortion);

			pGlassesConfigs->AddArrayElement(pCurrentGlass);

			pJson->AddItem("GlassesConfig", pGlassesConfigs);

			return pJson->Save(szFilePathName, g_EncKey);
		}


	}
}
