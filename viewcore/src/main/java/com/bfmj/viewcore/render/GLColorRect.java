package com.bfmj.viewcore.render;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import android.opengl.GLES30;
import com.bfmj.viewcore.util.GLShaderManager;

/**
 * 
 * ClassName: GLColorRect <br/>
 * @author lixianke    
 * @date: 2015-3-9 下午3:13:45 <br/>  
 * description:
 */
public class GLColorRect extends GLRect {
	private static GLColorRect instance;
	
	private FloatBuffer vertexBuffer;
    private int mProgram;
    private int muMVPMatrixHandle;
    private int muColorHandle;
    private int maPositionHandle;
    
    private int verLen=0;
	
	private int vboVertexNew = 0;   //bufferIndex++;
    private int []mvbo = new int[2];
    private float[] mColor;
    
    public static GLColorRect getInstance(){
    	if (instance == null){
    		instance = new GLColorRect();
    	}
    	return instance;
    }
    
    public static void initInstance(){
    	if (instance != null){
    		instance.release();
    		instance = null;
    	}
    	instance = new GLColorRect();
    }
    
    private GLColorRect(){
    	init();
    }
    
    private void init(){
        GLES30.glGenBuffers( 2, mvbo, 0 );
        vboVertexNew = mvbo[0];
		initVertex();
    	
    	createProgram();
    }
    
    /**
     * 设置颜色
     * @author lixianke  @Date 2015-3-17 下午2:48:56
     * @param color 颜色值 rgba数组
     * @return
     */
    public void setColor(float[] color){
    	mColor = color;
    }
    
    @Override
    public void setAlpha(float alpha){
    	mColor[3] = alpha;
    }

    public void beginDraw(){

        GLES30.glBlendFunc(GLES30.GL_SRC_ALPHA, GLES30.GL_SRC_COLOR);
        GLES30.glEnable(GLES30.GL_BLEND);
        GLES30.glUseProgram(mProgram);

        GLES30.glEnableVertexAttribArray(0);
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, vboVertexNew);
        GLES30.glVertexAttribPointer(maPositionHandle, 3, GLES30.GL_FLOAT, false, 0, 0);
    }

    public void endDraw(){
        GLES30.glDisableVertexAttribArray(0);
        GLES30.glDisable(GLES30.GL_BLEND);
        GLES30.glBindBuffer( GLES30.GL_ARRAY_BUFFER, 0 );
    }
    
    public void draw(float[] mtx) {
    	if (mColor == null){
    		return;
    	}
        
        GLES30.glUniformMatrix4fv(muMVPMatrixHandle, 1, false, mtx, 0);
        
        float[] color = new float[]{
        	mColor[0] * getMask(),
        	mColor[1] * getMask(),
        	mColor[2] * getMask(),
        	mColor[3]
        };
        
        GLES30.glUniform4fv(muColorHandle, 1, color, 0);

        GLES30.glDrawArrays(GLES30.GL_TRIANGLES, 0, getVertices().length / 3);

	}
    
    private void createProgram(){
		int vertexShader    = GLShaderManager.loadShader(GLES30.GL_VERTEX_SHADER, GLShaderManager.VERTEX_COLOR);
        int fragmentShader  = GLShaderManager.loadShader(GLES30.GL_FRAGMENT_SHADER, GLShaderManager.FRAGMENT_COLOR);

        mProgram = GLES30.glCreateProgram();
        GLES30.glAttachShader(mProgram, vertexShader);
        GLES30.glAttachShader(mProgram, fragmentShader);
        GLES30.glLinkProgram(mProgram);
        muMVPMatrixHandle = GLES30.glGetUniformLocation(mProgram, "uMVPMatrix"); 
        muColorHandle = GLES30.glGetUniformLocation(mProgram, "uColor");
        maPositionHandle = GLES30.glGetAttribLocation(mProgram, "aPosition");
	}
    
    private void vertexVBO() {
		GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, vboVertexNew);
		// 传送顶点位置数据
		GLES30.glVertexAttribPointer(maPositionHandle, 3, GLES30.GL_FLOAT,
				false, 0, 0);
		GLES30.glEnableVertexAttribArray(0);
	}
    
    protected void initVertex(){
		verLen = getVertices().length*4;
        ByteBuffer bb = ByteBuffer.allocateDirect(verLen);
        bb.order(ByteOrder.nativeOrder());
        vertexBuffer = bb.asFloatBuffer();
        vertexBuffer.put(getVertices());
        vertexBuffer.position(0);

        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, vboVertexNew);
		GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER, verLen, vertexBuffer,
				GLES30.GL_STATIC_DRAW);
        GLES30.glBindBuffer( GLES30.GL_ARRAY_BUFFER, 0 );
        return;
	}
    
    public void release(){
		GLES30.glDeleteBuffers(1, new int[]{vboVertexNew}, 0);
	}
}
