package com.bfmj.viewcore.render;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.util.ArrayList;

import com.bfmj.viewcore.util.GLMatrixState;
import com.bfmj.viewcore.util.GLShaderManager;
import com.bfmj.viewcore.view.GLPlayerView;
import com.bfmj.viewcore.view.GLRectView;
import com.bfmj.viewcore.view.GLView;

import android.annotation.SuppressLint;
import android.opengl.GLES11Ext;
import android.opengl.GLES30;
import android.opengl.Matrix;

/**
 * 
 * ClassName: GLVideoRect <br/>
 * @author lixianke    
 * @date: 2015-3-17 下午2:47:40 <br/>  
 * description:
 */
public class GLVideoRect extends GLRect {
	public enum TextureType {
		TEXTURE_TYPE_ALL, TEXTURE_TYPE_LEFT, TEXTURE_TYPE_RIGHT, TEXTURE_TYPE_TOP, TEXTURE_TYPE_BOTTOM
	}
	private static GLVideoRect instance;
	
    private int mProgram;
    private int muMVPMatrixHandle;
    private int mPositionHandle;
    private int mTextureCoordHandle;
	
	private int vboVertexNew = 0; 	//bufferIndex++;
	private int vboTextureNew = 0;	//bufferIndex++;
	private int vboTextureLeftNew = 0;	//bufferIndex++;
	private int vboTextureRightNew = 0;	//bufferIndex++;
	private int vboTextureTopNew = 0;
	private int vboTextureBottomNew = 0;
	private int []mvbo = new int[6];

	private TextureType mTextureType = TextureType.TEXTURE_TYPE_ALL;
	
	private float vertices[] = {
       -0.5f,  0.5f,
       -0.5f, -0.5f,
        0.5f, -0.5f,
       -0.5f,  0.5f,
        0.5f, -0.5f,
       	0.5f,  0.5f
    };
	
	public static float[] texture_all = {
    	0.0f, 0.0f,
    	0.0f, 1.0f,
        1.0f, 1.0f,
    	0.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };
	
	public static float[] texture_left = {
    	0.0f, 0.0f,
    	0.0f, 1.0f,
        0.5f, 1.0f,
    	0.0f, 0.0f,
        0.5f, 1.0f,
        0.5f, 0.0f
    };
	
	public static float[] texture_right = {
    	0.5f, 0.0f,
    	0.5f, 1.0f,
        1.0f, 1.0f,
    	0.5f, 0.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    };

	public static float[] texture_top = {
			0.0f, 0.0f,
			0.0f, 0.5f,
			1.0f, 0.5f,
			0.0f, 0.0f,
			1.0f, 0.5f,
			1.0f, 0.0f
	};

	public static float[] texture_bottom = {
			0.0f, 0.5f,
			0.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 0.5f,
			1.0f, 1.0f,
			1.0f, 0.5f
	};
    
    private int mTextureId = -1;
    
    public static GLVideoRect getInstance(){
    	if (instance == null){
    		instance = new GLVideoRect();
    	}
    	return instance;
    }
    
    public static void initInstance(){
    	if (instance != null){
    		instance.release();
    		instance = null;
    	}
    	instance = new GLVideoRect();
    }
    
    public GLVideoRect(){
    	init();
    }
    
    private void init(){
		GLES30.glGenBuffers( 6, mvbo, 0 );
		vboVertexNew = mvbo[0];
		vboTextureNew = mvbo[1];
		vboTextureLeftNew = mvbo[2];
		vboTextureRightNew = mvbo[3];
		vboTextureTopNew = mvbo[4];
		vboTextureBottomNew = mvbo[5];

    	initVertex();
    	initTextureBuffer();
    	
    	createProgram();
    }
    
    public void setTextureId(int textureId){
    	mTextureId = textureId;
    }
    
    public void setTextureType(TextureType type){
    	mTextureType = type;
    }

