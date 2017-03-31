package com.viewcore.test;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.opengl.Matrix;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

import com.baofeng.mojing.MojingSDK;
import com.bfmj.viewcore.interfaces.GLOnKeyListener;
import com.bfmj.viewcore.interfaces.GLViewFocusListener;
import com.bfmj.viewcore.render.GLColor;
import com.bfmj.viewcore.view.BaseViewActivity;
import com.bfmj.viewcore.view.GLCursorView;
import com.bfmj.viewcore.view.GLImageView;
import com.bfmj.viewcore.view.GLModelView;
import com.bfmj.viewcore.view.GLPanoView;
import com.bfmj.viewcore.view.GLRectView;
import com.bfmj.viewcore.view.GLRootView;
import com.bfmj.viewcore.view.GLTextView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

public class Model3dActivity extends BaseViewActivity {
    public static final int REQ_CODE_PERMISSION_REQUEST = 1000;

    public static final int SCENE_TYPE_DEFAULT = 0x0;
    public static final int SCENE_TYPE_CINEMA = 0x1;
    private int mSceneType = -1;

    private GLRootView rootView;
    private MyModelView modelView;

    public class MyModelView extends GLModelView {
        public boolean groyEnable = false;

        public MyModelView(Context context) {
            super(context);
            float[] prjMatrix = new float[16];
            float[] mvpMatrix = new float[16];
            float[] VMatrix = new float[16];
            Matrix.setLookAtM(VMatrix, 0,
                    0, 0, 20,
                    0, 0, 0,
                    0, 1, 0);
            Matrix.frustumM(prjMatrix, 0,
                    -1, 1,
                    -1, 1,
                    1, 2000);

            Matrix.multiplyMM(mvpMatrix, 0,
                    prjMatrix, 0,
                    VMatrix, 0);
//            Matrix.rotateM(mvpMatrix, 0,
//                    135, 0.0f, 1.0f, 0);
            this.setMatrix(mvpMatrix);
        }

        @Override
        public void draw() {
            if (groyEnable) {
                float[] tmpMatrix = new float[16];
                MojingSDK.getLastHeadView(tmpMatrix);
                float[] prjMatrix = new float[16];
                float[] mvpMatrix = new float[16];
                float[] VMatrix = new float[16];
                Matrix.setLookAtM(VMatrix, 0,
                        0, 0, 15,
                        0, 0, 0,
                        0, 1, 0);
                Matrix.frustumM(prjMatrix, 0,
                        -1, 1,
                        -1, 1,
                        3, 2000);

                Matrix.multiplyMM(mvpMatrix, 0,
                        VMatrix, 0, tmpMatrix, 0);

                Matrix.multiplyMM(mvpMatrix, 0,
                        prjMatrix, 0,
                        mvpMatrix, 0);
                this.setMatrix(mvpMatrix);
            }
            super.draw();
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
//        setContentView(R.layout.activity_model3d);
        ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE}, REQ_CODE_PERMISSION_REQUEST);

        rootView = getRootView();

        // text View
        {
            GLTextView textView = new GLTextView(this);
            textView.setX( 1000);
            textView.setY( 2000 );
            textView.setLayoutParams(1000, 200 );
            textView.setTextColor(new GLColor(0.0f, 1.0f, 1.0f));
            textView.setText("3d Model");
            textView.setTextSize(100);
            textView.setOnKeyListener(new GLOnKeyListener() {
                @Override
                public boolean onKeyDown(GLRectView view, int keycode) {
                    view.setAlpha( 0.3f );
                    return false;
                }

                @Override
                public boolean onKeyUp(GLRectView view, int keycode) {
                    view.setAlpha( 1.0f );
                    return false;
                }

                @Override
                public boolean onKeyLongPress(GLRectView view, int keycode) {
                    return false;
                }
            });
            textView.setFocusListener(new GLViewFocusListener() {
                @Override
                public void onFocusChange(GLRectView view, boolean focused) {
                    if( focused )
                        view.setAlpha( 0.3f );
                    else
                        view.setAlpha( 1.0f );
                }
            });

            //gridView.addView( textView );
            rootView.addView(textView);
        }

        // model view

