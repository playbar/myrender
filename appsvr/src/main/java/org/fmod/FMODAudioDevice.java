//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package org.fmod;

import android.media.AudioTrack;
import android.util.Log;
import java.nio.ByteBuffer;
import org.fmod.a;

public class FMODAudioDevice implements Runnable {
    private volatile Thread a = null;
    private volatile boolean b = false;
    private AudioTrack c = null;
    private boolean d = false;
    private ByteBuffer e = null;
    private byte[] f = null;
    private volatile a g;
    private static int h = 0;
    private static int i = 1;
    private static int j = 2;
    private static int k = 3;

    public FMODAudioDevice() {
    }

    public synchronized void start() {
        if(this.a != null) {
            this.stop();
        }

        this.a = new Thread(this, "FMODAudioDevice");
        this.a.setPriority(10);
        this.b = true;
        this.a.start();
        if(this.g != null) {
            this.g.b();
        }

    }

    public synchronized void stop() {
        while(this.a != null) {
            this.b = false;

            try {
                this.a.join();
                this.a = null;
            } catch (InterruptedException var1) {
                ;
            }
        }

        if(this.g != null) {
            this.g.c();
        }

    }

    public synchronized void close() {
        this.stop();
    }

    public boolean isRunning() {
        return this.a != null && this.a.isAlive();
    }

    public void run() {
        int var1 = 3;

        while(this.b) {
            if(!this.d && var1 > 0) {
                this.releaseAudioTrack();
                int var2;
                int var3 = Math.round((float)AudioTrack.getMinBufferSize(var2 = this.fmodGetInfo(h), 3, 2) * 1.1F) & -4;
                int var4 = this.fmodGetInfo(i);
                int var5 = this.fmodGetInfo(j);
                if(var4 * var5 * 4 > var3) {
                    var3 = var4 * var5 * 4;
                }

                this.c = new AudioTrack(3, var2, 3, 2, var3, 1);
                this.d = this.c.getState() == 1;
                if(this.d) {
                    var1 = 3;
                    this.e = ByteBuffer.allocateDirect(var4 * 2 * 2);
                    this.f = new byte[this.e.capacity()];
                    this.c.play();
                } else {
                    Log.e("FMOD", "AudioTrack failed to initialize (status " + this.c.getState() + ")");
                    this.releaseAudioTrack();
                    --var1;
                }
            }

            if(this.d) {
                if(this.fmodGetInfo(k) == 1) {
                    this.fmodProcess(this.e);
                    this.e.get(this.f, 0, this.e.capacity());
                    this.c.write(this.f, 0, this.e.capacity());
                    this.e.position(0);
                } else {
                    this.releaseAudioTrack();
                }
            }
        }

        this.releaseAudioTrack();
    }

    private void releaseAudioTrack() {
        if(this.c != null) {
            if(this.c.getState() == 1) {
                this.c.stop();
            }

            this.c.release();
            this.c = null;
        }

        this.e = null;
        this.f = null;
        this.d = false;
    }

    private native int fmodGetInfo(int var1);

    private native int fmodProcess(ByteBuffer var1);

    native int fmodProcessMicData(ByteBuffer var1, int var2);

    public synchronized int startAudioRecord(int var1, int var2, int var3) {
        if(this.g == null) {
            this.g = new a(this, var1, var2);
            this.g.b();
        }

        return this.g.a();
    }

    public synchronized void stopAudioRecord() {
        if(this.g != null) {
            this.g.c();
            this.g = null;
        }

    }
}
