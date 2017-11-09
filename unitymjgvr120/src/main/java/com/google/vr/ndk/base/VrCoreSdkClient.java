//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.google.vr.ndk.base;

import android.app.ActivityManager;
import android.app.AlertDialog;
import android.app.PendingIntent;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;
import com.google.vr.cardboard.UiUtils;
import com.google.vr.cardboard.R.string;
import com.google.vr.ndk.base.DaydreamUtilsWrapper;
import com.google.vr.ndk.base.FadeOverlayView;
import com.google.vr.ndk.base.GvrApi;
import com.google.vr.vrcore.base.api.VrCoreNotAvailableException;
import com.google.vr.vrcore.base.api.VrCoreUtils;
import com.google.vr.vrcore.common.api.HeadTrackingState;
import com.google.vr.vrcore.common.api.IDaydreamManager;
import com.google.vr.vrcore.common.api.IVrCoreSdkService;
import com.google.vr.vrcore.common.api.IVrCoreSdkService.Stub;
import java.lang.ref.WeakReference;

class VrCoreSdkClient {
    private static final String TAG = "VrCoreSdkClient";
    private static final boolean DEBUG = false;
    static final int MIN_VRCORE_API_VERSION = 5;
    static final int TARGET_VRCORE_API_VERSION = 10;
    private static final int FADE_FLUSH_DELAY_FOR_TRACKING_STABILIZATION_MILLIS = 200;
    private final Context context;
    private final GvrApi gvrApi;
    private final ComponentName componentName;
    private final DaydreamUtilsWrapper daydreamUtils;
    private final Runnable closeVrRunnable;
    private final FadeOverlayView fadeOverlayView;
    private final VrCoreSdkClient.DaydreamListenerImpl daydreamListener;
    private final boolean shouldBind;
    private boolean isBound;
    private boolean isResumed;
    private boolean isEnabled = true;
    private IVrCoreSdkService vrCoreSdkService;
    private IDaydreamManager daydreamManager;
    private AlertDialog helpCenterDialog;
    private final ServiceConnection serviceConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName var1, IBinder var2) {
            IVrCoreSdkService var3 = Stub.asInterface(var2);

            String var5;
            try {
                if(!var3.initialize(10)) {
                    Log.e("VrCoreSdkClient", "Failed to initialize VrCore SDK Service.");
                    VrCoreSdkClient.this.handleBindFailed();
                    return;
                }
            } catch (RemoteException var16) {
                var5 = String.valueOf(var16);
                Log.w("VrCoreSdkClient", (new StringBuilder(41 + String.valueOf(var5).length())).append("Failed to initialize VrCore SDK Service: ").append(var5).toString());
                VrCoreSdkClient.this.handleBindFailed();
                return;
            }

            VrCoreSdkClient.this.vrCoreSdkService = var3;

            try {
                VrCoreSdkClient.this.daydreamManager = VrCoreSdkClient.this.vrCoreSdkService.getDaydreamManager();
                if(VrCoreSdkClient.this.daydreamManager == null) {
                    Log.w("VrCoreSdkClient", "Failed to obtain DaydreamManager from VrCore SDK Service.");
                    VrCoreSdkClient.this.handleNoDaydreamManager();
                    return;
                }

                VrCoreSdkClient.this.daydreamManager.registerListener(VrCoreSdkClient.this.componentName, VrCoreSdkClient.this.daydreamListener);
            } catch (RemoteException var15) {
                var5 = String.valueOf(var15);
                Log.w("VrCoreSdkClient", (new StringBuilder(57 + String.valueOf(var5).length())).append("Failed to obtain DaydreamManager from VrCore SDK Service:").append(var5).toString());
                VrCoreSdkClient.this.handleNoDaydreamManager();
                return;
            }

            HeadTrackingState var4 = null;
            boolean var12 = false;

            label100: {
                label118: {
                    HeadTrackingState var17;
                    int var18;
                    try {
                        var12 = true;
                        var17 = VrCoreSdkClient.this.getHeadTrackingState();
                        if((var18 = VrCoreSdkClient.this.daydreamManager.prepareVr(VrCoreSdkClient.this.componentName, var17)) == 2) {
                            Log.e("VrCoreSdkClient", "Daydream VR preparation failed, closing VR session.");
                            VrCoreSdkClient.this.handlePrepareVrFailed();
                            var12 = false;
                            break label100;
                        }

                        var12 = false;
                    } catch (RemoteException var13) {
                        String var6 = String.valueOf(var13);
                        Log.w("VrCoreSdkClient", (new StringBuilder(61 + String.valueOf(var6).length())).append("Error while registering listener with the VrCore SDK Service:").append(var6).toString());
                        var12 = false;
                        break label118;
                    } finally {
                        if(var12) {
                            VrCoreSdkClient.this.resumeTracking((HeadTrackingState)null);
                        }
                    }

                    if(var18 == 0) {
                        var4 = var17;
                    }

                    VrCoreSdkClient.this.resumeTracking(var4);
                    return;
                }

                VrCoreSdkClient.this.resumeTracking((HeadTrackingState)null);
                return;
            }

            VrCoreSdkClient.this.resumeTracking((HeadTrackingState)null);
        }

        public void onServiceDisconnected(ComponentName var1) {
            VrCoreSdkClient.this.vrCoreSdkService = null;
            VrCoreSdkClient.this.daydreamManager = null;
        }
    };

    public VrCoreSdkClient(Context var1, GvrApi var2, ComponentName var3, DaydreamUtilsWrapper var4, Runnable var5, FadeOverlayView var6) {
        this.context = var1;
        this.gvrApi = var2;
        this.componentName = var3;
        this.daydreamUtils = var4;
        this.closeVrRunnable = var5;
        this.fadeOverlayView = var6;
        this.daydreamListener = new VrCoreSdkClient.DaydreamListenerImpl(var2, var6);
        this.shouldBind = hasCompatibleSdkService(var1);
        var2.setIgnoreManualTrackerPauseResume(true);
    }

    IDaydreamManager getDaydreamManager() {
        return this.daydreamManager;
    }

    HeadTrackingState getHeadTrackingState() {
        return new HeadTrackingState();
    }

    public boolean onResume() {
        this.isResumed = true;
        return !this.isEnabled?false:this.doBind();
    }

    public void onPause() {
        this.isResumed = false;
        this.daydreamListener.resetSafeguards();
        if(this.isEnabled) {
            this.doUnbind();
        }

    }

    public void setEnabled(boolean var1) {
        if(this.isEnabled != var1) {
            this.isEnabled = var1;
            this.gvrApi.setIgnoreManualTrackerPauseResume(var1);
            if(this.isResumed) {
                if(this.isEnabled) {
                    this.doBind();
                    return;
                }

                this.doUnbind();
            }

        }
    }

    public boolean launchInVr(PendingIntent var1) {
        if(this.daydreamManager != null) {
            try {
                this.daydreamManager.deprecatedLaunchInVr(var1);
            } catch (RemoteException var4) {
                String var3 = String.valueOf(var4);
                Log.w("VrCoreSdkClient", (new StringBuilder(28 + String.valueOf(var3).length())).append("Failed to launch app in VR: ").append(var3).toString());
            }

            return true;
        } else {
            return false;
        }
    }

    private boolean doBind() {
        if(this.isBound) {
            return true;
        } else {
            if(this.shouldBind) {
                Intent var1;
                (var1 = new Intent("com.google.vr.vrcore.BIND_SDK_SERVICE")).setPackage("com.google.vr.vrcore");
                this.isBound = this.context.bindService(var1, this.serviceConnection, 1);
            }

            if(!this.isBound) {
                this.handleBindFailed();
            }

            return this.isBound;
        }
    }

    private void doUnbind() {
        if(this.isResumed) {
            this.resumeTracking((HeadTrackingState)null);
        } else {
            this.gvrApi.pauseTrackingGetState();
        }

        if(this.isBound) {
            if(this.daydreamManager != null) {
                try {
                    this.daydreamManager.unregisterListener(this.componentName);
                } catch (RemoteException var3) {
                    String var2 = String.valueOf(var3);
                    Log.w("VrCoreSdkClient", (new StringBuilder(40 + String.valueOf(var2).length())).append("Failed to unregister Daydream listener: ").append(var2).toString());
                }

                this.daydreamManager = null;
            }

            this.vrCoreSdkService = null;
            this.context.unbindService(this.serviceConnection);
            this.isBound = false;
        }
    }

    private void handleBindFailed() {
        this.doUnbind();
        this.warnIfIncompatibleClient();
    }

    private void handleNoDaydreamManager() {
        this.doUnbind();
        this.warnIfIncompatibleClient();
    }

    private void handlePrepareVrFailed() {
        this.doUnbind();
        this.closeVrRunnable.run();
    }

    private void warnIfIncompatibleClient() {
        if(!this.daydreamUtils.isDaydreamPhone(this.context) && this.daydreamUtils.isDaydreamRequiredComponent(this.context) && !ActivityManager.isRunningInTestHarness()) {
            if(this.helpCenterDialog != null) {
                this.helpCenterDialog.show();
                return;
            }

            //todo helpCenterDialog
//            this.helpCenterDialog = UiUtils.showDaydreamHelpCenterDialog(this.context, string.dialog_title_incompatible_phone, string.dialog_message_incompatible_phone, this.closeVrRunnable);
        }

    }

    private void resumeTracking(HeadTrackingState var1) {
        resumeTracking(this.gvrApi, var1);
        if(this.fadeOverlayView != null) {
            this.fadeOverlayView.flushAutoFade(200L);
        }

    }

    private static void resumeTracking(GvrApi var0, HeadTrackingState var1) {
        var0.resumeTrackingSetState(var1 != null && !var1.isEmpty()?var1.getData():null);
    }

    private static boolean hasCompatibleSdkService(Context var0) {
        try {
            int var1;
            if((var1 = VrCoreUtils.getVrCoreClientApiVersion(var0)) >= 5) {
                return true;
            }

            Log.w("VrCoreSdkClient", String.format("VrCore service obsolete, GVR SDK requires API %d but found API %d.", new Object[]{Integer.valueOf(5), Integer.valueOf(var1)}));
        } catch (VrCoreNotAvailableException var2) {
            ;
        }

        return false;
    }

    private static final class DaydreamListenerImpl extends com.google.vr.vrcore.common.api.IDaydreamListener.Stub {
        private static final long TRACKING_SAFEGUARD_DELAY_MILLIS = 3000L;
        private static final long FADE_SAFEGUARD_DELAY_MILLIS = 3500L;
        private static final int MSG_TRACKING_RESUME_SAFEGUARD = 1;
        private static final int MSG_FADE_IN_SAFEGUARD = 2;
        private final WeakReference<GvrApi> gvrApiWeak;
        private final WeakReference<FadeOverlayView> fadeOverlayViewWeak;
        private final Handler safeguardHandler = new Handler() {
            public void handleMessage(Message var1) {
                switch(var1.what) {
                    case 1:
                        Log.w("VrCoreSdkClient", "Forcing tracking resume: VrCore unresponsive");
                        DaydreamListenerImpl.this.resumeHeadTrackingImpl((HeadTrackingState)null);
                        return;
                    case 2:
                        Log.w("VrCoreSdkClient", "Forcing fade in: VrCore unresponsive");
                        DaydreamListenerImpl.this.applyFadeImpl(1, 350L);
                        return;
                    default:
                        super.handleMessage(var1);
                }
            }
        };

        DaydreamListenerImpl(GvrApi var1, FadeOverlayView var2) {
            this.gvrApiWeak = new WeakReference(var1);
            this.fadeOverlayViewWeak = new WeakReference(var2);
        }

        final void resetSafeguards() {
            this.safeguardHandler.removeCallbacksAndMessages((Object)null);
        }

        public final int getTargetApiVersion() throws RemoteException {
            return 10;
        }

        public final HeadTrackingState requestStopTracking() throws RemoteException {
            GvrApi var1;
            if((var1 = (GvrApi)this.gvrApiWeak.get()) == null) {
                Log.w("VrCoreSdkClient", "Invalid requestStopTracking() call: GvrApi no longer valid");
                return null;
            } else {
                byte[] var2 = var1.pauseTrackingGetState();
                this.rescheduleSafeguard(1, 3000L);
                return var2 != null?new HeadTrackingState(var2):null;
            }
        }

        public final void applyFade(int var1, long var2) {
            this.applyFadeImpl(var1, var2);
        }

        public final void recenterHeadTracking() throws RemoteException {
            GvrApi var1;
            if((var1 = (GvrApi)this.gvrApiWeak.get()) == null) {
                Log.w("VrCoreSdkClient", "Invalid recenterHeadTracking() call: GvrApi no longer valid");
            } else {
                var1.recenterTracking();
            }
        }

        public final void dumpDebugData() throws RemoteException {
            GvrApi var1;
            if((var1 = (GvrApi)this.gvrApiWeak.get()) == null) {
                Log.w("VrCoreSdkClient", "Invalid dumpDebugData() call: GvrApi no longer valid");
            } else {
                var1.dumpDebugData();
            }
        }

        public final void resumeHeadTracking(HeadTrackingState var1) {
            this.resumeHeadTrackingImpl(var1);
        }

        private final void resumeHeadTrackingImpl(HeadTrackingState var1) {
            GvrApi var2;
            if((var2 = (GvrApi)this.gvrApiWeak.get()) == null) {
                Log.w("VrCoreSdkClient", "Invalid resumeHeadTracking() call: GvrApi no longer valid");
            } else {
                this.cancelSafeguard(1);
                VrCoreSdkClient.resumeTracking(var2, var1);
            }
        }

        private final void applyFadeImpl(final int var1, final long var2) {
            final FadeOverlayView var4;
            if((var4 = (FadeOverlayView)this.fadeOverlayViewWeak.get()) != null) {
                this.cancelSafeguard(2);
                var4.post(new Runnable() {
                    public void run() {
                        var4.startFade(var1, var2);
                    }
                });
                if(var1 == 2) {
                    this.rescheduleSafeguard(2, var2 + 3500L);
                }

            }
        }

        private final void cancelSafeguard(int var1) {
            this.safeguardHandler.removeMessages(var1);
        }

        private final void rescheduleSafeguard(int var1, long var2) {
            this.cancelSafeguard(var1);
            this.safeguardHandler.sendEmptyMessageDelayed(var1, var2);
        }
    }
}
