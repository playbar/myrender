package com.bfmj.viewcore.render;

public class GLScreenParams {
	private static float FOV = 96;
	private static float NEAR = 0.1f;
	private static float FAR = 200.0f;
	private static float screenWidth;
	private static float screenHeight;
	private static float xDpi = 2400;
	private static float yDpi = 2400;
	private static float defualtDepth = 4.0f;
	private static float eyeDistance = 0.06f;
	private static float scaleRate;

	public static float getFOV(){
		return FOV;
	}

	public static void setFOV(float fov){
		FOV = fov;
	}
	
	public static float getNear(){
		return NEAR;
	}
	
	public static void setNear(float near){
		NEAR = near;
	}
	
	public static float getFar(){
		return FAR;
	}
	
	public static float getScreenWidth(){
		return 4.0f * (float) Math.tan(FOV / 2) * 2;
	}
	
	public static float getScreenHeight(){
		return 4.0f * (float) Math.tan(FOV / 2) * 2;
	}
	
	public static void setDpi(float x, float y){
		xDpi = x;
		yDpi = y;
	}
	
	public static void setXDpi(float x){
		xDpi = x;
	}
	
	public static void setYDpi(float y){
		yDpi = y;
	}
	
	public static float getXDpi(){
		return xDpi;
	}
	
	public static float getYDpi(){
		return yDpi;
	}

	public static float getDefualtDepth() {
		return defualtDepth;
	}

	public static void setDefualtDepth(float defualtDepth) {
		GLScreenParams.defualtDepth = defualtDepth;
	}

	public static float getEyeDistance() {
		return eyeDistance;
	}

	public static void setEyeDistance(float eyeDistance) {
		GLScreenParams.eyeDistance = eyeDistance;
	}

	public static float getScaleRate() {
		return xDpi / 1024f;
	}
}
