package com.bn.mojing;
import static com.bn.mojing.ShaderUtil.createProgram;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;
import java.util.ArrayList;
import android.opengl.GLES20;
import android.opengl.GLES30;

public class FovTestModel {
	int mProgram;			//Shader的运行脚本ID
    int muMVPMatrixHandle;	//Shader的参数，用于MVP矩阵
	int muPMatrixHandle;
    int maPositionHandle; 	//
    int maTexCoorHandle;
    
	final int[] vertexbuffer = new int[1];
    final int[] uvbuffer = new int[1];
	final int[] indexbuffer = new int[1];


	FloatBuffer   mVertexBuffer;
	FloatBuffer   mTexCoorBuffer;
	ShortBuffer indexBuffer;

    int miIndexCount = 6;
	int mPosCount = 4;
    public FovTestModel()
    {
    	creatSkyBox( );
    	
    	String vertexShader = 
    			"uniform mat4 uMVPMatrix;" +
				"uniform mat4 uPMatrix;" +
    			"attribute vec3 aPosition;" +
    			"attribute vec2 aTexCoor;" +
    			"varying vec2 vTextureCoord;" +
    			"void main()" +     
    			"{" +  			                          	
    			"   gl_Position = uPMatrix * uMVPMatrix * vec4(aPosition,1);" +
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
//    	float []VertexBase ={
//				-50 ,-50, -50,
//				-50 , 50, -50,
//				 50 ,-50, -50,
//				 50 , 50, -50,
//		};
		int xx = 10;
		int yy = xx;
		mPosCount = xx * yy;
		float val = (float) (Math.atan2(1, Math.sqrt(2.0)) * 180 / Math.PI);
        float r = (float) Math.sqrt(50 * 50 * 3);
		float []VertexBase = new float[3 * xx * yy];
		float step = val * 2/(xx - 1);
		for( int x = 0; x < xx; x++ ){
			for(int y = 0; y < yy; y++){
				VertexBase[x* 3 * xx + y*3 + 0] = -val + x * step;
				VertexBase[x* 3 * xx + y*3 + 1] = -val + y * step;
				VertexBase[x* 3 * xx + y*3 + 2] = -r;
			}
		}

//		float []VertexBase ={
//				-val ,-val, -r,
//				-val, val, -r,
//				val ,-val, -r,
//				val, val, -r,
//		};

    	ByteBuffer vbb = ByteBuffer.allocateDirect(mPosCount * 3 *4);
        vbb.order(ByteOrder.nativeOrder());
        mVertexBuffer = vbb.asFloatBuffer();
        mVertexBuffer.put(VertexBase);
        mVertexBuffer.position(0);

//    	float UV[] = {
//				0, 1,
//    			0, 0,
//				1, 1,
//				1, 0,
//		};
		step = 1.0f / (xx -1);
		float []UV = new float[xx * yy * 2];
		for( int x = 0; x < xx; x++ ){
			for(int y = 0; y < yy; y++ ){
				UV[x * 2 * xx + y * 2 + 0] = x * step;
				UV[x * 2 * xx + y * 2 + 1] = 1 - y * step;
			}
		}

    	 ByteBuffer tbb = ByteBuffer.allocateDirect(mPosCount * 2 * 4);
         tbb.order(ByteOrder.nativeOrder());
         mTexCoorBuffer = tbb.asFloatBuffer();
         mTexCoorBuffer.put(UV);
         mTexCoorBuffer.position(0);

		miIndexCount = (xx - 1) * (yy - 1) * 2 * 3;
//    	short alIndex []= {
//				0,1,2 ,
//				1,2,3 ,
//    	};
        short []alIndex = new short[miIndexCount];
		for( short x = 0; x < (xx - 1); x++ ){
			for( short y = 0; y < (yy - 1); y++ ){
				alIndex[x * 6 * (xx - 1) + y * 6 + 0] = x;
				alIndex[x * 6 * (xx - 1) + y * 6 + 1] = (short) (x + 1);
				alIndex[x * 6 * (xx - 1) + y * 6 + 2] = (short) ((y + 1) * yy);

				alIndex[x * 6 * (xx - 1) + y * 6 + 3] = (short) (x + 1);
				alIndex[x * 6 * (xx - 1) + y * 6 + 4] = (short) ((y + 1) * yy);
				alIndex[x * 6 * (xx - 1) + y * 6 + 5] = (short) ((y + 1) * yy + x + 1);
			}
		}
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
		muPMatrixHandle = GLES20.glGetUniformLocation(mProgram, "uPMatrix");
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
         
//         GLES20.glUniformMatrix4fv(muMVPMatrixHandle, 1, false, MatrixState.getFinalMatrix(), 0);
		GLES20.glUniformMatrix4fv(muMVPMatrixHandle, 1, false, MatrixState.getMVMatrix(), 0);
		GLES20.glUniformMatrix4fv(muPMatrixHandle, 1, false, MatrixState.getProjMatrix(), 0);
         
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

//        GLES20.glDrawArrays(GLES20.GL_TRIANGLE_FAN, 0, mPosCount);
        
 	    GLES20.glDrawElements(GLES20.GL_TRIANGLES,  miIndexCount, GLES20.GL_UNSIGNED_SHORT,0 );
 	    GLES20.glClearColor(0,0,0,1);
// 	    GLES20.glDrawElements(GLES20.GL_LINES,  miIndexCount, GLES20.GL_UNSIGNED_SHORT,0 );
 	   
 	    
 		 GLES20.glUseProgram(0);  
 		 GLES20.glBindBuffer(GLES20.GL_ARRAY_BUFFER, 0);
 	     GLES20.glBindBuffer(GLES20.GL_ELEMENT_ARRAY_BUFFER, 0 );
 	    
 	    GLES20.glDisableVertexAttribArray(maPositionHandle );
 	    GLES20.glDisableVertexAttribArray( maTexCoorHandle );
    }

}
