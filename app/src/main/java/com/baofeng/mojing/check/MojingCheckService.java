package com.baofeng.mojing.check;


import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;

import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.os.Build;
import android.util.Log;


/**
 * Created by shiquanquan on 2017/4/25.
 */

public class MojingCheckService {


    public static void checkService(final Activity appCtx) {
        try {
			requestPermission(appCtx);
            PackageManager pm = appCtx.getPackageManager();
            ApplicationInfo appInfo = pm.getApplicationInfo("com.baofeng.mj", PackageManager.GET_META_DATA);
            Log.d("MojingCheckService", "Service(com.baofeng.mj) is exist");
        } catch (PackageManager.NameNotFoundException e) {
        	Log.d("MojingCheckService", "Service(com.baofeng.mj) is not exist");
            long saveID = DownloadUtil.readID(appCtx);
            final int actionType=DownloadUtil.getActionTypeByID(appCtx,saveID);//0 download 1重试 2正在下载 3install
            String btnText=DownloadUtil.getBtnTextByActionType(actionType);
            new AlertDialog.Builder(appCtx)
                    .setMessage("运行该游戏需要先安装暴风魔镜VR")
                    .setPositiveButton(btnText, new ClickListener(appCtx,actionType,saveID))
                    .setCancelable(false)
                    .show();
        }catch (Exception e) {
			Log.d("MojingCheckService", "Service(com.baofeng.mj) is not exist: exception");
            e.printStackTrace();
        }
    }

	private static void requestPermission(Activity appCtx){
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
        	Log.d("MojingCheckService", "checkSelfPermission WRITE_EXTERNAL_STORAGE");
			if (appCtx.checkSelfPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE)
				!= PackageManager.PERMISSION_GRANTED) {
	        	    Log.d("MojingCheckService", "requestPermissions READ_EXTERNAL_STORAGE|WRITE_EXTERNAL_STORAGE|INTERNET");
        			appCtx.requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE,
                                                    Manifest.permission.WRITE_EXTERNAL_STORAGE,
                                                    Manifest.permission.INTERNET}, 1);
													}
        }
    }
}


//            Toast.makeText(appCtx,"本应用需要搭配应用<暴风魔镜pro>才能运行",Toast.LENGTH_LONG).show();