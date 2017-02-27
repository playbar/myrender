#include "GLProgram.h"
#include "../Base/GlUtils.h"
#define ONLE_USING_R_LIMIT 1
//#define HIGH_RES 1

namespace Baofeng
{
	namespace Mojing
	{
#if defined(MJ_OS_WIN32) || defined(MJ_OS_IOS) || defined(MJ_OS_ANDROID)
		const char* VertexShader_820 =
			"#version 300 es\n"
			"\n"
			"precision highp float;\n"
			"\n"
			"in vec2 position;      // Position\n"
			"in vec2 normal;        // Red\n"
			"in vec2 color;         // Green\n"
			"in vec2 texcoord0;     // Blue\n"
			"layout(location = 1) uniform vec4 scaleOffset;\n"
			"layout(location = 2) uniform mat4 textureMtx;\n"
			"out vec2 vTexcoordR;\n"
			"out vec2 vTexcoordG;\n"
			"out vec2 vTexcoordB;\n"
			"out vec2 vClipCoord;\n"
			"\n"
			"//-----------------------------------------------------------------------------\n"
			"void main()\n"
			"//-----------------------------------------------------------------------------\n"
			"{\n"
			"	float projIZ;\n"
			"   // Red\n"
			"   vec4 tmpTexCoords = textureMtx * vec4(normal, -1.0, 1.0);\n"
			"	projIZ = 1.0 / max( tmpTexCoords.z, 0.00001 );\n"
			"	vTexcoordR = vec2( tmpTexCoords.x * projIZ, tmpTexCoords.y * projIZ );\n"
			"	vTexcoordR = 0.5 * vTexcoordR + vec2(0.5, 0.5);\n"
			"\n"
			"   // Green\n"
			"   tmpTexCoords = textureMtx * vec4(color, -1.0, 1.0);\n"
			"	projIZ = 1.0 / max( tmpTexCoords.z, 0.00001 );\n"
			"	vTexcoordG = vec2( tmpTexCoords.x * projIZ, tmpTexCoords.y * projIZ );\n"
			"	vTexcoordG = 0.5 * vTexcoordG + vec2(0.5, 0.5);\n"
			"\n"
			"   // Blue\n"
			"   tmpTexCoords = textureMtx * vec4(texcoord0, -1.0, 1.0);\n"
			"	projIZ = 1.0 / max( tmpTexCoords.z, 0.00001 );\n"
			"	vTexcoordB = vec2( tmpTexCoords.x * projIZ, tmpTexCoords.y * projIZ );\n"
			"	vTexcoordB = 0.5 * vTexcoordB + vec2(0.5, 0.5);\n"
			"\n"
			"   vClipCoord =  color;\n"
			"\n"
			"	gl_Position = vec4(position.xy * scaleOffset.xy + scaleOffset.zw, 0.0, 1.0);\n"
			"}\n";  // 820 VS

