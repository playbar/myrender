package com.baofeng.mojing.check;

import android.app.Activity;
import android.app.DownloadManager;
import android.content.Context;
import android.content.DialogInterface;
import android.net.Uri;

/**
 * Created by shiquanquan on 2017/5/11.
 */

public class ClickListener implements DialogInterface.OnClickListener {
    private Activity appCtx;
    private int actionType=0;//0 download 1重试 2正在下载 3install
    private long mID=0;

    public ClickListener(Activity appCtx, int actionType,long id) {
        this.appCtx = appCtx;
        this.actionType = actionType;
        this.mID=id;
    }



    @Override
    public void onClick(DialogInterface dialogInterface, int i) {
        switch (actionType){
            case 0:
                DownloadUtil.startDownload(appCtx, "下载暴风魔镜VR", null);
                break;
            case 1:
                DownloadManager dm = (DownloadManager) appCtx.getSystemService(Context.DOWNLOAD_SERVICE);
                dm.remove(mID);
                DownloadUtil.startDownload(appCtx, "重新下载暴风魔镜VR", null);
                break;
            case 2:
                appCtx.finish();
                break;
            case 3:
                String path=DownloadUtil.getDownloadedPathByID(appCtx, mID);
                DownloadUtil.promptInstall(appCtx, Uri.parse(path));
                appCtx.finish();
                break;
        }

    }
}
