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
import android.content.DialogInterface.OnClickListener;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.res.Configuration;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.Process;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.util.AttributeSet;
import android.view.InputEvent;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewParent;
import android.view.SurfaceHolder.Callback;
import android.widget.FrameLayout;
import android.widget.ProgressBar;
import android.widget.FrameLayout.LayoutParams;
import com.unity3d.player.NativeLoader;
import com.unity3d.player.UnityWebRequest;
import com.unity3d.player.WWW;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.Vector;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserFactory;

public class UnityPlayer extends FrameLayout implements com.unity3d.player.Dcls {
    public static Activity currentActivity = null;
    private boolean c = false;
    private boolean d = true;
    private Kcls e = new Kcls();
    private final ConcurrentLinkedQueue f = new ConcurrentLinkedQueue();
    private BroadcastReceiver g = null;
    UnityPlayer.c a = new UnityPlayer.c();
    private boolean h = false;
    private UnityPlayer.a i = new UnityPlayer.a();
    private TelephonyManager j;
    private Icls k;
    private ContextWrapper l;
    private SurfaceView m;
    private static boolean n;
    private boolean o;
    private Bundle p = new Bundle();
    private Mcls q;
    private boolean r = false;
    Hcls b = null;
    private ProgressBar s = null;
    private Runnable t = new Runnable() {
        public final void run() {
            int var1;
            if((var1 = UnityPlayer.this.nativeActivityIndicatorStyle()) >= 0) {
                if(UnityPlayer.this.s == null) {
                    int[] var2 = new int[]{16842874, 16843401, 16842873, 16843400};
                    UnityPlayer.this.s = new ProgressBar(UnityPlayer.this.l, (AttributeSet)null, var2[var1]);
                    UnityPlayer.this.s.setIndeterminate(true);
                    UnityPlayer.this.s.setLayoutParams(new LayoutParams(-2, -2, 51));
                    UnityPlayer.this.addView(UnityPlayer.this.s);
                }

                UnityPlayer.this.s.setVisibility(0);
                UnityPlayer.this.bringChildToFront(UnityPlayer.this.s);
            }

        }
    };
    private Runnable u = new Runnable() {
        public final void run() {
            if(UnityPlayer.this.s != null) {
                UnityPlayer.this.s.setVisibility(8);
                UnityPlayer.this.removeView(UnityPlayer.this.s);
                UnityPlayer.this.s = null;
            }

        }
    };
    private com.unity3d.player.Bcls v = new com.unity3d.player.Bcls(this);

    public UnityPlayer(ContextWrapper var1) {
        super(var1);
        if(var1 instanceof Activity) {
            currentActivity = (Activity)var1;
        }

        a(currentActivity);
        this.l = var1;
        this.b();
        if(currentActivity != null && this.p.getBoolean("showSplash")) {
            this.k = new Icls(this, com.unity3d.player.Icls.Enuma.a()[this.getSplashMode()]);
        }

        if(Gcls.b) {
            if(currentActivity != null) {
                Gcls.c.a(currentActivity, new Runnable() {
                    public final void run() {
                        UnityPlayer.this.a(new Runnable() {
                            public final void run() {
                                UnityPlayer.this.e.d();
                                UnityPlayer.this.f();
                            }
                        });
                    }
                });
            } else {
                this.e.d();
            }
        }

        a(this.l.getApplicationInfo());
        if(!Kcls.c()) {
            AlertDialog var2;
            (var2 = (new Builder(this.l)).setTitle("Failure to initialize!").setPositiveButton("OK", new DialogInterface.OnClickListener() {
                public final void onClick(DialogInterface var1, int var2) {
                    UnityPlayer.this.c();
                }
            }).setMessage("Your hardware does not support this application, sorry!").create()).setCancelable(false);
            var2.show();
        } else {
            this.initJni(var1);
            this.nativeFile(this.l.getPackageCodePath());
            this.h();
            this.m = this.a();
            this.m.setZOrderOnTop(true);
            this.addView(this.m);
            this.o = false;
            if(currentActivity != null) {
                this.v.a(currentActivity.getIntent());
            }

            this.nativeInitWWW(WWW.class);
            this.nativeInitWebRequest(UnityWebRequest.class);
            this.i();
            this.j = (TelephonyManager)this.l.getSystemService("phone");
            this.a.start();
        }
    }

