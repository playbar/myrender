//=============================================================================
// FILE: SvrApi.java
//                  Copyright (c) 2016 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//==============================================================================

package com.qualcomm.svrapi;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.graphics.Point;
import android.content.ActivityNotFoundException;
import android.view.View;
import android.view.Display;
import android.view.WindowManager;
import android.view.Choreographer;
import android.view.Surface;
import android.util.Log;

public class SvrApi implements android.view.Choreographer.FrameCallback 
{
    public static final String TAG = "SvrApi";
    public static SvrApi handler = new SvrApi();

    public static native void nativeVsync(long lastVsyncNano);
    public static Choreographer choreographerInstance;

    public static void NotifyNoVr( final Activity act )
    {
        act.runOnUiThread(new Runnable() {
                public void run() {

                    try
                    {
                    AlertDialog.Builder alertBuilder = new AlertDialog.Builder(act);
                    alertBuilder.setMessage("SnapdragonVR not supported on this device!");
                    alertBuilder.setCancelable(true);

                    alertBuilder.setPositiveButton(
                        "Close",
                        new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                dialog.cancel();
                            }
                        });

                    AlertDialog alertDlg = alertBuilder.create();
                    alertDlg.show();
                    }
                    catch (Exception e)
                    {
                        Log.e(TAG, "Exception displaying dialog box!");
                        Log.e(TAG, e.getMessage());
                    }
            }
        });
    }

    public static long getVsyncOffsetNanos( Activity act)
    {
         final Display disp = act.getWindowManager().getDefaultDisplay();
		 return disp.getAppVsyncOffsetNanos();
    }
	
	public static float getRefreshRate( Activity act )
	{
		final Display disp = act.getWindowManager().getDefaultDisplay();
		return disp.getRefreshRate();
	}
	
	public static int getDisplayWidth( Activity act )
	{
		final Display disp = act.getWindowManager().getDefaultDisplay();
		Point outSize = new Point();
		disp.getRealSize(outSize);
		return outSize.x;
	}
	
	public static int getDisplayHeight( Activity act )
	{
		final Display disp = act.getWindowManager().getDefaultDisplay();
		Point outSize = new Point();
		disp.getRealSize(outSize);
		return outSize.y;
	}
	
	public static int getDisplayOrientation( Activity act )
	{
		final Display disp = act.getWindowManager().getDefaultDisplay();
		int rot = disp.getRotation();
		if(rot == Surface.ROTATION_0)
		{
			return 0;
		}
		else if(rot == Surface.ROTATION_90)
		{
			return 90;
		}
		else if(rot == Surface.ROTATION_180)
		{
			return 180;
		}
		else if(rot == Surface.ROTATION_270)
		{
			return 270;
		}
		else
		{
			return -1;
		}
	}

    public static void startVsync( Activity act )
    {
    	act.runOnUiThread( new Thread()
    	{
		 @Override
    		public void run()
    		{
				// Look this up now, so the callback (which will be on the same thread)
				// doesn't have to.
				choreographerInstance = Choreographer.getInstance();

				// Make sure we never get multiple callbacks going.
				choreographerInstance.removeFrameCallback(handler);

				// Start up our vsync callbacks.
				choreographerInstance.postFrameCallback(handler);
    		}
    	});
	}

    public static void stopVsync( Activity act )
    {
		// This may not need to be run on the UI thread, but it doesn't hurt.
    	act.runOnUiThread( new Thread()
    	{
		 @Override
    		public void run()
    		{
				choreographerInstance.removeFrameCallback(handler);
    		}
    	});
	}

    public void doFrame(long frameTimeNanos)
    {
		nativeVsync(frameTimeNanos);
		choreographerInstance.postFrameCallback(this);
	}
}
