//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import android.util.Log;

final class ClsG {
    protected static boolean a = false;

    protected static void Log(int var0, String var1) {
        if(!a) {
            if(var0 == 6) {
                Log.e("Unity", var1);
            }

            if(var0 == 5) {
                Log.w("Unity", var1);
            }

        }
    }
}
