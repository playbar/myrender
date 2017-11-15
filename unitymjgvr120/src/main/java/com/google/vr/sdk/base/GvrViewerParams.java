//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.google.vr.sdk.base;

import android.net.Uri;
import android.net.Uri.Builder;
import android.nfc.NdefMessage;
import android.nfc.NdefRecord;
import android.util.Base64;
import android.util.Log;
import com.google.protobuf.nano.MessageNano;
import com.google.vr.cardboard.ConfigUtils;
import com.google.vrtoolkit.cardboard.proto.nano.CardboardDevice.DeviceParams;

public class GvrViewerParams {
    private static final String TAG = "GvrViewerParams";
    private static final String HTTP_SCHEME = "http";
    private static final String URI_HOST_GOOGLE_SHORT = "g.co";
    private static final String URI_HOST_GOOGLE = "google.com";
    private static final String URI_PATH_CARDBOARD_HOME = "cardboard";
    private static final String URI_PATH_CARDBOARD_CONFIG = "cardboard/cfg";
    private static final String URI_SCHEME_LEGACY_CARDBOARD = "cardboard";
    private static final String URI_HOST_LEGACY_CARDBOARD = "v1.0.0";
    private static final Uri URI_ORIGINAL_CARDBOARD_NFC = (new Builder()).scheme("cardboard").authority("v1.0.0").build();
    private static final Uri URI_ORIGINAL_CARDBOARD_QR_CODE = (new Builder()).scheme("http").authority("g.co").appendEncodedPath("cardboard").build();
    private static final String CARDBOARD_V2_2_VENDOR = "Google, Inc.";
    private static final String CARDBOARD_V2_2_MODEL = "Default Cardboard";
    private static final float CARDBOARD_V2_2_INTER_LENS_DISTANCE = 0.064F;
    private static final GvrViewerParams.VerticalAlignmentType CARDBOARD_V2_2_VERTICAL_ALIGNMENT;
    private static final float CARDBOARD_V2_2_VERTICAL_DISTANCE_TO_LENS_CENTER = 0.035F;
    private static final float CARDBOARD_V2_2_SCREEN_TO_LENS_DISTANCE = 0.039F;
    private static final String CARDBOARD_V1_VENDOR = "Google, Inc.";
    private static final String CARDBOARD_V1_MODEL = "Cardboard v1";
    private static final float CARDBOARD_V1_INTER_LENS_DISTANCE = 0.06F;
    private static final GvrViewerParams.VerticalAlignmentType CARDBOARD_V1_VERTICAL_ALIGNMENT;
    private static final float CARDBOARD_V1_VERTICAL_DISTANCE_TO_LENS_CENTER = 0.035F;
    private static final float CARDBOARD_V1_SCREEN_TO_LENS_DISTANCE = 0.042F;
    private static final GvrViewerParams DEFAULT_PARAMS;
    private String vendor;
    private String model;
    private float interLensDistance;
    private GvrViewerParams.VerticalAlignmentType verticalAlignment;
    private float verticalDistanceToLensCenter;
    private float screenToLensDistance;
    private FieldOfView leftEyeMaxFov;
    private boolean hasMagnet;
    private Distortion distortion;
    private DeviceParams originalDeviceProto;

    public GvrViewerParams() {
        this.setDefaultValues();
    }

    public GvrViewerParams(GvrViewerParams params) {
        this.copyFrom(params);
    }

    public GvrViewerParams(DeviceParams params) {
        this.setDefaultValues();
        if(params != null) {
            this.originalDeviceProto = params.clone();
            this.vendor = params.getVendor();
            this.model = params.getModel();
            this.interLensDistance = params.getInterLensDistance();
            this.verticalAlignment = GvrViewerParams.VerticalAlignmentType.fromProtoValue(params.getVerticalAlignment());
            this.verticalDistanceToLensCenter = params.getTrayToLensDistance();
            this.screenToLensDistance = params.getScreenToLensDistance();
            this.leftEyeMaxFov = FieldOfView.parseFromProtobuf(params.leftEyeFieldOfViewAngles);
            if(this.leftEyeMaxFov == null) {
                this.leftEyeMaxFov = new FieldOfView();
            }

            this.distortion = Distortion.parseFromProtobuf(params.distortionCoefficients);
            if(this.distortion == null) {
                this.distortion = new Distortion();
            }

            this.hasMagnet = params.getHasMagnet();
        }
    }

