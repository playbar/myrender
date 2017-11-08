//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.google.vr.ndk.base;

import android.app.PendingIntent;
import android.content.Context;
import android.os.Handler;
import android.os.RemoteException;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Surface;
import android.view.View;
import android.widget.FrameLayout;

import com.baofeng.mojing.MojingSDK;
import com.google.vr.cardboard.ContextUtils;
import com.google.vr.cardboard.VrContextWrapper;
import com.google.vr.cardboard.annotations.UsedByReflection;
import com.google.vr.vrcore.library.api.IGvrLayout;
import com.google.vr.vrcore.library.api.ObjectWrapper;

@UsedByReflection("Unity")
public class GvrLayout extends FrameLayout {
    private static final String TAG = "GvrLayout";
    private IGvrLayout impl;
    private GvrUiLayout uiLayout;
    private GvrApi gvrApi;
    private ExternalSurface videoSurface;

    public GvrLayout(Context var1) {
        super(var1);
        if(ContextUtils.getActivity(var1) == null) {
            throw new IllegalArgumentException("An Activity Context is required for VR functionality.");
        } else {
            this.init();
        }
    }

    public GvrLayout(Context var1, AttributeSet var2, int var3, int var4) {
        super(var1, var2, var3, var4);
        if(ContextUtils.getActivity(var1) == null) {
            throw new IllegalArgumentException("An Activity Context is required for VR functionality.");
        } else {
            this.init();
        }
    }

    public GvrLayout(VrContextWrapper var1) {
        super(var1);
        this.init();
    }

    GvrLayout(Context var1, GvrLayoutImpl var2, GvrApi var3) {
        super(var1);
        this.impl = new GvrLayoutImplWrapper(var2);
        this.gvrApi = var3;
        this.init();
    }

    private void init() {
        TraceCompat.beginSection("GvrLayout.init");

        try {
            if(this.impl == null) {
                this.impl = GvrLayoutFactory.create(this.getContext());
            }

            this.uiLayout = new GvrUiLayout(this.impl.getUiLayout());
            if(this.gvrApi == null) {
                this.gvrApi = new GvrApi(this.getContext(), this.impl.getNativeGvrContext());
            }
            MojingSDK.hookFun();
            this.addView((View)ObjectWrapper.unwrap(this.impl.getRootView(), View.class));
        } catch (RemoteException var5) {
            throw new RuntimeException(var5);
        } finally {
            TraceCompat.endSection();
        }

    }

    @UsedByReflection("Unity")
    public GvrUiLayout getUiLayout() {
        return this.uiLayout;
    }

    @UsedByReflection("Unity")
    public void onPause() {
        TraceCompat.beginSection("GvrLayout.onPause");

        try {
            this.impl.onPause();
        } catch (RemoteException var5) {
            throw new RuntimeException(var5);
        } finally {
            TraceCompat.endSection();
        }

    }

    @UsedByReflection("Unity")
    public void onResume() {
        TraceCompat.beginSection("GvrLayout.onResume");

        try {
            this.impl.onResume();
        } catch (RemoteException var5) {
            throw new RuntimeException(var5);
        } finally {
            TraceCompat.endSection();
        }

    }

    public void onBackPressed() {
        try {
            this.impl.onBackPressed();
        } catch (RemoteException var2) {
            throw new RuntimeException(var2);
        }
    }

    @UsedByReflection("Unity")
    public void shutdown() {
        TraceCompat.beginSection("GvrLayout.shutdown");

        try {
            this.impl.shutdown();
        } catch (RemoteException var5) {
            throw new RuntimeException(var5);
        } finally {
            TraceCompat.endSection();
        }

    }

    @UsedByReflection("Unity")
    public void setPresentationView(View var1) {
        try {
            this.impl.setPresentationView(ObjectWrapper.wrap(var1));
        } catch (RemoteException var3) {
            throw new RuntimeException(var3);
        }
    }

    public void setFixedPresentationSurfaceSize(int var1, int var2) {
        this.getGvrApi().setSurfaceSize(var1, var2);
    }

    public boolean enableAsyncReprojectionVideoSurface(GvrLayout.ExternalSurfaceListener var1, Handler var2, boolean var3) {
        int var4 = var3?1:0;

        try {
            if(!this.impl.enableAsyncReprojection(var4)) {
                return false;
            }
        } catch (RemoteException var6) {
            throw new RuntimeException(var6);
        }

        if(!this.gvrApi.isFeatureSupported(2)) {
            Log.e("GvrLayout", "External Surfaces are unsupported. Cannot enable video Surface.");
            return false;
        } else {
            this.videoSurface = this.gvrApi.createExternalSurface(var1, var2);
            return true;
        }
    }

    @UsedByReflection("Unity")
    public boolean setAsyncReprojectionEnabled(boolean var1) {
        if(!var1) {
            if(this.gvrApi.getAsyncReprojectionEnabled()) {
                throw new UnsupportedOperationException("Async reprojection cannot be disabled once enabled.");
            } else {
                return true;
            }
        } else {
            try {
                return this.impl.enableAsyncReprojection(0);
            } catch (RemoteException var3) {
                throw new RuntimeException(var3);
            }
        }
    }

    @UsedByReflection("Unity")
    public boolean enableAsyncReprojectionProtected() {
        try {
            return this.impl.enableAsyncReprojection(1);
        } catch (RemoteException var2) {
            throw new RuntimeException(var2);
        }
    }

    public boolean enableCardboardTriggerEmulation(Runnable var1) {
        try {
            return this.impl.enableCardboardTriggerEmulation(ObjectWrapper.wrap(var1));
        } catch (RemoteException var3) {
            throw new RuntimeException(var3);
        }
    }

    public int getAsyncReprojectionVideoSurfaceId() {
        if(this.videoSurface == null) {
            Log.w("GvrLayout", "Async reprojection video is not enabled. Did you call enableAsyncReprojectionVideoSurface()?");
            return -1;
        } else {
            return this.videoSurface.getId();
        }
    }

    public Surface getAsyncReprojectionVideoSurface() {
        if(this.videoSurface == null) {
            Log.w("GvrLayout", "Async reprojection video is not enabled. Did you call enableAsyncReprojectionVideoSurface()?");
            return null;
        } else {
            return this.videoSurface.getSurface();
        }
    }

    @UsedByReflection("Unity")
    public GvrApi getGvrApi() {
        return this.gvrApi;
    }

    public void setStereoModeEnabled(boolean var1) {
        try {
            this.impl.setStereoModeEnabled(var1);
        } catch (RemoteException var3) {
            throw new RuntimeException(var3);
        }
    }

    public void setReentryIntent(PendingIntent var1) {
        try {
            this.impl.setReentryIntent(ObjectWrapper.wrap(var1));
        } catch (RemoteException var3) {
            throw new RuntimeException(var3);
        }
    }

    public interface ExternalSurfaceListener {
        void onSurfaceAvailable(Surface var1);

        void onFrameAvailable();
    }
}
