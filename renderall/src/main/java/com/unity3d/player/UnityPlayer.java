//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.ContextWrapper;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.Configuration;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.hardware.Camera.Size;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.Process;
import android.util.AttributeSet;
import android.util.Log;
import android.view.InputEvent;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.SurfaceHolder.Callback;
import android.widget.FrameLayout;
import android.widget.ProgressBar;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Vector;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicLong;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserFactory;

public class UnityPlayer extends FrameLayout implements com.unity3d.player.a.ai {
    public static Activity currentActivity = null;
    private boolean c = false;
    private boolean d = true;
    private boolean e = false;
    private boolean f = false;
    private boolean g = false;
    private ClsL h = new ClsL();
    private final ConcurrentLinkedQueue i = new ConcurrentLinkedQueue();
    private BroadcastReceiver j = null;
    UnityPlayer.b a = new UnityPlayer.b();
    private ContextWrapper k;
    private SurfaceView l;
    public static boolean m;
    private boolean n;
    private Bundle o = new Bundle();
    private List p = new ArrayList();
    private m q;
    JDialog mDialog = null;
    private ProgressBar r = null;
    private Runnable s = new Runnable() {
        public final void run() {
            int var1;
            if((var1 = UnityPlayer.this.nativeActivityIndicatorStyle()) >= 0) {
                if(UnityPlayer.this.r == null) {
                    int[] var2 = new int[]{16842874, 16843401, 16842873, 16843400};
                    UnityPlayer.this.r = new ProgressBar(UnityPlayer.this.k, (AttributeSet)null, var2[var1]);
                    UnityPlayer.this.r.setIndeterminate(true);
                    UnityPlayer.this.r.setLayoutParams(new LayoutParams(-2, -2, 51));
                    UnityPlayer.this.addView(UnityPlayer.this.r);
                }

                UnityPlayer.this.r.setVisibility(View.VISIBLE);
                UnityPlayer.this.bringChildToFront(UnityPlayer.this.r);
            }

        }
    };
    private Runnable t = new Runnable() {
        public final void run() {
            if(UnityPlayer.this.r != null) {
                UnityPlayer.this.r.setVisibility(View.GONE);
                UnityPlayer.this.removeView(UnityPlayer.this.r);
                UnityPlayer.this.r = null;
            }

        }
    };
    private FrameLayout u = null;
    private SurfaceView v = null;
    private boolean w = false;
    private AtomicBoolean x = new AtomicBoolean(false);

    public UnityPlayer(ContextWrapper var1) {
        super(var1);
        if(var1 instanceof Activity) {
            currentActivity = (Activity)var1;
        }

        this.k = var1;
        this.b();
        if(ClsI.e) {
            if(currentActivity != null) {
                ClsI.h.a(currentActivity, new Runnable() {
                    public final void run() {
                        UnityPlayer.this.b(new Runnable() {
                            public final void run() {
                                UnityPlayer.this.h.d();
                                UnityPlayer.this.h();
                            }
                        });
                    }
                });
            } else {
                this.h.d();
            }
        }

        if(ClsI.a) {
            ClsI.f.a(this);
        }

        this.setFullscreen(true);
        a(this.k.getApplicationInfo());
        if(!ClsL.c()) {
            AlertDialog var2;
            (var2 = (new Builder(this.k)).setTitle("Failure to initialize!").setPositiveButton("OK", new DialogInterface.OnClickListener() {
                public final void onClick(DialogInterface var1, int var2) {
                    UnityPlayer.this.c();
                }
            }).setMessage("Your hardware does not support this application, sorry!").create()).setCancelable(false);
            var2.show();
        } else {
            this.initJni(var1);
            this.nativeFile(this.k.getPackageCodePath());
            this.m();
            this.l = this.a();
            this.addView(this.l);
            this.n = false;
            if(currentActivity != null && currentActivity.getIntent().getBooleanExtra("android.intent.extra.VR_LAUNCH", false)) {
                this.e = true;
                this.f = true;
                this.nativeSetHardwareVolumeOverride(true);
            }

            this.nativeInitWWW(WWW.class);
            this.nativeInitWebRequest(UnityWebRequest.class);
            if(ClsI.b) {
                ClsI.g.a(this, this.k);
            }

            this.n();
            this.a.start();
        }
    }