		const char* FragmentShader_820 =
			"#version 300 es\n"
			"\n"
			"precision highp float;\n"
			"\n"
			"layout(location = 3) uniform vec2 eyeBorder;\n"
			"uniform sampler2D srcTex;\n"
			"in vec2 vTexcoordR;\n"
			"in vec2 vTexcoordG;\n"
			"in vec2 vTexcoordB;\n"
			"in vec2 vClipCoord;\n"
			"in float whichTexLayer;\n"
			"\n"
			"out vec4 outColor;\n"
			"\n"
			"//-----------------------------------------------------------------------------\n"
			"void main()\n"
			"//-----------------------------------------------------------------------------\n"
			"{\n"
			"    if( vClipCoord.x < -1.0 || vClipCoord.x > 1.0 || vClipCoord.y < -1.0f || vClipCoord.y > 1.0f ) { discard; }\n"
			"    else {\n"
			"    // Each color channel has a different texture coordinate\n"
			"    vec4 TexColorR = texture(srcTex, vec2( clamp( vTexcoordR.x, eyeBorder.x, eyeBorder.y), vTexcoordR.y));\n"
			"    vec4 TexColorG = texture(srcTex, vec2( clamp( vTexcoordG.x, eyeBorder.x, eyeBorder.y), vTexcoordG.y));\n"
			"    vec4 TexColorB = texture(srcTex, vec2( clamp( vTexcoordB.x, eyeBorder.x, eyeBorder.y), vTexcoordB.y));\n"
			"\n"
			"    // Combine the colors to get final results\n"
			"    outColor = vec4(TexColorR.r, TexColorG.g, TexColorB.b, TexColorR.a);\n"
			"    }\n"
			"\n"
			"}\n"
			"\n"; // 820 FS
		const char* VertexShader = "uniform mat4 Mvpm;\n"
			"uniform mat4 Texm;\n"
			"attribute vec4 Position;\n"
			"attribute vec2 TexCoord;\n"	// green
			"attribute vec2 TexCoord1;\n"	// .x = interpolated warp frac, .y = intensity scale
			"attribute vec2 dR;\n"		// red
			"attribute vec2 dB;\n"		// blue
			"varying  vec2 oTexCoord1r;\n"
			"varying  vec2 oTexCoord1g;\n"
			"varying  vec2 oTexCoord1b;\n"
			"varying  float	intensity;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = Mvpm * Position;\n"
			"	vec3 proj;\n"
			"	float projIZ;\n"
			"   proj = vec3( Texm * vec4(dR,-1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1r = vec2( proj.x * projIZ, proj.y * projIZ );\n"
			"   proj = vec3( Texm * vec4(TexCoord,-1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1g = vec2( proj.x * projIZ, proj.y * projIZ );\n"
			"   proj = vec3( Texm * vec4(dB,-1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1b = vec2( proj.x * projIZ, proj.y * projIZ );\n"
			""
			"	intensity = TexCoord1.y;\n"
			"}\n";
		const char* VertexShaderLayout = "uniform mat4 Mvpm;\n"
			"uniform mat4 Texm;\n"
			"attribute vec4 Position;\n"
			"attribute vec2 TexCoord;\n"	// green
			"attribute vec2 TexCoord1;\n"	// .x = interpolated warp frac, .y = intensity scale
			"attribute vec2 dR;\n"		// red
			"attribute vec2 dB;\n"		// blue
			"varying  vec2 oTexCoord1r;\n"
			"varying  vec2 oTexCoord1g;\n"
			"varying  vec2 oTexCoord1b;\n"
			"varying  float	intensity;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = Mvpm * Position;\n"
			"	vec3 proj;\n"
			"	float projIZ;\n"
			"   proj = vec3( Texm * vec4(dR,1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1r = vec2( proj.x * projIZ, proj.y * projIZ );\n"
			"   proj = vec3( Texm * vec4(TexCoord,1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1g = vec2( proj.x * projIZ, proj.y * projIZ );\n"
			"   proj = vec3( Texm * vec4(dB,1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1b = vec2( proj.x * projIZ, proj.y * projIZ );\n"
			""
			"	intensity = TexCoord1.y;\n"
			"}\n";
		const char* VertexShaderDistortionRange = VertexShader;

		const char* FragmentShader = "uniform sampler2D Texture0;\n"
			"varying highp vec2 oTexCoord1r;\n"
			"varying highp vec2 oTexCoord1g;\n"
			"varying highp vec2 oTexCoord1b;\n"
			"uniform mediump vec2 TexClipX;\n"
			//				"uniform highp float TestR;\n"
			"varying mediump float	intensity;\n"
			"void main()\n"
			"{\n"
			"   const lowp float Edge = 0.01;\n"
			"	highp vec4 color1r = texture2D(Texture0, oTexCoord1r);\n"
			"	highp vec4 color1g = texture2D(Texture0, oTexCoord1g);\n"
			"	highp vec4 color1b = texture2D(Texture0, oTexCoord1b);\n"
			"	mediump float TestR = step(TexClipX.x + Edge , oTexCoord1r.s) * step(oTexCoord1r.s , TexClipX.y - Edge) * step(Edge , oTexCoord1r.t) * step(oTexCoord1r.t , 1.0 - Edge);\n"
#if ONLE_USING_R_LIMIT// 只用红色通道过滤边界
			"	highp vec4 color1 = vec4( TestR * color1r.x, TestR * color1g.y, TestR * color1b.z, 1.0);\n"//注意: 主图层的Alpha是1
			//"	highp vec4 color1 = vec4( 0.0, 0.0, 1.0 , TestR);\n"//注意: 主图层的Alpha是1
#else
			"	mediump float TestG = TestR * step(TexClipX.x + Edge , oTexCoord1g.s) * step(oTexCoord1g.s , TexClipX.y - Edge) * step(Edge , oTexCoord1g.t) * step(oTexCoord1g.t , 1.0 - Edge);\n"
			"	mediump float TestB = TestG * step(TexClipX.x + Edge , oTexCoord1b.s) * step(oTexCoord1b.s , TexClipX.y - Edge) * step(Edge , oTexCoord1b.t) * step(oTexCoord1b.t , 1.0 - Edge);\n"
			"	highp vec4 color1 = vec4( TestR * color1r.x, TestG * color1g.y, TestB * color1b.z, 1);\n"//注意: 主图层的Alpha是1
#endif
			"	gl_FragColor = intensity * color1;\n"
			"}\n";
		// 无色散的
		const char* FragmentShaderNoDispersion = "uniform sampler2D Texture0;\n"
			"varying highp vec2 oTexCoord1r;\n"
			"varying highp vec2 oTexCoord1g;\n"
			"varying highp vec2 oTexCoord1b;\n"
			"uniform mediump vec2 TexClipX;\n"
			"varying mediump float	intensity;\n"
			"void main()\n"
			"{\n"
			"   const lowp float Edge = 0.001;\n"
			"	mediump float TestG = step(TexClipX.x + Edge , oTexCoord1g.s) * step(oTexCoord1g.s , TexClipX.y - Edge) * step(Edge , oTexCoord1g.t) * step(oTexCoord1g.t , 1.0 - Edge);\n"
			"	highp vec4 color1 = texture2D(Texture0, oTexCoord1g) * TestG;\n"//注意: 主图层的Alpha是1
			//"	highp vec4 color1 = texture2D(Texture0, oTexCoord1g);\n"
			"	color1.w = 1.0;\n"
			"	gl_FragColor = intensity * color1;\n"
			"}\n";

