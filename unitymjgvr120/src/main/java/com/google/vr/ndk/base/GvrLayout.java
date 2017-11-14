//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.google.vr.ndk.base;

import android.app.PendingIntent;
import android.app.Presentation;
import android.content.ComponentName;
import android.content.Context;
import android.content.res.Configuration;
import android.hardware.display.DisplayManager;
import android.hardware.display.DisplayManager.DisplayListener;
import android.os.Handler;
import android.os.Looper;
import android.os.Build.VERSION;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Display;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager.InvalidDisplayException;
import android.widget.FrameLayout;
import android.widget.RelativeLayout.LayoutParams;

import com.baofeng.mojing.MojingSDK;
import com.google.vr.cardboard.ContextUtils;
import com.google.vr.cardboard.DisplaySynchronizer;
import com.google.vr.cardboard.DisplayUtils;
import com.google.vr.cardboard.EglFactory;
import com.google.vr.cardboard.EglReadyListener;
import com.google.vr.cardboard.MutableEglConfigChooser;
import com.google.vr.cardboard.ScanlineRacingRenderer;
import com.google.vr.cardboard.VrContextWrapper;
import com.google.vr.ndk.base.CardboardEmulator;
import com.google.vr.ndk.base.DaydreamTouchListener;
import com.google.vr.ndk.base.DaydreamUtilsWrapper;
import com.google.vr.ndk.base.FadeOverlayView;
import com.google.vr.ndk.base.GvrApi;
import com.google.vr.ndk.base.GvrSurfaceView;
import com.google.vr.ndk.base.GvrUiLayout;
import com.google.vr.ndk.base.TraceCompat;
import com.google.vr.ndk.base.VrCoreSdkClient;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.CountDownLatch;

public class GvrLayout extends FrameLayout {
    private static final String TAG = "GvrLayout";
    private static final boolean DEBUG = false;
    private static final int EXTERNAL_PRESENTATION_MIN_API = 16;
    private static final boolean CONTEXT_SHARING_ENABLED = false;
    private static final int SHOW_RENDERING_VIEWS_DELAY_FOR_FADE = 50;
    private static GvrLayout.PresentationFactory sOptionalPresentationFactory = null;
    private DaydreamUtilsWrapper daydreamUtils;
    private FrameLayout presentationLayout;
    private GvrUiLayout uiLayout;
    private DisplaySynchronizer displaySynchronizer;
    private View presentationView;
    private boolean isAsyncReprojectionVideoEnabled;
    private boolean isAsyncReprojectionUsingProtectedBuffers;
    private GvrLayout.AsyncReprojectionSurfaceView scanlineRacingView;
    private ScanlineRacingRenderer scanlineRacingRenderer;
    private EglReadyListener eglReadyListener;
    private EglFactory eglFactory;
    private FadeOverlayView fadeOverlayView;
    private GvrLayout.PresentationHelper presentationHelper;
    private VrCoreSdkClient vrCoreSdkClient;
    private DaydreamTouchListener daydreamTouchListener;
    private CardboardEmulator cardboardEmulator;
    private GvrApi gvrApi;
    private boolean isResumed = false;
    private int videoSurfaceId = -1;
    private boolean stereoModeEnabled = true;
    private final Runnable showRenderingViewsRunnable = new Runnable() {
        public void run() {
            GvrLayout.this.updateRenderingViewsVisibility(0);
        }
    };

    public GvrLayout(Context var1) {
        super(var1);
        if(ContextUtils.getActivity(var1) == null) {
            throw new IllegalArgumentException("An Activity Context is required for VR functionality.");
        } else {
            this.init((GvrApi)null, (DisplaySynchronizer)null, (EglReadyListener)null, (FadeOverlayView)null, (DaydreamUtilsWrapper)null);
        }
    }

    public GvrLayout(Context var1, AttributeSet var2, int var3, int var4) {
        super(var1, var2, var3, var4);
        if(ContextUtils.getActivity(var1) == null) {
            throw new IllegalArgumentException("An Activity Context is required for VR functionality.");
        } else {
            this.init((GvrApi)null, (DisplaySynchronizer)null, (EglReadyListener)null, (FadeOverlayView)null, (DaydreamUtilsWrapper)null);
        }
    }

    public GvrLayout(VrContextWrapper var1) {
        super(var1);
        this.init((GvrApi)null, (DisplaySynchronizer)null, (EglReadyListener)null, (FadeOverlayView)null, (DaydreamUtilsWrapper)null);
    }