    private SurfaceView a() {
        SurfaceView var1;
        (var1 = new SurfaceView(this.k)).getHolder().setFormat(2);
        var1.getHolder().addCallback(new Callback() {
            public final void surfaceCreated(SurfaceHolder var1) {
                a( var1.getSurface());
            }

            public final void surfaceChanged(SurfaceHolder var1, int var2, int var3, int var4) {
                a(var1.getSurface());
            }

            public final void surfaceDestroyed(SurfaceHolder var1) {
                a((Surface)null);
            }
        });
        var1.setFocusable(true);
        var1.setFocusableInTouchMode(true);
        return var1;
    }

    private void a( Surface var2) {
        if(!this.c) {
            this.b(0, var2);
        }
    }

    private boolean b(int var1, Surface var2) {
        if(!ClsL.c()) {
            return false;
        } else {
            this.nativeRecreateGfxState(var1, var2);
            return true;
        }
    }

    public boolean displayChanged(int var1, Surface var2) {
        if(var1 == 0) {
            this.c = var2 != null;
            this.b(new Runnable() {
                public final void run() {
                    if(UnityPlayer.this.c) {
                        UnityPlayer.this.removeView(UnityPlayer.this.l);
                    } else {
                        UnityPlayer.this.addView(UnityPlayer.this.l);
                    }
                }
            });
        }

        return this.b(var1, var2);
    }

    protected boolean installPresentationDisplay(int var1) {
        return ClsI.b?ClsI.g.a(this, this.k, var1):false;
    }

    private void b() {
        try {
            File var1;
            Object var6;
            if((var1 = new File(this.k.getPackageCodePath(), "assets/bin/Data/settings.xml")).exists()) {
                var6 = new FileInputStream(var1);
            } else {
                var6 = this.k.getAssets().open("bin/Data/settings.xml");
            }

            XmlPullParserFactory var2;
            (var2 = XmlPullParserFactory.newInstance()).setNamespaceAware(true);
            XmlPullParser var7;
            (var7 = var2.newPullParser()).setInput((InputStream)var6, (String)null);
            String var8 = null;
            String var3 = null;

            for(int var4 = var7.getEventType(); var4 != 1; var4 = var7.next()) {
                if(var4 == 2) {
                    var8 = var7.getName();

                    for(var4 = 0; var4 < var7.getAttributeCount(); ++var4) {
                        if(var7.getAttributeName(var4).equalsIgnoreCase("name")) {
                            var3 = var7.getAttributeValue(var4);
                        }
                    }
                } else if(var4 == 3) {
                    var8 = null;
                } else if(var4 == 4 && var3 != null) {
                    if(var8.equalsIgnoreCase("integer")) {
                        this.o.putInt(var3, Integer.parseInt(var7.getText()));
                    } else if(var8.equalsIgnoreCase("string")) {
                        this.o.putString(var3, var7.getText());
                    } else if(var8.equalsIgnoreCase("bool")) {
                        this.o.putBoolean(var3, Boolean.parseBoolean(var7.getText()));
                    } else if(var8.equalsIgnoreCase("float")) {
                        this.o.putFloat(var3, Float.parseFloat(var7.getText()));
                    }

                    var3 = null;
                }
            }

        } catch (Exception var5) {
            ClsG.Log(6, "Unable to locate player settings. " + var5.getLocalizedMessage());
            this.c();
        }
    }

    public Bundle getSettings() {
        return this.o;
    }

    public static void UnitySendMessage(String var0, String var1, String var2) {
        if(!ClsL.c()) {
            ClsG.Log(5, "Native libraries not loaded - dropping message for " + var0 + "." + var1);
        } else {
            nativeUnitySendMessage(var0, var1, var2);
        }
    }

    private static native void nativeUnitySendMessage(String var0, String var1, String var2);

    private void c() {
        if(this.u != null) {
            this.nativeShutdownGoogleVR();
        }

        if(this.k instanceof Activity && !((Activity)this.k).isFinishing()) {
            ((Activity)this.k).finish();
        }

    }

    static void a(Runnable var0) {
        (new Thread(var0)).start();
    }

    final void b(Runnable var1) {
        if(this.k instanceof Activity) {
            ((Activity)this.k).runOnUiThread(var1);
        } else {
            ClsG.Log(5, "Not running Unity from an Activity; ignored...");
        }
    }

    public void init(int var1, boolean var2) {
    }

    public View getView() {
        return this;
    }

    protected void daydreamQuitToOSLauncher() {
        if(this.f && !this.g && currentActivity != null) {
            this.g = true;
            Intent var1;
            (var1 = new Intent("android.intent.action.MAIN")).addCategory("android.intent.category.HOME");
            var1.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            currentActivity.startActivity(var1);
        }

    }

