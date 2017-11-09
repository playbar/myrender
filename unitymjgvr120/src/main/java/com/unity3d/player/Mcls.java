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
import android.widget.MediaController.MediaPlayerControl;
import java.io.FileInputStream;
import java.io.IOException;

public final class Mcls extends FrameLayout
        implements OnBufferingUpdateListener, OnCompletionListener, OnPreparedListener, OnVideoSizeChangedListener, Callback, MediaPlayerControl
{
    public static boolean a = false;
    private final Context b;
    private final SurfaceView c;
    private final SurfaceHolder d;
    private final String e;
    private final int f;
    private final int g;
    private final boolean h;
    private final long i;
    private final long j;
    private final FrameLayout k;
    private final Display l;
    private int mIm;
    private int n;
    private int o;
    private int p;
    private MediaPlayer q;
    private MediaController r;
    private boolean s = false;
    private boolean t = false;
    private int u = 0;
    private boolean v = false;
    private boolean w = false;
    private Mcls.a x;
    private Mcls.b y;
    private volatile int z = 0;

    private static void b(String var0) {
        Log.i("Video", "VideoPlayer: " + var0);
    }

    protected Mcls(Context var1, String var2, int var3, int var4, int var5, boolean var6, long var7, long var9, Mcls.a var11) {
        super(var1);
        this.x = var11;
        this.b = var1;
        this.k = this;
        this.c = new SurfaceView(var1);
        this.d = this.c.getHolder();
        this.d.addCallback(this);
        this.d.setType(3);
        this.k.setBackgroundColor(var3);
        this.k.addView(this.c);
        WindowManager var12 = (WindowManager)this.b.getSystemService("window");
        this.l = var12.getDefaultDisplay();
        this.e = var2;
        this.f = var4;
        this.g = var5;
        this.h = var6;
        this.i = var7;
        this.j = var9;
        if(a) {
            b("fileName: " + this.e);
        }

        if(a) {
            b("backgroundColor: " + var3);
        }

        if(a) {
            b("controlMode: " + this.f);
        }

        if(a) {
            b("scalingMode: " + this.g);
        }

        if(a) {
            b("isURL: " + this.h);
        }

        if(a) {
            b("videoOffset: " + this.i);
        }

        if(a) {
            b("videoLength: " + this.j);
        }

        this.setFocusable(true);
        this.setFocusableInTouchMode(true);
    }

    final boolean a() {
        return this.v;
    }

    private void a(int var1) {
        this.z = var1;
        if(this.x != null) {
            this.x.a(this.z);
        }

    }

    protected final void destroyPlayer() {
        if(a) {
            b("destroyPlayer");
        }

        if(!this.v) {
            this.pause();
        }

        this.doCleanUp();
    }

    private void c() {
        if(this.q != null) {
            this.q.setDisplay(this.d);
            if(!this.v) {
                if(a) {
                    b("Resuming playback");
                }

                this.q.start();
            }

        } else {
            this.a(0);
            this.doCleanUp();

            try {
                this.q = new MediaPlayer();
                if(this.h) {
                    this.q.setDataSource(this.b, Uri.parse(this.e));
                } else {
                    FileInputStream var1;
                    if(this.j != 0L) {
                        var1 = new FileInputStream(this.e);
                        this.q.setDataSource(var1.getFD(), this.i, this.j);
                        var1.close();
                    } else {
                        AssetManager var4 = this.getResources().getAssets();

                        try {
                            AssetFileDescriptor var5 = var4.openFd(this.e);
                            this.q.setDataSource(var5.getFileDescriptor(), var5.getStartOffset(), var5.getLength());
                            var5.close();
                        } catch (IOException var2) {
                            var1 = new FileInputStream(this.e);
                            this.q.setDataSource(var1.getFD());
                            var1.close();
                        }
                    }
                }

                this.q.setDisplay(this.d);
                this.q.setScreenOnWhilePlaying(true);
                this.q.setOnBufferingUpdateListener(this);
                this.q.setOnCompletionListener(this);
                this.q.setOnPreparedListener(this);
                this.q.setOnVideoSizeChangedListener(this);
                this.q.setAudioStreamType(3);
                this.q.prepareAsync();
                this.y = new Mcls.b(this);
                (new Thread(this.y)).start();
            } catch (Exception var3) {
                if(a) {
                    b("error: " + var3.getMessage() + var3);
                }

                this.a(2);
            }
        }
    }

    public final void CancelOnPrepare() {
        this.a(2);
    }

    public final boolean onKeyDown(int var1, KeyEvent var2) {
        if(var1 != 4 && (this.f != 2 || var1 == 0 || var2.isSystem())) {
            return this.r != null?this.r.onKeyDown(var1, var2):super.onKeyDown(var1, var2);
        } else {
            this.destroyPlayer();
            this.a(3);
            return true;
        }
    }

    public final boolean onTouchEvent(MotionEvent var1) {
        int var2 = var1.getAction() & 255;
        if(this.f == 2 && var2 == 0) {
            this.destroyPlayer();
            this.a(3);
            return true;
        } else {
            return this.r != null?this.r.onTouchEvent(var1):super.onTouchEvent(var1);
        }
    }

    public final void onBufferingUpdate(MediaPlayer var1, int var2) {
        if(a) {
            b("onBufferingUpdate percent:" + var2);
        }

        this.u = var2;
    }

    public final void onCompletion(MediaPlayer var1) {
        if(a) {
            b("onCompletion called");
        }

        this.destroyPlayer();
        this.a(3);
    }

    public final void onVideoSizeChanged(MediaPlayer var1, int var2, int var3) {
        if(a) {
            b("onVideoSizeChanged called " + var2 + "x" + var3);
        }

        if(var2 != 0 && var3 != 0) {
            this.s = true;
            this.o = var2;
            this.p = var3;
            if(this.t && this.s) {
                this.d();
            }

        } else {
            if(a) {
                b("invalid video width(" + var2 + ") or height(" + var3 + ")");
            }

        }
    }

    public final void onPrepared(MediaPlayer var1) {
        if(a) {
            b("onPrepared called");
        }

        if(this.y != null) {
            this.y.a();
            this.y = null;
        }

        if(this.f == 0 || this.f == 1) {
            this.r = new MediaController(this.b);
            this.r.setMediaPlayer(this);
            this.r.setAnchorView(this);
            this.r.setEnabled(true);
            this.r.show();
        }

        this.t = true;
        if(this.t && this.s) {
            this.d();
        }

    }

    public final void surfaceChanged(SurfaceHolder var1, int var2, int var3, int var4) {
        if(a) {
            b("surfaceChanged called " + var2 + " " + var3 + "x" + var4);
        }

        if(this.mIm != var3 || this.n != var4) {
            this.mIm = var3;
            this.n = var4;
            if(this.w) {
                this.updateVideoLayout();
            }
        }

    }

    public final void surfaceDestroyed(SurfaceHolder var1) {
        if(a) {
            b("surfaceDestroyed called");
        }

        this.w = false;
    }

    public final void surfaceCreated(SurfaceHolder var1) {
        if(a) {
            b("surfaceCreated called");
        }

        this.w = true;
        this.c();
    }

    protected final void doCleanUp() {
        if(this.y != null) {
            this.y.a();
            this.y = null;
        }

        if(this.q != null) {
            this.q.release();
            this.q = null;
        }

        this.o = 0;
        this.p = 0;
        this.t = false;
        this.s = false;
    }

    private void d() {
        if(!this.isPlaying()) {
            this.a(1);
            if(a) {
                b("startVideoPlayback");
            }

            this.updateVideoLayout();
            if(!this.v) {
                this.start();
            }

        }
    }

    protected final void updateVideoLayout() {
        if(a) {
            b("updateVideoLayout");
        }

        if(this.q != null) {
            if(this.mIm == 0 || this.n == 0) {
                WindowManager var1 = (WindowManager)this.b.getSystemService("window");
                this.mIm = var1.getDefaultDisplay().getWidth();
                this.n = var1.getDefaultDisplay().getHeight();
            }

            int var5 = this.mIm;
            int var2 = this.n;
            float var3 = (float)this.o / (float)this.p;
            float var4 = (float)this.mIm / (float)this.n;
            if(this.g == 1) {
                if(var4 <= var3) {
                    var2 = (int)((float)this.mIm / var3);
                } else {
                    var5 = (int)((float)this.n * var3);
                }
            } else if(this.g == 2) {
                if(var4 >= var3) {
                    var2 = (int)((float)this.mIm / var3);
                } else {
                    var5 = (int)((float)this.n * var3);
                }
            } else if(this.g == 0) {
                var5 = this.o;
                var2 = this.p;
            }

            if(a) {
                b("frameWidth = " + var5 + "; frameHeight = " + var2);
            }

            LayoutParams var6 = new LayoutParams(var5, var2, 17);
            this.k.updateViewLayout(this.c, var6);
        }
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
        return this.h?this.u:100;
    }

    public final int getCurrentPosition() {
        return this.q == null?0:this.q.getCurrentPosition();
    }

    public final int getDuration() {
        return this.q == null?0:this.q.getDuration();
    }

    public final boolean isPlaying() {
        boolean var1 = this.t && this.s;
        return this.q == null?!var1:this.q.isPlaying() || !var1;
    }

    public final void pause() {
        if(this.q != null) {
            if(this.w) {
                this.q.pause();
            }

            this.v = true;
        }
    }

    public final void seekTo(int var1) {
        if(this.q != null) {
            this.q.seekTo(var1);
        }
    }

    public final void start() {
        if(a) {
            b("Start");
        }

        if(this.q != null) {
            if(this.w) {
                this.q.start();
            }

            this.v = false;
        }
    }

    public class b implements Runnable {
        private Mcls b;
        private boolean c;

        public b(Mcls var2) {
            this.b = var2;
            this.c = false;
        }

        public final void run() {
            try {
                Thread.sleep(5000L);
            } catch (InterruptedException var1) {
                Thread.currentThread().interrupt();
            }

            if(!this.c) {
                if(Mcls.a) {
                    Mcls.b("Stopping the video player due to timeout.");
                }

                this.b.CancelOnPrepare();
            }

        }

        public final void a() {
            this.c = true;
        }
    }

    public interface a {
        void a(int var1);
    }
}
