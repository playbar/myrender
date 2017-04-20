package com.bfmj.viewcore.entity;

import android.util.Log;

/**
 * Created by liujiankang on 2016/11/7.
 */
public class Model3dLib {
    private static Model3dLib ourInstance = new Model3dLib();
    public static boolean _loadLibrary = false;

    public static Model3dLib getInstance() {
        return ourInstance;
    }

    private Model3dLib() {
    }

    public void loadModel3dLib() {
        try {
            System.loadLibrary("model3d");
            _loadLibrary = true;
        }catch (Error error) {
            // shared library
            _loadLibrary = false;
            Log.i("Model3dLib", "loadModel3dLib: c++ shared library not exist || " + error.toString());
        }
    }
}
