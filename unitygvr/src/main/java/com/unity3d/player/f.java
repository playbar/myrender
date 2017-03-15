//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.unity3d.player;

import android.app.Presentation;
import android.content.Context;
import android.hardware.display.DisplayManager;
import android.hardware.display.DisplayManager.DisplayListener;
import android.os.Bundle;
import android.os.Handler;
import android.view.Display;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.SurfaceHolder.Callback;
import com.unity3d.player.UnityPlayer;
import com.unity3d.player.d;

public final class f implements d {
    private Object a = new Object[0];
    private Presentation b;
    private DisplayListener c;

    public f() {
    }

    public final void a(final UnityPlayer var1, Context var2) {
        DisplayManager var3;
        if((var3 = (DisplayManager)var2.getSystemService("display")) != null) {
            var3.registerDisplayListener(new DisplayListener() {
                public final void onDisplayAdded(int var1x) {
                    var1.displayChanged(-1, (Surface)null);
                }

                public final void onDisplayRemoved(int var1x) {
                    var1.displayChanged(-1, (Surface)null);
                }

                public final void onDisplayChanged(int var1x) {
                    var1.displayChanged(-1, (Surface)null);
                }
            }, (Handler)null);
        }
    }

    public final void a(Context var1) {
        if(this.c != null) {
            DisplayManager var2;
            if((var2 = (DisplayManager)var1.getSystemService("display")) != null) {
                var2.unregisterDisplayListener(this.c);
            }
        }
    }

    public final boolean a(final UnityPlayer var1, final Context var2, int var3) {
        Object var4 = this.a;
        synchronized(this.a) {
            Display var5;
            if(this.b != null && this.b.isShowing() && (var5 = this.b.getDisplay()) != null && var5.getDisplayId() == var3) {
                return true;
            } else {
                DisplayManager var8;
                if((var8 = (DisplayManager)var2.getSystemService("display")) == null) {
                    return false;
                } else {
                    final Display var7;
                    if((var7 = var8.getDisplay(var3)) == null) {
                        return false;
                    } else {
                        var1.b(new Runnable() {
                            public final void run() {
                                synchronized(f.this.a) {
                                    if(f.this.b != null) {
                                        f.this.b.dismiss();
                                    }

                                    f.this.b = new Presentation(var2, var7) {
                                        protected final void onCreate(Bundle var1x) {
                                            SurfaceView var2x;
                                            (var2x = new SurfaceView(var2)).getHolder().addCallback(new Callback() {
                                                public final void surfaceCreated(SurfaceHolder var1x) {
                                                    var1.displayChanged(1, var1x.getSurface());
                                                }

                                                public final void surfaceChanged(SurfaceHolder var1x, int var2x, int var3, int var4) {
                                                    var1.displayChanged(1, var1x.getSurface());
                                                }

                                                public final void surfaceDestroyed(SurfaceHolder var1x) {
                                                    var1.displayChanged(1, (Surface)null);
                                                }
                                            });
                                            this.setContentView(var2x);
                                        }

                                        public final void onDisplayRemoved() {
                                            this.dismiss();
                                            synchronized(f.this.a) {
                                                f.this.b = null;
                                            }
                                        }
                                    };
                                    f.this.b.show();
                                }
                            }
                        });
                        return true;
                    }
                }
            }
        }
    }
}