	public void draw(GLPlayerView view){
		if (view == null){
			return;
		}

		beginDraw();

		float d = -view.getDepth();
		for (int i = 0; i < view.getRenders().size(); i++) {
			GLRenderParams render = null;
			try {
				render = view.getRenders().get(i);
			} catch (Exception e) {}

			if (render != null && render.getType() == GLRenderParams.RENDER_TYPE_VIDEO) {
				GLMatrixState state = view.getMatrixState();
				state.pushMatrix();
				float[] curMatrix = state.getCurrentMatrix();
				Matrix.translateM(curMatrix, 0, 0, 0, d + view.getZPosition() * 0.0008f);
				if (view.getAngelX() != 0){
					Matrix.rotateM(curMatrix, 0, view.getAngelX(), 1, 0, 0);
				}
				if (view.getAngelY() != 0){
					Matrix.rotateM(curMatrix, 0, view.getAngelY(), 0, 1, 0);
				}
				if (view.getAngelZ() != 0){
					Matrix.rotateM(curMatrix, 0, view.getAngelZ(), 0, 0, 1);
				}

				if (render.getScaleX() != 1.0f || render.getScaleY() != 1.0f){
					Matrix.scaleM(curMatrix, 0, render.getScaleX(), render.getScaleY(), 1);
				}

				setTextureId(render.getTextureId());
				setTextureType(render.getTextureType());
				setAlpha(render.getAlpha());
				setMask(render.getMask());
				draw(state.getFinalMatrix());

				d += 0.0004f;
				state.popMatrix();
			}
		}

		endDraw();
	}

	public void beginDraw(){
		GLES30.glDisable(GLES30.GL_DEPTH_TEST);
		GLES30.glUseProgram(mProgram);
		GLES30.glActiveTexture(GLES30.GL_TEXTURE0);

		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, vboVertexNew);
		GLES30.glEnableVertexAttribArray(0);
		GLES30.glVertexAttribPointer(mPositionHandle, 2, GLES30.GL_FLOAT, false, 0, 0);

