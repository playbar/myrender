//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.google.vr.cardboard;

import android.net.Uri;

public class VrSettingsProviderContract {
    public static final String VRCORE_AUTHORITY = "com.google.vr.vrcore.settings.mj";
    public static final String PROVIDER_INTENT_ACTION = "android.content.action.VR_SETTINGS_PROVIDER";
    public static final String DEVICE_PARAMS_SETTING = "device_params";
    public static final String PHONE_PARAMS_SETTING = "phone_params";
    public static final String DAYDREAM_SETUP_COMPLETED = "daydream_setup";
    public static final String SDK_CONFIGURATION_PARAMS_SETTING = "sdk_configuration_params";
    public static final String USER_PREFS_SETTING = "user_prefs";
    public static final String SETTING_VALUE_KEY = "value";

    public VrSettingsProviderContract() {
    }

    public static Uri createUri(String var0, String var1) {
        String var2 = String.valueOf("content://");
        return Uri.parse((new StringBuilder(1 + String.valueOf(var2).length() + String.valueOf(var0).length() + String.valueOf(var1).length())).append(var2).append(var0).append("/").append(var1).toString());
    }
}
