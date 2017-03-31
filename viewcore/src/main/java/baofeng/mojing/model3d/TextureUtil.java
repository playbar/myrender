package baofeng.mojing.model3d;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES30;
import android.opengl.GLUtils;
import android.util.Log;

import java.io.File;
import java.nio.IntBuffer;

/**
 * Created by liujiankang on 2016/11/14.
 */

public class TextureUtil {

    static int CreateTextureFromPic(String picPath){
        IntBuffer idTexture = IntBuffer.allocate(1);
        GLES30.glGenTextures(1, idTexture);
        try
        {
            Bitmap bitmap = null;
            File file = new File(picPath);
            if(file.exists())
            {
                bitmap = BitmapFactory.decodeFile(picPath);
//                Log.i("TextureUtil", "CreateTextureFromPic width=[" + bitmap.getWidth() + "] height=["+ bitmap.getHeight() + "]");
            }
            if (idTexture.get(0) > 0)
            {
                // bind the texture
                GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, idTexture.get(0));
                // specify linear filtering
                GLES30.glTexParameteri(GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_MAG_FILTER, GLES30.GL_LINEAR);
                GLES30.glTexParameteri(GLES30.GL_TEXTURE_2D, GLES30.GL_TEXTURE_MIN_FILTER, GLES30.GL_LINEAR);
                // load bitmap to texture
                GLUtils.texImage2D(GLES30.GL_TEXTURE_2D, 0, bitmap, 0);
                checkGlError("texTmage2D");
            }
            bitmap.recycle();
            return idTexture.get(0);
        } catch (Exception e)
        {
            // TODO: handle exception
        }

        return idTexture.get(0);
    }

    static void DeleteTextureId(int textureID) {
        Log.i("TextureUtil", "DeleteTextureId : " + textureID);
        GLES30.glBindTexture( GLES30.GL_TEXTURE_2D, textureID);
        int[] tID = {textureID};
        GLES30.glDeleteTextures(1, tID, 0);
        GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, 0);
        checkGlError("DeleteTextureId");
    }

    static private void checkGlError(String op) {
        int error;
        while ((error = GLES30.glGetError()) != GLES30.GL_NO_ERROR) {
            Log.e("TextureUtil", op + ": glError " + error);
            throw new RuntimeException(op + ": glError " + error);
        }
    }
}
