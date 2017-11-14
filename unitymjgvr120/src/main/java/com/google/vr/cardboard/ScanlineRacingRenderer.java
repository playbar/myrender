//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.google.vr.cardboard;

import android.graphics.Point;
import android.opengl.GLSurfaceView.Renderer;
import android.os.Process;
import com.google.vr.ndk.base.GvrApi;
import com.google.vr.ndk.base.GvrSurfaceView;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class ScanlineRacingRenderer implements Renderer {
    private static final String TAG = "ScanlineRacingRenderer";
    private final GvrApi gvrApi;
    private final ExternalSurfaceManager surfaceManager;
    private GvrSurfaceView gvrSurfaceView;

    public ScanlineRacingRenderer(GvrApi var1) {
        if(var1 == null) {
            throw new IllegalArgumentException("GvrApi must be supplied for proper scanline rendering");
        } else {
            this.gvrApi = var1;
            this.surfaceManager = new ExternalSurfaceManager(var1);
        }
    }

    public com.google.vr.ndk.base.GvrLayout.ExternalSurfaceManager getExternalSurfaceManager() {
        return this.surfaceManager;
    }

    public void setSurfaceView(GvrSurfaceView var1) {
        if(var1 == null) {
            throw new IllegalArgumentException("GvrSurfaceView must be supplied for proper scanline rendering");
        } else {
            this.gvrSurfaceView = var1;
        }
    }

    public void onSurfaceCreated(GL10 var1, EGLConfig var2) {
        Thread.currentThread().setPriority(10);
        AndroidNCompat.setVrThread(Process.myTid());
        this.gvrApi.onSurfaceCreatedReprojectionThread();
    }

    public void onSurfaceChanged(GL10 var1, int var2, int var3) {
        this.surfaceManager.consumerAttachToCurrentGLContext();
    }

    public void onDrawFrame(GL10 var1) {
        this.surfaceManager.consumerUpdateManagedSurfaces();
        Point var2;
        if((var2 = this.gvrApi.renderReprojectionThread()) != null) {
            this.setSurfaceSize(var2.x, var2.y);
        }

    }

    public void onDestroySurface() {
        this.onPause();
    }

    public void setSurfaceSize(final int var1, final int var2) {
        ThreadUtils.runOnUiThread(new Runnable() {
            public void run() {
                if(var1 > 0 && var2 > 0) {
                    ScanlineRacingRenderer.this.gvrSurfaceView.getHolder().setFixedSize(var1, var2);
                } else {
                    ScanlineRacingRenderer.this.gvrSurfaceView.getHolder().setSizeFromLayout();
                }
            }
        });
    }

    public void onPause() {
        this.gvrApi.onPauseReprojectionThread();
        this.surfaceManager.consumerDetachFromCurrentGLContext();
    }

    public void onResume()
    {
        this.surfaceManager.consumerAttachToCurrentGLContext();
    }

    public void shutdown() {
        this.surfaceManager.shutdown();
    }
}