    public static boolean isOriginalCardboardDeviceUri(Uri uri) {
        return URI_ORIGINAL_CARDBOARD_QR_CODE.equals(uri) || URI_ORIGINAL_CARDBOARD_NFC.getScheme().equals(uri.getScheme()) && URI_ORIGINAL_CARDBOARD_NFC.getAuthority().equals(uri.getAuthority());
    }

    private static boolean isCardboardDeviceUri(Uri uri) {
        return "http".equals(uri.getScheme()) && "google.com".equals(uri.getAuthority()) && "/cardboard/cfg".equals(uri.getPath());
    }

    public static boolean isGvrUri(Uri uri) {
        return isOriginalCardboardDeviceUri(uri) || isCardboardDeviceUri(uri);
    }

    public static GvrViewerParams createFromUri(Uri uri) {
        if(uri == null) {
            return null;
        } else if(isOriginalCardboardDeviceUri(uri)) {
            return cardboardV1ViewerParams();
        } else if(!isCardboardDeviceUri(uri)) {
            Log.w("GvrViewerParams", String.format("URI \"%s\" not recognized as GVR viewer.", new Object[]{uri}));
            return null;
        } else {
            return new GvrViewerParams(ConfigUtils.readDeviceParamsFromUri(uri));
        }
    }

    public static GvrViewerParams cardboardV1ViewerParams() {
        GvrViewerParams deviceParams = new GvrViewerParams();
        deviceParams.vendor = "Google, Inc.";
        deviceParams.model = "Cardboard v1";
        deviceParams.interLensDistance = 0.06F;
        deviceParams.verticalAlignment = CARDBOARD_V1_VERTICAL_ALIGNMENT;
        deviceParams.verticalDistanceToLensCenter = 0.035F;
        deviceParams.screenToLensDistance = 0.042F;
        deviceParams.leftEyeMaxFov = FieldOfView.cardboardV1FieldOfView();
        deviceParams.hasMagnet = true;
        deviceParams.distortion = Distortion.cardboardV1Distortion();
        return deviceParams;
    }

    public static GvrViewerParams createFromNfcContents(NdefMessage tagContents) {
        if(tagContents == null) {
            Log.w("GvrViewerParams", "Could not get contents from NFC tag.");
            return null;
        } else {
            NdefRecord[] var1 = tagContents.getRecords();
            int var2 = var1.length;

            for(int var3 = 0; var3 < var2; ++var3) {
                NdefRecord record = var1[var3];
                GvrViewerParams params = createFromUri(record.toUri());
                if(params != null) {
                    return params;
                }
            }

            return null;
        }
    }

    byte[] toByteArray() {
        return MessageNano.toByteArray(this.toProtobuf());
    }

    public DeviceParams toProtobuf() {
        DeviceParams params = this.originalDeviceProto != null?this.originalDeviceProto.clone():new DeviceParams();
        params.setVendor(this.vendor);
        params.setModel(this.model);
        params.setInterLensDistance(this.interLensDistance);
        params.setVerticalAlignment(this.verticalAlignment.toProtoValue());
        if(this.verticalAlignment == GvrViewerParams.VerticalAlignmentType.CENTER) {
            params.setTrayToLensDistance(0.035F);
        } else {
            params.setTrayToLensDistance(this.verticalDistanceToLensCenter);
        }

        params.setScreenToLensDistance(this.screenToLensDistance);
        params.leftEyeFieldOfViewAngles = this.leftEyeMaxFov.toProtobuf();
        params.distortionCoefficients = this.distortion.toProtobuf();
        if(this.hasMagnet) {
            params.setHasMagnet(this.hasMagnet);
        }

        return params;
    }

