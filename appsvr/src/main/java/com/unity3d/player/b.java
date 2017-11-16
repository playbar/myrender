//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Handler;
import android.view.View;
import android.view.View.OnSystemUiVisibilityChangeListener;

public final class b implements c {
    private static final SurfaceTexture a = new SurfaceTexture(-1);
    private static final int b;
    private volatile boolean c;

    public b() {
    }

    public final boolean a(Camera var1) {
        try {
            var1.setPreviewTexture(a);
            return true;
        } catch (Exception var2) {
            return false;
        }
    }

    public final void a(View var1, boolean var2) {
        this.c = var2;
        var1.setSystemUiVisibility(this.c?var1.getSystemUiVisibility() | b:var1.getSystemUiVisibility() & ~b);
    }

    public final void a(final View var1) {
        if(!ClsI.d) {
            var1.setOnSystemUiVisibilityChangeListener(new OnSystemUiVisibilityChangeListener() {
                public final void onSystemUiVisibilityChange(int var1x) {
                    b.this.a(var1);
//                    b.a(b.this, var1);
                }
            });
        }
    }

    public final void b(View var1) {
        if(!ClsI.c && this.c) {
            this.a(var1, false);
            this.c = true;
        }

        this.a(var1, 1000);
    }

    private void a(final View var1, int var2) {
        Handler var3;
        if((var3 = var1.getHandler()) == null) {
            this.a(var1, this.c);
        } else {
            var3.postDelayed(new Runnable() {
                public final void run() {
                    b.this.a(var1, b.this.c);
                }
            }, 1000L);
        }
    }

    static {
        b = ClsI.c?5894:1;
    }
}
