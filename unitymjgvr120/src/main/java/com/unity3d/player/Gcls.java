//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import android.os.Build.VERSION;

public final class Gcls {
    public static final boolean a;
    public static final boolean b;
    static final Ccls c;

    static {
        a = VERSION.SDK_INT >= 21;
        c = (b = VERSION.SDK_INT >= 23)?new Fcls():null;
    }
}
