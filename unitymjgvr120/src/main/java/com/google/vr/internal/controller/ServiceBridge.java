//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.google.vr.internal.controller;

import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.Parcel;
import android.os.RemoteException;
import android.util.Log;
import com.google.vr.cardboard.annotations.UsedByNative;
import com.google.vr.vrcore.base.api.VrCoreNotAvailableException;
import com.google.vr.vrcore.base.api.VrCoreUtils;
import com.google.vr.vrcore.controller.api.ControllerAccelEvent;
import com.google.vr.vrcore.controller.api.ControllerButtonEvent;
import com.google.vr.vrcore.controller.api.ControllerEventPacket;
import com.google.vr.vrcore.controller.api.ControllerEventPacket2;
import com.google.vr.vrcore.controller.api.ControllerGyroEvent;
import com.google.vr.vrcore.controller.api.ControllerInitResults;
import com.google.vr.vrcore.controller.api.ControllerListenerOptions;
import com.google.vr.vrcore.controller.api.ControllerOrientationEvent;
import com.google.vr.vrcore.controller.api.ControllerTouchEvent;
import com.google.vr.vrcore.controller.api.IControllerListener;
import com.google.vr.vrcore.controller.api.IControllerService;
import com.google.vr.vrcore.controller.api.IControllerListener.Stub;

@UsedByNative
public class ServiceBridge implements ServiceConnection {
    private static final String TAG = ServiceBridge.class.getSimpleName();
    private static final boolean DEBUG = false;
    static final int TARGET_SERVICE_API_VERSION = 11;
    static final String LISTENER_KEY = "com.google.vr.internal.controller.LISTENER_KEY";
    private static final int MIN_SERVICE_API_FOR_RECENTERING = 8;
    public static final int FLAG_SUPPORTS_RECENTER = 1;
    private final Context context;
    private final ControllerListenerOptions options = new ControllerListenerOptions();
    private final Handler mainThreadHandler;
    private IControllerService service;
    private final ServiceBridge.Callbacks callbacks;
    private boolean isBound;
    private final Runnable bindRunnable = new Runnable() {
        public void run() {
            ServiceBridge.this.doBind();
        }
    };
    private final Runnable unbindRunnable = new Runnable() {
        public void run() {
            ServiceBridge.this.doUnbind();
        }
    };
    private final IControllerListener controllerListener = new Stub() {
        public int getApiVersion() throws RemoteException {
            return 11;
        }

        public ControllerListenerOptions getOptions() throws RemoteException {
            return ServiceBridge.this.options;
        }

        public void onControllerStateChanged(int var1, int var2) throws RemoteException {
            ServiceBridge.this.callbacks.onControllerStateChanged(var1, var2);
        }

        public void onControllerEventPacket(ControllerEventPacket var1) throws RemoteException {
            ServiceBridge.this.callbacks.onControllerEventPacket(var1);
            var1.recycle();
        }

        public void onControllerEventPacket2(ControllerEventPacket2 var1) throws RemoteException {
            ServiceBridge.this.callbacks.onControllerEventPacket2(var1);
            var1.recycle();
        }

        public void onControllerRecentered(ControllerOrientationEvent var1) {
            ServiceBridge.this.callbacks.onControllerRecentered(var1);
        }

        public boolean deprecatedOnControllerButtonEventV1(ControllerButtonEvent var1) {
            return true;
        }

        public void deprecatedOnControllerAccelEvent(ControllerAccelEvent var1) {
            ControllerEventPacket var2 = ControllerEventPacket.obtain();
            Parcel var3 = Parcel.obtain();
            var1.writeToParcel(var3, 0);
            var3.setDataPosition(0);
            var2.addAccelEvent().readFromParcel(var3);
            ServiceBridge.this.callbacks.onControllerEventPacket(var2);
            var2.recycle();
            var3.recycle();
        }

        public void deprecatedOnControllerButtonEvent(ControllerButtonEvent var1) {
            ControllerEventPacket var2 = ControllerEventPacket.obtain();
            Parcel var3 = Parcel.obtain();
            var1.writeToParcel(var3, 0);
            var3.setDataPosition(0);
            var2.addButtonEvent().readFromParcel(var3);
            ServiceBridge.this.callbacks.onControllerEventPacket(var2);
            var2.recycle();
            var3.recycle();
        }

        public void deprecatedOnControllerGyroEvent(ControllerGyroEvent var1) {
            ControllerEventPacket var2 = ControllerEventPacket.obtain();
            Parcel var3 = Parcel.obtain();
            var1.writeToParcel(var3, 0);
            var3.setDataPosition(0);
            var2.addGyroEvent().readFromParcel(var3);
            ServiceBridge.this.callbacks.onControllerEventPacket(var2);
            var2.recycle();
            var3.recycle();
        }

        public void deprecatedOnControllerOrientationEvent(ControllerOrientationEvent var1) {
            ControllerEventPacket var2 = ControllerEventPacket.obtain();
            Parcel var3 = Parcel.obtain();
            var1.writeToParcel(var3, 0);
            var3.setDataPosition(0);
            var2.addOrientationEvent().readFromParcel(var3);
            ServiceBridge.this.callbacks.onControllerEventPacket(var2);
            var2.recycle();
            var3.recycle();
        }

        public void deprecatedOnControllerTouchEvent(ControllerTouchEvent var1) {
            ControllerEventPacket var2 = ControllerEventPacket.obtain();
            Parcel var3 = Parcel.obtain();
            var1.writeToParcel(var3, 0);
            var3.setDataPosition(0);
            var2.addTouchEvent().readFromParcel(var3);
            ServiceBridge.this.callbacks.onControllerEventPacket(var2);
            var2.recycle();
            var3.recycle();
        }
    };

