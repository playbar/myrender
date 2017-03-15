//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import com.unity3d.player.UnityPlayerActivity;

public class UnityPlayerProxyActivity extends Activity {
    public UnityPlayerProxyActivity() {
    }

    protected void onCreate(Bundle var1) {
        Log.w("Unity", "UnityPlayerNativeActivity has been deprecated, please update your AndroidManifest to use UnityPlayerActivity instead");
        super.onCreate(var1);
        Intent var3;
        (var3 = new Intent(this, UnityPlayerActivity.class)).addFlags(65536);
        Bundle var2;
        if((var2 = this.getIntent().getExtras()) != null) {
            var3.putExtras(var2);
        }

        this.startActivity(var3);
    }
}
