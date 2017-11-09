//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Build.VERSION;
import android.view.SurfaceView;
import android.view.View;
import com.unity3d.player.UnityPlayer;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicLong;

final class Bcls {
    private Lcls a = null;
    private boolean b = false;
    private boolean c = false;
    private boolean d = false;
    private Context e = null;
    private Dcls f = null;
    private String g = "";

    public Bcls(Dcls var1) {
        this.f = var1;
    }

    private void b(boolean var1) {
        this.c = var1;
    }

    private boolean e() {
        return this.c;
    }

    public final void a(Intent var1) {
        if(var1 != null && var1.getBooleanExtra("android.intent.extra.VR_LAUNCH", false)) {
            this.d = true;
        }

    }

    public final void a() {
        Activity var1 = (Activity)this.e;
        if(this.d && !this.b && var1 != null) {
            this.b = true;
            Intent var2;
            (var2 = new Intent("android.intent.action.MAIN")).addCategory("android.intent.category.HOME");
            var2.setFlags(268435456);
            var1.startActivity(var2);
        }

    }

    public final void b() {
        if(this.a != null) {
            this.a.a("pause", new Object[0]);
        }

    }

    public final void c() {
        if(this.a != null) {
            this.a.a("resume", new Object[0]);
        }

    }

    private void a(Runnable var1) {
        if(this.e instanceof Activity) {
            ((Activity)this.e).runOnUiThread(var1);
        } else {
            Ecls.Log(5, "Not running Google VR from an Activity; Ignoring execution request...");
        }
    }

    private boolean b(final Runnable var1) {
        boolean var2 = true;
        final Semaphore var3 = new Semaphore(0);
        this.a(new Runnable() {
            public final void run() {
                try {
                    var1.run();
                    return;
                } catch (Exception var4) {
                    Bcls.this.a("Exception unloading Google VR on UI Thread. " + var4.getLocalizedMessage());
                } finally {
                    var3.release();
                }

            }
        });

        try {
            if(!var3.tryAcquire(40L, TimeUnit.SECONDS)) {
                var2 = false;
                this.a("Timeout waiting for vr state change!");
            }
        } catch (InterruptedException var4) {
            var2 = false;
            this.a("Interrupted while trying to acquire sync lock. " + var4.getLocalizedMessage());
        }

        return var2;
    }

    public final boolean a(Activity var1, Context var2, SurfaceView var3) {
        if(var1 != null && var2 != null && var3 != null) {
            this.e = var2;
            if(!a(19)) {
                this.a("Google VR requires a device that supports an api version of 19 (KitKat) or better.");
                return false;
            } else if(this.d && !a(24)) {
                this.a("Daydream requires a device that supports an api version of 24 (Nougat) or better.");
                return false;
            } else {
                ClassLoader var4 = UnityPlayer.class.getClassLoader();
                if(!this.a(var4)) {
                    return false;
                } else {
                    boolean var6;
                    try {
                        var6 = ((Boolean)this.a.a("initialize", new Object[]{var1, var2, var3, Boolean.valueOf(this.d)})).booleanValue();
                    } catch (Exception var5) {
                        var6 = false;
                        this.a("Exception while trying to intialize Unity Google VR Library. " + var5.getLocalizedMessage());
                    }

                    if(!var6) {
                        this.a("Unable to initialize GoogleVR library.");
                    }

                    this.b(false);
                    this.b = false;
                    this.g = "";
                    return true;
                }
            }
        } else {
            this.a("Invalid parameters passed to Google VR initiialization.");
            return false;
        }
    }

