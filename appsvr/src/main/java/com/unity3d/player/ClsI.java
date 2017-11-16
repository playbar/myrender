//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import android.os.Build.VERSION;
import com.unity3d.player.b;
import com.unity3d.player.c;
import com.unity3d.player.d;
import com.unity3d.player.e;
import com.unity3d.player.f;
import com.unity3d.player.h;

public final class ClsI {
    static final boolean a;
    static final boolean b;
    static final boolean c;
    static final boolean d;
    static final boolean e;
    static final b f;
    static final f g;
    static final h h;

    static {
        a = VERSION.SDK_INT >= 11;
        b = VERSION.SDK_INT >= 17;
        c = VERSION.SDK_INT >= 19;
        d = VERSION.SDK_INT >= 21;
        e = VERSION.SDK_INT >= 23;
        f = a?new b():null;
        g = b?new f():null;
        h = e?new h():null;
    }
}
