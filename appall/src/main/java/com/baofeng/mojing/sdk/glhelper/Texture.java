package com.baofeng.mojing.sdk.glhelper;

import android.opengl.GLES20;

/**
 * Created by xuxiang on 2017/3/31.
 */
public class Texture {
    public int lastglerror;
    public int textureID;
    public int width;
    public int height;
    public int depth;
    public boolean isMipmapped = false;

    long glType = 0;
    long glTypeSize = 1;
    long glFormat;
    long glInternalFormat;
    long glBaseInternalFormat;

}