    public final long a(final boolean var1, final boolean var2, final boolean var3, final Runnable var4) {
        final AtomicLong var5 = new AtomicLong(0L);
        this.g = !var1 && !var2?"Cardboard":"Daydream";
        if(!this.b(new Runnable() {
            public final void run() {
                try {
                    var5.set(((Long)Bcls.this.a.a("load", new Object[]{Boolean.valueOf(var1), Boolean.valueOf(var2), Boolean.valueOf(var3), var4})).longValue());
                    Bcls.this.c();
                } catch (Exception var2x) {
                    Bcls.this.a("Exception caught while loading GoogleVR. " + var2x.getLocalizedMessage());
                    var5.set(0L);
                }
            }
        }) || var5.longValue() == 0L) {
            this.a("Google VR had a fatal issue while loading. VR will not be available.");
        }

        return var5.longValue();
    }

    public final void a(final boolean var1)
    {
        if(this.f != null && this.e != null) {
            this.a(new Runnable() {
                public final void run() {
                    if(var1 != Bcls.this.e()) {
                        try {
                            if(var1 && !Bcls.this.e()) {
                                if(Bcls.this.a != null && Bcls.this.f != null && !Bcls.this.f.addViewToPlayer((View)Bcls.this.a.a("getGvrLayout", new Object[0]), true)) {
                                    Bcls.this.a("Unable to add Google VR to view hierarchy.");
                                } else {
                                    if(Bcls.this.a != null) {
                                        Bcls.this.a.a("enable", new Object[]{Boolean.valueOf(true)});
                                    }

                                    Bcls.this.b(true);
                                }
                            } else {
                                if(!var1 && Bcls.this.e()) {
                                    Bcls.this.b(false);
                                    if(Bcls.this.a != null) {
                                        Bcls.this.a.a("enable", new Object[]{Boolean.valueOf(false)});
                                    }

                                    if(Bcls.this.a != null && Bcls.this.f != null) {
                                        Bcls.this.f.removeViewFromPlayer((View)Bcls.this.a.a("getGvrLayout", new Object[0]));
                                    }
                                }

                            }
                        } catch (Exception var2) {
                            Bcls.this.a("Exception enablinb Google VR on UI Thread. " + var2.getLocalizedMessage());
                        }
                    }
                }
            });
        }
    }

    public final void d() {
        this.a(false);
        this.a(new Runnable() {
            public final void run() {
                try {
                    if(Bcls.this.a != null) {
                        Bcls.this.a.a("unload", new Object[0]);
                        Bcls.this.a.a("deinitialize", new Object[0]);
                        //todo
//                        b.d(b.this);
                    }

                    Bcls.this.b(false);
                } catch (Exception var2) {
                    Bcls.this.a("Exception unloading Google VR on UI Thread. " + var2.getLocalizedMessage());
                }
            }
        });
    }

    private boolean a(ClassLoader var1) {
        try {
            Class var4;
            Object var2 = (var4 = var1.loadClass("com.unity3d.unitygvr.GoogleVR")).getConstructor(new Class[0]).newInstance(new Object[0]);
            this.a = new Lcls(var4, var2);
            this.a.a("initialize", new Class[]{Activity.class, Context.class, SurfaceView.class, Boolean.TYPE});
            this.a.a("deinitialize", new Class[0]);
            this.a.a("load", new Class[]{Boolean.TYPE, Boolean.TYPE, Boolean.TYPE, Runnable.class});
            this.a.a("enable", new Class[]{Boolean.TYPE});
            this.a.a("unload", new Class[0]);
            this.a.a("pause", new Class[0]);
            this.a.a("resume", new Class[0]);
            this.a.a("getGvrLayout", new Class[0]);
            return true;
        } catch (Exception var3) {
            this.a("Exception initializing GoogleVR from Unity library. " + var3.getLocalizedMessage());
            return false;
        }
    }

    private void a(String var1) {
        if(this.f != null) {
            this.f.reportError("Google VR Error [" + this.g + "]", var1);
        } else {
            Ecls.Log(6, "Google VR Error [" + this.g + "]: " + var1);
        }
    }

    private static boolean a(int var0) {
        return VERSION.SDK_INT >= var0;
    }
}
