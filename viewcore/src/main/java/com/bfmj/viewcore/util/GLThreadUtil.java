package com.bfmj.viewcore.util;

import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by mac on 16/8/19.
 */
public class GLThreadUtil {

    public static native void onDrawFrame(GL10 gl);

    public static native void onSurfaceChanged(GL10 gl, int width, int height);

    public static native void onSurfaceCreated(GL10 gl, EGLConfig config);

}