    protected void kill() {
        this.daydreamQuitToOSLauncher();
        Process.killProcess(Process.myPid());
    }

    public void quit() {
        this.daydreamQuitToOSLauncher();
        this.n = true;
        if(!this.h.e()) {
            this.pause();
        }

        this.a.a();

        try {
            this.a.join(4000L);
        } catch (InterruptedException var1) {
            this.a.interrupt();
        }

        if(this.j != null) {
            this.k.unregisterReceiver(this.j);
        }

        this.j = null;
        if(ClsL.c()) {
            this.removeAllViews();
        }

        if(ClsI.b) {
            ClsI.g.a(this.k);
        }

        this.kill();
        i();
    }

    private void d() {
        Iterator var1 = this.p.iterator();

        while(var1.hasNext()) {
            ((com.unity3d.player.a)var1.next()).c();
        }

    }

    private void e() {
        Iterator var1 = this.p.iterator();

        while(var1.hasNext()) {
            com.unity3d.player.a var2 = (com.unity3d.player.a)var1.next();

            try {
                var2.a(this);
            } catch (Exception var4) {
                String var3 = "Unable to initialize camera: " + var4.getMessage();
                ClsG.Log(6, var3);
                var2.c();
            }
        }

    }

    public void pause() {
        if(this.q != null) {
            this.q.onPause();
        } else {
            this.j();
            this.f();
        }
    }

    private void f() {
        this.reportSoftInputStr((String)null, 1, true);
        if(this.h.g()) {
            if(ClsL.c()) {
                final Semaphore var1 = new Semaphore(0);
                if(this.isFinishing()) {
                    this.c(new Runnable() {
                        public final void run() {
                            UnityPlayer.this.g();
                            var1.release();
                        }
                    });
                } else {
                    this.c(new Runnable() {
                        public final void run() {
                            if(UnityPlayer.this.nativePause()) {
                                UnityPlayer.i();
                                UnityPlayer.this.g();
                                var1.release(2);
                            } else {
                                var1.release();
                            }
                        }
                    });
                }

                try {
                    if(!var1.tryAcquire(4L, TimeUnit.SECONDS)) {
                        ClsG.Log(5, "Timeout while trying to pause the Unity Engine.");
                    }
                } catch (InterruptedException var2) {
                    ClsG.Log(5, "UI thread got interrupted while trying to pause the Unity Engine.");
                }

                if(var1.drainPermits() > 0) {
                    this.quit();
                }
            }

            this.h.c(false);
            this.h.b(true);
            this.d();
            this.a.c();
        }
    }

    public void resume() {
        if(ClsI.a) {
            ClsI.f.b(this);
        }

        this.h.b(false);
        this.h();
        this.k();
    }

    private void g() {
        this.nativeDone();
    }

    private void h() {
        if(this.h.f()) {
            if(this.q != null) {
                this.q.onResume();
            } else {
                this.h.c(true);
                this.e();
                if(ClsL.c()) {
                    this.m();
                }

                this.c(new Runnable() {
                    public final void run() {
                        UnityPlayer.this.nativeResume();
                    }
                });
                this.a.b();
            }
        }
    }

    public void configurationChanged(Configuration var1) {
        if(this.l instanceof SurfaceView) {
            this.l.getHolder().setSizeFromLayout();
        }

        if(this.q != null) {
            this.q.updateVideoLayout();
        }

    }

    public void windowFocusChanged(final boolean var1) {
        this.h.a(var1);
        if(var1 && this.mDialog != null) {
            this.reportSoftInputStr((String)null, 1, false);
        }

        if(ClsI.a && var1) {
            ClsI.f.b(this);
        }

        this.c(new Runnable() {
            public final void run() {
                UnityPlayer.this.nativeFocusChanged(var1);
            }
        });
        this.a.a(var1);
        this.h();
    }

    public static boolean loadLibraryStatic(String var0) {
        try {
            System.loadLibrary(var0);
            return true;
        } catch (UnsatisfiedLinkError var1) {
            ClsG.Log(6, "Unable to find " + var0);
            return false;
        } catch (Exception var2) {
            ClsG.Log(6, "Unknown error " + var2);
            return false;
        }
    }

    protected boolean loadLibrary(String var1) {
        return loadLibraryStatic(var1);
    }

    protected void startActivityIndicator() {
        this.b(this.s);
    }

    protected void stopActivityIndicator() {
        this.b(this.t);
    }

