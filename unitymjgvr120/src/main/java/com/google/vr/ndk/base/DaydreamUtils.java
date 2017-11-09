//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.google.vr.ndk.base;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.os.Build.VERSION;
import com.google.vrtoolkit.cardboard.proto.nano.CardboardDevice.DeviceParams;
import java.util.Iterator;
import java.util.List;

public class DaydreamUtils {
    static final String INTENT_CATEGORY_DAYDREAM = "com.google.intent.category.DAYDREAM";
    static final String INTENT_CATEGORY_CARDBOARD = "com.google.intent.category.CARDBOARD";
    public static final int DAYDREAM_NOT_SUPPORTED = 0;
    public static final int DAYDREAM_OPTIONAL = 1;
    public static final int DAYDREAM_REQUIRED = 2;
    private static boolean sDaydreamPhoneOverrideForTesting;

    public static boolean isDaydreamPhone(Context var0) {
        return true;
//        return sDaydreamPhoneOverrideForTesting?true:(VERSION.SDK_INT < 24?false:var0.getPackageManager().hasSystemFeature("android.hardware.vr.high_performance"));
    }

    public static boolean isDaydreamViewer(DeviceParams var0) {
        return var0 != null && var0.daydreamInternal != null;
    }

    public static int getComponentDaydreamCompatibility(Context var0, ComponentName var1) {
        return getComponentDaydreamCompatibility(var0, var0.getPackageManager(), var1);
    }

    static void setIsDaydreamPhoneForTesting(boolean var0) {
        sDaydreamPhoneOverrideForTesting = var0;
    }

    static int getComponentDaydreamCompatibility(Context var0, PackageManager var1, ComponentName var2) {
        Intent var3;
        (var3 = new Intent()).setPackage(var2.getPackageName());
        var3.addCategory("com.google.intent.category.DAYDREAM");
        if(!canResolveIntent(var1, var2, var3)) {
            return 0;
        } else {
            Intent var4;
            (var4 = new Intent()).setPackage(var2.getPackageName());
            var4.addCategory("com.google.intent.category.CARDBOARD");
            return canResolveIntent(var1, var2, var4)?1:2;
        }
    }

    private static boolean canResolveIntent(PackageManager var0, ComponentName var1, Intent var2) {
        List var3;
        if((var3 = var0.queryIntentActivities(var2, 128)) != null) {
            Iterator var4 = var3.iterator();

            while(var4.hasNext()) {
                ResolveInfo var5;
                if((var5 = (ResolveInfo)var4.next()) != null && var5.activityInfo != null && var5.activityInfo.name != null && var5.activityInfo.name.equals(var1.getClassName())) {
                    return true;
                }
            }
        }

        return false;
    }

    protected DaydreamUtils() {
    }
}
