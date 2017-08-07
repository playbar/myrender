package com.baofeng.mojing.check;

import android.app.DownloadManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.util.Log;

/**
 * Created by shiquanquan on 2017/5/10.
 */

public class DownloadReceiver extends BroadcastReceiver {

    @Override
    public void onReceive(Context context, Intent intent) {
        long savedID=DownloadUtil.readID(context);
        long downloadCompletedId = intent.getLongExtra(DownloadManager.EXTRA_DOWNLOAD_ID, 0);
        Log.i("test", "received id:" + downloadCompletedId + "saved:" + savedID);
        if (savedID != downloadCompletedId) {
            return;
        }
        int status = DownloadUtil.getDownloadStatusByID(context, downloadCompletedId);
        // 下载失败也会返回这个广播，所以要判断下是否真的下载成功
        if (DownloadManager.STATUS_SUCCESSFUL == status) {
            String path=DownloadUtil.getDownloadedPathByID(context, downloadCompletedId);
            Log.i("test", "uri:"+path);
            DownloadUtil.promptInstall(context, Uri.parse(path));
        }

    }
}
