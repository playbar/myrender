//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnBufferingUpdateListener;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnPreparedListener;
import android.media.MediaPlayer.OnVideoSizeChangedListener;
import android.net.Uri;
import android.util.Log;
import android.view.Display;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.WindowManager;
import android.view.SurfaceHolder.Callback;
import android.widget.FrameLayout;
import android.widget.MediaController;
import android.widget.FrameLayout.LayoutParams;
import android.widget.MediaController.MediaPlayerControl;
import com.unity3d.player.UnityPlayer;
import java.io.FileInputStream;
import java.io.IOException;

public final class m extends FrameLayout implements OnBufferingUpdateListener, OnCompletionListener, OnPreparedListener, OnVideoSizeChangedListener, Callback, MediaPlayerControl {
    private static boolean a = false;
    private final UnityPlayer b;
    private final Context c;
    private final SurfaceView d;
    private final SurfaceHolder e;
    private final String f;
    private final int g;
    private final int h;
    private final boolean i;
    private final long j;
    private final long k;
    private final FrameLayout l;
    private final Display m;
    private int n;
    private int o;
    private int p;
    private int q;
    private MediaPlayer r;
    private MediaController s;
    private boolean t = false;
    private boolean u = false;
    private int v = 0;
    private boolean w = false;
    private int x = 0;
    private boolean y;
    private m.a z;
    private volatile int A = 0;

    private static void a(String var0) {
        Log.v("Video", "VideoPlayer: " + var0);
    }

    protected m(UnityPlayer var1, Context var2, String var3, int var4, int var5, int var6, boolean var7, long var8, long var10, m.a var12) {
        super(var2);
        this.z = var12;
        this.b = var1;
        this.c = var2;
        this.l = this;
        this.d = new SurfaceView(var2);
        this.e = this.d.getHolder();
        this.e.addCallback(this);
        this.e.setType(3);
        this.l.setBackgroundColor(var4);
        this.l.addView(this.d);
        WindowManager var13 = (WindowManager)this.c.getSystemService("window");
        this.m = var13.getDefaultDisplay();
        this.f = var3;
        this.g = var5;
        this.h = var6;
        this.i = var7;
        this.j = var8;
        this.k = var10;
        if(a) {
            a("fileName: " + this.f);
        }

        if(a) {
            a("backgroundColor: " + var4);
        }

        if(a) {
            a("controlMode: " + this.g);
        }

        if(a) {
            a("scalingMode: " + this.h);
        }

        if(a) {
            a("isURL: " + this.i);
        }

        if(a) {
            a("videoOffset: " + this.j);
        }

        if(a) {
            a("videoLength: " + this.k);
        }

        this.setFocusable(true);
        this.setFocusableInTouchMode(true);
        this.y = true;
    }

    public final void onControllerHide() {
    }

    protected final void onPause() {
        if(a) {
            a("onPause called");
        }

        if(!this.w) {
            this.pause();
            this.w = false;
        }

        if(this.r != null) {
            this.x = this.r.getCurrentPosition();
        }

        this.y = false;
    }

    protected final void onResume() {
        if(a) {
            a("onResume called");
        }

        if(!this.y && !this.w) {
            this.start();
        }

        this.y = true;
    }

    protected final void onDestroy() {
        this.onPause();
        this.doCleanUp();
        UnityPlayer.a(new Runnable() {
            public final void run() {
                m.this.b.hideVideoPlayer();
            }
        });
    }

    private void a() {
        this.A = 0;
        if(this.z != null) {
            this.z.a(this.A);
        }

        this.doCleanUp();

        try {
            this.r = new MediaPlayer();
            if(this.i) {
                this.r.setDataSource(this.c, Uri.parse(this.f));
            } else {
                FileInputStream var1;
                if(this.k != 0L) {
                    var1 = new FileInputStream(this.f);
                    this.r.setDataSource(var1.getFD(), this.j, this.k);
                    var1.close();
                } else {
                    AssetManager var4 = this.getResources().getAssets();

                    try {
                        AssetFileDescriptor var5 = var4.openFd(this.f);
                        this.r.setDataSource(var5.getFileDescriptor(), var5.getStartOffset(), var5.getLength());
                        var5.close();
                    } catch (IOException var2) {
                        var1 = new FileInputStream(this.f);
                        this.r.setDataSource(var1.getFD());
                        var1.close();
                    }
                }
            }

            this.r.setDisplay(this.e);
            this.r.setScreenOnWhilePlaying(true);
            this.r.setOnBufferingUpdateListener(this);
            this.r.setOnCompletionListener(this);
            this.r.setOnPreparedListener(this);
            this.r.setOnVideoSizeChangedListener(this);
            this.r.setAudioStreamType(3);
            this.r.prepare();
            if(this.g == 0 || this.g == 1) {
                this.s = new MediaController(this.c);
                this.s.setMediaPlayer(this);
                this.s.setAnchorView(this);
                this.s.setEnabled(true);
                this.s.show();
            }

        } catch (Exception var3) {
            if(a) {
                a("error: " + var3.getMessage() + var3);
            }

            this.onDestroy();
            this.A = 2;
            if(this.z != null) {
                this.z.a(this.A);
            }

        }
    }