		GLES30.glEnableVertexAttribArray(1);
	}

	public void endDraw(){
		GLES30.glDisableVertexAttribArray(0);
		GLES30.glDisableVertexAttribArray(1);
		GLES30.glEnable(GLES30.GL_DEPTH_TEST);
		GLES30.glBindBuffer( GLES30.GL_ARRAY_BUFFER, 0 );
	}
    
    @SuppressLint("InlinedApi")
	public void draw(float[] mtx) {
    	if (mTextureId < 0){
    		return;
    	}

        GLES30.glBindTexture(GLES11Ext.GL_TEXTURE_EXTERNAL_OES, mTextureId);
        GLES30.glUniformMatrix4fv(muMVPMatrixHandle, 1, false, mtx, 0);

		int buffer = vboTextureNew;

		switch (mTextureType){
			case TEXTURE_TYPE_LEFT:
				buffer = vboTextureLeftNew;
				break;
			case TEXTURE_TYPE_RIGHT:
				buffer = vboTextureRightNew;
				break;
			case TEXTURE_TYPE_TOP:
				buffer = vboTextureTopNew;
				break;
			case TEXTURE_TYPE_BOTTOM:
				buffer = vboTextureBottomNew;
				break;
		}

		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, buffer);
		GLES30.glVertexAttribPointer(mTextureCoordHandle, 2, GLES30.GL_FLOAT, false, 0, 0);
        GLES30.glDrawArrays(GLES30.GL_TRIANGLES, 0, vertices.length / 2);
		return;
	}
    
    private void createProgram(){
		int vertexShader    = GLShaderManager.loadShader(GLES30.GL_VERTEX_SHADER, GLShaderManager.VERTEX_VIDEO);
        int fragmentShader  = GLShaderManager.loadShader(GLES30.GL_FRAGMENT_SHADER, GLShaderManager.FRAGMENT_VIDEO);

        mProgram = GLES30.glCreateProgram();             // create empty OpenGL ES Program
        GLES30.glAttachShader(mProgram, vertexShader);   // add the vertex shader to program
        GLES30.glAttachShader(mProgram, fragmentShader); // add the fragment shader to program
        GLES30.glLinkProgram(mProgram);                  // creates OpenGL ES program executables
        muMVPMatrixHandle = GLES30.glGetUniformLocation(mProgram, "uMVPMatrix");
        mTextureCoordHandle = GLES30.glGetAttribLocation(mProgram, "inputTextureCoordinate");
        mPositionHandle = GLES30.glGetAttribLocation(mProgram, "vPosition");
	}
    
    private void vertexVBO() {
		
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, vboVertexNew);
		// 传送顶点位置数据
		GLES30.glVertexAttribPointer(mPositionHandle, 2, GLES30.GL_FLOAT,
				false, 0, 0);
		GLES30.glEnableVertexAttribArray(0);
	}
    
    private void textureVBO() {
    	int buffer = vboTextureNew;

		switch (mTextureType){
			case TEXTURE_TYPE_LEFT:
				buffer = vboTextureLeftNew;
				break;
			case TEXTURE_TYPE_RIGHT:
				buffer = vboTextureRightNew;
				break;
			case TEXTURE_TYPE_TOP:
				buffer = vboTextureTopNew;
				break;
			case TEXTURE_TYPE_BOTTOM:
				buffer = vboTextureBottomNew;
				break;
		}
		
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, buffer);
		// 传送顶点位置数据
		GLES30.glVertexAttribPointer(mTextureCoordHandle, 2, GLES30.GL_FLOAT,
				false, 0, 0);
		GLES30.glEnableVertexAttribArray(1);
		
	}
    
    protected void initVertex(){
		int verLen = vertices.length*4;
        ByteBuffer bb = ByteBuffer.allocateDirect(verLen);
        bb.order(ByteOrder.nativeOrder());
        FloatBuffer vertexBuffer = bb.asFloatBuffer();
        vertexBuffer.put(vertices);
        vertexBuffer.position(0);
       
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, vboVertexNew);
		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER, verLen, vertexBuffer,
				GLES30.GL_STATIC_DRAW);
	}
	
	private void initTextureBuffer(){
		// all
		float[] texCoor = texture_all;
		int textureLen = texCoor.length*4;
		ByteBuffer llbb = ByteBuffer.allocateDirect(textureLen);
		llbb.order(ByteOrder.nativeOrder());
		FloatBuffer textureVerticesBuffer = llbb.asFloatBuffer();
		textureVerticesBuffer.put(texCoor);
		textureVerticesBuffer.position(0);
		
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, vboTextureNew);
		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER, textureLen, textureVerticesBuffer,
				GLES30.GL_STATIC_DRAW);
		
		// left
		texCoor = texture_left;
		
        textureLen = texCoor.length*4;
		llbb = ByteBuffer.allocateDirect(textureLen);
		llbb.order(ByteOrder.nativeOrder());
		textureVerticesBuffer=llbb.asFloatBuffer();
		textureVerticesBuffer.put(texCoor);
		textureVerticesBuffer.position(0);
		
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, vboTextureLeftNew);
		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER, textureLen, textureVerticesBuffer,
				GLES30.GL_STATIC_DRAW);
		
		// right
		texCoor = texture_right;
		
        textureLen = texCoor.length*4;
		llbb = ByteBuffer.allocateDirect(textureLen);
		llbb.order(ByteOrder.nativeOrder());
		textureVerticesBuffer=llbb.asFloatBuffer();
		textureVerticesBuffer.put(texCoor);
		textureVerticesBuffer.position(0);
		
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, vboTextureRightNew);
		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER, textureLen, textureVerticesBuffer,
				GLES30.GL_STATIC_DRAW);

		// right
		texCoor = texture_top;

		textureLen = texCoor.length*4;
		llbb = ByteBuffer.allocateDirect(textureLen);
		llbb.order(ByteOrder.nativeOrder());
		textureVerticesBuffer=llbb.asFloatBuffer();
		textureVerticesBuffer.put(texCoor);
		textureVerticesBuffer.position(0);

		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, vboTextureTopNew);
		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER, textureLen, textureVerticesBuffer,
				GLES30.GL_STATIC_DRAW);

		// right
		texCoor = texture_bottom;

		textureLen = texCoor.length*4;
		llbb = ByteBuffer.allocateDirect(textureLen);
		llbb.order(ByteOrder.nativeOrder());
		textureVerticesBuffer=llbb.asFloatBuffer();
		textureVerticesBuffer.put(texCoor);
		textureVerticesBuffer.position(0);

		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, vboTextureBottomNew);
		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER, textureLen, textureVerticesBuffer,
				GLES30.GL_STATIC_DRAW);
	}
	
	public void release(){
		GLES30.glDeleteBuffers(4, new int[]{vboVertexNew, vboTextureNew, vboTextureLeftNew, vboTextureRightNew, vboTextureTopNew, vboTextureBottomNew}, 0);
	}
}