    private static void a(Activity var0) {
        View var1;
        if(var0 != null && var0.getIntent().getBooleanExtra("android.intent.extra.VR_LAUNCH", false) && var0.getWindow() != null && (var1 = var0.getWindow().getDecorView()) != null) {
            var1.setSystemUiVisibility(7);
        }

    }

    private SurfaceView a() {
        SurfaceView var1;
        (var1 = new SurfaceView(this.l)).getHolder().setFormat(2);
        var1.getHolder().addCallback(new Callback() {
            public final void surfaceCreated(SurfaceHolder var1) {
                a( var1.getSurface());
            }

            public final void surfaceChanged(SurfaceHolder var1, int var2, int var3, int var4) {
                a(var1.getSurface());
            }

            public final void surfaceDestroyed(SurfaceHolder var1) {
                a( (Surface)null);
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
        if(!Kcls.c()) {
            return false;
        } else {
            this.nativeRecreateGfxState(var1, var2);
            return true;
        }
    }

    public boolean displayChanged(int var1, Surface var2) {
        if(var1 == 0) {
            this.c = var2 != null;
            this.a(new Runnable() {
                public final void run() {
                    if(UnityPlayer.this.c) {
                        UnityPlayer.this.removeView(UnityPlayer.this.m);
                    } else {
                        UnityPlayer.this.addView(UnityPlayer.this.m);
                    }
                }
            });
        }

        return this.b(var1, var2);
    }

    private void b() {
        try {
            File var1;
            Object var6;
            if((var1 = new File(this.l.getPackageCodePath(), "assets/bin/Data/settings.xml")).exists()) {
                var6 = new FileInputStream(var1);
            } else {
                var6 = this.l.getAssets().open("bin/Data/settings.xml");
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
                        this.p.putInt(var3, Integer.parseInt(var7.getText()));
                    } else if(var8.equalsIgnoreCase("string")) {
                        this.p.putString(var3, var7.getText());
                    } else if(var8.equalsIgnoreCase("bool")) {
                        this.p.putBoolean(var3, Boolean.parseBoolean(var7.getText()));
                    } else if(var8.equalsIgnoreCase("float")) {
                        this.p.putFloat(var3, Float.parseFloat(var7.getText()));
                    }

                    var3 = null;
                }
            }

        } catch (Exception var5) {
            Ecls.Log(6, "Unable to locate player settings. " + var5.getLocalizedMessage());
            this.c();
        }
    }

    public Bundle getSettings() {
        return this.p;
    }

    public static void UnitySendMessage(String var0, String var1, String var2) {
        if(!Kcls.c()) {
            Ecls.Log(5, "Native libraries not loaded - dropping message for " + var0 + "." + var1);
        } else {
            nativeUnitySendMessage(var0, var1, var2);
        }
    }

    private static native void nativeUnitySendMessage(String var0, String var1, String var2);

    private void c() {
        if(this.l instanceof Activity && !((Activity)this.l).isFinishing()) {
            ((Activity)this.l).finish();
        }

    }

    final void a(Runnable var1) {
        if(this.l instanceof Activity) {
            ((Activity)this.l).runOnUiThread(var1);
        } else {
            Ecls.Log(5, "Not running Unity from an Activity; ignored...");
        }
    }

    public void init(int var1, boolean var2) {
    }

    public View getView() {
        return this;
    }

    protected void kill() {
        Process.killProcess(Process.myPid());
    }

    public void quit() {
        this.v.a();
        this.o = true;
        if(!this.e.e()) {
            this.pause();
        }

        this.unloadGoogleVR();
        this.a.a();

        try {
            this.a.join(4000L);
        } catch (InterruptedException var1) {
            this.a.interrupt();
        }

        if(this.g != null) {
            this.l.unregisterReceiver(this.g);
        }

        this.g = null;
        if(Kcls.c()) {
            this.removeAllViews();
        }

        this.kill();
        g();
    }

    public void pause() {
        if(this.q != null) {
            this.r = this.q.a();
            if(!this.r) {
                this.q.pause();
            }

        } else {
            this.v.b();
            this.d();
        }
    }

    private void d() {
        this.reportSoftInputStr((String)null, 1, true);
        if(this.e.g()) {
            if(Kcls.c()) {
                final Semaphore var1 = new Semaphore(0);
                if(this.isFinishing()) {
                    this.b(new Runnable() {
                        public final void run() {
                            UnityPlayer.this.e();
                            var1.release();
                        }
                    });
                } else {
                    this.b(new Runnable() {
                        public final void run() {
                            if(UnityPlayer.this.nativePause()) {
                                i();
                                UnityPlayer.this.e();
                                var1.release(2);
                            } else {
                                var1.release();
                            }
                        }
                    });
                }

                try {
                    if(!var1.tryAcquire(4L, TimeUnit.SECONDS)) {
                        Ecls.Log(5, "Timeout while trying to pause the Unity Engine.");
                    }
                } catch (InterruptedException var2) {
                    Ecls.Log(5, "UI thread got interrupted while trying to pause the Unity Engine.");
                }

                if(var1.drainPermits() > 0) {
                    this.quit();
                }
            }

            this.e.c(false);
            this.e.b(true);
            if(this.h) {
                this.j.listen(this.i, 0);
            }

            this.a.c();
        }
    }

    public void resume() {
        this.e.b(false);
        if(this.q != null) {
            if(!this.r) {
                this.q.start();
            }

        } else {
            this.f();
            this.v.c();
        }
    }

    public void lowMemory() {
        this.b(new Runnable() {
            public final void run() {
                UnityPlayer.this.nativeLowMemory();
            }
        });
    }

    private void e() {
        this.nativeDone();
    }

    private void f() {
        if(this.e.f()) {
            this.e.c(true);
            if(Kcls.c()) {
                this.h();
            }

            this.b(new Runnable() {
                public final void run() {
                    UnityPlayer.this.nativeResume();
                }
            });
            this.a.b();
        }
    }

    public void configurationChanged(Configuration var1) {
        if(this.m instanceof SurfaceView) {
            this.m.getHolder().setSizeFromLayout();
        }

        if(this.q != null) {
            this.q.updateVideoLayout();
        }

    }

    public void windowFocusChanged(final boolean var1) {
        this.e.a(var1);
        if(var1 && this.b != null) {
            this.reportSoftInputStr((String)null, 1, false);
        }

        this.b(new Runnable() {
            public final void run() {
                UnityPlayer.this.nativeFocusChanged(var1);
            }
        });
        this.a.a(var1);
        this.f();
    }

    protected static boolean loadLibraryStatic(String var0) {
        try {
            System.loadLibrary(var0);
            return true;
        } catch (UnsatisfiedLinkError var1) {
            Ecls.Log(6, "Unable to find " + var0);
            return false;
        } catch (Exception var2) {
            Ecls.Log(6, "Unknown error " + var2);
            return false;
        }
    }

    protected boolean loadLibrary(String var1) {
        return loadLibraryStatic(var1);
    }

    protected void startActivityIndicator() {
        this.a(this.t);
    }

    protected void stopActivityIndicator() {
        this.a(this.u);
    }

    protected void addPhoneCallListener() {
        this.h = true;
        this.j.listen(this.i, 32);
    }

    private final native void nativeFile(String var1);

    private final native void initJni(Context var1);

    private final native boolean nativeRender();

    private final native void nativeSetInputString(String var1);

    private final native void nativeSetInputCanceled(boolean var1);

    private final native boolean nativePause();

    private final native void nativeResume();

    private final native void nativeLowMemory();

    private final native void nativeFocusChanged(boolean var1);

    private final native void nativeRecreateGfxState(int var1, Surface var2);

    private final native void nativeDone();

    private final native void nativeSoftInputClosed();

    private final native void nativeInitWWW(Class var1);

    private final native void nativeInitWebRequest(Class var1);

    private final native int nativeActivityIndicatorStyle();

    private final native boolean nativeInjectEvent(InputEvent var1);

    private final native void nativeMuteMasterAudio(boolean var1);

    private static void a(ApplicationInfo var0) {
        if(n && NativeLoader.load(var0.nativeLibraryDir)) {
            Kcls.a();
        }

    }

    private static void g() {
        if(Kcls.c()) {
            if(!NativeLoader.unload()) {
                throw new UnsatisfiedLinkError("Unable to unload libraries from libmain.so");
            } else {
                Kcls.b();
            }
        }
    }

    protected void showSoftInput(final String var1, final int var2, final boolean var3, final boolean var4, final boolean var5, final boolean var6, final String var7) {
        this.a(new Runnable() {
            public final void run() {
                UnityPlayer.this.b = new Hcls(UnityPlayer.this.l, UnityPlayer.this, var1, var2, var3, var4, var5, var7);
                UnityPlayer.this.b.show();
            }
        });
    }

    protected void hideSoftInput() {
        final Runnable var1 = new Runnable() {
            public final void run() {
                if(UnityPlayer.this.b != null) {
                    UnityPlayer.this.b.dismiss();
                    UnityPlayer.this.b = null;
                }

            }
        };
        if(Gcls.a) {
            this.a(new UnityPlayer.d() {
                public final void a() {
                    UnityPlayer.this.a(var1);
                }
            });
        } else {
            this.a(var1);
        }
    }

    protected void setSoftInputStr(final String var1) {
        this.a(new Runnable() {
            public final void run() {
                if(UnityPlayer.this.b != null && var1 != null) {
                    UnityPlayer.this.b.a(var1);
                }

            }
        });
    }

    protected void reportSoftInputStr(final String var1, final int var2, final boolean var3) {
        if(var2 == 1) {
            this.hideSoftInput();
        }

        this.a(new UnityPlayer.d() {
            public final void a() {
                if(var3) {
                    //todo
//                    UnityPlayer.o(UnityPlayer.this);
                } else if(var1 != null) {
                    UnityPlayer.this.nativeSetInputString(var1);
                }

                if(var2 == 1) {
                    UnityPlayer.this.nativeSoftInputClosed();
                }

            }
        });
    }

    protected long loadGoogleVR(boolean var1, boolean var2, boolean var3) {
        if(!this.v.a(currentActivity, this.l, this.a())) {
            Ecls.Log(6, "Unable to initialize Google VR subsystem.");
            return 0L;
        } else {
            Runnable var4 = new Runnable() {
                public final void run() {
                    UnityPlayer.this.c();
                }
            };
            return this.v.a(var1, var2, var3, var4);
        }
    }

    protected void setGoogleVREnabled(boolean var1) {
        this.v.a(var1);
    }

    protected void unloadGoogleVR() {
        this.v.d();
    }

    protected boolean showVideoPlayer(final String var1, final int var2, final int var3, final int var4, final boolean var5, final int var6, final int var7) {
        final Semaphore var8 = new Semaphore(0);
        final AtomicInteger var9 = new AtomicInteger(-1);
        this.a(new Runnable() {
            public final void run() {
                if(UnityPlayer.this.q != null) {
                    Ecls.Log(5, "Video already playing");
                    var9.set(2);
                    var8.release();
                } else {
                    UnityPlayer.this.q = new Mcls(UnityPlayer.this.l, var1, var2, var3, var4, var5, (long)var6, (long)var7, new com.unity3d.player.Mcls.a() {
                        public final void a(int var1x) {
                            var9.set(var1x);
                            if(var1x == 3) {
                                if(UnityPlayer.this.m.getParent() == null) {
                                    UnityPlayer.this.addView(UnityPlayer.this.m);
                                }

                                if(UnityPlayer.this.q != null) {
                                    UnityPlayer.this.q.destroyPlayer();
                                    UnityPlayer.this.removeView(UnityPlayer.this.q);
                                    UnityPlayer.this.q = null;
                                }

                                UnityPlayer.this.resume();
                            }

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
            var8.acquire();
            var11 = var9.get() != 2;
        } catch (InterruptedException var10) {
            ;
        }

        if(var11) {
            if(this.q != null) {
                this.a(new Runnable() {
                    public final void run() {
                        if(UnityPlayer.this.q != null) {
                            UnityPlayer.this.d();
                            UnityPlayer.this.q.requestFocus();
                            UnityPlayer.this.removeView(UnityPlayer.this.m);
                        }

                    }
                });
            }
        } else if(this.q != null) {
            this.a(new Runnable() {
                public final void run() {
                    if(UnityPlayer.this.m.getParent() == null) {
                        UnityPlayer.this.addView(UnityPlayer.this.m);
                    }

                    if(UnityPlayer.this.q != null) {
                        UnityPlayer.this.q.destroyPlayer();
                        UnityPlayer.this.removeView(UnityPlayer.this.q);
                        UnityPlayer.this.q = null;
                    }

                    UnityPlayer.this.resume();
                }
            });
        }

        return var11;
    }

    protected int getSplashMode() {
        return this.p.getInt("splash_mode", 0);
    }

    protected void executeGLThreadJobs() {
        Runnable var1;
        while((var1 = (Runnable)this.f.poll()) != null) {
            var1.run();
        }

    }

    protected void disableLogger() {
        Ecls.a = true;
    }

    private void b(Runnable var1) {
        if(Kcls.c()) {
            if(Thread.currentThread() == this.a) {
                var1.run();
            } else {
                this.f.add(var1);
            }
        }
    }

    private void a(UnityPlayer.d var1) {
        if(!this.isFinishing()) {
            this.b((Runnable)var1);
        }
    }

    protected boolean isFinishing() {
        return this.o || (this.o = this.l instanceof Activity && ((Activity)this.l).isFinishing());
    }

    private void h() {
        if(this.p.getBoolean("useObb")) {
            String[] var1;
            int var2 = (var1 = a((Context)this.l)).length;

            for(int var3 = 0; var3 < var2; ++var3) {
                String var4;
                String var5 = a(var4 = var1[var3]);
                if(this.p.getBoolean(var5)) {
                    this.nativeFile(var4);
                }

                this.p.remove(var5);
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

    private static String a(String var0) {
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

    private void i() {
        if(this.l instanceof Activity) {
            ((Activity)this.l).getWindow().setFlags(1024, 1024);
        }

    }

    public boolean injectEvent(InputEvent var1) {
        return this.nativeInjectEvent(var1);
    }

    public boolean onKeyUp(int var1, KeyEvent var2) {
        return this.injectEvent(var2);
    }

    public boolean onKeyDown(int var1, KeyEvent var2) {
        return this.injectEvent(var2);
    }

    public boolean onKeyMultiple(int var1, int var2, KeyEvent var3) {
        return this.injectEvent(var3);
    }

    public boolean onKeyLongPress(int var1, KeyEvent var2) {
        return this.injectEvent(var2);
    }

    public boolean onTouchEvent(MotionEvent var1) {
        return this.injectEvent(var1);
    }

    public boolean onGenericMotionEvent(MotionEvent var1) {
        return this.injectEvent(var1);
    }

    private void a(View var1, View var2) {
        ViewParent var3;
        if(var1 != null && (!((var3 = var1.getParent()) instanceof UnityPlayer) || (UnityPlayer)var3 != this)) {
            if(var3 instanceof ViewGroup) {
                ((ViewGroup)var3).removeView(var1);
            }

            this.addView(var1);
        }

        if(var2 != null && var2.getParent() == this) {
            this.removeView(var2);
        }

    }

    public boolean addViewToPlayer(View var1, boolean var2) {
        this.a((View)var1, (View)(var2?this.m:null));
        boolean var5 = var1.getParent() == this;
        var2 = var2 && this.m.getParent() == null;
        boolean var3 = this.m.getParent() == this;
        boolean var4;
        if(!(var4 = var5 && (var2 || var3))) {
            if(!var5) {
                Ecls.Log(6, "addViewToPlayer: Failure adding view to hierarchy");
            }

            if(!var2 && !var3) {
                Ecls.Log(6, "addViewToPlayer: Failure removing old view from hierarchy");
            }
        }

        return var4;
    }

    public void removeViewFromPlayer(View var1) {
        this.a((View)this.m, (View)var1);
        boolean var3 = var1.getParent() == null;
        boolean var2 = this.m.getParent() == this;
        if(!var3 || !var2) {
            if(!var3) {
                Ecls.Log(6, "removeViewFromPlayer: Failure removing view from hierarchy");
            }

            if(!var2) {
                Ecls.Log(6, "removeVireFromPlayer: Failure agging old view to hierarchy");
            }
        }

    }

    public void reportError(String var1, String var2) {
        StringBuilder var3;
        (var3 = new StringBuilder()).append(var1);
        var3.append(": ");
        var3.append(var2);
        Ecls.Log(6, var3.toString());
    }

    static {
        (new Jcls()).a();
        n = false;
        n = loadLibraryStatic("main");
    }

    private abstract class d implements Runnable {
        private d() {
        }

        public final void run() {
            if(!UnityPlayer.this.isFinishing()) {
                this.a();
            }

        }

        public abstract void a();
    }

    private class a extends PhoneStateListener {
        private a() {
        }

        public final void onCallStateChanged(int var1, String var2) {
            UnityPlayer.this.nativeMuteMasterAudio(var1 == 1);
        }
    }

    private class c extends Thread {
        Handler a;
        boolean b;

        private c() {
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
                        UnityPlayer.Eb var2;
                        if((var2 = (UnityPlayer.Eb)var1.obj) == UnityPlayer.Eb.c) {
                            Looper.myLooper().quit();
                        } else if(var2 == UnityPlayer.Eb.b) {
                            c.this.b = true;
                        } else if(var2 == UnityPlayer.Eb.a) {
                            c.this.b = false;
                            UnityPlayer.this.executeGLThreadJobs();
                        } else if(var2 == UnityPlayer.Eb.e) {
                            if(!c.this.b) {
                                UnityPlayer.this.executeGLThreadJobs();
                            }
                        } else if(var2 == UnityPlayer.Eb.f) {
                            UnityPlayer.this.executeGLThreadJobs();
                            if(!UnityPlayer.this.isFinishing() && !UnityPlayer.this.nativeRender()) {
                                UnityPlayer.this.c();
                            }
                        }

                        if(c.this.b) {
                            Message.obtain(c.this.a, 2269, UnityPlayer.Eb.f).sendToTarget();
                        }

                        return true;
                    }
                }
            });
            Looper.loop();
        }

        public final void a() {
            this.a(UnityPlayer.Eb.c);
        }

        public final void b() {
            this.a(UnityPlayer.Eb.b);
        }

        public final void c() {
            this.a(UnityPlayer.Eb.a);
        }

        public final void a(boolean var1) {
            this.a(var1?UnityPlayer.Eb.d:UnityPlayer.Eb.e);
        }

        private void a(UnityPlayer.Eb var1) {
            Message.obtain(this.a, 2269, var1).sendToTarget();
        }
    }

    static enum Eb {
        a,
        b,
        c,
        d,
        e,
        f;

        private Eb() {
        }
    }
}
