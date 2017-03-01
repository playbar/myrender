package com.qualcomm.svrapitest;

import android.app.NativeActivity;
import android.os.Bundle;
import android.util.Log;

public class MyNativeActivity extends NativeActivity {

    static
    {
//        System.loadLibrary( "svrapi" );
//        System.loadLibrary( "apitest" );
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        System.loadLibrary( "svrapi" );
        System.loadLibrary( "apitest" );
        Log.v("test","oncreate");
    }

    public void LoadWebSite(String url){
        Log.v("test", "GetWebPicture, url is:" + url);
    }
}