    private final native void nativeFile(String var1);

    private final native void initJni(Context var1);

    private final native boolean nativeRender();

    private final native void nativeSetInputString(String var1);

    private final native void nativeSetInputCanceled(boolean var1);

    private final native boolean nativePause();

    private final native void nativeResume();

    private final native void nativeFocusChanged(boolean var1);

    private final native void nativeRecreateGfxState(int var1, Surface var2);

    private final native void nativeDone();

    private final native void nativeSoftInputClosed();

    private final native void nativeInitWWW(Class var1);

    private final native void nativeInitWebRequest(Class var1);

    private final native void nativeVideoFrameCallback(int var1, byte[] var2, int var3, int var4);

    private final native int nativeActivityIndicatorStyle();

    private final native boolean nativeInjectEvent(InputEvent var1);

    private final native void nativeSetHardwareVolumeOverride(boolean var1);

    private final native void nativeShutdownGoogleVR();

    private static void a(ApplicationInfo var0) {
        if(m && NativeLoader.load(var0.nativeLibraryDir)) {
            ClsL.a();
        }

    }

    private static void i() {
        if(ClsL.c()) {
            if(!NativeLoader.unload()) {
                throw new UnsatisfiedLinkError("Unable to unload libraries from libmain.so");
            } else {
                ClsL.b();
            }
        }
    }

    protected void setFullscreen(final boolean var1) {
        this.d = var1;
        if(ClsI.a) {
            this.b(new Runnable() {
                public final void run() {
                    ClsI.f.a(UnityPlayer.this, var1);
                }
            });
        }

    }

    protected void showSoftInput(final String var1, final int var2, final boolean var3, final boolean var4, final boolean var5, final boolean var6, final String var7) {
        this.b(new Runnable() {
            public final void run() {
                UnityPlayer.this.mDialog = new JDialog(UnityPlayer.this.k, UnityPlayer.this, var1, var2, var3, var4, var5, var7);
                UnityPlayer.this.mDialog.show();
            }
        });
    }

    protected void hideSoftInput() {
        final Runnable var1 = new Runnable() {
            public final void run() {
                if(UnityPlayer.this.mDialog != null) {
                    UnityPlayer.this.mDialog.dismiss();
                    UnityPlayer.this.mDialog = null;
                }

            }
        };
        if(ClsI.d) {
            this.a(new UnityPlayer.c() {
                public final void a() {
                    UnityPlayer.this.b(var1);
                }
            });
        } else {
            this.b(var1);
        }
    }

    protected void setSoftInputStr(final String var1) {
        this.b(new Runnable() {
            public final void run() {
                if(UnityPlayer.this.mDialog != null && var1 != null) {
                    UnityPlayer.this.mDialog.a(var1);
                }

            }
        });
    }

    protected void reportSoftInputStr(final String var1, final int var2, final boolean var3) {
        if(var2 == 1) {
            this.hideSoftInput();
        }

        this.a(new UnityPlayer.c() {
            public final void a() {
                if(var3) {
                    n();
                } else if(var1 != null) {
                    UnityPlayer.this.nativeSetInputString(var1);
                }

                if(var2 == 1) {
                    UnityPlayer.this.nativeSoftInputClosed();
                }

            }
        });
    }

    protected int[] initCamera(int var1, int var2, int var3, int var4) {
        com.unity3d.player.a var6 = new com.unity3d.player.a(var1, var2, var3, var4);

        try {
            var6.a(this);
            this.p.add(var6);
            Size var8 = var6.b();
            return new int[]{var8.width, var8.height};
        } catch (Exception var5) {
            String var7 = "Unable to initialize camera: " + var5.getMessage();
            ClsG.Log(6, var7);
            var6.c();
            return null;
        }
    }

    protected void closeCamera(int var1) {
        Iterator var2 = this.p.iterator();

        com.unity3d.player.a var3;
        do {
            if(!var2.hasNext()) {
                return;
            }
        } while((var3 = (com.unity3d.player.a)var2.next()).a() != var1);

        var3.c();
        this.p.remove(var3);
    }

    protected int getNumCameras() {
        return !this.l()?0:Camera.getNumberOfCameras();
    }

    public void onCameraFrame(final com.unity3d.player.a var1, final byte[] var2) {
        final int var3 = var1.a();
        final Size var4 = var1.b();
        this.a(new UnityPlayer.c() {
            public final void a() {
                UnityPlayer.this.nativeVideoFrameCallback(var3, var2, var4.width, var4.height);
                var1.a(var2);
            }
        });
    }

