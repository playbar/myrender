package com.baofeng.mojing;

public class MojingVRRom
{
static{
		System.loadLibrary("mojingvrrom");
		}
	/*
	modelType: 0 skybox, 1 sphere
	textureType: 0 skybox
				 1 sphere
				 2 sphere
				 3 sphere
				 4 Rect
	modelView: { -0.5f, 0.0f, 0.0f, // leftCameraPos
				 0.5f, 0.0f, 0.0f, // rightCameraPos
				 0.0f, 0.0f, -1.0f, // lookAt
				 0.0f, 1.0f, 0.0f // upVec
			   };
	perspective:
	*/
    public static native int CreateModel(int modelType, int textureType, String atlasJson);
    
	/*
	iModelKey : return by CreateModel function
	eye: 0, 1
	textureId: model texture id
	mvp: Model View Projection Matrix
	*/
	public static native void DrawModel(int iModelKey , int eye, int textureId, float[] mvp);

	/*
	vertices:
	*/
	public static native void SetRectModelPosition(int iModelKey ,float[] vertices);
	
	public static int OverlayIndexOSD(){ return 1;} // FULL SCREEN , Z = 0x40000000
	public static int OverlayIndexAIM(){ return 0;} // 1440 x 256 , Z = 0x41000000

	public static native boolean DrawOverlay(int OverlayIndex, int LeftEyeTexID, int RightEyeTexID, float fLeft, float fTop, float fWidth, float fHeight, float DistanceInMetre);
	public static native boolean DirectionalRadiaInRect(float[] jDirection, float[] v2TopLeft, float[] v2BottomRight, float fZ, float[] v2OutPoint);
	public static native int SelectRectByDirectional(float[]  jm4Direction, float[]  jv3TopLeft, float[]  jv3BottomRight);

	// DO NOT CALL THIS EVER!
	public static native boolean CreateDistortion();

	// DO NOT CALL THIS EVER!
	public static native boolean DrawDistortion(int iTexture);
}