    public Uri toUri() {
        byte[] paramsData = this.toByteArray();
        int paramsSize = paramsData.length;
        return (new Builder()).scheme("http").authority("google.com").appendEncodedPath("cardboard/cfg").appendQueryParameter("p", Base64.encodeToString(paramsData, 0, paramsSize, 11)).build();
    }

    public void setVendor(String vendor) {
        this.vendor = vendor != null?vendor:"";
    }

    public String getVendor() {
        return this.vendor;
    }

    public void setModel(String model) {
        this.model = model != null?model:"";
    }

    public String getModel() {
        return this.model;
    }

    public void setInterLensDistance(float interLensDistance) {
        this.interLensDistance = interLensDistance;
    }

    public float getInterLensDistance() {
        return this.interLensDistance;
    }

    public GvrViewerParams.VerticalAlignmentType getVerticalAlignment() {
        return this.verticalAlignment;
    }

    public void setVerticalAlignment(GvrViewerParams.VerticalAlignmentType verticalAlignment) {
        this.verticalAlignment = verticalAlignment;
    }

    public void setVerticalDistanceToLensCenter(float verticalDistanceToLensCenter) {
        this.verticalDistanceToLensCenter = verticalDistanceToLensCenter;
    }

    public float getVerticalDistanceToLensCenter() {
        return this.verticalDistanceToLensCenter;
    }

    float getYEyeOffsetMeters(ScreenParams screen)
    {
        switch (getVerticalAlignment())
        {
            case CENTER:
                return screen.getHeightMeters() / 2.0F;
            case BOTTOM:
                return getVerticalDistanceToLensCenter() - screen.getBorderSizeMeters();
            default:
                return screen.getHeightMeters() - (getVerticalDistanceToLensCenter() - screen.getBorderSizeMeters());
        }

    }

    public void setScreenToLensDistance(float screenToLensDistance) {
        this.screenToLensDistance = screenToLensDistance;
    }

    public float getScreenToLensDistance() {
        return this.screenToLensDistance;
    }

    public Distortion getDistortion() {
        return this.distortion;
    }

    public FieldOfView getLeftEyeMaxFov() {
        return this.leftEyeMaxFov;
    }

    public boolean getHasMagnet() {
        return this.hasMagnet;
    }

    public void setHasMagnet(boolean magnet) {
        this.hasMagnet = magnet;
    }

    public boolean equals(Object other) {
        if(other == null) {
            return false;
        } else if(other == this) {
            return true;
        } else if(!(other instanceof GvrViewerParams)) {
            return false;
        } else {
            GvrViewerParams o = (GvrViewerParams)other;
            boolean fieldsHandledByThisClassAreEqual = this.vendor.equals(o.vendor) && this.model.equals(o.model) && this.interLensDistance == o.interLensDistance && this.verticalAlignment == o.verticalAlignment && (this.verticalAlignment == GvrViewerParams.VerticalAlignmentType.CENTER || this.verticalDistanceToLensCenter == o.verticalDistanceToLensCenter) && this.screenToLensDistance == o.screenToLensDistance && this.leftEyeMaxFov.equals(o.leftEyeMaxFov) && this.distortion.equals(o.distortion) && this.hasMagnet == o.hasMagnet;
            return !fieldsHandledByThisClassAreEqual?false:MessageNano.messageNanoEquals(this.originalDeviceProto, o.originalDeviceProto);
        }
    }