    protected boolean isCameraFrontFacing(int var1) {
        CameraInfo var2 = new CameraInfo();
        Camera.getCameraInfo(var1, var2);
        return var2.facing == 1;
    }

    protected int getCameraOrientation(int var1) {
        CameraInfo var2 = new CameraInfo();
        Camera.getCameraInfo(var1, var2);
        return var2.orientation;
    }

    protected long loadGoogleVR(final boolean var1, final boolean var2, final boolean var3) {
        final Semaphore var4 = new Semaphore(0);
        final AtomicLong var5 = new AtomicLong(0L);
        final AtomicBoolean var6 = new AtomicBoolean(false);
        final AtomicBoolean var7 = new AtomicBoolean(true);
        this.b(new Runnable() {
            public final void run() {
                try {
                    if(!UnityPlayer.this.x.get()) {
                        boolean var1x = UnityPlayer.this.loadLibrary("gvr");
                        UnityPlayer.this.x.set(var1x);
                    }

                    if(UnityPlayer.this.x.get() && UnityPlayer.this.u == null) {
                        ClassLoader var6x = UnityPlayer.class.getClassLoader();
                        UnityPlayer.this.v = UnityPlayer.this.a();
                        Class var2x;
                        Constructor var3x = (var2x = var6x.loadClass("com.google.vr.ndk.base.GvrLayout")).getConstructor(new Class[]{Context.class});
                        UnityPlayer.this.u = (FrameLayout)var3x.newInstance(new Object[]{UnityPlayer.this.k});
                        var2x.getMethod("setPresentationView", new Class[]{View.class}).invoke(UnityPlayer.this.u, new Object[]{UnityPlayer.this.v});
                        if(var1) {
                            ClsG.Log(4, "Enabling Asynchronous Projection Mode.");
                            Boolean var4x = (Boolean)var2x.getMethod("setAsyncReprojectionEnabled", new Class[]{Boolean.TYPE}).invoke(UnityPlayer.this.u, new Object[]{Boolean.valueOf(true)});
                            ClsG.Log(4, "Asynchronous Projection enabled: " + var4x);
                            var7.set(var4x.booleanValue());
                            if(var7.get() && var2) {
                                ClsG.Log(4, "Enabling sustained performance");
                                var6x.loadClass("com.google.vr.ndk.base.AndroidCompat").getMethod("setSustainedPerformanceMode", new Class[]{Activity.class, Boolean.TYPE}).invoke((Object)null, new Object[]{UnityPlayer.this.k, Boolean.valueOf(true)});
                                var6.set(true);
                            }
                        }

                        if(var7.get()) {
                            Class var9 = var6x.loadClass("com.google.vr.ndk.base.GvrUiLayout");
                            Object var10 = var2x.getMethod("getUiLayout", new Class[0]).invoke(UnityPlayer.this.u, new Object[0]);
                            var9.getMethod("setCloseButtonListener", new Class[]{Runnable.class}).invoke(var10, new Object[]{new Runnable() {
                                public final void run() {
                                    UnityPlayer.this.injectEvent(new KeyEvent(0, 4));
                                    UnityPlayer.this.injectEvent(new KeyEvent(1, 4));
                                }
                            }});
                            if(!UnityPlayer.this.f) {
                                var9.getMethod("setTransitionViewEnabled", new Class[]{Boolean.TYPE}).invoke(var10, new Object[]{Boolean.valueOf(var3)});
                            }

                            Object var8 = var2x.getMethod("getGvrApi", new Class[0]).invoke(UnityPlayer.this.u, new Object[0]);
                            Long var7x = (Long)var6x.loadClass("com.google.vr.ndk.base.GvrApi").getMethod("getNativeGvrContext", new Class[0]).invoke(var8, new Object[0]);
                            var5.set(var7x.longValue());
                            if(UnityPlayer.this.k instanceof Activity) {
                                ((Activity)UnityPlayer.this.k).getWindow().addFlags(128);
                            }
                        }
                    }
                } catch (Exception var5x) {
                    var5x.printStackTrace();
                }

                var4.release();
            }
        });

        try {
            if(!var4.tryAcquire(4L, TimeUnit.SECONDS)) {
                ClsG.Log(5, "Timeout waiting for GoogleVR to initialize");
            }
        } catch (InterruptedException var8) {
            ;
        }

        if(var7.get()) {
            ClsG.Log(4, "GoogleVR Initialization Succeeded.");
            this.w = var6.get();
        } else {
            ClsG.Log(5, "GoogleVR Initialization Failed.");
            this.w = false;
            this.u = null;
            this.v = null;
        }

        return var5.longValue();
    }

