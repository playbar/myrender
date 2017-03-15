//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

final class ClsL {
    private static boolean a = false;
    private boolean b;
    private boolean c;
    private boolean d;
    private boolean e;

    ClsL() {
        this.b = !ClsI.e;
        this.c = false;
        this.d = false;
        this.e = true;
    }

    static void a() {
        a = true;
    }

    static void b() {
        a = false;
    }

    static boolean c() {
        return a;
    }

    final void d() {
        this.b = true;
    }

    final void a(boolean var1) {
        this.c = var1;
    }

    final void b(boolean var1) {
        this.e = var1;
    }

    final boolean e() {
        return this.e;
    }

    final void c(boolean var1) {
        this.d = var1;
    }

    final boolean f() {
        return a && this.c && this.b && !this.e && !this.d;
    }

    final boolean g() {
        return this.d;
    }

    public final String toString() {
        return super.toString();
    }
}
