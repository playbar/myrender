package com.bn.mojing;

import java.io.InputStream;
import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.opengl.GLSurfaceView;
import android.util.Log;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import com.baofeng.mojing.MojingSDK;
import com.bn.mojing.MatrixState;
import com.bn.mojing.SphereModel;

import android.opengl.GLES20;
import android.opengl.GLES30;
import android.opengl.GLUtils;
import java.nio.IntBuffer;
import java.lang.Math;

import android.content.Context;
import java.io.*;
import com.mj.b4815.R;

public class VrRomNativeRender implements GLSurfaceView.Renderer {
	static int[] fboIds = { -1, -1};
	static int[] fboTexs = { -1, -1, -1, -1, -1};
	int g_LeftTexLayout = 0;
	int g_RightTexLayout = 0;
	Activity mContext;
	public VrRomNativeRender(Activity activity) {
		//this.mojingSurfaceView = mojingSurfaceView;
		mContext = activity;
	}

	void checkGlError()
	{
		int err = GLES20.glGetError();

		if (err == GLES20.GL_NO_ERROR) {
		} else {
			switch (err) {
			case GLES20.GL_INVALID_ENUM:
				System.out.print("invalid enum");
				break;
			case GLES20.GL_INVALID_VALUE:
				System.out.print("invalid value");
				break;
			case GLES20.GL_INVALID_FRAMEBUFFER_OPERATION:
				System.out.print("invalid framebuffer operation");
				break;
			case GLES20.GL_INVALID_OPERATION:
				System.out.print("invalid operation");
				break;
			case GLES20.GL_OUT_OF_MEMORY:
				System.out.print("out of memory");
				break;
			}
		}
	}
	public int initImageTexture(int iResourceID) {
		InputStream is = mContext.getResources().openRawResource(iResourceID);
		Bitmap bitmap;
		bitmap = BitmapFactory.decodeStream(is);

		int[] textures = new int[1];
		GLES20.glGenTextures(1, textures, 0);

		int textureId = textures[0];
		GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, textureId);

		GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
		GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
		GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
		GLES20.glTexParameterf(GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);

		GLUtils.texImage2D( GLES20.GL_TEXTURE_2D, 0, bitmap, 0);

		bitmap.recycle();