    protected boolean setGoogleVREnabled(final boolean var1) {
        final Semaphore var2 = new Semaphore(0);
        this.b(new Runnable() {
            public final void run() {
                try {
                    ClassLoader var1x;
                    Class var2x = (var1x = UnityPlayer.class.getClassLoader()).loadClass("com.google.vr.ndk.base.GvrLayout");
                    Method var7 = var1x.loadClass("com.google.vr.ndk.base.AndroidCompat").getMethod("setVrModeEnabled", new Class[]{Activity.class, Boolean.TYPE});
                    if(var1) {
                        if(UnityPlayer.this.u.getParent() == null) {
                            UnityPlayer.this.addView(UnityPlayer.this.u);
                            UnityPlayer.this.removeView(UnityPlayer.this.l);
                            var2x.getMethod("onResume", new Class[0]).invoke(UnityPlayer.this.u, new Object[0]);
                            var7.invoke((Object)null, new Object[]{UnityPlayer.this.k, Boolean.valueOf(true)});
                            UnityPlayer.this.e = true;
                            UnityPlayer.this.nativeSetHardwareVolumeOverride(true);
                            return;
                        }
                    } else {
                        UnityPlayer.this.e = false;
                        UnityPlayer.this.nativeSetHardwareVolumeOverride(false);
                        if(UnityPlayer.this.u != null && UnityPlayer.this.u.getParent() != null) {
                            var7.invoke((Object)null, new Object[]{UnityPlayer.this.k, Boolean.valueOf(false)});
                            if(UnityPlayer.this.l.getParent() == null) {
                                UnityPlayer.this.addView(UnityPlayer.this.l);
                                UnityPlayer.this.removeView(UnityPlayer.this.u);
                            }

                            return;
                        }
                    }

                    return;
                } catch (Exception var5) {
                    var5.printStackTrace();
                    UnityPlayer.this.e = false;
                } finally {
                    var2.release();
                }

            }
        });

        try {
            if(!var2.tryAcquire(4L, TimeUnit.SECONDS)) {
                ClsG.Log(5, "Timeout waiting for video");
            }
        } catch (InterruptedException var3) {
            ;
        }

        return this.w && var1;
    }

    protected void unloadGoogleVR() {
        final Semaphore var1 = new Semaphore(0);
        this.b(new Runnable() {
            public final void run() {
                try {
                    if(UnityPlayer.this.u != null) {
                        Class var1x = UnityPlayer.class.getClassLoader().loadClass("com.google.vr.ndk.base.GvrLayout");
                        if(UnityPlayer.this.l.getParent() == null) {
                            UnityPlayer.this.removeView(UnityPlayer.this.u);
                        }

                        var1x.getMethod("shutdown", new Class[0]).invoke(UnityPlayer.this.u, new Object[0]);
                        UnityPlayer.this.u = null;
                        if(UnityPlayer.this.l.getParent() == null) {
                            UnityPlayer.this.addView(UnityPlayer.this.l);
                            return;
                        }
                    }

                    return;
                } catch (Exception var4) {
                    var4.printStackTrace();
                } finally {
                    var1.release();
                }

            }
        });

        try {
            if(!var1.tryAcquire(4L, TimeUnit.SECONDS)) {
                ClsG.Log(5, "Timeout waiting for GoogleVR to unload");
            }

        } catch (InterruptedException var2) {
            ;
        }
    }

    private void j() {
        this.a("onPause");
    }

    private void k() {
        this.a("onResume");
    }

    private void a(String var1) {
        if(this.u != null) {
            try {
                UnityPlayer.class.getClassLoader().loadClass("com.google.vr.ndk.base.GvrLayout").getMethod(var1, new Class[0]).invoke(this.u, new Object[0]);
                return;
            } catch (Exception var2) {
                ClsG.Log(6, "Unable to pause GvrLayout.");
            }
        }

    }

