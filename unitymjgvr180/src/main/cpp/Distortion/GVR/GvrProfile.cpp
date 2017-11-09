#include "GvrProfile.h"
#include <math.h>
#include "../MojingDistortion.h"
#include "../../Profile/ProfileV2/DayDreamParameters.h"
#include "../../MojingManager.h"
#include "../../Parameters/MojingParameters.h"
#ifdef LOG4CPLUS_IMPORT
#include "../../3rdPart/log4cplus/LogInterface.h"
#else
#include "../../LogTraker/LogInterface.h"
#endif
#ifdef ENABLE_LOGGER
extern MojingLogger g_APIlogger;
#endif

#define Deg2Rad(D) ((D) * PI / 180.0f)
#define Rad2Deg(R) ((R) * 180.0f / PI)
namespace Baofeng
{
	namespace Mojing
	{
		GvrProfile::GvrProfile():
			m_fpVertices(NULL),
			m_fpTexUV(NULL),
			m_pIndices(NULL)
		{
		}


		GvrProfile::~GvrProfile()
		{
			ReleaseBuffers();
		}

		void GvrProfile::ReAllocBuffers(const int iWidth, const int iHeight)
		{
			ReleaseBuffers();
			m_fpVertices = new float[2 * 3 * iWidth * iHeight];
			m_fpTexUV = new float[2 * 2 * iWidth * iHeight];
			m_pIndices = new unsigned short[2 * (iWidth - 1) * (iHeight - 1) * 6];
			SetWidth(iWidth);
			SetHeight(iHeight);
		}
		void GvrProfile::ReleaseBuffers()
		{
			m_iWidth = m_iHeight = 0;
			
			if (m_fpVertices)
				delete m_fpVertices;
			if (m_fpTexUV)
				delete m_fpTexUV;

			if (m_pIndices)
				delete m_pIndices;

			m_fpVertices = NULL;
			m_fpTexUV = NULL;
			m_pIndices = NULL;
		}

		struct EyeVisibleTanAngles GvrProfile::GetLeftEyeVisibleTanAngles()const
		{
			// Tan-angles from the max FOV.
			float fovLeft = tanf(Deg2Rad (- m_Viewer.maxFOV.outer));
			float fovTop = tanf(Deg2Rad(m_Viewer.maxFOV.upper));
			float fovRight = tanf(Deg2Rad(m_Viewer.maxFOV.inner));
			float fovBottom = tanf(Deg2Rad(-m_Viewer.maxFOV.lower));
			

			// Viewport size.
			float halfWidth = m_Screen.width / 4;
			float halfHeight = m_Screen.height / 2;
			// Viewport center, measured from left lens position.
			float centerX = m_Viewer.lenses.separation / 2 - halfWidth;
			float centerY = -VerticalLensOffset();
			float centerZ = m_Viewer.lenses.screenDistance;
			// Tan-angles of the viewport edges, as seen through the lens.
			float screenLeft = m_Viewer.distortion.distort((centerX - halfWidth) / centerZ);
			float screenTop = m_Viewer.distortion.distort((centerY + halfHeight) / centerZ);
			float screenRight = m_Viewer.distortion.distort((centerX + halfWidth) / centerZ);
			float screenBottom = m_Viewer.distortion.distort((centerY - halfHeight) / centerZ);
			// Compare the two sets of tan-angles and take the value closer to zero on each side.
			
			EyeVisibleTanAngles FovRect(fovLeft, fovTop, fovRight, fovBottom); 
			EyeVisibleTanAngles ScreenRect(screenLeft, screenTop, screenRight, screenBottom);

			EyeVisibleTanAngles ret = ScreenRect & FovRect; 
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, "LeftEyeVisibleTanAngles = " << ret.ToString() << " , FovRect = " << FovRect.ToString() << " , ScreenRect = " << ScreenRect.ToString());
#endif