    GvrLayout(Context var1, GvrApi var2, DisplaySynchronizer var3, EglReadyListener var4, FadeOverlayView var5, DaydreamUtilsWrapper var6) {
        super(var1);
        this.init(var2, var3, var4, var5, var6);
    }

    private void init(GvrApi var1, DisplaySynchronizer var2, EglReadyListener var3, FadeOverlayView var4, DaydreamUtilsWrapper var5) {
        TraceCompat.beginSection("GvrLayout.init");

        try {
            var2 = var2 != null?var2:GvrApi.createDefaultDisplaySynchronizer(this.getContext());
            MojingSDK.hookFun();
            var3 = var3 != null?var3:new EglReadyListener();
            var1 = var1 != null?var1:new GvrApi(this.getContext(), var2);
            var5 = var5 != null?var5:new DaydreamUtilsWrapper();
            this.daydreamUtils = var5;
            this.presentationLayout = new FrameLayout(this.getContext());
            this.uiLayout = new GvrUiLayout(this.getContext());
            this.gvrApi = var1;
            this.displaySynchronizer = var2;
            this.eglReadyListener = var3;
            this.presentationHelper = this.tryCreatePresentationHelper();
            this.addView(this.presentationLayout, 0);
            this.addView(this.uiLayout, 1);
            this.updateUiLayout();
            boolean var6;
            if(var6 = var5.isDaydreamPhone(this.getContext())) {
                this.daydreamTouchListener = this.createDaydreamTouchListener();
                this.uiLayout.setOnTouchListener(this.daydreamTouchListener);
            }

            int var7;
            boolean var8 = (var7 = var5.getComponentDaydreamCompatibility(this.getContext())) != 0;
            boolean var9 = var7 == 2;
            if(var6 || var9) {
                if(var8) {
                    this.fadeOverlayView = var4 != null?var4:new FadeOverlayView(this.getContext());
                    this.addView(this.fadeOverlayView, 2);
                }
                //delete daydream config
//                this.vrCoreSdkClient = this.createVrCoreSdkClient(this.getContext(), var1, var5, this.fadeOverlayView);
            }
        } finally {
            TraceCompat.endSection();
        }

    }

    public GvrUiLayout getUiLayout() {
        return this.uiLayout;
    }

    public void onWindowVisibilityChanged(int var1) {
        super.onWindowVisibilityChanged(var1);
        this.updateFadeVisibility();
    }

    public void onPause() {
        TraceCompat.beginSection("GvrLayout.onPause");

        try {
            this.gvrApi.pause();
            if(this.scanlineRacingView != null) {
                this.scanlineRacingView.queueEvent(new Runnable() {
                    public void run() {
                        GvrLayout.this.scanlineRacingRenderer.onPause();
                    }
                });
                this.scanlineRacingView.onPause();
            }

            if(this.presentationHelper != null) {
                this.presentationHelper.onPause();
            }

            this.displaySynchronizer.onPause();
            if(this.vrCoreSdkClient != null) {
                this.vrCoreSdkClient.onPause();
            }

            if(this.cardboardEmulator != null) {
                this.cardboardEmulator.onPause();
            }

            this.isResumed = false;
            this.updateFadeVisibility();
        } finally {
            TraceCompat.endSection();
        }

    }

    public void onResume() {
        TraceCompat.beginSection("GvrLayout.onResume");
//        MojingSDK.hookFun();
        try {
            this.gvrApi.resume();
            if(this.daydreamTouchListener != null) {
                this.daydreamTouchListener.refreshViewerProfile();
            }

            this.displaySynchronizer.onResume();
            if(this.presentationHelper != null) {
                this.presentationHelper.onResume();
            }

            if(this.scanlineRacingView != null) {
                this.scanlineRacingView.queueEvent(new Runnable() {
                    public void run() {
                        GvrLayout.this.scanlineRacingRenderer.onResume();
                    }
                });
                this.scanlineRacingView.onResume();
            }

            if(this.vrCoreSdkClient != null) {
                this.vrCoreSdkClient.onResume();
            }

            if(this.cardboardEmulator != null && this.gvrApi.getViewerType() == 1) {
                this.cardboardEmulator.onResume();
            }

            this.isResumed = true;
            this.updateFadeVisibility();
            this.updateUiLayout();
        } finally {
            TraceCompat.endSection();
        }

    }