    protected boolean showVideoPlayer(final String var1, final int var2, final int var3, final int var4, final boolean var5, final int var6, final int var7) {
        final Semaphore var8 = new Semaphore(0);
        final AtomicInteger var9 = new AtomicInteger(-1);
        this.b(new Runnable() {
            public final void run() {
                if(UnityPlayer.this.q != null) {
                    ClsG.Log(5, "Video already playing");
                    var9.set(2);
                    var8.release();
                } else {
                    UnityPlayer.this.q = new m(UnityPlayer.this, UnityPlayer.this.k, var1, var2, var3, var4, var5, (long)var6, (long)var7, new com.unity3d.player.m.a() {
                        public final void a(int var1x) {
                            var9.set(var1x);
                            if(var1x != 0) {
                                var8.release();
                            }

                        }
                    });
                    UnityPlayer.this.addView(UnityPlayer.this.q);
                }
            }
        });
        boolean var11 = false;

        try {
            if(!var8.tryAcquire(4L, TimeUnit.SECONDS)) {
                ClsG.Log(5, "Timeout waiting for video");
            } else {
                var11 = var9.get() != 2;
            }
        } catch (InterruptedException var10) {
            ;
        }

        if(var11) {
            this.b(new Runnable() {
                public final void run() {
                    UnityPlayer.this.f();
                    UnityPlayer.this.q.requestFocus();
                    UnityPlayer.this.removeView(UnityPlayer.this.l);
                }
            });
        }

        return var11;
    }

    protected void hideVideoPlayer() {
        this.b(new Runnable() {
            public final void run() {
                if(UnityPlayer.this.q != null) {
                    if(UnityPlayer.this.l.getParent() == null) {
                        UnityPlayer.this.addView(UnityPlayer.this.l);
                    }

                    UnityPlayer.this.removeView(UnityPlayer.this.q);
                    UnityPlayer.this.q = null;
                    UnityPlayer.this.resume();
                }
            }
        });
    }

    private boolean l() {
        return !ClsI.a?false:this.k.getPackageManager().hasSystemFeature("android.hardware.camera") ||
                this.k.getPackageManager().hasSystemFeature("android.hardware.camera.front");
    }

    protected int getSplashMode() {
        return this.o.getInt("splash_mode");
    }

    protected void executeGLThreadJobs() {
        Runnable var1;
        while((var1 = (Runnable)this.i.poll()) != null) {
            var1.run();
        }

    }

    protected void disableLogger() {
        ClsG.a = true;
    }

    private void c(Runnable var1) {
        if(ClsL.c()) {
            if(Thread.currentThread() == this.a) {
                var1.run();
            } else {
                this.i.add(var1);
            }
        }
    }

    private void a(UnityPlayer.c var1) {
        if(!this.isFinishing()) {
            this.c((Runnable)var1);
        }
    }

    protected boolean isFinishing() {
        return this.n || (this.n = this.k instanceof Activity && ((Activity)this.k).isFinishing());
    }

    private void m() {
        if(this.o.getBoolean("useObb")) {
            String[] var1;
            int var2 = (var1 = a((Context)this.k)).length;

            for(int var3 = 0; var3 < var2; ++var3) {
                String var4;
                String var5 = b(var4 = var1[var3]);
                if(this.o.getBoolean(var5)) {
                    this.nativeFile(var4);
                }

                this.o.remove(var5);
            }

        }
    }

    private static String[] a(Context var0) {
        String var1 = var0.getPackageName();
        Vector var2 = new Vector();

        int var6;
        try {
            var6 = var0.getPackageManager().getPackageInfo(var1, 0).versionCode;
        } catch (NameNotFoundException var5) {
            return new String[0];
        }

        if(Environment.getExternalStorageState().equals("mounted")) {
            File var3 = Environment.getExternalStorageDirectory();
            if((var3 = new File(var3.toString() + "/Android/obb/" + var1)).exists()) {
                String var4;
                if(var6 > 0) {
                    var4 = var3 + File.separator + "main." + var6 + "." + var1 + ".obb";
                    if((new File(var4)).isFile()) {
                        var2.add(var4);
                    }
                }

                if(var6 > 0) {
                    var4 = var3 + File.separator + "patch." + var6 + "." + var1 + ".obb";
                    if((new File(var4)).isFile()) {
                        var2.add(var4);
                    }
                }
            }
        }

        String[] var7 = new String[var2.size()];
        var2.toArray(var7);
        return var7;
    }