        // cursor view
        {
            GLCursorView cursorView = new GLCursorView(this);
            cursorView.setX(1190);
            cursorView.setY(1190);
            cursorView.setLayoutParams( 20, 20);
            cursorView.setBackground(new GLColor(1.0f, 0, 0));
            cursorView.setDepth(4.0f);
            rootView.addView(cursorView);
        }
        {
            GLImageView imageView = new GLImageView(this);
            imageView.setLayoutParams(512, 512);
            imageView.setX(1000);
            imageView.setY(1000);
            imageView.setBackground(R.drawable.address_book);
//            rootView.addView(imageView);
        }

        loadModel();

        rootView.onResume();
        showSkyBox(SCENE_TYPE_DEFAULT);
//        rootView.setDoubleScreen(false);
//        setDistortionEnable(false);
    }

    private void loadModel(){
        {
            modelView = new MyModelView(this);
            if (modelView != null) {
                modelView.groyEnable = isGroyEnable();

                String objPath = "amenemhat";
                String filePath = getFilesDir().getAbsolutePath() + "/" + objPath;
                String fileName = filePath + "/amenemhat.obj";
                copyFilesFassets(this, objPath, filePath);

//            modelView.loadModel("/sdcard/model/amenemhat/amenemhat.obj");
//                modelView.loadModel("/mnt/sdcard/space/space_screen03.FBX");
                modelView.loadModel(fileName);
//                modelView.loadModel("/mnt/sdcard/space/space_screen03.FBX");
                rootView.addView(modelView);
            }
        }
    }


    String getFileName(String fileName) {
        String onlyName = "";
        int slashIndex = fileName.lastIndexOf('/');

        if (slashIndex != -1) {
            onlyName = fileName.substring(slashIndex + 1);
        }
        return onlyName;
    }

    /**
     * 从assets目录中复制整个文件夹内容
     *
     * @param context Context 使用CopyFiles类的Activity
     * @param oldPath String  原文件路径  如：/aa
     * @param newPath String  复制后路径  如：xx:/bb/cc
     */
    public void copyFilesFassets(Context context, String oldPath, String newPath) {
        try {
            String fileNames[] = context.getAssets().list(oldPath);//获取assets目录下的所有文件及目录名
            if (fileNames.length > 0) {//如果是目录
                File file = new File(newPath);
                file.mkdirs();//如果文件夹不存在，则递归
                for (String fileName : fileNames) {
                    copyFilesFassets(context, oldPath + "/" + fileName, newPath + "/" + fileName);
                }
            } else {//如果是文件
                InputStream is = context.getAssets().open(oldPath);
                FileOutputStream fos = new FileOutputStream(new File(newPath));
                byte[] buffer = new byte[1024];
                int byteCount = 0;
                while ((byteCount = is.read(buffer)) != -1) {//循环从输入流读取 buffer字节
                    fos.write(buffer, 0, byteCount);//将读取的输入流写入到输出流
                }
                fos.flush();//刷新缓冲区
                is.close();
                fos.close();
            }
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    /**
     * 显示天空盒场景
     * @param type 场景类型
     */
    public synchronized void showSkyBox(int type)
    {
        mSceneType = type;

        GLPanoView mSkyboxView = GLPanoView.getSharedPanoView(this);
        mSkyboxView.reset();
        if(type == SCENE_TYPE_CINEMA){
            mSkyboxView.setImage(R.drawable.skybox_cinema);
        } else {
            mSkyboxView.setImage(R.drawable.skybox_launcher);
        }

        mSkyboxView.setVisible(true);
    }
    public void setSkyboxFixed(boolean fixed){
        GLPanoView.getSharedPanoView(this).setFixed(fixed);
    }

    /**
     * 隐藏天空盒场景
     */
    public void hideSkyBox(){
        GLPanoView.getSharedPanoView(this).setVisible(false);
    }

//    @Override
//    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
//        if (requestCode == REQ_CODE_PERMISSION_REQUEST
//                && PackageManager.PERMISSION_GRANTED == ContextCompat.checkSelfPermission(Model3dActivity.this,
//                Manifest.permission.WRITE_EXTERNAL_STORAGE)){
//            modelView.loadModel("/mnt/sdcard/space/space_screen03.FBX");
//        }
//    }
}