    @UsedByNative
    public ServiceBridge(Context var1, ServiceBridge.Callbacks var2) {
        this.context = var1.getApplicationContext();
        this.callbacks = var2;
        this.mainThreadHandler = new Handler(Looper.getMainLooper());
    }

    @UsedByNative
    public void setOrientationEnabled(boolean var1) {
        this.options.enableOrientation = var1;
    }

    @UsedByNative
    public void setTouchEnabled(boolean var1) {
        this.options.enableTouch = var1;
    }

    @UsedByNative
    public void setGyroEnabled(boolean var1) {
        this.options.enableGyro = var1;
    }

    @UsedByNative
    public void setAccelEnabled(boolean var1) {
        this.options.enableAccel = var1;
    }

    @UsedByNative
    public void setGesturesEnabled(boolean var1) {
        this.options.enableGestures = var1;
    }

    @UsedByNative
    public void setPositionEnabled(boolean var1) {
    }

    @UsedByNative
    public void requestBind() {
        this.mainThreadHandler.post(this.bindRunnable);
    }

    @UsedByNative
    public void requestUnbind() {
        this.mainThreadHandler.post(this.unbindRunnable);
    }

    private void doBind() {
        this.ensureOnMainThread();
        if(this.isBound) {
            Log.w(TAG, "Service is already bound.");
        } else {
            Intent var1;
            (var1 = new Intent("com.google.vr.vrcore.controller.BIND")).setPackage("com.baofeng.mj");
            if(!this.context.bindService(var1, this, 1)) {
                Log.w(TAG, "Bind failed. Service is not available.");
                this.callbacks.onServiceUnavailable();
            } else {
                this.isBound = true;
            }
        }
    }

    private void unregisterListener() {
        if(this.service != null) {
            try {
                this.service.unregisterListener("com.google.vr.internal.controller.LISTENER_KEY");
            } catch (RemoteException var2) {
                var2.printStackTrace();
                Log.w(TAG, "RemoteException while unregistering listener.");
            }
        }
    }

    private void doUnbind() {
        this.ensureOnMainThread();
        if(!this.isBound) {
            Log.w(TAG, "Service is already unbound.");
        } else {
            this.unregisterListener();
            this.context.unbindService(this);
            this.isBound = false;
        }
    }

    public void onServiceConnected(ComponentName var1, IBinder var2) {
        this.ensureOnMainThread();
        this.service = com.google.vr.vrcore.controller.api.IControllerService.Stub.asInterface(var2);

        int var3;
        try {
            var3 = this.service.initialize(11);
        } catch (RemoteException var8) {
            var8.printStackTrace();
            Log.e(TAG, "Failed to call initialize() on controller service (RemoteException).");
            this.callbacks.onServiceFailed();
            this.doUnbind();
            return;
        }

        if(var3 != 0) {
//            String var10000 = TAG;
//            String var10002 = String.valueOf(ControllerInitResults.toString(var3));
//            String var10001;
//            if(var10002.length() != 0) {
//                var10001 = "initialize() returned error: ".concat(var10002);
//            } else {
//                String var10003 = new String;
//                var10001 = var10003;
//                var10003.<init>("initialize() returned error: ");
//            }
//
//            Log.e(var10000, var10001);
            this.callbacks.onServiceInitFailed(var3);
            this.doUnbind();
        } else {
            int var4;
            if((var4 = this.getVrCoreClientApiVersion()) < 0) {
                Log.w(TAG, "Failed to get VrCore client API version.");
                this.callbacks.onServiceFailed();
                this.doUnbind();
            } else {
                byte var5 = 0;
                if(var4 >= 8) {
                    var5 = 1;
                } else {
                    Log.w(TAG, (new StringBuilder(62)).append("Recentering is not supported by VrCore API version ").append(var4).toString());
                }

                this.callbacks.onServiceConnected(var5);

                try {
                    if(!this.service.registerListener(0, "com.google.vr.internal.controller.LISTENER_KEY", this.controllerListener)) {
                        Log.w(TAG, "Failed to register listener.");
                        this.callbacks.onServiceFailed();
                        this.doUnbind();
                    }
                } catch (RemoteException var7) {
                    var7.printStackTrace();
                    Log.w(TAG, "RemoteException while registering listener.");
                    this.callbacks.onServiceFailed();
                    this.doUnbind();
                }
            }
        }
    }

    public void onServiceDisconnected(ComponentName var1) {
        this.ensureOnMainThread();
        this.service = null;
        this.callbacks.onServiceDisconnected();
    }

    protected int getVrCoreClientApiVersion() {
        try {
            return VrCoreUtils.getVrCoreClientApiVersion(this.context);
        } catch (VrCoreNotAvailableException var2) {
            Log.w(TAG, "VrCore not available.", var2);
            return -1;
        }
    }

    private void ensureOnMainThread() {
        if(Looper.myLooper() != Looper.getMainLooper()) {
            throw new IllegalStateException("This should be running on the main thread.");
        }
    }

    @UsedByNative
    public interface Callbacks {
        void onServiceConnected(int var1);

        void onServiceDisconnected();

        void onServiceUnavailable();

        void onServiceFailed();

        void onServiceInitFailed(int var1);

        void onControllerStateChanged(int var1, int var2);

        void onControllerEventPacket(ControllerEventPacket var1);

        void onControllerRecentered(ControllerOrientationEvent var1);

        void onControllerEventPacket2(ControllerEventPacket2 var1);
    }
}
