//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import android.os.Bundle;
import android.util.Log;
import com.unity3d.player.UnityPlayerActivity;

public class UnityPlayerNativeActivity extends UnityPlayerActivity {
    public UnityPlayerNativeActivity() {
    }

    protected void onCreate(Bundle var1) {
        Log.w("Unity", "UnityPlayerNativeActivity has been deprecated, please update your AndroidManifest to use UnityPlayerActivity instead");
        super.onCreate(var1);
    }
}
