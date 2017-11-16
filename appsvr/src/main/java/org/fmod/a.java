//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package org.fmod;

import android.media.AudioRecord;
import android.util.Log;
import java.nio.ByteBuffer;
import org.fmod.FMODAudioDevice;

final class a implements Runnable {
    private final FMODAudioDevice a;
    private final ByteBuffer b;
    private final int c;
    private final int d;
    private final int e;
    private volatile Thread f;
    private volatile boolean g;
    private AudioRecord h;
    private boolean i;

    a(FMODAudioDevice var1, int var2, int var3) {
        this.a = var1;
        this.c = var2;
        this.d = var3;
        this.e = 2;
        this.b = ByteBuffer.allocateDirect(AudioRecord.getMinBufferSize(var2, var3, 2));
    }

    public final int a() {
        return this.b.capacity();
    }

    public final void b() {
        if(this.f != null) {
            this.c();
        }

        this.g = true;
        this.f = new Thread(this);
        this.f.start();
    }

    public final void c() {
        while(this.f != null) {
            this.g = false;

            try {
                this.f.join();
                this.f = null;
            } catch (InterruptedException var1) {
                ;
            }
        }

    }

    public final void run() {
        int var1 = 3;

        while(this.g) {
            if(!this.i && var1 > 0) {
                this.d();
                this.h = new AudioRecord(1, this.c, this.d, this.e, this.b.capacity());
                this.i = this.h.getState() == 1;
                if(this.i) {
                    var1 = 3;
                    this.b.position(0);
                    this.h.startRecording();
                } else {
                    Log.e("FMOD", "AudioRecord failed to initialize (status " + this.h.getState() + ")");
                    --var1;
                    this.d();
                }
            }

            if(this.i && this.h.getRecordingState() == 3) {
                int var2 = this.h.read(this.b, this.b.capacity());
                this.a.fmodProcessMicData(this.b, var2);
                this.b.position(0);
            }
        }

        this.d();
    }

    private void d() {
        if(this.h != null) {
            if(this.h.getState() == 1) {
                this.h.stop();
            }

            this.h.release();
            this.h = null;
        }

        this.b.position(0);
        this.i = false;
    }
}
