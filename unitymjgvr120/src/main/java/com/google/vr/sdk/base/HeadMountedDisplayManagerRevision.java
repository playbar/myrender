//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.google.vr.sdk.base;

import android.content.Context;
import android.opengl.GLES20;
import android.util.Log;
import android.view.Display;
import android.view.WindowManager;
import com.google.vr.cardboard.VrParamsProvider;
import com.google.vr.cardboard.VrParamsProviderFactoryRevision;
import com.google.vrtoolkit.cardboard.proto.nano.CardboardDevice.DeviceParams;
import com.google.vrtoolkit.cardboard.proto.nano.Phone.PhoneParams;

public class HeadMountedDisplayManagerRevision {
    private static final String TAG = "HMDMRevision";
    private final HeadMountedDisplay hmd;
    private final Context context;
    private final VrParamsProvider paramsProvider;

    public HeadMountedDisplayManagerRevision(Context context) {
        this.context = context;
        this.paramsProvider = VrParamsProviderFactoryRevision.create(context);
        this.hmd = new HeadMountedDisplay(this.createScreenParams(), this.createGvrViewerParams());
    }

    public HeadMountedDisplay getHeadMountedDisplay() {
        return this.hmd;
    }

    public void onResume() {
        DeviceParams deviceProto = this.paramsProvider.readDeviceParams();
        GvrViewerParams deviceParams = deviceProto != null?new GvrViewerParams(deviceProto):null;
        if(deviceParams != null && !deviceParams.equals(this.hmd.getGvrViewerParams())) {
            this.hmd.setGvrViewerParams(deviceParams);
            Log.i("HMDMRevision", "Successfully read updated device params from external storage");
        }

        PhoneParams phoneProto = this.paramsProvider.readPhoneParams();
        ScreenParams screenParams = phoneProto != null?ScreenParams.fromProto(this.getDisplay(), phoneProto):null;
        if(screenParams != null && !screenParams.equals(this.hmd.getScreenParams())) {
            this.hmd.setScreenParams(screenParams);
            Log.i("HMDMRevision", "Successfully read updated screen params from external storage");
        }

    }

    public void onPause() {
    }

    public boolean updateGvrViewerParams(GvrViewerParams gvrViewerParams) {
        if(gvrViewerParams != null && !gvrViewerParams.equals(this.hmd.getGvrViewerParams())) {
//            gvrViewerParams.setModel("bfmj");
//            gvrViewerParams.setVendor("bfmjs1");
            String str = gvrViewerParams.toString();
            Log.e("updateGvrViewerParams", gvrViewerParams.getModel() + ", " + str);
            this.paramsProvider.writeDeviceParams(gvrViewerParams.toProtobuf());
            this.hmd.setGvrViewerParams(gvrViewerParams);

            GLES20.glFinish();
            return true;
        } else {
            return false;
        }
//        return false;
    }

    public boolean updateScreenParams(ScreenParams screenParams) {
        if(screenParams != null && !screenParams.equals(this.hmd.getScreenParams())) {
            this.hmd.setScreenParams(screenParams);
            return true;
        } else {
            return false;
        }
    }

    private Display getDisplay() {
        WindowManager windowManager = (WindowManager)this.context.getSystemService("window");
        return windowManager.getDefaultDisplay();
    }

    private ScreenParams createScreenParams() {
        Display display = this.getDisplay();
        ScreenParams params = ScreenParams.fromProto(display, this.paramsProvider.readPhoneParams());
        return params != null?params:new ScreenParams(display);
    }

    private GvrViewerParams createGvrViewerParams() {
        return new GvrViewerParams(this.paramsProvider.readDeviceParams());
    }
}