			return ret;
		}
		struct  EyeVisibleTanAngles  GvrProfile::GetLeftEyeNoLensTanAngles()const
		{
			// Tan-angles from the max FOV.
			float fovLeft = m_Viewer.distortion.distortInv(tanf(Deg2Rad(-m_Viewer.maxFOV.outer)));
			float fovTop = m_Viewer.distortion.distortInv(tanf(Deg2Rad(m_Viewer.maxFOV.upper)));
			float fovRight = m_Viewer.distortion.distortInv(tanf(Deg2Rad(m_Viewer.maxFOV.inner)));
			float fovBottom = m_Viewer.distortion.distortInv(tanf(Deg2Rad(-m_Viewer.maxFOV.lower)));
			// Viewport size.
			float halfWidth = m_Screen.width / 4;
			float halfHeight = m_Screen.height / 2;
			// Viewport center, measured from left lens position.
			float centerX = m_Viewer.lenses.separation / 2 - halfWidth;
			float centerY = -VerticalLensOffset();
			float centerZ = m_Viewer.lenses.screenDistance;
			// Tan-angles of the viewport edges, as seen through the lens.
			float screenLeft = (centerX - halfWidth) / centerZ;
			float screenTop = (centerY + halfHeight) / centerZ;
			float screenRight = (centerX + halfWidth) / centerZ;
			float screenBottom = (centerY - halfHeight) / centerZ;
			// Compare the two sets of tan-angles and take the value closer to zero on each side.
			EyeVisibleTanAngles FovRect(fovLeft, fovTop, fovRight, fovBottom);
			EyeVisibleTanAngles ScreenRect(screenLeft, screenTop, screenRight, screenBottom);

			EyeVisibleTanAngles ret = ScreenRect & FovRect; 
			// 			EyeVisibleTanAngles ret;
// 			ret.m_fLeft = fmax(fovLeft, screenLeft);
// 			ret.m_fTop = fmin(fovTop, screenTop);
// 			ret.m_fRight = fmin(fovRight, screenRight);
// 			ret.m_fBottom = fmax(fovBottom, screenBottom);

#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, "EyeNoLensTanAngles = " << ret.ToString() << " , FovRect = " << FovRect.ToString() << " , ScreenRect = " << ScreenRect.ToString());
#endif
			return ret;
		}
		Rectf GvrProfile::GetLeftEyeVisibleScreenRect(EyeVisibleTanAngles undistortedFrustum) const
		{
			float dist = m_Viewer.lenses.screenDistance;
			float eyeX = (m_Screen.width - m_Viewer.lenses.separation) / 2;
			float eyeY = VerticalLensOffset() + m_Screen.height / 2;
			float left = (undistortedFrustum.m_fLeft * dist + eyeX) / m_Screen.width;
			float top = (undistortedFrustum.m_fTop * dist + eyeY) / m_Screen.height;
			float right = (undistortedFrustum.m_fRight * dist + eyeX) / m_Screen.width;
			float bottom = (undistortedFrustum.m_fBottom * dist + eyeY) / m_Screen.height;
			Rectf Ret(left, bottom, right - left, top - bottom);
			return Ret;
		}

		float  GvrProfile::GetMaxRadius(EyeVisibleTanAngles tanAngleRect)
		{
			float x = fmax(abs(tanAngleRect.m_fLeft), abs(tanAngleRect.m_fRight));
			float y = fmax(abs(tanAngleRect.m_fTop), abs(tanAngleRect.m_fBottom));
			return sqrtf(x * x + y * y);
		}
		
