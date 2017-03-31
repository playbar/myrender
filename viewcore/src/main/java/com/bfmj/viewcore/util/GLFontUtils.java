package com.bfmj.viewcore.util;

import android.content.Context;
import android.graphics.Typeface;

public class GLFontUtils {
	private Typeface typeface;
	private static GLFontUtils mInstance;
	
	public static GLFontUtils getInstance(Context context) {    
		if (mInstance == null) {
			mInstance = new GLFontUtils(context); 
		}    
		
		return mInstance;    
	}
	
	public GLFontUtils(Context context) {
		try {
			typeface = Typeface.createFromAsset(context.getAssets(), "fz.ttf");
		} catch (Exception e) {}
	}
	
	public Typeface getFontTypeface() {
		return typeface;
	}
}