    private static String b(String var0) {
        byte[] var2 = null;

        int var11;
        try {
            MessageDigest var1 = MessageDigest.getInstance("MD5");
            FileInputStream var3 = new FileInputStream(var0);
            long var4 = (new File(var0)).length();
            var3.skip(var4 - Math.min(var4, 65558L));
            byte[] var9 = new byte[1024];

            for(var11 = 0; var11 != -1; var11 = var3.read(var9)) {
                var1.update(var9, 0, var11);
            }

            var2 = var1.digest();
        } catch (FileNotFoundException var6) {
            ;
        } catch (IOException var7) {
            ;
        } catch (NoSuchAlgorithmException var8) {
            ;
        }

        if(var2 == null) {
            return null;
        } else {
            StringBuffer var10 = new StringBuffer();

            for(var11 = 0; var11 < var2.length; ++var11) {
                var10.append(Integer.toString((var2[var11] & 255) + 256, 16).substring(1));
            }

            return var10.toString();
        }
    }

    private void n() {
//        if(this.k instanceof Activity) {
//            ((Activity)this.k).getWindow().setFlags(1024, 1024);
//        }

    }

    public boolean injectEvent(InputEvent var1) {
        return this.nativeInjectEvent(var1);
    }

    public boolean onKeyUp(int var1, KeyEvent var2) {
        ClsG.Log(4, "Handling onKeyUp");
        return this.injectEvent(var2);
    }

    public boolean onKeyDown(int var1, KeyEvent var2) {
        ClsG.Log(4, "Handling onKeyDown");
        return this.injectEvent(var2);
    }

    public boolean onKeyMultiple(int var1, int var2, KeyEvent var3) {
        ClsG.Log(4, "Handling onKeyMultiple");
        return this.injectEvent(var3);
    }

    public boolean onKeyLongPress(int var1, KeyEvent var2) {
        ClsG.Log(4, "Handling onKeyLongPress");
        return this.injectEvent(var2);
    }

    public boolean onTouchEvent(MotionEvent var1) {
        ClsG.Log(4, "Handling onTouchEvent");
        return this.injectEvent(var1);
    }

    public boolean onGenericMotionEvent(MotionEvent var1) {
        ClsG.Log(4, "Handling onGenericMotionEvent");
        return this.injectEvent(var1);
    }

    public void nativeEnsureAppLaunchesWithGVR()
    {
        Log.e("UnityPlayer", "nativeEnsureAppLaunchesWithGVR");
    }

    static {
        (new k()).a();
        m = false;
        m = loadLibraryStatic("main");
    }

    private abstract class c implements Runnable {
        private c() {
        }

        public final void run() {
            if(!UnityPlayer.this.isFinishing()) {
                this.a();
            }

        }

        public abstract void a();
    }

    private class b extends Thread {
        Handler a;
        boolean b;

        private b() {
            this.b = false;
        }

        public final void run() {
            this.setName("UnityMain");
            Looper.prepare();
            this.a = new Handler(new android.os.Handler.Callback() {
                public final boolean handleMessage(Message var1) {
                    if(var1.what != 2269) {
                        return false;
                    } else {
                        UnityPlayer.Ea var2;
                        if((var2 = (UnityPlayer.Ea)var1.obj) == UnityPlayer.Ea.c) {
                            Looper.myLooper().quit();
                        } else if(var2 == UnityPlayer.Ea.b) {
                            b.this.b = true;
                        } else if(var2 == UnityPlayer.Ea.a) {
                            b.this.b = false;
                            UnityPlayer.this.executeGLThreadJobs();
                        } else if(var2 == UnityPlayer.Ea.e) {
                            if(!b.this.b) {
                                UnityPlayer.this.executeGLThreadJobs();
                            }
                        } else if(var2 == UnityPlayer.Ea.f) {
                            UnityPlayer.this.executeGLThreadJobs();
                            if(!UnityPlayer.this.isFinishing() && !UnityPlayer.this.nativeRender()) {
                                UnityPlayer.this.c();
                            }
                        }

                        if(b.this.b) {
                            Message.obtain(b.this.a, 2269, UnityPlayer.Ea.f).sendToTarget();
                        }

                        return true;
                    }
                }
            });
            Looper.loop();
        }

        public final void a() {
            this.a(UnityPlayer.Ea.c);
        }

        public final void b() {
            this.a(UnityPlayer.Ea.b);
        }

        public final void c() {
            this.a(UnityPlayer.Ea.a);
        }

        public final void a(boolean var1) {
            this.a(var1?UnityPlayer.Ea.d:UnityPlayer.Ea.e);
        }

        private void a(UnityPlayer.Ea var1) {
            Message.obtain(this.a, 2269, var1).sendToTarget();
        }
    }

    static enum Ea {
        a,
        b,
        c,
        d,
        e,
        f;

        private Ea() {
        }
    }
}