		DoubleArray solveLinear(double* a, double* y, int numCoefficients) {
			int n = numCoefficients;

			// Gaussian elimination (no row exchange) to triangular matrix.
			// The input matrix is a A^T A product which should be a positive
			// definite symmetrical matrix, and if I remember my linear
			// algebra right this implies that the pivots will be nonzero and
			// calculations sufficiently accurate without needing row
			// exchange.
			for (int j = 0; j < n - 1; ++j) {
				for (int k = j + 1; k < n; ++k) {
					double p = a[k*numCoefficients + j] / a[j*numCoefficients + j];
					for (int i = j + 1; i < n; ++i) {
						a[k*numCoefficients + i] -= p * a[j*numCoefficients + i];
					}
					y[k] -= p * y[j];
				}
			}
			// From this point on, only the matrix elements a[j][i] with i>=j are
			// valid. The elimination doesn't fill in eliminated 0 values.

			//double[] x = new double[n];
			DoubleArray x;
			x.resize(numCoefficients);
			// Back substitution.
			for (int j = n - 1; j >= 0; --j) {
				double v = y[j];
				for (int i = j + 1; i < n; ++i) {
					v -= a[j*numCoefficients + i] * x[i];
				}
				x[j] = v / a[j*numCoefficients + j];
			}

			return x;
		}
		DoubleArray solveLeastSquares(double* matA, double* vecY, int numSamples, int numCoefficients) 
		{
			// int numSamples = matA.GetLength(0);
			// int numCoefficients = matA.GetLength(1);
			// Calculate transpose(A) * A
			double* matATA = new double[numCoefficients * numCoefficients];
			for (int k = 0; k < numCoefficients; ++k) 
			{
				for (int j = 0; j < numCoefficients; ++j) 
				{
					double sum = 0.0;
					for (int i = 0; i < numSamples; ++i) {
						sum += matA[i * numCoefficients + j] * matA[i * numCoefficients + k];
					}
					matATA[j* numCoefficients+ k] = sum;
				}
			}

			// Calculate transpose(A) * y
			double* vecATY = new double[numCoefficients];
			for (int j = 0; j < numCoefficients; ++j) {
				double sum = 0.0;
				for (int i = 0; i < numSamples; ++i) {
					sum += matA[i * numCoefficients + j] * vecY[i];
				}
				vecATY[j] = sum;
			}

			// Now solve (A * transpose(A)) * x = transpose(A) * y.
			DoubleArray ret = solveLinear(matATA, vecATY, numCoefficients);

			delete matATA;
			delete vecATY;
			return ret;
		}

		void  Viewer::ApproximateInverse(float maxRadius /*= 1*/, int numSamples /*= 100*/)
		{
			const int numCoefficients = 6;

			// R + K1*R^3 + K2*R^5 = r, with R = rp = distort(r)
			// Repeating for numSamples:
			//   [ R0^3, R0^5 ] * [ K1 ] = [ r0 - R0 ]
			//   [ R1^3, R1^5 ]   [ K2 ]   [ r1 - R1 ]
			//   [ R2^3, R2^5 ]            [ r2 - R2 ]
			//   [ etc... ]                [ etc... ]
			// That is:
			//   matA * [K1, K2] = y
			// Solve:
			//   [K1, K2] = inverse(transpose(matA) * matA) * transpose(matA) * y
			double* matA = new double[numSamples * numCoefficients];
			double* vecY = new double[numSamples];
			for (int i = 0; i < numSamples; ++i)
			{
				float r = maxRadius * (i + 1) / (float)numSamples;
				double rp = distortion.distort(r);
				double v = rp;
				for (int j = 0; j < numCoefficients; ++j) {
					v *= rp * rp;
					matA[i*numCoefficients+ j] = v;
				}
				vecY[i] = r - rp;
			}
			DoubleArray vecK = solveLeastSquares(matA, vecY, numSamples , numCoefficients);
			// Convert to float for use in a fresh Distortion object.
// 			DoubleArray coefficients ;
// 			for (int i = 0; i < vecK.size(); ++i) {
// 				coefficients[i] = (float)vecK[i];
// 			}

			GvrDistortion Ret(vecK);
			delete matA;
			delete vecY;
			inverse = Ret;
		}