		const char* FragmentShaderLayout = "uniform sampler2D Texture0;\n"
			"varying highp vec2 oTexCoord1r;\n"
			"varying highp vec2 oTexCoord1g;\n"
			"varying highp vec2 oTexCoord1b;\n"
			"uniform mediump vec2 TexClipX;\n"
			"varying mediump float	intensity;\n"
			"void main()\n"
			"{\n"
			"   const lowp float Edge = 0.01;\n"
			"	highp vec4 color1r = texture2D(Texture0, oTexCoord1r);\n"
			"	highp vec4 color1g = texture2D(Texture0, oTexCoord1g);\n"
			"	highp vec4 color1b = texture2D(Texture0, oTexCoord1b);\n"
			"	mediump float TestR = step(TexClipX.x + Edge , oTexCoord1r.s) * step(oTexCoord1r.s , TexClipX.y - Edge) * step(Edge , oTexCoord1r.t) * step(oTexCoord1r.t , 1.0 - Edge);\n"
#if ONLE_USING_R_LIMIT
			"	highp vec4 color1 = vec4( TestR * color1r.x, TestR * color1g.y, TestR * color1b.z, TestR*color1r.w);\n"//color1r.x, color1g.y, color1b.z
#else
			"	mediump float TestG = TestR * step(TexClipX.x + Edge , oTexCoord1g.s) * step(oTexCoord1g.s , TexClipX.y - Edge) * step(Edge , oTexCoord1g.t) * step(oTexCoord1g.t , 1.0 - Edge);\n"
			"	mediump float TestB = TestG * step(TexClipX.x + Edge , oTexCoord1b.s) * step(oTexCoord1b.s , TexClipX.y - Edge) * step(Edge , oTexCoord1b.t) * step(oTexCoord1b.t , 1.0 - Edge);\n"
			"	highp vec4 color1 = vec4( TestR * color1r.x, TestG * color1g.y, TestB * color1b.z, TestR*color1r.w);\n"//color1r.x, color1g.y, color1b.z
#endif
			"	gl_FragColor = color1;\n"
			"}\n";

		const char* FragmentShaderLayoutNoDispersion = "uniform sampler2D Texture0;\n"
			"varying highp vec2 oTexCoord1r;\n"
			"varying highp vec2 oTexCoord1g;\n"
			"varying highp vec2 oTexCoord1b;\n"
			"uniform mediump vec2 TexClipX;\n"
			//"uniform highp float TestR;\n"
			"varying mediump float	intensity;\n"
			"void main()\n"
			"{\n"
			"   const lowp float Edge = 0.01;\n"
			"	mediump float TestG = step(TexClipX.x + Edge , oTexCoord1g.s) * step(oTexCoord1g.s , TexClipX.y - Edge) * step(Edge , oTexCoord1g.t) * step(oTexCoord1g.t , 1.0 - Edge);\n"
			"	highp vec4 color1 = texture2D(Texture0, oTexCoord1g) * TestG;\n"
			"	gl_FragColor = intensity * color1;\n"
			"}\n";