    public final boolean onKeyDown(int var1, KeyEvent var2) {
        if(var1 != 4 && (this.g != 2 || var1 == 0 || var2.isSystem())) {
            return this.s != null?this.s.onKeyDown(var1, var2):super.onKeyDown(var1, var2);
        } else {
            this.onDestroy();
            return true;
        }
    }

    public final boolean onTouchEvent(MotionEvent var1) {
        int var2 = var1.getAction() & 255;
        if(this.g == 2 && var2 == 0) {
            this.onDestroy();
            return true;
        } else {
            return this.s != null?this.s.onTouchEvent(var1):super.onTouchEvent(var1);
        }
    }

    public final void onBufferingUpdate(MediaPlayer var1, int var2) {
        if(a) {
            a("onBufferingUpdate percent:" + var2);
        }

        this.v = var2;
    }

    public final void onCompletion(MediaPlayer var1) {
        if(a) {
            a("onCompletion called");
        }

        this.onDestroy();
    }

    public final void onVideoSizeChanged(MediaPlayer var1, int var2, int var3) {
        if(a) {
            a("onVideoSizeChanged called " + var2 + "x" + var3);
        }

        if(var2 != 0 && var3 != 0) {
            this.t = true;
            this.p = var2;
            this.q = var3;
            if(this.u && this.t) {
                this.b();
            }

        } else {
            if(a) {
                a("invalid video width(" + var2 + ") or height(" + var3 + ")");
            }

        }
    }

    public final void onPrepared(MediaPlayer var1) {
        if(a) {
            a("onPrepared called");
        }

        this.u = true;
        if(this.u && this.t) {
            this.b();
        }

    }

    public final void surfaceChanged(SurfaceHolder var1, int var2, int var3, int var4) {
        if(a) {
            a("surfaceChanged called " + var2 + " " + var3 + "x" + var4);
        }

        if(this.n != var3 || this.o != var4) {
            this.n = var3;
            this.o = var4;
            this.updateVideoLayout();
        }

    }

    public final void surfaceDestroyed(SurfaceHolder var1) {
        if(a) {
            a("surfaceDestroyed called");
        }

        this.doCleanUp();
    }

    public final void surfaceCreated(SurfaceHolder var1) {
        if(a) {
            a("surfaceCreated called");
        }

        this.a();
        this.seekTo(this.x);
    }

    protected final void doCleanUp() {
        if(this.r != null) {
            this.r.release();
            this.r = null;
        }

        this.p = 0;
        this.q = 0;
        this.u = false;
        this.t = false;
    }

    private void b() {
        if(!this.isPlaying()) {
            this.A = 1;
            if(this.z != null) {
                this.z.a(this.A);
            }

            if(a) {
                a("startVideoPlayback");
            }

            this.updateVideoLayout();
            if(!this.w) {
                this.start();
            }

        }
    }

    protected final void updateVideoLayout() {
        if(a) {
            a("updateVideoLayout");
        }

        if(this.n == 0 || this.o == 0) {
            WindowManager var1 = (WindowManager)this.c.getSystemService("window");
            this.n = var1.getDefaultDisplay().getWidth();
            this.o = var1.getDefaultDisplay().getHeight();
        }

        int var5 = this.n;
        int var2 = this.o;
        float var3 = (float)this.p / (float)this.q;
        float var4 = (float)this.n / (float)this.o;
        if(this.h == 1) {
            if(var4 <= var3) {
                var2 = (int)((float)this.n / var3);
            } else {
                var5 = (int)((float)this.o * var3);
            }
        } else if(this.h == 2) {
            if(var4 >= var3) {
                var2 = (int)((float)this.n / var3);
            } else {
                var5 = (int)((float)this.o * var3);
            }
        } else if(this.h == 0) {
            var5 = this.p;
            var2 = this.q;
        }

        if(a) {
            a("frameWidth = " + var5 + "; frameHeight = " + var2);
        }

        LayoutParams var6 = new LayoutParams(var5, var2, 17);
        this.l.updateViewLayout(this.d, var6);
    }

    public final boolean canPause() {
        return true;
    }

    public final boolean canSeekBackward() {
        return true;
    }

    public final boolean canSeekForward() {
        return true;
    }

    public int  getAudioSessionId() { return 0;}

    public final int getBufferPercentage() {
        return this.i?this.v:100;
    }

    public final int getCurrentPosition() {
        return this.r == null?0:this.r.getCurrentPosition();
    }

    public final int getDuration() {
        return this.r == null?0:this.r.getDuration();
    }

    public final boolean isPlaying() {
        boolean var1 = this.u && this.t;
        return this.r == null?!var1:this.r.isPlaying() || !var1;
    }

    public final void pause() {
        if(this.r != null) {
            this.r.pause();
            this.w = true;
        }
    }

    public final void seekTo(int var1) {
        if(this.r != null) {
            this.r.seekTo(var1);
        }
    }

    public final void start() {
        if(this.r != null) {
            this.r.start();
            this.w = false;
        }
    }

    public interface a {
        void a(int var1);
    }
}
