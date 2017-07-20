package com.bn.mojing;
import static com.bn.mojing.ShaderUtil.createProgram;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;
import java.util.ArrayList;
import android.opengl.GLES20;
public class FovTestModel {
	int mProgram;			//Shader的运行脚本ID
    int muMVPMatrixHandle;	//Shader的参数，用于MVP矩阵
    int maPositionHandle; 	//
    int maTexCoorHandle;
    
	final int[] vertexbuffer = new int[1];
    final int[] uvbuffer = new int[1];
	final int[] indexbuffer = new int[1];


	FloatBuffer   mVertexBuffer;
	FloatBuffer   mTexCoorBuffer;
	ShortBuffer indexBuffer;

    int miIndexCount = 6;
    public FovTestModel()
    {
    	creatSkyBox( );
    	
    	String vertexShader = 
    			"uniform mat4 uMVPMatrix;" +
    			"attribute vec3 aPosition;" +
    			"attribute vec2 aTexCoor;" +
    			"varying vec2 vTextureCoord;" +
    			"void main()" +     
    			"{" +  			                          	
    			"   gl_Position = uMVPMatrix * vec4(aPosition,1);" +
    			"   vTextureCoord = aTexCoor;" +
    			"}";
    			
    			String fragmentShader =			
    			"precision mediump float;" +
    			"uniform sampler2D sTexture;" +
    			"varying vec2 vTextureCoord;" +
    			"void main()" +                         
    			"{" +
    			"   vec4 finalColor=texture2D(sTexture, vTextureCoord);" +
    			"   gl_FragColor = finalColor;" +
    			"}";
    			
    	//璋冪敤鍒濆鍖栫潃鑹插櫒鐨刬ntShader鏂规硶
    	initShader(vertexShader,fragmentShader);
    }
    
    public void creatSkyBox()
    {
    	float fBoxHalfSize = 1.0f;
    	float []VertexBase ={
				-20 ,-20, -150,
				-20 , 20, -150,
				 20 ,-20, -150,
				 20 , 20, -150,
		};
    	float []Vertex = new float[VertexBase.length];
    	for (int iVertexIndex = 0 ; iVertexIndex < VertexBase.length ; iVertexIndex++)
    	{
    		Vertex[iVertexIndex] = VertexBase[iVertexIndex] * fBoxHalfSize;
    	}
    	ByteBuffer vbb = ByteBuffer.allocateDirect(12 *4);
        vbb.order(ByteOrder.nativeOrder());
        mVertexBuffer = vbb.asFloatBuffer();
        mVertexBuffer.put(VertexBase);
        mVertexBuffer.position(0);

    	float UV[] = {
				0, 1,
    			0, 0,
				1, 1,
				1, 0,
		};

    	 ByteBuffer tbb = ByteBuffer.allocateDirect(8*4);
         tbb.order(ByteOrder.nativeOrder());
         mTexCoorBuffer = tbb.asFloatBuffer();
         mTexCoorBuffer.put(UV);
         mTexCoorBuffer.position(0);
    	short alIndex []= {
				0,1,2 ,
				1,2,3 ,
    	};
    	ByteBuffer ibb =  ByteBuffer.allocateDirect(miIndexCount*2);        
        ibb.order(ByteOrder.nativeOrder());
        indexBuffer=ibb .asShortBuffer();
        indexBuffer.put(alIndex);  
        indexBuffer.position(0); 
    }
    public void initShader(String vertexShader, String fragmentShader)
    {
        mProgram = createProgram(vertexShader, fragmentShader);
        maPositionHandle = GLES20.glGetAttribLocation(mProgram, "aPosition");
        maTexCoorHandle= GLES20.glGetAttribLocation(mProgram, "aTexCoor");
        muMVPMatrixHandle = GLES20.glGetUniformLocation(mProgram, "uMVPMatrix");
        
    }
    public void  CreateVBO(){
        // ======== 创建并绑定VBO
         GLES20.glGenBuffers(1, vertexbuffer, 0);
         GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vertexbuffer[0]);
         GLES20.glBufferData(GLES20.GL_ARRAY_BUFFER, mVertexBuffer.capacity() * 4, mVertexBuffer, GLES20.GL_STATIC_DRAW);
//         
         GLES20.glGenBuffers(1, uvbuffer,0);
         GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, uvbuffer[0]);
         GLES20.glBufferData(GLES20.GL_ARRAY_BUFFER, mTexCoorBuffer.capacity() * 4, mTexCoorBuffer , GLES20.GL_STATIC_DRAW);
         
