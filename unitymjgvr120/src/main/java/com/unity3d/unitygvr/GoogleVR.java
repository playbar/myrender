//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.unitygvr;

import android.app.Activity;
import android.content.Context;
import android.util.Log;
import android.view.SurfaceView;
import com.google.vr.ndk.base.AndroidCompat;
import com.google.vr.ndk.base.GvrApi;
import com.google.vr.ndk.base.GvrLayout;
import com.google.vr.ndk.base.GvrUiLayout;

public class GoogleVR {
    private static final String TAG = "Unity";
    private GvrApi mGvrApi = null;
    private GvrLayout mGvrLayout = null;
    private GvrUiLayout mGvrUiLayout = null;
    private SurfaceView mGvrGlView = null;
    private Activity mActivity = null;
    private Context mContext = null;
    private boolean mGvrLoaded = false;
    private boolean mIsDaydream = false;

    public static native void HookInit();

    static {
        try {
            System.loadLibrary("gvrhook");
        } catch (UnsatisfiedLinkError var0) {
            ;
        }
    }

    public GoogleVR() {
    }

    public GvrLayout getGvrLayout() {
        return this.mGvrLayout;
    }

    public boolean initialize(Activity activity, Context context, SurfaceView glView, boolean isDaydream) {
        if(this.mGvrLoaded) {
            return true;
        } else if(activity != null && context != null && glView != null) {
            this.mActivity = activity;
            this.mContext = context;
            this.mGvrGlView = glView;
            this.mIsDaydream = isDaydream;
            return this.loadGvrLibrary();
        } else {
            Log.e("Unity", "One or more parameters were invalid when trying to initialize GoogleVR.");
            return false;
        }
    }

    public void deinitialize() {
        this.mGvrApi = null;
        this.mGvrLayout = null;
        this.mGvrUiLayout = null;
        this.mGvrGlView = null;
        this.mActivity = null;
        this.mContext = null;
        this.mGvrLoaded = false;
        this.mIsDaydream = false;
    }

    public long load(boolean enableAsynchronousProjection, boolean enableSustainedPerformanceMode, boolean enableTransitionView, Runnable closeButtonHandler) {
        if(this.mActivity != null && this.mContext != null) {
            if(this.mGvrLayout == null) {
                this.mGvrLayout = new GvrLayout(this.mContext);
//                GoogleVR.HookInit();
                this.mGvrLayout.setPresentationView(this.mGvrGlView);
                if(enableAsynchronousProjection) {
                    if(!this.mGvrLayout.setAsyncReprojectionEnabled(true)) {
                        Log.e("Unity", "Error Enabling Async Projection. Aborting load operation.");
                        return 0L;
                    }

                    if(enableSustainedPerformanceMode) {
                        AndroidCompat.setSustainedPerformanceMode(this.mActivity, true);
                    }
                }

                this.mGvrUiLayout = this.mGvrLayout.getUiLayout();
                if(closeButtonHandler != null) {
                    this.mGvrUiLayout.setCloseButtonListener(closeButtonHandler);
                }

                this.mGvrUiLayout.setTransitionViewEnabled(enableTransitionView);
                this.mGvrApi = this.mGvrLayout.getGvrApi();
                return this.mGvrApi.getNativeGvrContext();
            } else {
                return 0L;
            }
        } else {
            Log.e("Unity", "Google VR is in an invalid state. Failing load");
            return 0L;
        }
    }

    public void pause() {
        this.mGvrLayout.onPause();
    }

    public void resume() {
        this.mGvrLayout.onResume();
    }

    public boolean enable(boolean enabled) {
        if(this.mContext == null) {
            Log.e("Unity", "Google VR is in an invalid state. Failing enable.");
            return false;
        } else {
            AndroidCompat.setVrModeEnabled(this.mActivity, enabled);
            return true;
        }
    }

    public boolean unload() {
        this.mGvrLayout.shutdown();
        return true;
    }

    private static boolean loadLibraryStatic(String libName) {
        try {
            System.loadLibrary(libName);
            return true;
        } catch (UnsatisfiedLinkError var2) {
            Log.e("Unity", "Unable to find " + libName);
            return false;
        } catch (Exception var3) {
            Log.e("Unity", "Unknown error " + var3);
            return false;
        }
    }

    private boolean loadLibrary(String libName) {
        return loadLibraryStatic(libName);
    }

    private boolean loadGvrLibrary() {
        if(!this.mGvrLoaded) {
            this.mGvrLoaded = this.loadLibrary("gvr");
            if(!this.mGvrLoaded) {
                Log.e("Unity", "Unable to load Google GVR Library.");
            }
        }

        return this.mGvrLoaded;
    }
}
