package baofeng.mojing.model3d;

import android.content.res.AssetManager;

/**
 * Created by liujiankang on 2016/11/7.
 */

public class ModelObj {
    private native void CreateObjectNative(AssetManager assetManager, String pathToInternalDir);
    private native void DeleteObjectNative();
    private native void DrawFrameNative(float[] mvpMatrix);
    private native boolean LoadModel(String modelName);

    public void init(AssetManager assetManager, String modelPath) {
        CreateObjectNative(assetManager, modelPath);
    }

    public void uninit() {
        DeleteObjectNative();
    }

    public void draw(float[] mvpMatrix) {
        DrawFrameNative(mvpMatrix);
    }

    public boolean load(String modelName) {
       return LoadModel(modelName);
    }
}