		const char* FragmentShaderDistortionRange = "uniform sampler2D Texture0;\n"
			"varying highp vec2 oTexCoord1r;\n"
			"varying highp vec2 oTexCoord1g;\n"
			"varying highp vec2 oTexCoord1b;\n"
			"uniform mediump vec2 TexClipX;\n"
			//				"uniform highp float TestR;\n"
			"varying mediump float	intensity;\n"
			"void main()\n"
			"{\n"
			"   const lowp float Edge = 0.01;\n"
			"	mediump float TestR = step(TexClipX.x + Edge , oTexCoord1r.s) * step(oTexCoord1r.s , TexClipX.y - Edge) * step(Edge , oTexCoord1r.t) * step(oTexCoord1r.t , 1.0 - Edge);\n"
			"	highp vec4 color1 = vec4( max(0.5 , 1.0 * TestR) , 1.0 * TestR, 0.0 , 1.0);\n"//注意: 主图层的Alpha是1
			"	gl_FragColor = intensity * color1;\n"
			"}\n";

		const char* FragmentShaderDistortionRangeDark = "uniform sampler2D Texture0;\n"
			"varying highp vec2 oTexCoord1r;\n"
			"varying highp vec2 oTexCoord1g;\n"
			"varying highp vec2 oTexCoord1b;\n"
			"uniform mediump vec2 TexClipX;\n"
			//				"uniform highp float TestR;\n"
			"varying mediump float	intensity;\n"
			"void main()\n"
			"{\n"
			"	highp vec4 color1 = vec4( 0.0 , 0.0, 0.0 , 1.0);\n"//注意: 主图层的Alpha是1
			"	gl_FragColor = color1;\n"
			"}\n";
#else
		const char*  VertexShader =
			"#version 300 es\n"
			"uniform mat4 Mvpm;\n"
			"uniform mat4 Texm;\n"
			"in vec4 Position;\n"
			"in vec2 TexCoord;\n"	// green
			"in vec2 TexCoord1;\n"	// .x = interpolated warp frac, .y = intensity scale
			"in vec2 dR;\n"		// red
			"in vec2 dB;\n"		// blue
			"out highp vec2 oTexCoord1r;\n"
			"out highp vec2 oTexCoord1g;\n"
			"out highp vec2 oTexCoord1b;\n"

			"out  mediump float	intensity;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = Mvpm * Position;\n"
			"	vec3 proj;\n"
			"	float projIZ;\n"
			"   proj = vec3( Texm * vec4(dR,-1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1r = vec2( proj.x * projIZ, proj.y * projIZ );\n"
			"   proj = vec3( Texm * vec4(TexCoord,-1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1g = vec2( proj.x * projIZ, proj.y * projIZ );\n"
			"   proj = vec3( Texm * vec4(dB,-1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1b = vec2( proj.x * projIZ, proj.y * projIZ );\n"

			"	intensity = TexCoord1.y;\n"
			"}\n";

		const char* FragmentShader =
			"#version 300 es\n"
			"precision mediump float;\n"
			"precision mediump int;\n"
			"precision mediump sampler2D;\n"

			"uniform sampler2D Texture0;\n"
			"uniform mediump vec2 TexClipX;\n"
			"uniform int layerIndex;\n"

			"in highp vec2 oTexCoord1r;\n"
			"in highp vec2 oTexCoord1g;\n"
			"in highp vec2 oTexCoord1b;\n"

			"in mediump float intensity;\n"
			"out vec4 fragColor;\n"
			"void main()\n"
			"{\n"
			"   const lowp float Edge = 0.01;\n"

			"	highp vec4 color1r = texture(Texture0, oTexCoord1r);\n"
			"	highp vec4 color1g = texture(Texture0, oTexCoord1g);\n"
			"	highp vec4 color1b = texture(Texture0, oTexCoord1b);\n"

			"	mediump float TestR = step(TexClipX.x + Edge , oTexCoord1r.s) * step(oTexCoord1r.s , TexClipX.y - Edge) * step(Edge , oTexCoord1r.t) * step(oTexCoord1r.t , 1.0 - Edge);\n"
			"	highp vec4 color1 = vec4(TestR * color1r.x, TestR * color1g.y, TestR * color1b.z, 1.0);\n"
			"	fragColor = intensity * color1;\n"
			"}\n";

