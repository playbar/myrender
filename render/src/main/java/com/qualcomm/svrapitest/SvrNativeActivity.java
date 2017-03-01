//=============================================================================
// FILE: SvrNativeActivity.java
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================
package com.qualcomm.svrapitest;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.app.Activity;

public class SvrNativeActivity extends android.app.NativeActivity
{
	static
	{
		System.loadLibrary( "svrapi" );
	}

	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Log.v("test","oncreate");
	}
	@Override 
	public void onWindowFocusChanged (boolean hasFocus)
	{
		if(android.os.Build.VERSION.SDK_INT >= 19) 
		{
			if(hasFocus) 
			{
				getWindow().getDecorView().setSystemUiVisibility(
						View.SYSTEM_UI_FLAG_LAYOUT_STABLE
						| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
						| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
						| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
						| View.SYSTEM_UI_FLAG_FULLSCREEN
						| View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY);
			}
		}
	}
}
