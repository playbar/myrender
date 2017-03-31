package com.bfmj.viewcore.util;

import android.opengl.GLES30;
import android.util.Log;

/**
 * 
 * ClassName: GLShaderManager <br/>
 * @author lixianke    
 * @date: 2015-3-9 上午9:46:19 <br/>  
 * description:
 */
public class GLShaderManager {
	
	public static final String VERTEX_IMAGE =
		"uniform mat4 uMVPMatrix;" +
		"uniform float uAlpha;" +
		"uniform float uMask;" +
        "attribute vec3 vPosition;" +
        "attribute vec2 inputTextureCoordinate;" +
		"attribute vec4 aColor;" +
        "varying vec2 textureCoordinate;" +
        "varying float vAlpha;" +
        "varying float vMask;" +
		"varying vec4 vColor;" +
        "void main()" +
        "{"+
            "gl_Position = uMVPMatrix * vec4(vPosition, 1);"+
            "textureCoordinate = inputTextureCoordinate;" +
			"vColor = aColor;" +
            "vAlpha = uAlpha;" +
            "vMask = uMask;" +
        "}";
	
	public static final String FRAGMENT_IMAGE =
		"precision mediump float;" +
        "varying vec2 textureCoordinate;\n" +
        "uniform sampler2D s_texture;\n" +
		"uniform float uWidth;" +
		"uniform float uHeight;" +
        "varying float vAlpha;" +
        "varying float vMask;" +
		"varying vec4 vColor;"+
        "void main() {" +
			"vec2 coord = textureCoordinate;" +
        	"vec4 color = texture2D( s_texture, coord );\n" +
			"if(coord.x > 1.0001 - uWidth || coord.x < uWidth - 0.0001 || coord.y < uHeight - 0.0001 || coord.y > 1.0001 - uHeight )"+
			"	gl_FragColor = vColor;"+
			"else"+
			"	gl_FragColor = vec4(color.r * vMask, color.g * vMask, color.b * vMask, color.a * vAlpha);" +
        "}";
	
	public static final String VERTEX_SENCE =
			"uniform mat4 uMVPMatrix;" +
			"uniform float uAlpha;" +
			"uniform float uMask;" +
	        "attribute vec3 aPosition;" +
	        "attribute vec2 inputTextureCoordinate;" +
	        "varying vec2 textureCoordinate;" +
	        "varying float vAlpha;" +
	        "varying float vMask;" +
	        "void main()" +
	        "{"+
	            "gl_Position = uMVPMatrix * vec4(aPosition, 1);"+
	            "textureCoordinate = inputTextureCoordinate;" +
	            "vAlpha = uAlpha;" +
	            "vMask = uMask;" +
	        "}";
		
	public static final String FRAGMENT_SENCE_IMAGE =
			"precision mediump float;" +
	        "varying vec2 textureCoordinate;\n" +
	        "uniform sampler2D s_texture;\n" +
	        "varying float vAlpha;" +
	        "varying float vMask;" +
	        "void main() {" +
	        	"vec4 color = texture2D( s_texture, textureCoordinate );\n" +
	        	"gl_FragColor = vec4(color.r * vMask, color.g * vMask, color.b * vMask, color.a * vAlpha);\n" +
	        "}";

	public static final String FRAGMENT_SENCE_VIDEO =
			"#extension GL_OES_EGL_image_external : require\n"+
			"precision mediump float;" +
			"varying vec2 textureCoordinate;\n" +
			"uniform samplerExternalOES s_texture;\n" +
			"varying float vAlpha;" +
			"varying float vMask;" +
			"void main() {" +
			"vec4 color = texture2D( s_texture, textureCoordinate );\n" +
			"gl_FragColor = vec4(color.r * vMask, color.g * vMask, color.b * vMask, color.a * vAlpha);\n" +
			"}";
	
	public static final String VERTEX_COLOR =
		"uniform mat4 uMVPMatrix;" +
		"uniform vec4 uColor;" + 
        "attribute vec3 aPosition;" +
        "varying vec4 vColor;" +
        "void main(){"+
            "gl_Position = uMVPMatrix * vec4(aPosition, 1);"+
            "vColor = uColor;" +
        "}";
	
	public static final String FRAGMENT_COLOR =
		"precision mediump float;" +
		"varying vec4 vColor;" +
        "void main(){" +
        	"gl_FragColor = vColor;\n" +
        "}";
	
	public static final String VERTEX_POINT =
		"uniform mat4 uMVPMatrix;" +
        "attribute vec3 aPosition;" +
        "attribute float aSize;" +
        "attribute vec4 aColor;" +
        "varying vec4 vColor;" +
        "void main()" +
        "{"+	
        	"gl_PointSize = aSize;"+
            "gl_Position = uMVPMatrix * vec4(aPosition, 1);"+
            "vColor = aColor;"+
        "}";

	public static final String FRAGMENT_POINT =
        "precision mediump float;" +
        "varying vec4 vColor;" +
        "void main() {" +
        	"gl_FragColor = vColor;\n" +
        "}";
	
	public static final String VERTEX_VIDEO =
		"uniform mat4 uMVPMatrix;" +
        "attribute vec3 vPosition;" +
        "attribute vec2 inputTextureCoordinate;" +
        "varying vec2 textureCoordinate;" +
        "void main()" +
        "{"+
            "gl_Position = uMVPMatrix * vec4(vPosition, 1);"+
            "textureCoordinate = inputTextureCoordinate;" +
        "}";

	public static final String FRAGMENT_VIDEO =
       "#extension GL_OES_EGL_image_external : require\n"+
        "precision mediump float;" +
        "varying vec2 textureCoordinate;\n" +
        "uniform samplerExternalOES s_texture;\n" +
        "void main() {" +
        "  gl_FragColor = texture2D( s_texture, textureCoordinate );\n" +
        "}";
	
	public static int loadShader(int type, String shaderCode){
        int shader = GLES30.glCreateShader(type);
        GLES30.glShaderSource(shader, shaderCode);
        GLES30.glCompileShader(shader);
		int status[] = new int[1];
		GLES30.glGetShaderiv( shader, GLES30.GL_COMPILE_STATUS, status, 0 );
		if( status[0] != GLES30.GL_TRUE ){
//			Log.e("GLImageRect", " " + GLES30.glGetShaderInfoLog(shader));
		}
        return shader;
    }
}