		// Multiview
		const char* VertexShaderMultiviewNoDispertion = VertexShader;
		const char* FragmentShaderMultiviewNoDispertion =
			"#version 300 es\n"
			"precision mediump float;\n"
			"precision mediump int;\n"
			"precision mediump sampler2DArray;\n"

			"uniform sampler2DArray Texture0;\n"
			"uniform mediump vec2 TexClipX;\n"
			"uniform int layerIndex;\n"

			"in highp vec2 oTexCoord1r;\n"
			"in highp vec2 oTexCoord1g;\n"
			"in highp vec2 oTexCoord1b;\n"

			"in mediump float	intensity;\n"

			"out vec4 fragColor;\n"
			"void main()\n"
			"{\n"
			"   const lowp float Edge = 0.001;\n"
			"	mediump float TestG = step(TexClipX.x + Edge , oTexCoord1g.s) * step(oTexCoord1g.s , TexClipX.y - Edge) * step(Edge , oTexCoord1g.t) * step(oTexCoord1g.t , 1.0 - Edge);\n"
			"	highp vec4 color1 = texture(Texture0, vec3(oTexCoord1g, layerIndex)) * TestG;\n"//注意: 主图层的Alpha是1
			"	fragColor = color1;\n"
			"}\n";
		const char* VertexShaderMultiviewHighResNoDispertion = 
			"#version 300 es\n"
			"uniform mat4 Mvpm;\n"
			"uniform mat4 Texm;\n"
			"in vec4 Position;\n"
			"in vec2 TexCoord;\n"	// green
			"in vec2 TexCoord1;\n"	// .x = interpolated warp frac, .y = intensity scale
			"in vec2 dR;\n"		// red
			"in vec2 dB;\n"		// blue
			"out highp vec2 oTexCoord1r;\n"
			"out highp vec2 oTexCoord1g;\n"
			"out highp vec2 oTexCoord1b;\n"

			"out highp vec2 oTexCoord1r_highRes;\n"
			"out highp vec2 oTexCoord1g_highRes;\n"
			"out highp vec2 oTexCoord1b_highRes;\n"

			"out  mediump float	intensity;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = Mvpm * Position;\n"
			"	vec3 proj;\n"
			"	float projIZ;\n"
			"   proj = vec3( Texm * vec4(dR,-1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1r = vec2( proj.x * projIZ, proj.y * projIZ );\n"
			"   proj = vec3( Texm * vec4(TexCoord,-1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1g = vec2( proj.x * projIZ, proj.y * projIZ );\n"
			"   proj = vec3( Texm * vec4(dB,-1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1b = vec2( proj.x * projIZ, proj.y * projIZ );\n"

			"	oTexCoord1r_highRes = oTexCoord1r * 2.0f - 0.5f;\n"
			"	oTexCoord1g_highRes = oTexCoord1g * 2.0f - 0.5f;\n"
			"	oTexCoord1b_highRes = oTexCoord1b * 2.0f - 0.5f;\n"

			"	intensity = TexCoord1.y;\n"
			"}\n";
		const char* FragmentShaderMultiviewHighResNoDispertion = 
			"#version 300 es\n"
			"precision mediump float;\n"
			"precision mediump int;\n"
			"precision mediump sampler2DArray;\n"

			"uniform sampler2DArray Texture0;\n"
			"uniform mediump vec2 TexClipX;\n"
			"uniform int layerIndex;\n"

			//"in highp vec2 oTexCoord1r;\n"
			"in highp vec2 oTexCoord1g;\n"
			//"in highp vec2 oTexCoord1b;\n"

			//"in highp vec2 oTexCoord1r_highRes;\n"
			"in highp vec2 oTexCoord1g_highRes;\n"
			//"in highp vec2 oTexCoord1b_highRes;\n"

			"in mediump float intensity;\n"

			"out vec4 fragColor;\n"
			"void main()\n"
			"{\n"
			"   const lowp float Edge = 0.001;\n"
			"	mediump float TestG = step(TexClipX.x + Edge , oTexCoord1g.s) * step(oTexCoord1g.s , TexClipX.y - Edge) * step(Edge , oTexCoord1g.t) * step(oTexCoord1g.t , 1.0 - Edge);\n"
			"	highp vec4 color1 = texture(Texture0, vec3(oTexCoord1g, layerIndex)) * TestG;\n"
			"	highp vec4 color1_highRes = texture(Texture0, vec3(oTexCoord1g_highRes, layerIndex + 2)) * TestG;\n"
			"	vec2 distVec = vec2(0.5) - oTexCoord1g_highRes;\n"
			"	float squaredDist = dot(distVec, distVec);\n"
			"	float lerpVal = smoothstep(-0.25, -0.04, -squaredDist);\n"
			"	color1 = mix(color1, color1_highRes, lerpVal);\n"

