package com.bfmj.viewcore.util;

import android.content.Context;
import android.graphics.Bitmap;
import android.util.Log;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;

/**
 * Created by lixianke on 2016/8/11.
 */
public class GenTextureTask implements Runnable {
    private Context mContext;
    private int mHashCode;
    private Bitmap mBmp;
    ExportTextureId exprotTex = null;
    //////////////
    public static EGLContext mEglContext;

    public void SetInterfaceTex( ExportTextureId tex ){
        this.exprotTex = tex;
    }

    public GenTextureTask(Context context, int hashcode, Bitmap bmp ) {
        this.mContext = context;
        this.mHashCode = hashcode;
        this.mBmp = bmp;
    }

    @Override
    public void run() {
        if( mEglContext == null ){
            return;
        }
//        Log.e("GetTextureTask", "--->run");
        EGL10 egl = (EGL10) EGLContext.getEGL();
        EGLDisplay eglDisplay =egl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY); // egl.eglGetCurrentDisplay();

        egl.eglMakeCurrent(eglDisplay, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_SURFACE, mEglContext);

//            int textureId = createTexture(mBmp);
        int textureId = GLTextureUtils.initImageTexture(mBmp, false, false);
        if( null != exprotTex ){
            exprotTex.exportId( textureId, mHashCode );
        }

        return;
    }

    public interface ExportTextureId{
        void exportId( int texid, int hashcode);
    }
}
