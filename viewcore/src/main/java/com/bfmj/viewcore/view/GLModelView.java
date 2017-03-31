package com.bfmj.viewcore.view;

import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLES30;
import android.opengl.Matrix;
import android.util.Log;

import com.bfmj.viewcore.entity.Model3dLib;

import baofeng.mojing.model3d.ModelObj;

/**
 * Created by liujiankang on 2016/10/31.
 */

public class GLModelView extends GLView {
    private float[] _SetMatrix = new float[]{
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f };

    private ModelObj _model = new ModelObj();
    private String  _modelName;
    private boolean _needLoad = false;

    public GLModelView(Context context) {
        super(context);
        init();
    }

    private void init() {
        if(!Model3dLib.getInstance()._loadLibrary) {
            return;
        }
        BaseViewActivity activity = (BaseViewActivity) getContext();
        AssetManager assetManager = activity.getAssets();
        String pathToInternalDir = activity.getFilesDir().getAbsolutePath();

        Log.i("AssimpDebug",  "path = " + pathToInternalDir);
        // call the native constructors to create an object
        _model.init(assetManager, pathToInternalDir);
    }

    private void uninit() {
        if(!Model3dLib.getInstance()._loadLibrary) {
            return;
        }
        _model.uninit();
    }

    public void setMatrix(float[] matrix) {
        System.arraycopy(matrix, 0, _SetMatrix, 0, 16);
    }

    public void loadModel(String modelName) {
        if(!Model3dLib.getInstance()._loadLibrary) {
            return;
        }

        _modelName = modelName;
        _needLoad = true;
    }

    private void DrawModels() {
        if(!Model3dLib.getInstance()._loadLibrary) {
            return;
        }
        _model.draw(_SetMatrix);
    }

    @Override
    public void initDraw() {}

    @Override
    public void draw() {
        if(_needLoad) {
            _needLoad = !_model.load(_modelName);
        }
        DrawModels();
    }

    @Override
    public void release() {
        uninit();
    }

    @Override
    public void onBeforeDraw(boolean isLeft) {
//        if(_needLoad) {
//            _needLoad = !_model.load(_modelName);
//        }
    }

    @Override
    public void onAfterDraw(boolean isLeft) {}

    @Override
    public void onSurfaceCreated() {}

    @Override
    public void onSurfaceChanged(int width, int height) {}
}