//         
//      // ========= 创建并绑定IBO
         GLES20.glGenBuffers(1, indexbuffer,0);
         GLES20.glBindBuffer(GLES20.GL_ELEMENT_ARRAY_BUFFER, indexbuffer[0]);
         GLES20.glBufferData(GLES20.GL_ELEMENT_ARRAY_BUFFER, indexBuffer.capacity() * 2, indexBuffer, GLES20.GL_STATIC_DRAW);
         

 	    GLES20.glEnableVertexAttribArray( maPositionHandle );
 	     GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vertexbuffer[0]);
         GLES20.glVertexAttribPointer(maPositionHandle, 3, GLES20.GL_FLOAT, false, 0, 0);
         GLES20.glEnableVertexAttribArray( maTexCoorHandle );
    
         GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, uvbuffer[0]);
         GLES20.glVertexAttribPointer(maTexCoorHandle, 2, GLES20.GL_FLOAT, false, 0, 0);

         GLES20.glDisableVertexAttribArray( maPositionHandle );
  	    GLES20.glDisableVertexAttribArray( maTexCoorHandle );
     }
 	public void EndVBO(){
 	   GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);
 	   GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);
        GLES20.glBindBuffer(GLES20.GL_ELEMENT_ARRAY_BUFFER, 0);

     GLES20.glDeleteBuffers(1, vertexbuffer, 0);
     GLES20.glDeleteBuffers(1, uvbuffer, 0);
 	GLES20.glDeleteBuffers(1, indexbuffer, 0);
     }
 	
 	public void Draw(int texId){
    	
    	//GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
    	
    	 MatrixState.rotate(0, 1, 0, 0);
    	 MatrixState.rotate(0, 0, 1, 0);
    	 MatrixState.rotate(0, 0, 0, 1);
    	
    	 GLES20.glUseProgram(mProgram);
         
         GLES20.glUniformMatrix4fv(muMVPMatrixHandle, 1, false, MatrixState.getFinalMatrix(), 0);
         
         GLES20.glUniform1i(GLES20.glGetUniformLocation(mProgram, "sTexture"), 0);

         GLES20.glEnableVertexAttribArray( maPositionHandle );
	     GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, vertexbuffer[0]);
        GLES20.glVertexAttribPointer(maPositionHandle, 3, GLES20.GL_FLOAT, false, 0, 0);
        GLES20.glEnableVertexAttribArray( maTexCoorHandle );
   
        GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, uvbuffer[0]);
        GLES20.glVertexAttribPointer(maTexCoorHandle, 2, GLES20.GL_FLOAT, false, 0, 0);

        GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
        GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texId);
        
        GLES20.glBindBuffer(GLES20.GL_ELEMENT_ARRAY_BUFFER, indexbuffer[0]);
        
 	    GLES20.glDrawElements(GLES20.GL_TRIANGLES,  miIndexCount, GLES20.GL_UNSIGNED_SHORT,0 );
 	    GLES20.glClearColor(0,0,0,1);
 	    GLES20.glDrawElements(GLES20.GL_LINES,  miIndexCount, GLES20.GL_UNSIGNED_SHORT,0 );
 	   
 	    
 		 GLES20.glUseProgram(0);  
 		 GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);
 	     GLES20.glBindBuffer(GLES20.GL_ELEMENT_ARRAY_BUFFER, 0 );
 	    
 	    GLES20.glDisableVertexAttribArray(maPositionHandle );
 	    GLES20.glDisableVertexAttribArray( maTexCoorHandle );
    }
    public void drawSelf(int texId)
    {
    	 MatrixState.rotate(0, 1, 0, 0);
    	 MatrixState.rotate(0, 0, 1, 0);
    	 MatrixState.rotate(0, 0, 0, 1);
    	
    	 GLES20.glUseProgram(mProgram);
         
         GLES20.glUniformMatrix4fv(muMVPMatrixHandle, 1, false, MatrixState.getFinalMatrix(), 0);
         GLES20.glVertexAttribPointer(maPositionHandle, 3, GLES20.GL_FLOAT, false, 3*4, mVertexBuffer);
		 GLES20.glVertexAttribPointer(maTexCoorHandle, 2, GLES20.GL_FLOAT, false, 2*4, mTexCoorBuffer);
         
         GLES20.glEnableVertexAttribArray(maPositionHandle);
         GLES20.glEnableVertexAttribArray(maTexCoorHandle);
         
         GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
         GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, texId);
         
         GLES20.glDrawElements(GLES20.GL_TRIANGLES, miIndexCount, GLES20.GL_UNSIGNED_SHORT, indexBuffer);
    }
}
