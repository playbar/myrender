//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import android.app.Activity;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.MotionEvent;
import com.unity3d.player.UnityPlayer;

public class UnityPlayerActivity extends Activity {
    protected UnityPlayer mUnityPlayer;

    public UnityPlayerActivity() {
    }

    protected void onCreate(Bundle var1) {
        this.requestWindowFeature(1);
        super.onCreate(var1);
        this.getWindow().setFormat(2);
        this.mUnityPlayer = new UnityPlayer(this);
        this.setContentView(this.mUnityPlayer);
        this.mUnityPlayer.requestFocus();
    }

    protected void onNewIntent(Intent var1) {
        this.setIntent(var1);
    }

    protected void onDestroy() {
        this.mUnityPlayer.quit();
        super.onDestroy();
    }

    protected void onPause() {
        super.onPause();
        this.mUnityPlayer.pause();
    }

    protected void onResume() {
        super.onResume();
        this.mUnityPlayer.resume();
    }

    public void onConfigurationChanged(Configuration var1) {
        super.onConfigurationChanged(var1);
        this.mUnityPlayer.configurationChanged(var1);
    }

    public void onWindowFocusChanged(boolean var1) {
        super.onWindowFocusChanged(var1);
        this.mUnityPlayer.windowFocusChanged(var1);
    }

    public boolean dispatchKeyEvent(KeyEvent var1) {
        return var1.getAction() == 2?this.mUnityPlayer.injectEvent(var1):super.dispatchKeyEvent(var1);
    }

    public boolean onKeyUp(int var1, KeyEvent var2) {
        return this.mUnityPlayer.injectEvent(var2);
    }

    public boolean onKeyDown(int var1, KeyEvent var2) {
        return this.mUnityPlayer.injectEvent(var2);
    }

    public boolean onTouchEvent(MotionEvent var1) {
        return this.mUnityPlayer.injectEvent(var1);
    }

    public boolean onGenericMotionEvent(MotionEvent var1) {
        return this.mUnityPlayer.injectEvent(var1);
    }
}