		bool GvrProfile::InitFromDayDreamParameters(const CDayDreamParameters &DDP)
		{
			if (DDP.GetNumK() <= 0)
				return false;
			ReleaseBuffers();
			MOJING_TRACE(g_APIlogger , "Using Parmate : " << DDP.GetCompanyName() << " :: " << DDP.GetViewerName());
			/************************************************************************/
			/* Viewer                                                                      */
			/************************************************************************/
			// Coef
			DoubleArray DA;
			for (int i = 0; i < DDP.GetNumK(); i++)
			{
				DA.push_back(DDP[i]);
			}

			m_Viewer.SetCoef(DA);

			// Lenses
			m_Viewer.lenses.screenDistance = DDP.GetScreenToLens();
			m_Viewer.lenses.alignment = Lenses::AlignCenter;
			switch (DDP.GetScreenVerticalAlignmentType())
			{
			case SVA_BOTTOM:
				m_Viewer.lenses.alignment = Lenses::AlignBottom;
				break;
			case SVA_TOP:
				m_Viewer.lenses.alignment = Lenses::AlignTop;
				break;
			default:
				m_Viewer.lenses.alignment = Lenses::AlignCenter;
			}
			m_Viewer.lenses.offset = DDP.GetScreenVerticalAlignment();
			m_Viewer.lenses.separation = DDP.GetInterLens();

			// FOV
			m_Viewer.maxFOV.inner = DDP.GetInnerFOV();
			m_Viewer.maxFOV.outer = DDP.GetOuterFOV();
			m_Viewer.maxFOV.lower = DDP.GetBottomFOV();
			m_Viewer.maxFOV.upper = DDP.GetTopFOV();


			/************************************************************************/
			/* Screen                                                               */
			/************************************************************************/
			Parameters* pParameters = Manager::GetMojingManager()->GetParameters();
			MojingDisplayParameters * pDisplay = pParameters->GetDisplayParameters();
			m_Screen.border = pDisplay->GetMobileEdge();
			m_Screen.width = pDisplay->GetScreenWidthMeter();
			m_Screen.height = pDisplay->GetScreenHeightMeter();

#ifdef _DEBUG
			m_Viewer.ApproximateInverse(1);
			MOJING_TRACE(g_APIlogger, "R = 1 " << " , inverse =" << m_Viewer.inverse.ToString());
#endif

			EyeVisibleTanAngles LeftEyeNoLensTanAngles = GetLeftEyeNoLensTanAngles();
			float R = GetMaxRadius(LeftEyeNoLensTanAngles);
			m_Viewer.ApproximateInverse(R);
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, "MaxR = " << R << " , inverse =" << m_Viewer.inverse.ToString());
#endif
			return true;
		}