    public void shutdown() {
        TraceCompat.beginSection("GvrLayout.shutdown");

        try {
            this.displaySynchronizer.shutdown();
            if(this.daydreamTouchListener != null) {
                this.daydreamTouchListener.shutdown();
            }

            this.removeView(this.presentationLayout);
            this.removeView(this.uiLayout);
            if(this.scanlineRacingRenderer != null) {
                this.scanlineRacingRenderer.shutdown();
                this.scanlineRacingRenderer = null;
            }

            this.scanlineRacingView = null;
            this.presentationView = null;
            if(this.presentationHelper != null) {
                this.presentationHelper.shutdown();
                this.presentationHelper = null;
            }

            if(this.vrCoreSdkClient != null) {
                this.vrCoreSdkClient.onPause();
                this.vrCoreSdkClient = null;
            }

            if(this.cardboardEmulator != null) {
                this.cardboardEmulator.onPause();
                this.cardboardEmulator = null;
            }

            if(this.gvrApi != null) {
                this.gvrApi.shutdown();
                this.gvrApi = null;
            }
        } finally {
            TraceCompat.endSection();
        }

    }

    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
        if(this.presentationHelper != null) {
            this.presentationHelper.onDetachedFromWindow();
        }

    }

    protected void onConfigurationChanged(Configuration var1) {
        super.onConfigurationChanged(var1);
        this.displaySynchronizer.onConfigurationChanged();
    }

    public void setPresentationView(View var1) {
        if(this.presentationView != null) {
            this.presentationLayout.removeView(this.presentationView);
        }

        this.presentationLayout.addView(var1, 0);
        this.presentationView = var1;
    }

    public void setFixedPresentationSurfaceSize(int var1, int var2) {
        this.gvrApi.setSurfaceSize(var1, var2);
    }

    public boolean enableAsyncReprojectionVideoSurface(GvrLayout.ExternalSurfaceListener var1, Handler var2, boolean var3) {
        if(!this.daydreamUtils.isDaydreamPhone(this.getContext())) {
            Log.e("GvrLayout", "Only Daydream devices support async reprojection. Cannot enable video Surface.");
            return false;
        } else if(this.scanlineRacingView != null) {
            Log.e("GvrLayout", "Async reprojection is already enabled. Cannot call enableAsyncReprojectionVideoSurface after calling setAsyncReprojectionEnabled.");
            return false;
        } else if(this.gvrApi.usingVrDisplayService()) {
            Log.e("GvrLayout", "Async reprojection video is not supported on this device.");
            return false;
        } else {
            this.isAsyncReprojectionVideoEnabled = true;
            this.isAsyncReprojectionUsingProtectedBuffers = var3;
            this.scanlineRacingRenderer = new ScanlineRacingRenderer(this.gvrApi);
            GvrLayout.ExternalSurfaceManager var4 = this.scanlineRacingRenderer.getExternalSurfaceManager();
            this.videoSurfaceId = var4.createExternalSurface(var1, var2);
            return true;
        }
    }

    public boolean setAsyncReprojectionEnabled(boolean var1) {
        if(Looper.getMainLooper() != Looper.myLooper()) {
            throw new IllegalStateException("setAsyncReprojectionEnabled may only be called from the UI thread");
        } else if(this.scanlineRacingView != null && !var1) {
            throw new UnsupportedOperationException("Async reprojection cannot be disabled once enabled");
        } else if(var1 && !this.daydreamUtils.isDaydreamPhone(this.getContext())) {
            return false;
        } else {
            boolean var2 = this.gvrApi.setAsyncReprojectionEnabled(var1);
            if(var1) {
                if(var2) {
                    if(!this.gvrApi.usingVrDisplayService()) {
                        this.addScanlineRacingView();
                    }
                } else {
                    Log.e("GvrLayout", "Failed to initialize async reprojection, unsupported device.");
                    this.isAsyncReprojectionVideoEnabled = false;
                    this.scanlineRacingRenderer = null;
                }
            }

            return var2;
        }
    }

    public boolean enableCardboardTriggerEmulation(Runnable var1) {
        if(var1 == null) {
            throw new IllegalArgumentException("The Cardboard trigger listener must not be null.");
        } else if(this.cardboardEmulator != null) {
            return true;
        } else if(!this.daydreamUtils.isDaydreamPhone(this.getContext())) {
            return false;
        } else {
            this.cardboardEmulator = new CardboardEmulator(this.getContext(), var1);
            return true;
        }
    }

    private void addScanlineRacingView() {
        if(this.scanlineRacingView == null) {
            this.eglFactory = new EglFactory();
            this.eglFactory.setUsePriorityContext(true);
            this.eglFactory.setUseProtectedBuffers(this.isAsyncReprojectionUsingProtectedBuffers);
            this.eglFactory.setEGLContextClientVersion(2);
            this.scanlineRacingView = new GvrLayout.AsyncReprojectionSurfaceView(this.getContext());
            this.scanlineRacingView.setEGLContextClientVersion(2);
            this.scanlineRacingView.setEGLConfigChooser(new MutableEglConfigChooser());
            this.scanlineRacingView.setZOrderMediaOverlay(true);
            this.scanlineRacingView.setEGLContextFactory(this.eglFactory);
            this.scanlineRacingView.setEGLWindowSurfaceFactory(this.eglFactory);
            if(!this.stereoModeEnabled) {
                Log.w("GvrLayout", "Disabling stereo mode with async reprojection enabled may not work properly.");
                this.scanlineRacingView.setVisibility(8);
            }

            if(this.scanlineRacingRenderer == null) {
                this.scanlineRacingRenderer = new ScanlineRacingRenderer(this.gvrApi);
            }

            this.scanlineRacingRenderer.setSurfaceView(this.scanlineRacingView);
            this.scanlineRacingView.setRenderer(this.scanlineRacingRenderer);
            this.scanlineRacingView.setSwapMode(0);
            this.presentationLayout.addView(this.scanlineRacingView, 0);
        }
    }

    public int getAsyncReprojectionVideoSurfaceId() {
        if(!this.isAsyncReprojectionVideoEnabled) {
            Log.w("GvrLayout", "Async reprojection video is not enabled. Did you call enableAsyncReprojectionVideoSurface()?");
        }

        return this.videoSurfaceId;
    }

    public Surface getAsyncReprojectionVideoSurface() {
        if(!this.isAsyncReprojectionVideoEnabled) {
            Log.w("GvrLayout", "Async reprojection video is not enabled. Did you call enableAsyncReprojectionVideoSurface()?");
            return null;
        } else {
            if(this.scanlineRacingView == null) {
                Log.w("GvrLayout", "No async reprojection view has been set. Cannot get async reprojection managed Surfaces. Have you called setAsyncReprojectionEnabled()?");
            }

            return this.scanlineRacingRenderer.getExternalSurfaceManager().getSurface(this.videoSurfaceId);
        }
    }

    public GvrApi getGvrApi() {
        return this.gvrApi;
    }

    public void addPresentationListener(GvrLayout.PresentationListener var1) {
        if(this.presentationHelper != null) {
            this.presentationHelper.addListener(var1);
        }

    }

    protected VrCoreSdkClient createVrCoreSdkClient(Context var1, GvrApi var2, DaydreamUtilsWrapper var3, FadeOverlayView var4) {
        ComponentName var5 = ContextUtils.getComponentName(var1);
        Runnable var6 = new Runnable() {
            public void run() {
                GvrLayout.this.uiLayout.invokeCloseButtonListener();
            }
        };
        return new VrCoreSdkClient(var1, var2, var5, var3, var6, var4);
    }

    public void launchInVr(PendingIntent var1) {
        if(this.vrCoreSdkClient == null || !this.vrCoreSdkClient.launchInVr(var1)) {
            try {
                var1.send();
                return;
            } catch (Exception var3) {
                Log.e("GvrLayout", "Error launching PendingIntent.", var3);
            }
        }

    }

    public void setStereoModeEnabled(boolean var1) {
        if(this.stereoModeEnabled != var1) {
            this.stereoModeEnabled = var1;
            this.uiLayout.setEnabled(var1);
            if(this.vrCoreSdkClient != null) {
                this.vrCoreSdkClient.setEnabled(var1);
            }

            if(this.fadeOverlayView != null) {
                this.fadeOverlayView.setEnabled(var1);
            }

            if(this.daydreamTouchListener != null) {
                this.daydreamTouchListener.setEnabled(var1);
            }

            this.updateRenderingViewsVisibility(0);
        }
    }

    public boolean onTouchEvent(MotionEvent var1) {
        return this.presentationView != null && this.isPresenting() && this.presentationView.dispatchTouchEvent(var1)?true:super.onTouchEvent(var1);
    }

    public boolean isPresenting() {
        return this.presentationView != null && this.presentationHelper != null && this.presentationHelper.isPresenting();
    }

    static void setPresentationFactory(GvrLayout.PresentationFactory var0) {
        sOptionalPresentationFactory = var0;
    }

    VrCoreSdkClient getVrCoreSdkClient() {
        return this.vrCoreSdkClient;
    }

    FadeOverlayView getFadeOverlayView() {
        return this.fadeOverlayView;
    }

    DaydreamTouchListener createDaydreamTouchListener() {
        return new DaydreamTouchListener(this.getContext(), this.gvrApi);
    }

    private GvrLayout.PresentationHelper tryCreatePresentationHelper() {
        if(VERSION.SDK_INT <= 16) {
            return null;
        } else {
            String var1;
            if((var1 = DisplayUtils.getExternalDisplayName(this.getContext())) == null) {
                Log.e("GvrLayout", "HDMI display name could not be found, disabling external presentation support");
                return null;
            } else {
                return new GvrLayout.PresentationHelper(this.getContext(), this, this.presentationLayout, this.displaySynchronizer, var1);
            }
        }
    }

    private void updateRenderingViewsVisibility(int var1) {
        if(this.presentationView != null) {
            this.presentationView.setVisibility(this.stereoModeEnabled?var1:0);
        }

        if(this.scanlineRacingView != null) {
            this.scanlineRacingView.setVisibility(this.stereoModeEnabled?var1:8);
        }

    }

    private void updateFadeVisibility() {
        boolean var1 = this.getWindowVisibility() == 0;
        if(this.fadeOverlayView != null) {
            if(var1 && this.isResumed) {
                this.fadeOverlayView.onVisible();
                this.removeCallbacks(this.showRenderingViewsRunnable);
                this.postDelayed(this.showRenderingViewsRunnable, 50L);
                return;
            }

            if(!var1 && !this.isResumed) {
                this.fadeOverlayView.onInvisible();
                this.updateRenderingViewsVisibility(4);
                this.removeCallbacks(this.showRenderingViewsRunnable);
            }
        }

    }

    private void updateUiLayout() {
        boolean var1 = this.gvrApi.getViewerType() == 1;
        this.uiLayout.setDaydreamModeEnabled(var1);
    }

    private static class PresentationHelper implements DisplayListener {
        private final LayoutParams layout = new LayoutParams(-1, -1);
        private final Context context;
        private final DisplayManager displayManager;
        private final DisplaySynchronizer displaySynchronizer;
        private final FrameLayout originalParent;
        private final View view;
        private final List<GvrLayout.PresentationListener> listeners;
        private String externalDisplayName;
        private Presentation presentation;

        PresentationHelper(Context var1, FrameLayout var2, View var3, DisplaySynchronizer var4, String var5) {
            this.context = var1;
            this.originalParent = var2;
            this.view = var3;
            this.displaySynchronizer = var4;
            this.externalDisplayName = var5;
            this.displayManager = (DisplayManager)var1.getSystemService("display");
            this.listeners = new ArrayList();
        }

        public boolean isPresenting() {
            return this.presentation != null && this.presentation.isShowing();
        }

        public void onPause() {
            this.displayManager.unregisterDisplayListener(this);
        }

        public void onResume() {
            this.externalDisplayName = DisplayUtils.getExternalDisplayName(this.context);
            if(this.externalDisplayName == null) {
                this.setDisplay((Display)null);
            } else {
                this.displayManager.registerDisplayListener(this, (Handler)null);
                Display var1 = null;
                Display[] var2;
                int var3 = (var2 = this.displayManager.getDisplays()).length;

                for(int var4 = 0; var4 < var3; ++var4) {
                    Display var5 = var2[var4];
                    if(this.isValidExternalDisplay(var5)) {
                        var1 = var5;
                        break;
                    }
                }

                this.setDisplay(var1);
            }
        }

        public void shutdown() {
            this.displayManager.unregisterDisplayListener(this);
            if(this.presentation != null) {
                this.presentation.cancel();
                this.presentation = null;
                Iterator var1 = this.listeners.iterator();

                while(var1.hasNext()) {
                    ((GvrLayout.PresentationListener)var1.next()).onPresentationStopped();
                }
            }

        }

        public void onDetachedFromWindow() {
            this.displayManager.unregisterDisplayListener(this);
            this.setDisplay((Display)null);
        }

        public void addListener(GvrLayout.PresentationListener var1) {
            if(!this.listeners.contains(var1)) {
                this.listeners.add(var1);
                if(this.presentation != null) {
                    var1.onPresentationStarted(this.presentation.getDisplay());
                }

            }
        }

        public void onDisplayAdded(int var1) {
            Display var2 = this.displayManager.getDisplay(var1);
            if(this.isValidExternalDisplay(var2)) {
                this.setDisplay(var2);
            }

        }

        public void onDisplayRemoved(int var1) {
            if(this.presentation != null && this.presentation.getDisplay().getDisplayId() == var1) {
                this.setDisplay((Display)null);
            }

        }

        public void onDisplayChanged(int var1) {
        }

        private void setDisplay(Display var1) {
            Display var2 = this.presentation != null?this.presentation.getDisplay():null;
            if(this.hasCurrentPresentationExpired() || !DisplayUtils.isSameDisplay(var1, var2)) {
                Presentation var3 = this.presentation;
                if(this.presentation != null) {
                    this.presentation.dismiss();
                    this.presentation = null;
                }

                label61: {
                    detachViewFromParent(this.view);
                    if(var1 != null) {
                        this.presentation = GvrLayout.sOptionalPresentationFactory != null?GvrLayout.sOptionalPresentationFactory.create(this.context, var1):new Presentation(this.context, var1);
                        this.presentation.addContentView(this.view, this.layout);

                        try {
                            this.presentation.show();
                            break label61;
                        } catch (InvalidDisplayException var6) {
                            String var5 = String.valueOf(var6);
                            Log.e("GvrLayout", (new StringBuilder(57 + String.valueOf(var5).length())).append("Attaching Cardboard View to the external display failed: ").append(var5).toString());
                            this.presentation.cancel();
                            this.presentation = null;
                            detachViewFromParent(this.view);
                        }
                    }

                    this.originalParent.addView(this.view, 0);
                }

                this.displaySynchronizer.setDisplay(this.presentation != null?this.presentation.getDisplay():DisplayUtils.getDefaultDisplay(this.context));
                Iterator var4;
                if(var3 != null) {
                    var4 = this.listeners.iterator();

                    while(var4.hasNext()) {
                        ((GvrLayout.PresentationListener)var4.next()).onPresentationStopped();
                    }
                }

                if(this.presentation != null) {
                    var4 = this.listeners.iterator();

                    while(var4.hasNext()) {
                        ((GvrLayout.PresentationListener)var4.next()).onPresentationStarted(this.presentation.getDisplay());
                    }
                }

            }
        }

        private static void detachViewFromParent(View var0) {
            ViewGroup var1;
            if((var1 = (ViewGroup)var0.getParent()) != null) {
                var1.removeView(var0);
            }

        }

        private boolean isValidExternalDisplay(Display var1) {
            return var1 != null && var1.isValid() && var1.getName().equals(this.externalDisplayName);
        }

        private boolean hasCurrentPresentationExpired() {
            return this.presentation == null?false:!this.presentation.isShowing() || !this.presentation.getDisplay().isValid();
        }
    }

    private static class AsyncReprojectionSurfaceView extends GvrSurfaceView {
        private ScanlineRacingRenderer scanlineRacingRenderer;

        AsyncReprojectionSurfaceView(Context var1) {
            super(var1);
        }

        public void setRenderer(ScanlineRacingRenderer var1) {
            this.scanlineRacingRenderer = var1;
            super.setRenderer(var1);
        }

        public void surfaceDestroyed(SurfaceHolder var1) {
            if(!this.isDetachedFromWindow() && this.scanlineRacingRenderer != null) {
                final CountDownLatch var2 = new CountDownLatch(1);
                this.queueEvent(new Runnable() {
                    public void run() {
                        AsyncReprojectionSurfaceView.this.scanlineRacingRenderer.onDestroySurface();
                        var2.countDown();
                    }
                });

                try {
                    var2.await();
                } catch (InterruptedException var4) {
                    Log.e("GvrLayout", "Interrupted during surface destroyed: ", var4);
                }
            }

            super.surfaceDestroyed(var1);
        }
    }

    interface PresentationFactory {
        Presentation create(Context var1, Display var2);
    }

    public interface PresentationListener {
        void onPresentationStarted(Display var1);

        void onPresentationStopped();
    }

    public interface ExternalSurfaceManager {
        int createExternalSurface();

        int createExternalSurface(GvrLayout.ExternalSurfaceListener var1, Handler var2);

        void releaseExternalSurface(int var1);

        Surface getSurface(int var1);

        int getSurfaceCount();
    }

    public interface ExternalSurfaceListener {
        void onSurfaceAvailable(Surface var1);

        void onFrameAvailable();
    }
}
