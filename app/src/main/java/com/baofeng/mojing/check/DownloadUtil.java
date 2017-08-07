package com.baofeng.mojing.check;

import android.app.Activity;
import android.app.DownloadManager;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.database.Cursor;
import android.net.Uri;
import android.os.Environment;
import android.util.Log;
import android.app.AlertDialog;

import java.io.File;

import org.json.JSONException;
import org.json.JSONObject;

import com.androidquery.AQuery;
import com.androidquery.callback.AjaxCallback;
import com.androidquery.callback.AjaxStatus;

/**
 * Created by shiquanquan on 2017/5/10.
 */

public class DownloadUtil {
	final static String TAG = "DownloadUtil";

    public static void saveID(Context appCtx, long id) {
        SharedPreferences preferences = appCtx.getSharedPreferences("downloadmj", Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = preferences.edit();
        editor.putLong("id", id);
        editor.commit();
    }

    public static long readID(Context appCtx) {
        SharedPreferences preferences = appCtx.getSharedPreferences("downloadmj", Context.MODE_PRIVATE);
        long savedId = preferences.getLong("id",0);
        return savedId;
    }

    public static int getDownloadStatusByID(Context appCtx,long id){
        int ret=0;
        DownloadManager dm= (DownloadManager) appCtx.getSystemService(Context.DOWNLOAD_SERVICE);
        DownloadManager.Query query = new DownloadManager.Query();
        query.setFilterById(id);
        Cursor c=null;
        try {
            c = dm.query(query);
            if (c != null && c.moveToFirst()) {
                int columnIndex = c.getColumnIndex(DownloadManager.COLUMN_STATUS);
                ret = c.getInt(columnIndex);
            }
        }catch (Exception e){
            e.printStackTrace();
        }finally {
            if(c!=null) {
                c.close();
            }
        }
        return ret;
    }


    public static String getDownloadedPathByID(Context appCtx, long id){
        String ret="";
        DownloadManager dm= (DownloadManager) appCtx.getSystemService(Context.DOWNLOAD_SERVICE);
        DownloadManager.Query query = new DownloadManager.Query();
        query.setFilterById(id);
        Cursor c=null;
        try {
            c = dm.query(query);
            if (c != null && c.moveToFirst()) {
                int columnIndex = c.getColumnIndex(DownloadManager.COLUMN_LOCAL_URI);
                ret = c.getString(columnIndex);
            }
        }catch (Exception e){
            e.printStackTrace();
        }finally {
            if(c!=null) {
                c.close();
            }
        }
        return ret;
    }

    public static void startDownload(final Activity appCtx, final String title, final String description) {
    	Log.i(TAG, "startDownload...");       
        //long time =  (System.currentTimeMillis() / 1000); 
        StringBuffer parameterSign = new StringBuffer();
        parameterSign.append("1").append("Bf@)(*$s1&2^3XVF#Mj");
        
    	AQuery aq = new AQuery(appCtx);
        StringBuffer parameter = new StringBuffer();
        parameter.append("system=").append("1"); 
        parameter.append("&sign=").append(MD5Util.MD5(parameterSign.toString()));

        String url = "http://fuwu.mojing.cn/api/appdownload";
        url += "?" + parameter.toString();
        //Log.i(TAG, "MJApp query url = " + url);

        aq.ajax(url, JSONObject.class, new AjaxCallback<JSONObject>() {

        	@Override
            public void callback(String url, JSONObject jsonObject, AjaxStatus status) {
            	// 处理结果
                //String code = "00000"; // 网络异常
                //String failedJson = "";   
            	//Log.i(TAG, "MJApp query callback"); 
				boolean bDownloadStart = false; 
             	if (jsonObject != null) {
					try {
						//Log.i(TAG, "MJApp query result: " + jsonObject.toString());  
						String result = jsonObject.getString("code");
						if (result.equals("0")) {
							//code = "14000";
							JSONObject jo = jsonObject.getJSONObject("data");
							if (jo != null) {  
								String strUrlResult = jo.getString("download_url");
								Log.i(TAG, "MJApp download_url: " + strUrlResult); 
               
								DownloadManager dm = (DownloadManager) appCtx.getSystemService(Context.DOWNLOAD_SERVICE);
                                DownloadManager.Request req = new DownloadManager.Request(Uri.parse(strUrlResult));

                                req.setAllowedNetworkTypes(DownloadManager.Request.NETWORK_WIFI);
                                //req.setAllowedOverRoaming(false);

                                req.setNotificationVisibility(DownloadManager.Request.VISIBILITY_VISIBLE_NOTIFY_COMPLETED);

                                //设置文件的保存的位置[三种方式]
                                //第一种
                                //file:///storage/emulated/0/Android/data/your-package/files/Download/update.apk
                                //req.setDestinationInExternalFilesDir(context, Environment.DIRECTORY_DOWNLOADS, "update.apk");
                                //第二种
                                //file:///storage/emulated/0/Download/update.apk
                                req.setDestinationInExternalPublicDir(Environment.DIRECTORY_DOWNLOADS, "暴风魔镜VR.apk");

                                // 设置一些基本显示信息
                                req.setTitle(title);
                                req.setDescription(description);
                                req.setMimeType("application/vnd.android.package-archive");

                                //加入下载队列
                                long id = dm.enqueue(req); 
                                DownloadUtil.saveID(appCtx,id);
								bDownloadStart = true;
							}        
						} 
						else {
							Log.e(TAG, "MJApp query result code: " + result); 
                             //code = "14001";
                             //failedJson = jsonObject.toString();
                        }
					} 
					catch (JSONException e) {
					    Log.e(TAG, "MJApp query JSONException");  
                        e.printStackTrace();
					}
				}
             	else
             	{
					Log.e(TAG, "MJApp query faied!");  
             	}
				if(!bDownloadStart)
				{
					//long saveID = readID(appCtx);
					 new AlertDialog.Builder(appCtx)
                    .setMessage("下载暴风魔镜VR异常，请检查网络设置。")
                    .setPositiveButton("确定", new ClickListener(appCtx,2,0))
                    .setCancelable(false)
                    .show();
				}
				else
				{
					appCtx.finish();
				}
			}
		});	    
    }

    public static int getActionTypeByID(Context appCtx,long saveID){
        int actiontype=0;
        if(saveID!=0){
            int status=DownloadUtil.getDownloadStatusByID(appCtx,saveID);
            Log.i(TAG,"status"+status);
            switch(status){
                case DownloadManager.STATUS_FAILED://重试
                case DownloadManager.STATUS_PAUSED://重试
                    actiontype=1;
                    break;
                case DownloadManager.STATUS_PENDING://正在下载
                case DownloadManager.STATUS_RUNNING://正在下载
                    actiontype=2;
                    break;
                case DownloadManager.STATUS_SUCCESSFUL://安装or重试
                    String path=DownloadUtil.getDownloadedPathByID(appCtx,saveID);
                    path=path.replace("file://","");
                    Log.i("test","path:"+path);
                    File f=new File(path);
                    if(f.exists()){
                        actiontype=3;
                    }else{
                        actiontype=1;
                    }
                    break;
            }
        }
        return  actiontype;
    }

    public static String getBtnTextByActionType(int actionType){
        String btnText="立即下载";
        switch(actionType){
            case 1:
                btnText="重试下载";
                break;
            case 2:
                btnText="下载中";
                break;
            case 3:
                btnText="安装暴风魔镜";
                break;
        }
        return  btnText;
    }

    public static void promptInstall(Context ctx, Uri data) {
        Log.i(TAG, "install");
        Intent promptInstall = new Intent(Intent.ACTION_VIEW)
                .setDataAndType(data, "application/vnd.android.package-archive");
        promptInstall.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        ctx.startActivity(promptInstall);
    }
}
