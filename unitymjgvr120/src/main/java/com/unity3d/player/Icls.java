//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.LayerDrawable;
import android.view.View;
import android.view.View.OnLayoutChangeListener;

public final class Icls implements OnLayoutChangeListener {
    final View a;
    final int b;
    final int c;
    static Bitmap d;
    Bitmap e;
    Drawable f;

    public Icls(View var1, int var2) {
        this.a = var1;
        this.b = var2;
        this.c = var1.getResources().getIdentifier("unity_static_splash", "drawable", var1.getContext().getPackageName());
        if(this.c != 0) {
            this.a.addOnLayoutChangeListener(this);
            this.a.forceLayout();
        }

    }

    public final void onLayoutChange(View var1, int var2, int var3, int var4, int var5, int var6, int var7, int var8, int var9) {
        if(this.f == null) {
            this.f = var1.getBackground();
        }

        if(this.d == null) {
            this.d = BitmapFactory.decodeResource(var1.getResources(), this.c);
        }

//        var2 = this.d.getWidth();
//        var3 = this.d.getHeight();
//        var4 = var1.getWidth();
//        var5 = var1.getHeight();
//        if(var4 != 0 && var5 != 0)
//        {
//            label47:
//            {
//                float var11 = (float)var2 / (float)var3;
//                boolean var13 = (float)var4 / (float)var5 <= var11;
//                switch(null.a[this.b - 1]) {
//                    case 1:
//                        if(var4 < var2) {
//                            var2 = var4;
//                            var3 = (int)((float)var4 / var11);
//                        }
//
//                        if(var5 >= var3) {
//                            break label47;
//                        }
//
//                        var3 = var5;
//                        break;
//                    case 2:
//                    case 3:
//                        var2 = var4;
//                        var3 = var5;
//                        if(var13 ^ this.b == i.a.c) {
//                            var3 = (int)((float)var4 / var11);
//                            break label47;
//                        }
//                        break;
//                    default:
//                        break label47;
//                }
//
//                var2 = (int)((float)var3 * var11);
//            }
//
//            if(this.e == null || this.e.getWidth() != var2 || this.e.getHeight() != var3) {
//                this.e = Bitmap.createScaledBitmap(this.d, var2, var3, true);
//                ColorDrawable var10 = new ColorDrawable(-16777216);
//                BitmapDrawable var12;
//                (var12 = new BitmapDrawable(var1.getResources(), this.e)).setGravity(17);
//                var1.setBackgroundDrawable(new LayerDrawable(new Drawable[]{var10, var12}));
//            }
//        }
    }

    static enum Enuma {
        ena(1),
        enb(2),
        enc(3);

        int mval = 0;
        Enuma(int val){
            mval = val;
        }
        public static int[] a() {
            return (int[])null;//d.clone();
        }

//        static {
//            d = new int[]{a, b, c};
//        }
    }
}
