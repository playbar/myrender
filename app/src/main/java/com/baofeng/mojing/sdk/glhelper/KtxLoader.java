package com.baofeng.mojing.sdk.glhelper;

import android.opengl.GLES20;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

/**
 * Created by xuxiang on 2017/3/31.
 */
public class KtxLoader {
    static
    {
        System.loadLibrary("glHelper");
    }
    public int LoadTextureFromStream(InputStream is, Texture texture)
    {
	    int length = 0;
        byte[] buffer;
        try {
			length = is.available();
			buffer = new byte[length];
            is.read(buffer);

        } catch (IOException e) {
            e.printStackTrace();
            return 8;
        }
        return LoadTextureFromMemory(buffer, texture);
    }

    public native int LoadTextureFromMemory(byte[] bytes, Texture texture);
    public native int LoadTextureFromFile(String fileName, Texture texture);
    public native int SaveTextureToFile(String fileName, Texture texture);

}
