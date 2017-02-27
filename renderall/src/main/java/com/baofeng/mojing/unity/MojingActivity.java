//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by Fernflower decompiler)
//

package com.baofeng.mojing.unity;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import com.baofeng.mojing.unity.MojingVrActivity;
import com.mojing.sdk.pay.common.MjConfig;
import com.mojing.sdk.pay.utils.MjHttp;
import com.mojing.sdk.pay.utils.MjPaySdkUtil;
import com.mojing.sdk.pay.utils.MjPaySdk.MjPaySdkInnerCallBack;
import com.mojing.sdk.pay.widget.MojingPayValidationActivity;
import com.mojing.sdk.pay.widget.MojingPayValidationSingleActivity;
import com.unity3d.player.UnityPlayer;

public class MojingActivity extends MojingVrActivity {
    private static Activity mActivity;
    private static String mGameObject = "MojingCallback";
    private static String mUserid = "";
    private static boolean mIsVerify = false;
    private static String mMerchantId = "";
    private static String mAppId = "";
    private static String mAppKey = "";
    private static boolean mIsStart = false;
    private static String mjAppUid = "";
    private static MjPaySdkInnerCallBack innerCallBack;

    public MojingActivity() {
    }

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.initMjApp();
        mActivity = UnityPlayer.currentActivity;
        MjConfig.apiChar = (new StringBuilder(MjConfig.apiChar)).reverse().toString();
        mIsStart = true;
    }

    private void initMjApp() {
        mMerchantId = MjPaySdkUtil.getCustomMetaData(this, "DEVELOPER_MERCHANT_ID");
        mAppId = MjPaySdkUtil.getCustomMetaData(this, "DEVELOPER_APP_ID");
        mAppKey = MjPaySdkUtil.getCustomMetaData(this, "DEVELOPER_APP_KEY");
        mjAppUid = MjPaySdkUtil.getUidFromIntent(this.getIntent());
    }

    /** @deprecated */
    @Deprecated
    public static void mjAppAutoLogin() {
        if(!mIsVerify) {
            String code = "11003";
            unitySendMessage("MJLoginCallback", code);
        } else {
            if(mjAppUid != null && !"".equals(mjAppUid) && !"-1".equals(mjAppUid)) {
                onLoginCallback(mjAppUid);
            } else {
                unitySendMessage("MJLoginCallback", "10001");
            }

        }
    }

    public static void syncMjAppLoginState() {
        if(!mIsVerify) {
            String code = "11003";
            unitySendMessage("MJLoginCallback", code);
        } else {
            if(mjAppUid != null && !"".equals(mjAppUid) && !"-1".equals(mjAppUid)) {
                onLoginCallback(mjAppUid);
            } else {
                unitySendMessage("MJLoginCallback", "10001");
            }

        }
    }

    public static void unitySendMessage(String interfaceName, String result) {
        if(result != null && !"".equals(result)) {
            try {
                UnityPlayer.UnitySendMessage(mGameObject, interfaceName, result);
            } catch (Exception var3) {
                var3.printStackTrace();
            }

        } else {
            Log.i("MojingActivity", "unitySendMessage result==null , interfaceName=" + interfaceName);
        }
    }

    public static void mjSetCallbackGameObject(String gameObject) {
        mGameObject = gameObject;
    }

    public static void mjMerchantVerification() {
        MjHttp.getInstance(mActivity).merchantVerification(mMerchantId, mAppId, mAppKey);
    }

    public static void mjGetPayToken(final String sum, final String clientOrder, String screenType) {
        String code = "";
        if(!mIsVerify) {
            code = "11003";
            unitySendMessage("MjGetPayTokenData", "");
            unitySendMessage("MjGetPayTokenCallback", code);
        } else if("".equals(mUserid)) {
            code = "10001";
            unitySendMessage("MjGetPayTokenData", "");
            unitySendMessage("MjGetPayTokenCallback", code);
            return;
        }

        innerCallBack = new MjPaySdkInnerCallBack() {
            public void mjGetPayTokenValidationCallback(boolean isContinue) {
                if(isContinue) {
                    MjHttp.getInstance(MojingActivity.mActivity).getPayToken(MojingActivity.mMerchantId, MojingActivity.mAppId, MojingActivity.mAppKey, MojingActivity.mUserid, sum, clientOrder);
                } else {
                    MojingActivity.unitySendMessage("MjGetPayTokenCallback", "14002");
                }

            }
        };

        try {
            Intent e = new Intent();
            if(screenType == null || !"1".equals(screenType) && !"2".equals(screenType)) {
                e.setClass(mActivity, MojingPayValidationSingleActivity.class);
            } else {
                e.setClass(mActivity, MojingPayValidationActivity.class);
            }

            e.putExtra("sum", sum);
            e.putExtra("screenType", screenType);
            e.putExtra("isUnity", true);
            mActivity.startActivity(e);
        } catch (Exception var5) {
            ;
        }

    }

    public static MjPaySdkInnerCallBack getInnerCallBack() {
        return innerCallBack;
    }

    public static void mjPayMobi(String mobi, String serviceArea, String payToken, String clientOrder) {
        String code = "";
        if("".equals(mUserid)) {
            code = "10001";
            unitySendMessage("MjPayCallback", code);
        } else if(!mIsVerify) {
            code = "11003";
            unitySendMessage("MjPayCallback", code);
        } else if("".equals(payToken)) {
            code = "12001";
            unitySendMessage("MjPayCallback", code);
        } else {
            MjHttp.getInstance(mActivity).payMobi(mMerchantId, mAppId, mAppKey, mUserid, mobi, serviceArea, payToken, clientOrder);
        }
    }

    public static void mjCallRegister() {
        if(!mIsVerify) {
            String code = "11003";
            unitySendMessage("MJLoginCallback", code);
        } else {
            MjPaySdkUtil.startMjAppActivity(mActivity, "com.baofeng.mj.user.activity.SdkRegisterActivity", true);
        }
    }

    public static void mjCallSingleLogin() {
        if(!mIsVerify) {
            String code = "11003";
            unitySendMessage("MJLoginCallback", code);
        } else {
            MjPaySdkUtil.startMjAppActivity(mActivity, "com.baofeng.mj.user.activity.SdkSingleLoginActivity", true);
        }
    }

    public static void mjCallDoubleLogin() {
        if(!mIsVerify) {
            String code = "11003";
            unitySendMessage("MJLoginCallback", code);
        } else {
            MjPaySdkUtil.startMjAppActivity(mActivity, "com.baofeng.mjgl.pubblico.layout.SdkDoubleLoginActivity", true);
        }
    }

    public static boolean mjCheckLogin() {
        return !"".equals(mUserid);
    }

    public static boolean mjLoginOut() {
        mUserid = "";
        return true;
    }

    public static void onLoginCallback(String uid) {
        mUserid = uid;
        unitySendMessage("MJLoginUid", uid);
        unitySendMessage("MJLoginCallback", "10000");
    }

    public static void onVerifyCallback(String code) {
        if(code.equals("11000")) {
            mIsVerify = true;
        }

        unitySendMessage("MjVerification", code);
    }

    public static boolean isStart() {
        return mIsStart;
    }

    public static void mjAutoLogin(String uid) {
        mUserid = uid;
    }

    public static void mjGetBalance() {
        MjHttp.getInstance(mActivity).getBalance(mUserid);
    }
}