			"	fragColor = color1;\n"
			"}\n";

		//const char* VertexShader = "uniform mat4 Mvpm;\n"
		//	"uniform mat4 Texm;\n"
		//	"attribute vec4 Position;\n"
		//	"attribute vec2 TexCoord;\n"	// green
		//	"attribute vec2 TexCoord1;\n"	// .x = interpolated warp frac, .y = intensity scale
		//	"attribute vec2 dR;\n"		// red
		//	"attribute vec2 dB;\n"		// blue
		//	"varying  vec2 oTexCoord1r;\n"
		//	"varying  vec2 oTexCoord1g;\n"
		//	"varying  vec2 oTexCoord1b;\n"
		//	"varying  float	intensity;\n"
		//	"void main()\n"
		//	"{\n"
		//	"   gl_Position = Mvpm * Position;\n"
		//	"	vec3 proj;\n"
		//	"	float projIZ;\n"
		//	"   proj = vec3( Texm * vec4(dR,-1,1) );\n"
		//	"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
		//	"	oTexCoord1r = vec2( proj.x * projIZ, proj.y * projIZ );\n"
		//	"   proj = vec3( Texm * vec4(TexCoord,-1,1) );\n"
		//	"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
		//	"	oTexCoord1g = vec2( proj.x * projIZ, proj.y * projIZ );\n"
		//	"   proj = vec3( Texm * vec4(dB,-1,1) );\n"
		//	"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
		//	"	oTexCoord1b = vec2( proj.x * projIZ, proj.y * projIZ );\n"
		//	""
		//	"	intensity = TexCoord1.y;\n"
		//	"}\n";
		const char* VertexShaderLayout = 
			"#version 300 es\n"
			"uniform mat4 Mvpm;\n"
			"uniform mat4 Texm;\n"
			"in vec4 Position;\n"
			"in vec2 TexCoord;\n"	// green
			"in vec2 TexCoord1;\n"	// .x = interpolated warp frac, .y = intensity scale
			"in vec2 dR;\n"		// red
			"in vec2 dB;\n"		// blue
			"out highp vec2 oTexCoord1r;\n"
			"out highp vec2 oTexCoord1g;\n"
			"out highp vec2 oTexCoord1b;\n"
#if defined(HIGH_RES)
			"out highp vec2 oTexCoord1r_highRes;\n"
			"out highp vec2 oTexCoord1g_highRes;\n"
			"out highp vec2 oTexCoord1b_highRes;\n"
#endif
			"out  mediump float	intensity;\n"
			"void main()\n"
			"{\n"
			"   gl_Position = Mvpm * Position;\n"
			"	vec3 proj;\n"
			"	float projIZ;\n"
			"   proj = vec3( Texm * vec4(dR,1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1r = vec2( proj.x * projIZ, proj.y * projIZ );\n"
			"   proj = vec3( Texm * vec4(TexCoord,1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1g = vec2( proj.x * projIZ, proj.y * projIZ );\n"
			"   proj = vec3( Texm * vec4(dB,1,1) );\n"
			"	projIZ = 1.0 / max( proj.z, 0.00001 );\n"
			"	oTexCoord1b = vec2( proj.x * projIZ, proj.y * projIZ );\n"
			""
			"	intensity = TexCoord1.y;\n"
			"}\n";

