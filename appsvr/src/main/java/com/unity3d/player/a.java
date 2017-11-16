//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.hardware.Camera.Parameters;
import android.hardware.Camera.PreviewCallback;
import android.hardware.Camera.Size;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

final class a {
    private final Object[] f = new Object[0];
    private final int g;
    private final int h;
    private final int i;
    private final int j;
    Camera a;
    Parameters b;
    Size c;
    int d;
    int[] e;

    public a(int var1, int var2, int var3, int var4) {
        this.g = var1;
        this.h = a(var2, 640);
        this.i = a(var3, 480);
        this.j = a(var4, 24);
    }

    private void b(final a.ai var1) {
        Object[] var2 = this.f;
        synchronized(this.f) {
            this.a = Camera.open(this.g);
            this.b = this.a.getParameters();
            this.c = this.f();
            this.e = this.e();
            this.d = this.d();
            a(this.b);
            this.b.setPreviewSize(this.c.width, this.c.height);
            this.b.setPreviewFpsRange(this.e[0], this.e[1]);
            this.a.setParameters(this.b);
            PreviewCallback var5 = new PreviewCallback() {
                long a = 0L;

                public final void onPreviewFrame(byte[] var1x, Camera var2) {
                    if(a.this.a == var2) {
                        var1.onCameraFrame(a.this, var1x);
                    }
                }
            };
            int var3 = this.c.width * this.c.height * this.d / 8 + 4096;
            this.a.addCallbackBuffer(new byte[var3]);
            this.a.addCallbackBuffer(new byte[var3]);
            this.a.setPreviewCallbackWithBuffer(var5);
        }
    }

    private static void a(Parameters var0) {
        if(var0.getSupportedColorEffects() != null) {
            var0.setColorEffect("none");
        }

        if(var0.getSupportedFocusModes().contains("continuous-video")) {
            var0.setFocusMode("continuous-video");
        }

    }

    public final int a() {
        return this.g;
    }

    public final Size b() {
        return this.c;
    }

    public final void a(a.ai var1) {
        Object[] var2 = this.f;
        synchronized(this.f) {
            if(this.a == null) {
                this.b(var1);
            }

            if(ClsI.a && ClsI.f.a(this.a)) {
                this.a.startPreview();
            }
        }
    }

    public final void a(byte[] var1) {
        Object[] var2 = this.f;
        synchronized(this.f) {
            if(this.a != null) {
                this.a.addCallbackBuffer(var1);
            }

        }
    }

    public final void c() {
        Object[] var1 = this.f;
        synchronized(this.f) {
            if(this.a != null) {
                this.a.setPreviewCallbackWithBuffer((PreviewCallback)null);
                this.a.stopPreview();
                this.a.release();
                this.a = null;
            }

        }
    }

    private final int d() {
        this.b.setPreviewFormat(17);
        return ImageFormat.getBitsPerPixel(17);
    }

    private final int[] e() {
        double var1 = (double)(this.j * 1000);
        Object var3;
        if((var3 = this.b.getSupportedPreviewFpsRange()) == null) {
            var3 = new ArrayList();
        }

        int[] var4 = new int[]{this.j * 1000, this.j * 1000};
        double var5 = 1.7976931348623157E308D;
        Iterator var10 = ((List)var3).iterator();

        while(var10.hasNext()) {
            int[] var7 = (int[])var10.next();
            double var8;
            if((var8 = Math.abs(Math.log(var1 / (double)var7[0])) + Math.abs(Math.log(var1 / (double)var7[1]))) < var5) {
                var5 = var8;
                var4 = var7;
            }
        }

        return var4;
    }

    private final Size f() {
        double var1 = (double)this.h;
        double var3 = (double)this.i;
        List var5 = this.b.getSupportedPreviewSizes();
        Size var6 = null;
        double var7 = 1.7976931348623157E308D;
        Iterator var12 = var5.iterator();

        while(var12.hasNext()) {
            Size var9 = (Size)var12.next();
            double var10;
            if((var10 = Math.abs(Math.log(var1 / (double)var9.width)) + Math.abs(Math.log(var3 / (double)var9.height))) < var7) {
                var7 = var10;
                var6 = var9;
            }
        }

        return var6;
    }

    private static final int a(int var0, int var1) {
        return var0 != 0?var0:var1;
    }

    interface ai {
        void onCameraFrame(a var1, byte[] var2);
    }
}