    public String toString() {
        StringBuilder var10000 = (new StringBuilder()).append("{\n");
        String var1 = this.vendor;
        var10000 = var10000.append((new StringBuilder(12 + String.valueOf(var1).length())).append("  vendor: ").append(var1).append(",\n").toString());
        var1 = this.model;
        var10000 = var10000.append((new StringBuilder(11 + String.valueOf(var1).length())).append("  model: ").append(var1).append(",\n").toString());
        float var2 = this.interLensDistance;
        var10000 = var10000.append((new StringBuilder(40)).append("  inter_lens_distance: ").append(var2).append(",\n").toString());
        var1 = String.valueOf(this.verticalAlignment);
        var10000 = var10000.append((new StringBuilder(24 + String.valueOf(var1).length())).append("  vertical_alignment: ").append(var1).append(",\n").toString());
        var2 = this.verticalDistanceToLensCenter;
        var10000 = var10000.append((new StringBuilder(53)).append("  vertical_distance_to_lens_center: ").append(var2).append(",\n").toString());
        var2 = this.screenToLensDistance;
        var10000 = var10000.append((new StringBuilder(44)).append("  screen_to_lens_distance: ").append(var2).append(",\n").toString());
        var1 = String.valueOf(this.leftEyeMaxFov.toString().replace("\n", "\n  "));
        var10000 = var10000.append((new StringBuilder(22 + String.valueOf(var1).length())).append("  left_eye_max_fov: ").append(var1).append(",\n").toString());
        var1 = String.valueOf(this.distortion.toString().replace("\n", "\n  "));
        var10000 = var10000.append((new StringBuilder(16 + String.valueOf(var1).length())).append("  distortion: ").append(var1).append(",\n").toString());
        boolean var3 = this.hasMagnet;
        return var10000.append((new StringBuilder(17)).append("  magnet: ").append(var3).append(",\n").toString()).append("}\n").toString();
    }

    public boolean isDefault() {
        return DEFAULT_PARAMS.equals(this);
    }

    private void setDefaultValues() {
        this.vendor = "Google, Inc.";
        this.model = "Default Cardboard";
        this.interLensDistance = 0.064F;
        this.verticalAlignment = CARDBOARD_V2_2_VERTICAL_ALIGNMENT;
        this.verticalDistanceToLensCenter = 0.035F;
        this.screenToLensDistance = 0.039F;
        this.leftEyeMaxFov = new FieldOfView();
        this.hasMagnet = false;
        this.distortion = new Distortion();
    }

    private void copyFrom(GvrViewerParams params) {
        this.vendor = params.vendor;
        this.model = params.model;
        this.interLensDistance = params.interLensDistance;
        this.verticalAlignment = params.verticalAlignment;
        this.verticalDistanceToLensCenter = params.verticalDistanceToLensCenter;
        this.screenToLensDistance = params.screenToLensDistance;
        this.leftEyeMaxFov = new FieldOfView(params.leftEyeMaxFov);
        this.hasMagnet = params.hasMagnet;
        this.distortion = new Distortion(params.distortion);
        this.originalDeviceProto = params.originalDeviceProto;
    }

    static {
        CARDBOARD_V2_2_VERTICAL_ALIGNMENT = GvrViewerParams.VerticalAlignmentType.BOTTOM;
        CARDBOARD_V1_VERTICAL_ALIGNMENT = GvrViewerParams.VerticalAlignmentType.BOTTOM;
        DEFAULT_PARAMS = new GvrViewerParams();
    }

    public static enum VerticalAlignmentType {
        BOTTOM(0),
        CENTER(1),
        TOP(2);

        private final int protoValue;

        private VerticalAlignmentType(int protoValue) {
            this.protoValue = protoValue;
        }

        int toProtoValue() {
            return this.protoValue;
        }

        static GvrViewerParams.VerticalAlignmentType fromProtoValue(int protoValue) {
            GvrViewerParams.VerticalAlignmentType[] var1 = values();
            int var2 = var1.length;

            for(int var3 = 0; var3 < var2; ++var3) {
                GvrViewerParams.VerticalAlignmentType type = var1[var3];
                if(type.protoValue == protoValue) {
                    return type;
                }
            }

            Log.e("GvrViewerParams", String.format("Unknown alignment type from proto: %d", new Object[]{Integer.valueOf(protoValue)}));
            return BOTTOM;
        }
    }
}
