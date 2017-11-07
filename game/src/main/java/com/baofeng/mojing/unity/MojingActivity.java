package com.baofeng.mojing.unity;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import com.mojing.sdk.pay.common.MjConfig;
import com.mojing.sdk.pay.service.AIDLService;
import com.mojing.sdk.pay.utils.MjHttp;
import com.mojing.sdk.pay.utils.MjPaySdk.MjPaySdkInnerCallBack;
import com.mojing.sdk.pay.utils.MjPaySdkUtil;
import com.mojing.sdk.pay.widget.MojingPayValidationActivity;
import com.mojing.sdk.pay.widget.MojingPayValidationSingleActivity;
import com.unity3d.player.UnityPlayer;

public class MojingActivity extends MojingVrActivity {
    private static Activity mActivity;
    private static String mGameObject = MjConfig.gameObject;
    private static String mUserid = "";
    private static boolean mIsVerify = false;
    private static String mMerchantId = "";
    private static String mAppId = "";
    private static String mAppKey = "";
    private static boolean mIsStart = false;
    private static String mjAppUid = "";
    // 获取支付token用户确认回调
    private static MjPaySdkInnerCallBack innerCallBack;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        initMjApp();
        mActivity = UnityPlayer.currentActivity;
        MjConfig.apiChar = new StringBuilder(MjConfig.apiChar).reverse().toString();
        mIsStart = true;
    }

    /**
     * 从暴风魔镜APP打开游戏，记录uid，以便商户验证完成后自动登录
     *
     * @param {引入参数名} {引入参数说明}
     * @return {返回值说明}
     * @author liumansong @Date 2016-3-17 下午7:27:03
     */
    private void initMjApp() {
        mMerchantId = MjPaySdkUtil.getCustomMetaData(this, "DEVELOPER_MERCHANT_ID");
        mAppId = MjPaySdkUtil.getCustomMetaData(this, "DEVELOPER_APP_ID");
        mAppKey = MjPaySdkUtil.getCustomMetaData(this, "DEVELOPER_APP_KEY");
        mjAppUid = MjPaySdkUtil.getUidFromIntent(getIntent());
    }

    /**
     * 从暴风魔镜APP打开游戏自动登录
     *
     * @param {引入参数名} {引入参数说明}
     * @return {返回值说明}
     * @author liumansong @Date 2016-3-17 下午2:11:39
     * @deprecated Use syncMjAppLoginState instead.
     */
    @Deprecated
    public static void mjAppAutoLogin() {
        if (!mIsVerify) {
            String code = "11003";
            unitySendMessage("MJLoginCallback", code);
            return;
        }

        if (mjAppUid != null && !"".equals(mjAppUid) && !"-1".equals(mjAppUid)) {
            onLoginCallback(mjAppUid);
        } else {
            unitySendMessage("MJLoginCallback", "10001");
        }
    }

    /**
     * 从暴风魔镜APP打开游戏自动登录
     *
     * @param {引入参数名} {引入参数说明}
     * @return {返回值说明}
     * @author liumansong @Date 2016-3-17 下午2:11:39
     */
    public static void syncMjAppLoginState() {
        if (!mIsVerify) {
            String code = "11003";
            unitySendMessage("MJLoginCallback", code);
            return;
        }

        if (mjAppUid != null && !"".equals(mjAppUid) && !"-1".equals(mjAppUid)) {
            onLoginCallback(mjAppUid);
        } else {
            unitySendMessage("MJLoginCallback", "10001");
        }
    }

    /**
     * 回调unity
     *
     * @param interfaceName 接口名
     * @param result        返回结果
     * @return void
     * @author linzanxian @Date 2015-10-8 下午3:17:09
     */
    public static void unitySendMessage(String interfaceName, String result) {
        if (result == null || "".equals(result)) {
            Log.i("MojingActivity", "unitySendMessage result==null , interfaceName=" + interfaceName);
            return;
        }

        try {
            UnityPlayer.UnitySendMessage(mGameObject, interfaceName, result);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    /** interface **/

    /**
     * 设置回调Unity物体名
     *
     * @param gameObject 物体名
     * @return void
     * @author linzanxian @Date 2015-10-8 下午3:04:02
     */
    public static void mjSetCallbackGameObject(String gameObject) {
        mGameObject = gameObject;
    }

    /**
     * 商户验证接口, 结果通过异步回调
     *
     * @return void
     * @author linzanxian @Date 2015-10-8 下午3:04:29
     */
    public static void mjMerchantVerification() {
        MjHttp.getInstance(mActivity).merchantVerification(mMerchantId, mAppId, mAppKey);
    }

    /**
     * 获取支付token
     *
     * @param {sum}         {支付金额}
     * @param {screenType}  {2，双屏头控，1双屏手柄，0单屏}
     * @param {clientOrder} {订单号}
     * @return {返回值说明}
     * @author liumansong @Date 2016-4-18 下午3:08:32
     */
    public static void mjGetPayToken(final String sum, final String clientOrder, String screenType) {
        String code = "";
        if (!mIsVerify) {
            code = "11003";
            unitySendMessage("MjGetPayTokenData", "");
            unitySendMessage("MjGetPayTokenCallback", code);
        } else if ("".equals(mUserid)) {
            code = "10001";
            unitySendMessage("MjGetPayTokenData", "");
            unitySendMessage("MjGetPayTokenCallback", code);
            return;
        }
        //自启动支付服务
        mActivity.startService(new Intent(mActivity, AIDLService.class));
        innerCallBack = new MjPaySdkInnerCallBack() {

            @Override
            public void mjGetPayTokenValidationCallback(boolean isContinue) {
                if (isContinue) {
                    // request
                    MjHttp.getInstance(mActivity).getPayToken(mMerchantId, mAppId, mAppKey, mUserid, sum, clientOrder);
                } else {// 通知开发者，用户取消
                    // toast dev user
                    unitySendMessage("MjGetPayTokenCallback", "14002");
                }
            }
        };

        if (screenType != null && ("1".equals(screenType) || "2".equals(screenType))) {
            MjPaySdkUtil.startMjAppActivity(mActivity,
                    "com.mojing.sdk.pay.activity.MjPayValidationVrActivity"
                    , false
                    , sum
                    , screenType
            );
            return;
        }

        Intent intent = new Intent();
        intent.setClass(mActivity.getApplicationContext(), MojingPayValidationSingleActivity.class);
        intent.putExtra("sum", sum);
        mActivity.startActivity(intent);
    }

    /**
     * 内部回调
     *
     * @param {引入参数名} {引入参数说明}
     * @return {返回值说明}
     * @author liumansong @Date 2016-4-18 下午5:21:28
     */
    public static MjPaySdkInnerCallBack getInnerCallBack() {
        return innerCallBack;
    }

    /**
     * @param serviceArea 区服
     * @return void
     * @author linzanxian @Date 2015-10-8 下午3:35:47
     */
    public static void mjPayMobi(String mobi, String serviceArea, String payToken, String clientOrder) {
        String code = "";
        if ("".equals(mUserid)) {
            code = "10001";
            unitySendMessage("MjPayCallback", code);
            return;
        } else if (!mIsVerify) {
            code = "11003";
            unitySendMessage("MjPayCallback", code);
            return;
        } else if ("".equals(payToken)) {
            code = "12001";
            unitySendMessage("MjPayCallback", code);
            return;
        }

        MjHttp.getInstance(mActivity).payMobi(mMerchantId, mAppId, mAppKey, mUserid, mobi, serviceArea, payToken, clientOrder);
    }

    /**
     * 调用注册页
     *
     * @return void
     * @author linzanxian @Date 2015-10-8 下午7:12:16
     */
    public static void mjCallRegister() {
        if (!mIsVerify) {
            String code = "11003";
            unitySendMessage("MJLoginCallback", code);
            return;
        }

        MjPaySdkUtil.startMjAppActivity(mActivity, "com.baofeng.mj.user.activity.SdkRegisterActivity", true);
    }

    /**
     * 调用单屏登录
     *
     * @return void
     * @author linzanxian @Date 2015-10-8 下午7:12:55
     */
    public static void mjCallSingleLogin() {
        if (!mIsVerify) {
            String code = "11003";
            unitySendMessage("MJLoginCallback", code);
            return;
        }
        MjPaySdkUtil.startMjAppActivity(mActivity, "com.baofeng.mj.user.activity.SdkSingleLoginActivity", true);
    }

    /**
     * 调用双屏登录
     *
     * @return void
     * @author linzanxian @Date 2015-10-8 下午7:13:23
     */
    public static void mjCallDoubleLogin() {
        if (!mIsVerify) {
            String code = "11003";
            unitySendMessage("MJLoginCallback", code);
            return;
        }
        MjPaySdkUtil.startMjAppActivity(mActivity, "com.baofeng.mjgl.pubblico.layout.SdkDoubleLoginActivity", true);

    }

    /**
     * 是否登录
     *
     * @return void
     * @author linzanxian @Date 2015-10-13 下午5:29:46
     */
    public static boolean mjCheckLogin() {
        return "".equals(mUserid) ? false : true;
    }

    /**
     * 退出登录
     *
     * @return void
     * @author linzanxian @Date 2015-10-13 下午5:31:08
     */
    public static boolean mjLoginOut() {
        mUserid = "";

        return true;
    }

    /**
     * 接收登录成功后返回的用户ID
     *
     * @param uid 用户ID
     * @return void
     * @author linzanxian @Date 2015-10-13 下午2:38:48
     */
    public static void onLoginCallback(String uid) {
        mUserid = uid;

        unitySendMessage("MJLoginUid", uid);
        unitySendMessage("MJLoginCallback", "10000");
    }

    /**
     * 商户验证返回结果处理
     *
     * @param code 返回码
     * @return void
     * @author linzanxian @Date 2015-10-13 下午4:15:58
     */
    public static void onVerifyCallback(String code) {
        if (code.equals("11000")) {
            mIsVerify = true;
        }

        unitySendMessage("MjVerification", code);
    }

    /**
     * 是否启动当前activity
     *
     * @return boolean
     * @author linzanxian @Date 2015-10-16 下午2:41:27
     */
    public static boolean isStart() {
        return mIsStart;
    }

    /**
     * 自动登录
     *
     * @param uid 用户ID
     * @return void
     * @author linzanxian @Date 2016-1-5 下午2:51:04
     */
    public static void mjAutoLogin(String uid) {
        mUserid = uid;
    }

    /**
     * 获取魔豆
     *
     * @return void
     * @author linzanxian @Date 2016-1-5 下午1:59:29
     */
    public static void mjGetBalance() {
        MjHttp.getInstance(mActivity).getBalance(mUserid);
    }
}