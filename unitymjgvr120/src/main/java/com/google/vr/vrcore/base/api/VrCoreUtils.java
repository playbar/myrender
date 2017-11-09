//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.google.vr.vrcore.base.api;

import android.content.ComponentName;
import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.Signature;
import android.content.pm.PackageInstaller.SessionInfo;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Build.VERSION;
import android.util.Log;
import com.google.vr.cardboard.annotations.UsedByNative;
import java.util.Iterator;

@UsedByNative
public final class VrCoreUtils {
    private static final String TAG = VrCoreUtils.class.getSimpleName();
    private static final boolean DEBUG = false;

    public VrCoreUtils() {
    }

    public static int checkVrCoreAvailability(Context var0) {
        return checkVrCoreAvailabilityImpl(var0);
    }

    public static boolean isVrCoreAvailable(Context var0) {
        return checkVrCoreAvailability(var0) == 0;
    }

    @UsedByNative
    public static int getVrCoreClientApiVersion(Context var0) throws VrCoreNotAvailableException {
        try {
            ApplicationInfo var1;
            if((var1 = var0.getPackageManager().getApplicationInfo("com.baofeng.mj", 128)).enabled) {
                return var1.metaData != null?var1.metaData.getInt("com.google.vr.vrcore.ClientApiVersion", 0):0;
            } else {
                throw new VrCoreNotAvailableException(2);
            }
        } catch (NameNotFoundException var2) {
            throw new VrCoreNotAvailableException(checkVrCoreAvailability(var0));
        }
    }

    public static int getVrCoreVersionCode(Context var0) throws VrCoreNotAvailableException {
        try {
            return var0.getPackageManager().getPackageInfo("com.baofeng.mj", 0).versionCode;
        } catch (NameNotFoundException var2) {
            Log.e(TAG, "Could not find VrCore package", var2);
            throw new VrCoreNotAvailableException(checkVrCoreAvailability(var0));
        }
    }

    public static String getVrCoreVersionName(Context var0) throws VrCoreNotAvailableException {
        try {
            return var0.getPackageManager().getPackageInfo("com.baofeng.mj", 0).versionName;
        } catch (NameNotFoundException var2) {
            Log.e(TAG, "Could not find VrCore package", var2);
            throw new VrCoreNotAvailableException(checkVrCoreAvailability(var0));
        }
    }

    public static String getVrCoreSdkLibraryVersion(Context var0) throws VrCoreNotAvailableException {
        ApplicationInfo var1;
        try {
            var1 = var0.getPackageManager().getApplicationInfo("com.baofeng.mj", 128);
        } catch (NameNotFoundException var3) {
            throw new VrCoreNotAvailableException(checkVrCoreAvailability(var0));
        }

        if(var1 == null) {
            throw new VrCoreNotAvailableException(8);
        } else if(!var1.enabled) {
            throw new VrCoreNotAvailableException(2);
        } else if(var1.metaData == null) {
            throw new VrCoreNotAvailableException(4);
        } else {
            String var2;
            if((var2 = var1.metaData.getString("com.google.vr.vrcore.SdkLibraryVersion", "")).isEmpty()) {
                throw new VrCoreNotAvailableException(4);
            } else {
                return var2.substring(1);
            }
        }
    }

    public static String getConnectionResultString(int var0) {
        switch(var0) {
            case 0:
                return "VR Service present";
            case 1:
                return "VR Service missing";
            case 2:
                return "VR Service disabled";
            case 3:
                return "VR Service updating";
            case 4:
                return "VR Service obsolete";
            case 5:
                return "VR Service not connected";
            case 6:
                return "No permission to do operation";
            case 7:
                return "This operation is not supported on this device";
            case 8:
                return "An unknown failure occurred";
            default:
                return (new StringBuilder(38)).append("Invalid connection result: ").append(var0).toString();
        }
    }

    public static boolean isVrCorePackage(String var0) {
        return "com.baofeng.mj".equalsIgnoreCase(var0);
    }

    public static boolean isVrCoreComponent(ComponentName var0) {
        return var0 != null && isVrCorePackage(var0.getPackageName());
    }

    private static int checkVrCoreAvailabilityImpl(Context var0) {
        if("com.baofeng.mj".equals(var0.getPackageName())) {
            return 0;
        } else {
            try {
                return !var0.getPackageManager().getApplicationInfo("com.baofeng.mj", 0).enabled?2:(!verifyRemotePackageSignature(var0)?9:0);
            } catch (NameNotFoundException var5) {
                if(VERSION.SDK_INT >= 21) {
                    Iterator var2 = var0.getPackageManager().getPackageInstaller().getAllSessions().iterator();

                    while(var2.hasNext()) {
                        SessionInfo var3 = (SessionInfo)var2.next();
                        if("com.baofeng.mj".equals(var3.getAppPackageName())) {
                            return 3;
                        }
                    }
                }

                PackageManager var1 = var0.getPackageManager();

                try {
                    if(var1.getApplicationInfo("com.baofeng.mj", 8192).enabled) {
                        return 3;
                    }
                } catch (NameNotFoundException var4) {
                    ;
                }

                return 1;
            }
        }
    }

    private static boolean verifyRemotePackageSignature(Context var0) throws NameNotFoundException {
        PackageInfo var1;
        return SignatureUtils.verifySignature(var1 = var0.getPackageManager().getPackageInfo("com.baofeng.mj", 64), new Signature[]{SignatureUtils.VRCORE_RELEASE_SIGNATURE})?true:(BuildUtils.isDebugBuild(var0)?SignatureUtils.verifySignature(var1, new Signature[]{SignatureUtils.VRCORE_DEBUG_SIGNATURE}):false);
    }

    public static class ConnectionResult {
        public static final int SUCCESS = 0;
        public static final int SERVICE_MISSING = 1;
        public static final int SERVICE_DISABLED = 2;
        public static final int SERVICE_UPDATING = 3;
        public static final int SERVICE_OBSOLETE = 4;
        public static final int SERVICE_NOT_CONNECTED = 5;
        public static final int NO_PERMISSION = 6;
        public static final int NOT_SUPPORTED = 7;
        public static final int UNKNOWN = 8;
        public static final int SERVICE_INVALID = 9;

        public ConnectionResult() {
        }
    }
}