		const char* VertexShaderDistortionRange = VertexShader;

//		const char* FragmentShader = "uniform sampler2D Texture0;\n"
//			"varying highp vec2 oTexCoord1r;\n"
//			"varying highp vec2 oTexCoord1g;\n"
//			"varying highp vec2 oTexCoord1b;\n"
//			"uniform mediump vec2 TexClipX;\n"
//			//				"uniform highp float TestR;\n"
//			"varying mediump float	intensity;\n"
//			"void main()\n"
//			"{\n"
//			"   const lowp float Edge = 0.01;\n"
//			"	highp vec4 color1r = texture2D(Texture0, oTexCoord1r);\n"
//			"	highp vec4 color1g = texture2D(Texture0, oTexCoord1g);\n"
//			"	highp vec4 color1b = texture2D(Texture0, oTexCoord1b);\n"
//			"	mediump float TestR = step(TexClipX.x + Edge , oTexCoord1r.s) * step(oTexCoord1r.s , TexClipX.y - Edge) * step(Edge , oTexCoord1r.t) * step(oTexCoord1r.t , 1.0 - Edge);\n"
//#if ONLE_USING_R_LIMIT// 只用红色通道过滤边界
//			"	highp vec4 color1 = vec4( TestR * color1r.x, TestR * color1g.y, TestR * color1b.z, 1.0);\n"//注意: 主图层的Alpha是1
//			//"	highp vec4 color1 = vec4( 0.0, 0.0, 1.0 , TestR);\n"//注意: 主图层的Alpha是1
//#else
//			"	mediump float TestG = TestR * step(TexClipX.x + Edge , oTexCoord1g.s) * step(oTexCoord1g.s , TexClipX.y - Edge) * step(Edge , oTexCoord1g.t) * step(oTexCoord1g.t , 1.0 - Edge);\n"
//			"	mediump float TestB = TestG * step(TexClipX.x + Edge , oTexCoord1b.s) * step(oTexCoord1b.s , TexClipX.y - Edge) * step(Edge , oTexCoord1b.t) * step(oTexCoord1b.t , 1.0 - Edge);\n"
//			"	highp vec4 color1 = vec4( TestR * color1r.x, TestG * color1g.y, TestB * color1b.z, 1);\n"//注意: 主图层的Alpha是1
//#endif
//			"	gl_FragColor = intensity * color1;\n"
//			"}\n";
		// 无色散的
		const char* FragmentShaderNoDispersion = 
			"#version 300 es\n"
			"precision mediump float;\n"
			"precision mediump int;\n"
			"precision mediump sampler2DArray;\n"

			"uniform sampler2D Texture0;\n"
			"uniform mediump vec2 TexClipX;\n"
			"uniform int layerIndex;\n"

			"in highp vec2 oTexCoord1r;\n"
			"in highp vec2 oTexCoord1g;\n"
			"in highp vec2 oTexCoord1b;\n"
			
			"in mediump float	intensity;\n"
			
			"out vec4 fragColor;\n"
			"void main()\n"
			"{\n"
			"   const lowp float Edge = 0.001;\n"
			"	mediump float TestG = step(TexClipX.x + Edge , oTexCoord1g.s) * step(oTexCoord1g.s , TexClipX.y - Edge) * step(Edge , oTexCoord1g.t) * step(oTexCoord1g.t , 1.0 - Edge);\n"
			"	highp vec4 color1 = texture(Texture0, oTexCoord1g) * TestG;\n"//注意: 主图层的Alpha是1
			"	fragColor = color1;\n"
			"}\n";

		const char* FragmentShaderLayout = 
			"#version 300 es\n"
			"precision mediump float;\n"
			"precision mediump int;\n"
			"precision mediump sampler2DArray;\n"
			"\n"
			"uniform sampler2D Texture0;\n"
			"in highp vec2 oTexCoord1r;\n"
			"in highp vec2 oTexCoord1g;\n"
			"in highp vec2 oTexCoord1b;\n"
			"uniform mediump vec2 TexClipX;\n"
			"uniform int layerIndex;\n"
			"in mediump float	intensity;\n"
			"out vec4 fragColor;\n"
			"void main()\n"
			"{\n"
			"   const lowp float Edge = 0.01;\n"
			"	highp vec4 color1r = texture(Texture0, oTexCoord1r);\n"
			"	highp vec4 color1g = texture(Texture0, oTexCoord1g);\n"
			"	highp vec4 color1b = texture(Texture0, oTexCoord1b);\n"
			"	mediump float TestR = step(TexClipX.x + Edge , oTexCoord1r.s) * step(oTexCoord1r.s , TexClipX.y - Edge) * step(Edge , oTexCoord1r.t) * step(oTexCoord1r.t , 1.0 - Edge);\n"
#if ONLE_USING_R_LIMIT
			"	highp vec4 color1 = vec4( TestR * color1r.x, TestR * color1g.y, TestR * color1b.z, TestR*color1r.w);\n"//color1r.x, color1g.y, color1b.z
#else
			"	mediump float TestG = TestR * step(TexClipX.x + Edge , oTexCoord1g.s) * step(oTexCoord1g.s , TexClipX.y - Edge) * step(Edge , oTexCoord1g.t) * step(oTexCoord1g.t , 1.0 - Edge);\n"
			"	mediump float TestB = TestG * step(TexClipX.x + Edge , oTexCoord1b.s) * step(oTexCoord1b.s , TexClipX.y - Edge) * step(Edge , oTexCoord1b.t) * step(oTexCoord1b.t , 1.0 - Edge);\n"
			"	highp vec4 color1 = vec4( TestR * color1r.x, TestG * color1g.y, TestB * color1b.z, TestR*color1r.w);\n"//color1r.x, color1g.y, color1b.z
#endif
			"	fragColor = color1;\n"
			"}\n";

