//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.content.pm.ActivityInfo;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageItemInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.os.Bundle;
import java.util.LinkedList;

import static com.unity3d.player.UnityPlayer.Eb.e;

public final class Fcls implements Ccls {
    public Fcls() {
    }

    private static boolean a(PackageItemInfo var0) {
        try {
            return var0.metaData.getBoolean("unityplayer.SkipPermissionsDialog");
        } catch (Exception var1) {
            return false;
        }
    }

    public final void a(Activity var1, final Runnable var2) {
        if(var1 != null) {
            PackageManager var3 = var1.getPackageManager();

            try {
                ActivityInfo var4 = var3.getActivityInfo(var1.getComponentName(), 128);
                ApplicationInfo var5 = var3.getApplicationInfo(var1.getPackageName(), 128);
                if(a(var4) || a(var5)) {
                    var2.run();
                    return;
                }
            } catch (Exception var11) {
                ;
            }

            try {
                PackageInfo var12;
                if((var12 = var3.getPackageInfo(var1.getPackageName(), 4096)).requestedPermissions == null) {
                    var12.requestedPermissions = new String[0];
                }

                final LinkedList var15 = new LinkedList();
                String[] var13;
                int var6 = (var13 = var12.requestedPermissions).length;

                for(int var7 = 0; var7 < var6; ++var7) {
                    String var8 = var13[var7];

                    try {
                        if(var3.getPermissionInfo(var8, 128).protectionLevel == 1 && var1.checkCallingOrSelfPermission(var8) != 0) {
                            var15.add(var8);
                        }
                    } catch (NameNotFoundException var9) {
                        Ecls.Log(5, "Failed to get permission info for " + var8 + ", manifest likely missing custom permission declaration");
                        Ecls.Log(5, "Permission " + var8 + " ignored");
                    }
                }

                if(var15.isEmpty()) {
                    var2.run();
                } else {
                    final FragmentManager var14 = var1.getFragmentManager();
                    Fragment var16 = new Fragment() {
                        public final void onCreate(Bundle var1) {
                            super.onCreate(var1);
                            this.requestPermissions((String[])var15.toArray(new String[0]), 15881);
                        }

                        public final void onRequestPermissionsResult(int var1, String[] var2x, int[] var3) {
                            if(var1 == 15881) {
                                for(var1 = 0; var1 < var2x.length && var1 < var3.length; ++var1) {
                                    Ecls.Log(4, var2x[var1] + (var3[var1] == 0?" granted":" denied"));
                                }

                                FragmentTransaction var4;
                                (var4 = var14.beginTransaction()).remove(this);
                                var4.commit();
                                var2.run();
                            }
                        }
                    };
                    FragmentTransaction var17;
                    (var17 = var14.beginTransaction()).add(0, var16);
                    var17.commit();
                }
            } catch (Exception var10) {
                Ecls.Log(6, "Unable to query for permission: " + var10.getMessage());
            }
        }
    }
}
