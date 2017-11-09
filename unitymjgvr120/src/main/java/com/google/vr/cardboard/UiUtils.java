//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.google.vr.cardboard;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.DialogInterface.OnCancelListener;
import android.content.DialogInterface.OnClickListener;
import android.content.pm.PackageManager;
import android.content.pm.ResolveInfo;
import android.net.Uri;
import android.widget.Toast;
import com.google.vr.cardboard.ContextUtils;
import com.google.vr.cardboard.PackageUtils;
import com.google.vr.cardboard.StoragePermissionUtils;
import com.google.vr.cardboard.VrParamsProviderFactory;
import com.google.vr.cardboard.R.string;
import com.google.vr.cardboard.R.style;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class UiUtils {
    private static final String CARDBOARD_WEBSITE = "https://google.com/cardboard/cfg";
    private static final String CARDBOARD_CONFIGURE_ACTION = "com.google.vrtoolkit.cardboard.CONFIGURE";
    private static final String DAYDREAM_HELP_CENTER_LINK = "https://support.google.com/daydream?p=daydream_help_menu";
    public static StoragePermissionUtils permissionUtils = new StoragePermissionUtils();
    public static Builder dialogBuilderForTesting;

    public UiUtils() {
    }

    public static void launchOrInstallCardboard(Context var0) {
        PackageManager var1 = var0.getPackageManager();
        Intent var2;
        (var2 = new Intent()).setAction("com.google.vrtoolkit.cardboard.CONFIGURE");
        List var3 = var1.queryIntentActivities(var2, 0);
        ArrayList var4 = new ArrayList();
        Integer var5 = null;
        Iterator var6 = var3.iterator();

        while(true) {
            ResolveInfo var7;
            String var8;
            int var9;
            do {
                do {
                    if(!var6.hasNext()) {
                        if(!VrParamsProviderFactory.isContentProviderAvailable(var0)) {
                            permissionUtils.requestStoragePermission(var0);
                        }

                        if(var4.isEmpty()) {
                            showInstallDialog(var0);
                            return;
                        }

                        Intent var11 = var4.size() == 1?(Intent)var4.get(0):var2;
                        var0.startActivity(var11);
                        return;
                    }
                } while(!PackageUtils.isGooglePackage(var8 = (var7 = (ResolveInfo)var6.next()).activityInfo.packageName));

                var9 = var7.priority;
                if(PackageUtils.isSystemPackage(var0, var8)) {
                    ++var9;
                }

                if(var5 == null) {
                    var5 = Integer.valueOf(var9);
                    break;
                }

                if(var9 > var5.intValue()) {
                    var5 = Integer.valueOf(var9);
                    var4.clear();
                    break;
                }
            } while(var9 < var5.intValue());

            Intent var10;
            (var10 = new Intent(var2)).setClassName(var8, var7.activityInfo.name);
            var4.add(var10);
        }
    }

    public static AlertDialog showDaydreamHelpCenterDialog(final Context var0, int var1, int var2, final Runnable var3) {
        OnClickListener var4 = new OnClickListener() {
            public final void onClick(DialogInterface var1, int var2) {
                Intent var3 = new Intent("android.intent.action.VIEW", Uri.parse("https://support.google.com/daydream?p=daydream_help_menu"));

                try {
                    var0.startActivity(var3);
                } catch (ActivityNotFoundException var4) {
                    Toast.makeText(var0, string.no_browser_text, 1).show();
                    var1.cancel();
                }
            }
        };
        Builder var5;
        (var5 = createAlertDialogBuilder(var0)).setTitle(var1).setMessage(var2).setCancelable(false).setPositiveButton(string.dialog_button_open_help_center, var4).setNegativeButton(string.dialog_button_got_it, new OnClickListener() {
            public final void onClick(DialogInterface var1, int var2) {
                var1.cancel();
            }
        });
        if(var3 != null) {
            var5.setOnCancelListener(new OnCancelListener() {
                public final void onCancel(DialogInterface var1) {
                    var3.run();
                }
            });
        }

        AlertDialog var6;
        (var6 = var5.create()).setCanceledOnTouchOutside(false);
        return showImmersiveDialog(var0, var6);
    }

    private static void showInstallDialog(final Context var0) {
        OnClickListener var1 = new OnClickListener() {
            public final void onClick(DialogInterface var1, int var2) {
                try {
                    var0.startActivity(new Intent("android.intent.action.VIEW", Uri.parse("https://google.com/cardboard/cfg")));
                } catch (ActivityNotFoundException var3) {
                    Toast.makeText(var0, string.no_browser_text, Toast.LENGTH_LONG).show();
                }
            }
        };
        Builder var2;
        (var2 = createAlertDialogBuilder(var0)).setTitle(string.dialog_title).setMessage(string.dialog_message_no_cardboard).setPositiveButton(string.go_to_playstore_button, var1).setNegativeButton(string.cancel_button, (OnClickListener)null);
        showImmersiveDialog(var0, var2.create());
    }

    private static AlertDialog showImmersiveDialog(Context var0, AlertDialog var1) {
        var1.getWindow().setFlags(8, 8);
        var1.show();
        Activity var2;
        if((var2 = ContextUtils.getActivity(var0)) != null) {
            var1.getWindow().getDecorView().setSystemUiVisibility(var2.getWindow().getDecorView().getSystemUiVisibility());
        }

        var1.getWindow().clearFlags(8);
        return var1;
    }

    private static Builder createAlertDialogBuilder(Context var0) {
        return dialogBuilderForTesting != null?dialogBuilderForTesting:new Builder(var0, style.GvrDialogTheme);
    }
}