		const char* FragmentShaderLayoutNoDispersion = 
			"#version 300 es\n"
			"precision mediump float;\n"
			"precision mediump int;\n"
			"precision mediump sampler2DArray;\n"
			"\n"
			"uniform sampler2D Texture0;\n"
			"in highp vec2 oTexCoord1r;\n"
			"in highp vec2 oTexCoord1g;\n"
			"in highp vec2 oTexCoord1b;\n"
			"uniform mediump vec2 TexClipX;\n"
			"uniform int layerIndex;\n"
			"in mediump float	intensity;\n"
			"out vec4 fragColor;\n"
			"void main()\n"
			"{\n"
			"   const lowp float Edge = 0.01;\n"
			"	mediump float TestG = step(TexClipX.x + Edge , oTexCoord1g.s) * step(oTexCoord1g.s , TexClipX.y - Edge) * step(Edge , oTexCoord1g.t) * step(oTexCoord1g.t , 1.0 - Edge);\n"
			"	highp vec4 color1 = texture(Texture0, oTexCoord1g) * TestG;\n"
			"	fragColor = intensity * color1;\n"
			"}\n";

		const char* FragmentShaderDistortionRange = 
			"#version 300 es\n"
			"precision mediump float;\n"
			"precision mediump int;\n"
			"precision mediump sampler2DArray;\n"
			"\n"
			"uniform sampler2D Texture0;\n"
			"in highp vec2 oTexCoord1r;\n"
			"in highp vec2 oTexCoord1g;\n"
			"in highp vec2 oTexCoord1b;\n"
			"uniform mediump vec2 TexClipX;\n"
			"uniform int layerIndex;\n"
			//				"uniform highp float TestR;\n"
			"in mediump float	intensity;\n"
			"out vec4 fragColor;\n"
			"void main()\n"
			"{\n"
			"   const lowp float Edge = 0.01;\n"
			"	mediump float TestR = step(TexClipX.x + Edge , oTexCoord1r.s) * step(oTexCoord1r.s , TexClipX.y - Edge) * step(Edge , oTexCoord1r.t) * step(oTexCoord1r.t , 1.0 - Edge);\n"
			"	highp vec4 color1 = vec4( max(0.5 , 1.0 * TestR) , 1.0 * TestR, 0.0 , 1.0);\n"//注意: 主图层的Alpha是1
			"	fragColor = color1;\n"
			"}\n";

		const char* FragmentShaderDistortionRangeDark = 
			"#version 300 es\n"
			"precision mediump float;\n"
			"precision mediump int;\n"
			"precision mediump sampler2DArray;\n"
			"\n"
			"uniform sampler2D Texture0;\n"
			"in highp vec2 oTexCoord1r;\n"
			"in highp vec2 oTexCoord1g;\n"
			"in highp vec2 oTexCoord1b;\n"
			"uniform mediump vec2 TexClipX;\n"
			"uniform int layerIndex;\n"
			//				"uniform highp float TestR;\n"
			"in mediump float	intensity;\n"
			"out vec4 fragColor;\n"
			"void main()\n"
			"{\n"
			"	highp vec4 color1 = vec4( 0.0 , 0.0, 0.0 , 1.0);\n"//注意: 主图层的Alpha是1
			"	fragColor = color1;\n"
			"}\n";

		const char* FragmentShaderMultiview = "";
#endif

		GlProgram::GlProgram() :
			program(0),
			vertexShader(0),
			fragmentShader(0),
			uMvp(0),
			uModel(0),
			uView(0),
			uTexm(0),
			uTexClipX(0),
			uTexture0(-1)
		{
		}


		GlProgram::~GlProgram()
		{
		}
		void GlProgram::Release()
		{
			program = 0;
			vertexShader = 0;
			fragmentShader = 0;
			uMvp = 0;
			uModel = 0;
			uView = 0;
			uTexm = 0;
			uTexClipX = 0;
			uTexture0 = 0;
		}
	}
}

