package com.bfmj.distortion;


import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.IntBuffer;

import com.baofeng.mojing.EyeTextureParameter;
import com.baofeng.mojing.MojingSDK;

import android.graphics.Bitmap;
import android.opengl.GLES30;
import android.util.Log;

public class Distortion {	
	private int[] mTextureIds = {0, 0};
	private int framebufferId;
	private int depthbufferId;
	private int colorbufferId;
	private int mTextureWidth = 0;
	private int mTextureHeight = 0;
	private static Distortion mInstance;
	
	public static Distortion getInstance() {    
		if (mInstance == null) {
			mInstance = new Distortion(); 
		}    
		
		return mInstance;    
	}
	
	public Distortion() {
		initData();
	}
	
	public void setScreen(int width, int height) {
		this.framebufferId = generateFrameBufferObject();
		IntBuffer renderBuffer = IntBuffer.allocate(2);
		GLES30.glGenRenderbuffers(2, renderBuffer);
		this.depthbufferId = renderBuffer.get(0);
		this.colorbufferId = renderBuffer.get(1);

		// bind framebuffer with renderbuffer
		GLES30.glBindFramebuffer(GLES30.GL_FRAMEBUFFER, this.framebufferId);

		GLES30.glBindRenderbuffer( GLES30.GL_RENDERBUFFER, this.depthbufferId);
		GLES30.glRenderbufferStorage( GLES30.GL_RENDERBUFFER, GLES30.GL_DEPTH_COMPONENT24,
				2048, 2048);
		GLES30.glBindRenderbuffer( GLES30.GL_RENDERBUFFER, this.colorbufferId);
		GLES30.glRenderbufferStorage( GLES30.GL_RENDERBUFFER, GLES30.GL_RGBA,
				2048, 2048);

		GLES30.glFramebufferRenderbuffer(GLES30.GL_FRAMEBUFFER, GLES30.GL_DEPTH_ATTACHMENT,
				GLES30.GL_RENDERBUFFER, this.depthbufferId);
		GLES30.glFramebufferRenderbuffer(GLES30.GL_RENDERBUFFER, GLES30.GL_COLOR_ATTACHMENT0,
				GLES30.GL_RENDERBUFFER, this.colorbufferId);

		GLES30.glBindRenderbuffer(GLES30.GL_RENDERBUFFER, 0);
		GLES30.glBindFramebuffer(GLES30.GL_FRAMEBUFFER, 0);
	}
	
	private void initData() {
	    this.framebufferId = -1;
	}
	
	public void beforeDraw(int eye){
		GLES30.glBindFramebuffer(GLES30.GL_FRAMEBUFFER, this.framebufferId);
		EyeTextureParameter textureParameter = MojingSDK.GetEyeTextureParameter(eye + 1);
		this.mTextureIds[eye] = textureParameter.m_EyeTexID;
		GLES30.glFramebufferTexture2D(GLES30.GL_FRAMEBUFFER, GLES30.GL_COLOR_ATTACHMENT0, GLES30.GL_TEXTURE_2D, this.mTextureIds[eye], 0);

		GLES30.glDepthMask(true);
		GLES30.glEnable(GLES30.GL_DEPTH_TEST);
		GLES30.glClear(GLES30.GL_DEPTH_BUFFER_BIT | GLES30.GL_COLOR_BUFFER_BIT );
//    	GLES30.glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//		GLES30.glClearColor(0.3f, 0.3f, 0.6f, 1.0f);
    	mTextureWidth = textureParameter.m_Width;
    	mTextureHeight = textureParameter.m_Height;
    	GLES30.glViewport(0, 0, mTextureWidth, mTextureHeight);
	}

	public void ReadData(){
		int width = mTextureWidth;
		int height = mTextureHeight;
		IntBuffer PixelBuffer = IntBuffer.allocate(width*height);
		PixelBuffer.position(0);
		GLES30.glReadPixels(0, 0, width, height, GLES30.GL_RGBA, GLES30.GL_UNSIGNED_BYTE, PixelBuffer);
		PixelBuffer.position(0);//这里要把读写位置重置下
		int pix[] = new int[width*height];
		PixelBuffer.get(pix);//这是将intbuffer中的数据赋值到pix数组中

		Bitmap bmp = Bitmap.createBitmap(pix, width, height,Bitmap.Config.ARGB_8888);//pix是上面读到的像素
		FileOutputStream fos = null;
		try {
			fos = new FileOutputStream("/sdcard/screen.png");//注意app的sdcard读写权限问题
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}

		bmp.compress(Bitmap.CompressFormat.PNG, 100, fos);//压缩成png,100%显示效果
		try {
			fos.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}

	}

	
	public void afterDraw() {
//		ReadData();
		GLES30.glFramebufferTexture2D(GLES30.GL_FRAMEBUFFER, GLES30.GL_COLOR_ATTACHMENT0, GLES30.GL_TEXTURE_2D, 0, 0);
		GLES30.glBindFramebuffer(GLES30.GL_FRAMEBUFFER, 0);

//		Log.e("Distortion","afterDraw begin");
		if (GLES30.glIsTexture(this.mTextureIds[0]) && GLES30.glIsTexture(this.mTextureIds[1])){
			MojingSDK.DrawTexture(this.mTextureIds[0], this.mTextureIds[1]);
		} else {
			this.framebufferId = generateFrameBufferObject();
		}
//		Log.e("Distortion","afterDraw end");
		GLES30.glDepthMask(false);
		GLES30.glDisable(GLES30.GL_DEPTH_TEST);
	}
	
	private static int generateFrameBufferObject() {
		IntBuffer framebuffer = IntBuffer.allocate(1);
		GLES30.glGenFramebuffers(1, framebuffer); 
		
		return framebuffer.get(0);
	}
	
	public int getTextureWidth(){
		return mTextureWidth;
	}
	
	public int getTextureHeight(){
		return mTextureHeight;
	}

}
