//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.google.vr.cardboard;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.AlertDialog.Builder;
import android.content.ActivityNotFoundException;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.DialogInterface.OnClickListener;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.net.Uri;
import android.os.PowerManager;
import android.os.Build.VERSION;
import android.provider.Settings.Secure;
import android.util.Log;
import android.view.Window;
import com.google.vr.cardboard.R.string;
import com.google.vr.cardboard.R.style;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.Iterator;
import java.util.List;

public class AndroidNCompat {
    private static final String TAG = AndroidNCompat.class.getSimpleName();
    private static final boolean DEBUG = false;
    public static final int N_SDK_LEVEL = 24;
    public static final int NMR1_SDK_LEVEL = 25;
    private static final String DEFAULT_VR_MODE_PACKAGE = "com.baofeng.mj";
    private static final String DEFAULT_VR_MODE_CLASS = "com.google.vr.vrcore.common.VrCoreListenerService";
    private static final String ACTION_VR_LISTENER_SETTINGS = "android.settings.VR_LISTENER_SETTINGS";
    private static final String ENABLED_VR_LISTENERS = "enabled_vr_listeners";
    private static int sSdkLevelOverride = 0;
    private static final int SUCCESS = 0;
    private static final int PACKAGE_NOT_PRESENT = -1;
    private static final int PACKAGE_NOT_ENABLED = -2;
    public static final int FLAG_VR_MODE_ENABLE_FALLBACK = 1;

    public static boolean setVrModeEnabled(Activity var0, boolean var1) {
        return setVrModeEnabled(var0, var1, 1);
    }

    public static boolean setVrModeEnabled(Activity var0, boolean var1, int var2) {
        if(!isVrModeSupported(var0)) {
            if(isAtLeastN()) {
                Log.d(TAG, "VR mode is not supported on this N device.");
            }

            return false;
        } else {
            ComponentName var3 = new ComponentName("com.baofeng.mj", "com.google.vr.vrcore.common.VrCoreListenerService");

            String var10000;
            String var5;
            try {
                var0.setVrModeEnabled(var1, var3);
                return true;
            } catch (NameNotFoundException var6) {
                var10000 = TAG;
                var5 = String.valueOf(var6);
                Log.w(var10000, (new StringBuilder(25 + String.valueOf(var5).length())).append("No VR service component: ").append(var5).toString());
                if((var2 & 1) != 0 && handleVrCoreAbsence(var0, checkForVrCorePresence(var0))) {
                    Log.w(TAG, "Failed to handle missing VrCore package.");
                }
            } catch (UnsupportedOperationException var7) {
                var10000 = TAG;
                var5 = String.valueOf(var7);
                Log.w(var10000, (new StringBuilder(23 + String.valueOf(var5).length())).append("Failed to set VR mode: ").append(var5).toString());
            }

            return false;
        }
    }

    public static boolean isVrModeSupported(Context var0) {
        return isAtLeastN() && var0.getPackageManager().hasSystemFeature("android.software.vr.mode");
    }

    public static boolean setSustainedPerformanceMode(Activity var0, boolean var1) {
        if(!isAtLeastN()) {
            return false;
        } else if(!((PowerManager)var0.getSystemService("power")).isSustainedPerformanceModeSupported()) {
            Log.d(TAG, "Sustained performance mode is not supported on this device.");
            return false;
        } else {
            Window var3;
            if((var3 = var0.getWindow()) == null) {
                Log.e(TAG, "Activity does not have a window");
                return false;
            } else {
                var3.setSustainedPerformanceMode(var1);
                return true;
            }
        }
    }

    public static void setVrThread(int var0) {
        if(isAtLeastN()) {
            String var10000;
            Method var1;
            String var3;
            try {
                var1 = ActivityManager.class.getMethod("setVrThread", new Class[]{Integer.TYPE});
            } catch (RuntimeException | NoSuchMethodException var5) {
                if(isAtLeastNMR1()) {
                    var10000 = TAG;
                    var3 = String.valueOf(var5);
                    Log.e(var10000, (new StringBuilder(38 + String.valueOf(var3).length())).append("Failed to acquire setVrThread method: ").append(var3).toString());
                    return;
                }

                var10000 = TAG;
                var3 = String.valueOf(var5);
                Log.w(var10000, (new StringBuilder(38 + String.valueOf(var3).length())).append("Failed to acquire setVrThread method: ").append(var3).toString());
                return;
            }

            try {
                var1.invoke((Object)null, new Object[]{Integer.valueOf(var0)});
            } catch (IllegalAccessException | RuntimeException | InvocationTargetException var4) {
                var10000 = TAG;
                var3 = String.valueOf(var4);
                Log.e(var10000, (new StringBuilder(30 + String.valueOf(var3).length())).append("Failed to invoke setVrThread: ").append(var3).toString());
            }
        }
    }

    public static void setSdkLevelForTesting(int var0) {
        sSdkLevelOverride = var0;
    }

    private static boolean isAtLeastN() {
        return sSdkLevelOverride >= 24 || VERSION.SDK_INT >= 24;
    }

    private static boolean isAtLeastNMR1() {
        return sSdkLevelOverride >= 25 || "NMR1".equals(VERSION.CODENAME) || VERSION.SDK_INT >= 25;
    }

    private static int checkForVrCorePresence(Context var0) {
        List var1 = var0.getPackageManager().getInstalledApplications(0);
        boolean var2 = false;
        Iterator var3 = var1.iterator();

        while(var3.hasNext()) {
            if(((ApplicationInfo)var3.next()).packageName.equals("com.baofeng.mj")) {
                var2 = true;
                break;
            }
        }

        if(!var2) {
            return -1;
        } else {
            String var5 = Secure.getString(var0.getContentResolver(), "enabled_vr_listeners");
            ComponentName var4 = new ComponentName("com.baofeng.mj", "com.google.vr.vrcore.common.VrCoreListenerService");
            return var5 != null && var5.contains(var4.flattenToString())?0:-2;
        }
    }

    private static boolean handleVrCoreAbsence(final Context var0, int var1) {
        if(var1 == -1) {
            showWarningDialog(var0, string.dialog_vr_core_not_installed, string.go_to_playstore_button, new OnClickListener() {
                public final void onClick(DialogInterface var1, int var2) {
                    Intent var3;
                    (var3 = new Intent("android.intent.action.VIEW")).setData(Uri.parse("market://details?id=com.google.vr.vrcore"));
                    var3.setPackage("com.android.vending");

                    try {
                        var0.startActivity(var3);
                    } catch (ActivityNotFoundException var4) {
                        Log.e(AndroidNCompat.TAG, "Google Play Services is not installed, unable to download VrCore.");
                    }
                }
            });
            return false;
        } else if(var1 == -2) {
            showWarningDialog(var0, string.dialog_vr_core_not_enabled, string.go_to_vr_listeners_settings_button, new OnClickListener() {
                public final void onClick(DialogInterface var1, int var2) {
                    var0.startActivity(new Intent("android.settings.VR_LISTENER_SETTINGS"));
                }
            });
            return false;
        } else {
            return true;
        }
    }

    private static void showWarningDialog(Context var0, int var1, int var2, OnClickListener var3) {
        Builder var4;
        (var4 = new Builder(var0, style.GvrDialogTheme)).setMessage(var1).setTitle(string.dialog_title_warning).setPositiveButton(var2, var3).setNegativeButton(string.cancel_button, new OnClickListener() {
            public final void onClick(DialogInterface var1, int var2) {
            }
        });
        var4.create().show();
    }

    private AndroidNCompat() {
    }
}