		return textureId;
	}

	int iMaxRender = 2;
	boolean bUsingLayer = true;
	int EyeTex[] = {0 , 0};
	float Camera[] = { -1f , 1f};

	float[] fM  = new float[16];
	int iDrawFrames = 0;
	public void onDrawFrame(GL10 gl) {
		for (int iEye = 0 ; iEye < iMaxRender ; iEye++) {
			GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, fboIds[0]);
			com.baofeng.mojing.EyeTextureParameter EyeTexture = com.baofeng.mojing.MojingSDK.GetEyeTextureParameter(iEye + 1);

			EyeTex[iEye] = EyeTexture.m_EyeTexID;
			if (EyeTex[iEye] != 0 && GLES20.glIsTexture(EyeTex[iEye])) {
				GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, EyeTexture.m_EyeTexID, 0);

				GLES20.glViewport(0, 0, EyeTexture.m_Width, EyeTexture.m_Height);

				int status = GLES20.glCheckFramebufferStatus(GLES20.GL_FRAMEBUFFER);
				if (status == GLES20.GL_FRAMEBUFFER_COMPLETE) {
					GLES20.glClearColor(0, 0, 0, 1);

					// update MVP 
					com.baofeng.mojing.MojingSDK.getLastHeadView(fM);
					MatrixState.setViewMatrix(fM);

					//com.baofeng.mojing.MojingVRRom.DrawModel(modelKey, iEye, fboTexs[1], MatrixState.getFinalMatrix());
					com.baofeng.mojing.MojingVRRom.DrawModel(rectKey, iEye, fboTexs[4], MatrixState.getFinalMatrix());

				}
			}// endi if (EyeTex[iEye] != 0)
			GLES20.glFramebufferTexture2D(GLES20.GL_FRAMEBUFFER, GLES20.GL_COLOR_ATTACHMENT0, GLES20.GL_TEXTURE_2D, 0, 0);
			GLES20.glBindFramebuffer(GLES20.GL_FRAMEBUFFER, 0);
		}


		com.baofeng.mojing.MojingSDK.DrawTexture(EyeTex[0], EyeTex[iMaxRender - 1]  );
		iDrawFrames = (iDrawFrames + 1) % 200;

		//if (iDrawFrames == 50) {
		//	com.baofeng.mojing.MojingSDK3288.DrawOverlay(com.baofeng.mojing.MojingSDK3288.OverlayIndexOSD() , g_LeftTexLayout , g_LeftTexLayout, 0 , 0 , 1 , 1 , 0 );
		//	com.baofeng.mojing.MojingSDK3288.DrawOverlay(com.baofeng.mojing.MojingSDK3288.OverlayIndexAIM() , g_RightTexLayout , g_RightTexLayout, 0.48f , 0.48f , 0.04f , 0.04f , 0 );
		//} else if (iDrawFrames == 150) {
		//	com.baofeng.mojing.MojingSDK3288.DrawOverlay(com.baofeng.mojing.MojingSDK3288.OverlayIndexOSD() , g_RightTexLayout , g_RightTexLayout, 0 , 0 , 1 , 1 , 0 );
		//	com.baofeng.mojing.MojingSDK3288.DrawOverlay(com.baofeng.mojing.MojingSDK3288.OverlayIndexAIM() , g_LeftTexLayout , g_LeftTexLayout, 0.48f , 0.48f , 0.04f , 0.04f , 0 );
		//}
	}

	static int modelKey = 0;
	static int rectKey = 0;
	public void onSurfaceChanged(GL10 gl, int width, int height) {
		// setup MVP
		float fFOV = com.baofeng.mojing.MojingSDK.GetMojingWorldFOV(); 
            float ratio = (float) Math.tan(Math.toRadians(fFOV/2)) * 1.0f;
            Log.e("Mojing", String.format("radio is %f", ratio));
			float fNear = 400 * 0.1f;
			float fFar = 400 *3f;
            MatrixState.setProjectFrustum(-ratio * fNear, ratio * fNear, -ratio * fNear, ratio * fNear, fNear, fFar);
            MatrixState.setCamera(0,0,0,		
								  0f,0.0f,-0.1f,
								  0f,1.0f,0.0f); 


		// gen fbo
		GLES20.glGenFramebuffers(2, fboIds, 0);
		checkGlError();
		

		//modelKey = com.baofeng.mojing.MojingVRRom.CreateModel(1, 0);
		System.out.print(atlas_json);
		rectKey = com.baofeng.mojing.MojingVRRom.CreateModel(4, 0, atlas_json);
		float[] pos = {
			-1.0f, 1.0f, -10.0f,
				+1.0f, 1.0f, -10.0f,
				+1.0f, -1.0f, -10.0f,
				-1.0f, -1.0f, -10.0f,
		};
		com.baofeng.mojing.MojingVRRom.SetRectModelPosition(rectKey, pos);
	}

	static String atlas_json;

	private String readFromFile(Context context) {

		String ret = "";

		try {
			InputStream inputStream = mContext.getResources().getAssets().open("atlas_settings.json");

			if ( inputStream != null ) {
				InputStreamReader inputStreamReader = new InputStreamReader(inputStream);
				BufferedReader bufferedReader = new BufferedReader(inputStreamReader);
				String receiveString = "";
				StringBuilder stringBuilder = new StringBuilder();

				while ( (receiveString = bufferedReader.readLine()) != null ) {
					stringBuilder.append(receiveString);
				}

				inputStream.close();
				ret = stringBuilder.toString();
			}
		}
		catch (FileNotFoundException e) {
			Log.e("login activity", "File not found: " + e.toString());
		} catch (IOException e) {
			Log.e("login activity", "Can not read file: " + e.toString());
		}

		return ret;
	}

	public void onSurfaceCreated(GL10 gl, EGLConfig config) {
		MatrixState.setInitStack();

		//atlas_json = mContext.getResources().getString(R.string.atlas_settings);
		atlas_json = readFromFile(mContext.getApplicationContext());

		fboTexs[0] = initImageTexture(R.drawable.skybox);
		fboTexs[1] = initImageTexture(R.drawable.strip1);
		//fboTexs[2] = initImageTexture(R.drawable.sphere_v_split_00);
		//fboTexs[3] = initImageTexture(R.drawable.sphere_h_split_00);
		fboTexs[4] = initImageTexture(R.drawable.atlas_settings); 

		g_LeftTexLayout = initImageTexture(R.drawable.star);
		g_RightTexLayout = initImageTexture(R.drawable.bird);
	}
}