//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.google.vr.cardboard;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager.NameNotFoundException;

public class PackageUtils {
    private static final String GOOGLE_PACKAGE_PREFIX = "com.baofeng.";

    public PackageUtils() {
    }

    public static boolean isGooglePackage(String var0) {
        return var0 != null && var0.startsWith("com.baofeng.");
    }

    public static boolean isSystemPackage(Context var0, String var1) {
        try {
            ApplicationInfo var2;
            int var10000 = (var2 = var0.getPackageManager().getApplicationInfo(var1, 0)) != null?var2.flags:0;
            int var3 = var10000;
            return (var10000 & 1) != 0 || (var3 & 128) != 0;
        } catch (NameNotFoundException var4) {
            return false;
        }
    }
}