#define CLAMP01(__V__) ((__V__) < 0 ?  0 : (__V__) > 1? 1 : (__V__)) 
#define LERP(__FROM__ , __TO__ , __RATE__) ((__FROM__) * (1-__RATE__) + (__TO__) * (__RATE__))
		int  GvrProfile::ComputeDistortionBuffer(const int iWidth /*= 32*/, const int iHeight /*= 32*/, bool bDistortVertices /*= true*/, Mesh_820 * pMesh /*= NULL*/)
		{		
			// 用户体验范围
			EyeVisibleTanAngles lensFrustum =GetLeftEyeVisibleTanAngles();

			// 不考虑畸变时，屏幕显示的范围。小于lensFrustum
			EyeVisibleTanAngles noLensFrustum = GetLeftEyeNoLensTanAngles();
			Rectf viewport = GetLeftEyeVisibleScreenRect(noLensFrustum);
#ifdef _DEBUG
			MOJING_TRACE(g_APIlogger, "viewport[x,y,w,h] = " << viewport.x << " , " << viewport.y << " , " << viewport.w << " , " << viewport.h);
#endif
			ReAllocBuffers(iWidth, iHeight);
			int halfwidth = iWidth / 2;
			int halfheight = iHeight / 2;
			int iidx = 0;
			for (int e = 0, vidx = 0; e < 2; e++) 
			{
				for (int j = 0; j < iHeight; j++) 
				{
					for (int i = 0; i < iWidth; i++, vidx++) 
					{

						float u = (float)i / (iWidth - 1);
						float v = (float)j / (iHeight - 1);
						float s, t;  // The texture coordinates in StereoScreen to read from.
						bool bOutOfBound = false;
						if (bDistortVertices) 
						{
							// Grid points regularly spaced in StreoScreen, and barrel distorted in the mesh.
							s = u;
							t = v;

							float x = LERP(lensFrustum[0], lensFrustum[2], u);
							float y = LERP(lensFrustum[3], lensFrustum[1], v);
							float d = sqrt(x * x + y * y);
							if (d != 0)
							{
								float r = m_Viewer.distortion.distortInv(d);
								float p = d == 0 ? 0 : x * r / d;
								float q = d == 0 ? 0 : y * r / d;

								u = (p - noLensFrustum[0]) / (noLensFrustum[2] - noLensFrustum[0]);
								v = (q - noLensFrustum[3]) / (noLensFrustum[1] - noLensFrustum[3]);
							}
							else
							{
								u = 0.5;
								v = 0.5;
							}
							bOutOfBound = (i == 0 || j == 0 || i == (iWidth - 1) || j == (iHeight - 1));
						}
						else {
							// Grid points regularly spaced in the mesh, and pincushion distorted in
							// StereoScreen.
							float p = LERP(noLensFrustum[0], noLensFrustum[2], u);
							float q = LERP(noLensFrustum[3], noLensFrustum[1], v);
							float r = sqrt(p * p + q * q);
							float d = m_Viewer.distortion.distort(r);
							float x = r == 0 ? 0 : p * d / r;
							float y = r == 0 ? 0 : q * d / r;
							float XBound = (x - lensFrustum[0]) / (lensFrustum[2] - lensFrustum[0]);
							float YBound = (y - lensFrustum[3]) / (lensFrustum[1] - lensFrustum[3]);
							s = CLAMP01(XBound);
							t = CLAMP01(YBound);
							bOutOfBound = (XBound <= 0 || YBound <= 0 || XBound >= 1 || YBound >= 1);								
						}
						// Convert u,v to mesh screen coordinates.
						// 程序执行到这里， u , v是tan值，需要转换为屏幕坐标。
						// U = u * 


						float aspect = m_Screen.width / m_Screen.height;
#if 1
						u = (viewport.x + u * viewport.w - 0.5f) * aspect;
#else
						// 魔镜的上屏Mesh算法是不依赖于长宽比的。
						u = (viewport.x + u * viewport.w - 0.5f)/* / aspect*/;
						u *= 2; 
#endif					
						v = viewport.y + v * viewport.h - 0.5f;
						// 整理取值范围
						
						v *= 2;
						// Adjust s to account for left/right split in StereoScreen.
// 						s = (s + e) / 2; 
// 						s = (s - 0.5 *e) * 4 - 1;
 						s = s * 2 - 1;
						t = t * 2 - 1;

						m_fpVertices[3 * vidx + 0] = u;
						m_fpVertices[3 * vidx + 1] = v;

						// 我们实际上没有使用这个数据，所以我们考虑将其变为只是是否超出被绘制的畸变后边界的数据。
						// 如果超出了，那么是0，否则是1
						// m_fpVertices[3 * vidx + 2] = bOutOfBound ? 0 : 1;
						m_fpVertices[3 * vidx + 2] = 1;
						
						m_fpTexUV[2 * vidx + 0] = s;
						m_fpTexUV[2 * vidx + 1] = t;


						if (pMesh)
						{
							std::vector<float> &V = (e == 0 ?pMesh->vertices_left :pMesh->vertices_right);
							V.push_back(u);
							V.push_back(v);

							V.push_back(s);
							V.push_back(t);
							V.push_back(s);
							V.push_back(t);
							V.push_back(s);
							V.push_back(t);
						}

						// index
						/*if (i != 0 && j != 0)
						{
							if ((i <= halfwidth) == (j <= halfheight)) {
								// Quad diagonal lower left to upper right.
								m_pIndices[iidx++] = vidx;
								m_pIndices[iidx++] = vidx - iWidth;
								m_pIndices[iidx++] = vidx - iWidth - 1;
								m_pIndices[iidx++] = vidx - iWidth - 1;
								m_pIndices[iidx++] = vidx - 1;
								m_pIndices[iidx++] = vidx;
							}
							else {
								// Quad diagonal upper left to lower right.
								m_pIndices[iidx++] = vidx - 1;
								m_pIndices[iidx++] = vidx;
								m_pIndices[iidx++] = vidx - iWidth;
								m_pIndices[iidx++] = vidx - iWidth;
								m_pIndices[iidx++] = vidx - iWidth - 1;
								m_pIndices[iidx++] = vidx - 1;
							}

							if (pMesh && e == 0)
							{
								pMesh->indices.push_back(m_pIndices[iidx - 6]);
								pMesh->indices.push_back(m_pIndices[iidx - 5]);
								pMesh->indices.push_back(m_pIndices[iidx - 4]);
								pMesh->indices.push_back(m_pIndices[iidx - 3]);
								pMesh->indices.push_back(m_pIndices[iidx - 2]);
								pMesh->indices.push_back(m_pIndices[iidx - 1]);
							}
						}*/
						if (i < iWidth - 1 && j < iHeight - 1)
						{
							short PT00 = iWidth*iHeight * e + j * iWidth + i;
							short PT01 = PT00 + 1;
							short PT10 = iWidth*iHeight * e + j * iWidth + i + iWidth;
							short PT11 = PT10 + 1;

							if ((i <= halfwidth) ^ (j <= halfheight)) {
								// Quad diagonal lower left to upper right.
								m_pIndices[iidx++] = PT00;
								m_pIndices[iidx++] = PT01;
								m_pIndices[iidx++] = PT11;
								m_pIndices[iidx++] = PT00;
								m_pIndices[iidx++] = PT11;
								m_pIndices[iidx++] = PT10;
							}
							else {
								// Quad diagonal upper left to lower right.
								m_pIndices[iidx++] = PT00;
								m_pIndices[iidx++] = PT01;
								m_pIndices[iidx++] = PT10;
								m_pIndices[iidx++] = PT10;
								m_pIndices[iidx++] = PT01;
								m_pIndices[iidx++] = PT11;
							}

							if (pMesh && e == 0)
							{
								pMesh->indices.push_back(m_pIndices[iidx - 6]);
								pMesh->indices.push_back(m_pIndices[iidx - 5]);
								pMesh->indices.push_back(m_pIndices[iidx - 4]);
								pMesh->indices.push_back(m_pIndices[iidx - 3]);
								pMesh->indices.push_back(m_pIndices[iidx - 2]);
								pMesh->indices.push_back(m_pIndices[iidx - 1]);
							}
						}
					}
				}
				float w = lensFrustum[2] - lensFrustum[0];
				lensFrustum[0] = -(w + lensFrustum[0]);
				lensFrustum[2] = w - lensFrustum[2];
				w = noLensFrustum[2] - noLensFrustum[0];
				noLensFrustum[0] = -(w + noLensFrustum[0]);
				noLensFrustum[2] = w - noLensFrustum[2];
				viewport.x = 1 - (viewport.x + viewport.w);
			}

			
#ifdef _DEBUG
			SaveDistortionBuffer("/sdcard/MojingSDK/log/MeshDURL.txt");
			SaveDDDDFile("/sdcard/MojingSDK/log/DDD2.dat");
#endif
			return iWidth * iHeight * 2;
		}	
		void GvrProfile::SaveDistortionBuffer(const char* pszFilePathName)const
		{
			if (GetWidth() && GetHeight() && m_fpTexUV && m_fpVertices)
			{
				FILE *pFile = fopen(pszFilePathName, "w");
				if (pFile)
				{
					int iWidth = GetWidth();
					int iHeight = GetHeight();
					fprintf(pFile, "/*DistortionBuffer = %d x %d , Index = %d*/\n", GetWidth(), GetHeight(), 2 * (iWidth - 1) * (iHeight - 1) * 6);
					
					for (int Eye = 0; Eye < 2; Eye++)
					{
						fprintf(pFile, "float %s[] = [\n", Eye == 0 ? "LeftEye" : "RightEye");
						fprintf(pFile, "/* X ,  Y*//*---VX--- ,  ---VY--- ,  ---S---  , ---T--- */\n");
						for (int i = 0; i < iHeight; i++)
						{
							for (int j = 0; j < iWidth; j++)
							{
								fprintf(pFile, "/*%02d , %02d*/ ", i, j);
								int iOffset = Eye * iHeight * iWidth + i * iWidth + j;

								fprintf(pFile, "%04f , %04f , %04f , %04f ", m_fpVertices[iOffset * 3 + 0], m_fpVertices[iOffset * 3 + 1], m_fpTexUV[iOffset * 2 + 0], m_fpTexUV[iOffset * 2 + 1]);

								if (i == iHeight-1 && j == iWidth - 1)
								{
									fprintf(pFile, "\n];\n\n");
								}
								else
								{
									fprintf(pFile, ",\n");
								}
							}// end of j
							fprintf(pFile, "\n");
							fflush(pFile);
						}
					}// end of Eye
					fclose(pFile);
				}// end of if pFile
			}// end of if
		}

		void GvrProfile::SaveDDDDFile(const char* pszFilePathName)const
		{
			if (GetWidth() && GetHeight() && m_fpTexUV && m_fpVertices)
			{
				FILE *pFile = fopen(pszFilePathName, "w");
				if (pFile)
				{
					int iWidth = GetWidth();
					int iHeight = GetHeight();
					int iCellCountWidth = GetWidth() - 1;
					int iCellCountHeight = GetHeight() - 1;
					unsigned int Tag = (*((long*)"MJDB"));
					fwrite(&Tag, 1, 4, pFile);
					fwrite(&iCellCountWidth, 1, 4, pFile);
					fwrite(&iCellCountHeight, 1, 4, pFile);
					/**
					写文件前，需要调整属性表的顺序
					*/
					float *pFileBuffer = new float[2 * iWidth* iHeight* 9];
					for (int eye = 0; eye < 2; eye++)
					{
						for (int i = 0; i < iHeight; i++)
						{
							// 注意：一体机中使用的数据是左右眼交织的，左眼一行、右眼一行
							int iLineBegin = 2 * i * iWidth + eye * iWidth;
							for (int j = 0; j < iWidth; j++)
							{
								int iIndexSrc = eye * iHeight * iWidth+ i * iWidth + j;
								int iIndexDest = iLineBegin + j;
								float * pAttributesFile = pFileBuffer + iIndexDest * 9;
								pAttributesFile[0] = pAttributesFile[2] = pAttributesFile[4] = m_fpTexUV[iIndexSrc * 2];
								pAttributesFile[1] = pAttributesFile[3] = pAttributesFile[5] = m_fpTexUV[iIndexSrc * 2 + 1];
								pAttributesFile[6] = 1;
								pAttributesFile[7] = m_fpVertices[iIndexSrc * 3];
								pAttributesFile[8] = m_fpVertices[iIndexSrc * 3 + 1];
							}
						}
					}

					fwrite(pFileBuffer, 1, 2 * iWidth* iHeight * 9 * sizeof(float), pFile);
					fflush(pFile);
					fclose(pFile);
					delete pFileBuffer;


				}
			}
		}
	}
}
