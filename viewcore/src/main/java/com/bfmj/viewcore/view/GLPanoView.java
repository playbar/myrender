package com.bfmj.viewcore.view;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.SurfaceTexture;
import android.opengl.GLES11Ext;
import android.opengl.GLES30;
import android.opengl.Matrix;

import com.baofeng.mojing.MojingSDK;
import com.bfmj.viewcore.render.GLMesh;
import com.bfmj.viewcore.render.GLVector2;
import com.bfmj.viewcore.render.GLVector3;
import com.bfmj.viewcore.util.GLMatrixState;
import com.bfmj.viewcore.util.GLObjFileUtils;
import com.bfmj.viewcore.util.GLShaderManager;
import com.bfmj.viewcore.util.GLTextureUtils;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;

/**
 * Created by lixianke on 2016/7/29.
 */
public class GLPanoView extends GLView {
    public static final int RENDER_TYPE_IMAGE = 0x0;
    public static final int RENDER_TYPE_VIDEO = 0x1;

    public static final int SCENE_TYPE_SKYBOX = 0x0;
    public static final int SCENE_TYPE_SPHERE = 0x1;
    public static final int SCENE_TYPE_HALF_SPHERE = 0x2;

    public static final int PLAY_TYPE_2D = 0x0;
    public static final int PLAY_TYPE_3D_LR = 0x1; //左右3d
    public static final int PLAY_TYPE_3D_TB = 0x2; //上下3d

    private static final int RENDER_TYPE_COUNT = 2;
    private static final int SCENE_TYPE_COUNT = 3;

    private static final int PLAY_UV_2D = 0x0;
    private static final int PLAY_UV_3D_L = 0x1;
    private static final int PLAY_UV_3D_R = 0x2;
    private static final int PLAY_UV_3D_T = 0x3;
    private static final int PLAY_UV_3D_B = 0x4;
    private static final int PLAY_UV_COUNT = 5;

    private static final String[] SCENE_OBJS = {"skybox.obj", "sphere.obj", "half_sphere.obj"};

    private int mSceneType = SCENE_TYPE_SKYBOX;
    private int mPlayType = PLAY_TYPE_2D;
    private int mRenderType = RENDER_TYPE_IMAGE;

    private int[] mPrograms = new int[RENDER_TYPE_COUNT];
    private int[] muMVPMatrixHandles = new int[RENDER_TYPE_COUNT];
    private int[] muAlphaHandles = new int[RENDER_TYPE_COUNT];
    private int[] muMaskHandles = new int[RENDER_TYPE_COUNT];
    private int[] mPositionHandles = new int[RENDER_TYPE_COUNT];
    private int[] mTextureCoordHandles = new int[RENDER_TYPE_COUNT];

    private float[][] vertices = new float[SCENE_TYPE_COUNT][];
    private float[][][] texCoors = new float[SCENE_TYPE_COUNT][PLAY_UV_COUNT][];

    private int[] vboVertexBufferIds = new int[SCENE_TYPE_COUNT];
    private int[][] vboTexCoorBufferIds = new int[SCENE_TYPE_COUNT][PLAY_UV_COUNT];

    private boolean isSurfaceCreated = false;
    private boolean isNeedInitVertex = false;

    private SurfaceTexture mSurfaceTexture = null;
    private int mTextureId = -1;
    private int mResId = -1;
    private Bitmap mBitmap;
    private int mLeftTextureId = -1;
    private int mLeftResId = -1;
    private Bitmap mLeftBitmap;
    private boolean mVideoPrepared = false;

    private float[] mRotateHeadview = new float[3];

    private static GLPanoView instance = null;

    /**
     * 获取共享的GLPanoView
     * @param context 上下文
     * @return GLPanoView
     */
    public static  GLPanoView getSharedPanoView(Context context){
        if (instance == null){
            instance = new GLPanoView(context);
            instance.getRootView().addView(instance);
        }
        return instance;
    }

    public static void finish(){
        if (instance != null){
            GLES30.glDeleteBuffers(instance.vboVertexBufferIds.length, instance.vboVertexBufferIds, 0);
            for (int i = 0; i < instance.vboTexCoorBufferIds.length; i++){
                GLES30.glDeleteBuffers(instance.vboTexCoorBufferIds[i].length, instance.vboTexCoorBufferIds[i], 0);
            }
            instance = null;
        }
    }

    private GLPanoView(Context context) {
        super(context);
        new Thread(new Runnable() {
            @Override
            public void run() {
                loadObj();
            }
        }).start();
    }

    private void loadObj(){
        for (int i = 0; i < SCENE_TYPE_COUNT && i < SCENE_OBJS.length; i++){
            setObjFile(SCENE_OBJS[i], i);
            isNeedInitVertex = true;
        }
    }

    /**
     * 设置OBJ模型文件
     * @author lixianke  @Date 2015-3-12 下午3:45:46
     * @param filename 文件名，需将obj文件放置到assets文件夹
     * @return
     */
    private void setObjFile(String filename, int sceneType){
        GLMesh mesh = GLObjFileUtils.LoadMesh(getContext().getResources(), filename);

        int vCount = mesh.getVertexIndexs().length;
        vertices[sceneType] = new float[vCount * 3];
        for(int i=0;i < vCount; i++){
            GLVector3 vector3 = mesh.getVertices()[mesh.getVertexIndexs()[i]];
            vertices[sceneType][i * 3]= vector3.getX();
            vertices[sceneType][i * 3 + 1]= vector3.getY();
            vertices[sceneType][i * 3 + 2]= vector3.getZ();
        }

        int tCount = mesh.getTextureIndexs().length;
        for (int m = 0; m < PLAY_UV_COUNT; m++){
            texCoors[sceneType][m] = new float[tCount * 2];
        }
        for(int i=0;i < tCount; i++){
            GLVector2 vector2 = mesh.getTextures()[mesh.getTextureIndexs()[i]];
            texCoors[sceneType][PLAY_UV_2D][i * 2] = vector2.getS();
            texCoors[sceneType][PLAY_UV_2D][i * 2 + 1] = vector2.getT();
            texCoors[sceneType][PLAY_UV_3D_L][i * 2] = vector2.getS() / 2;
            texCoors[sceneType][PLAY_UV_3D_L][i * 2 + 1] = vector2.getT();
            texCoors[sceneType][PLAY_UV_3D_R][i * 2] = vector2.getS() / 2 + 0.5F;
            texCoors[sceneType][PLAY_UV_3D_R][i * 2 + 1] = vector2.getT();
            texCoors[sceneType][PLAY_UV_3D_T][i * 2] = vector2.getS();
            texCoors[sceneType][PLAY_UV_3D_T][i * 2 + 1] = vector2.getT() / 2;
            texCoors[sceneType][PLAY_UV_3D_B][i * 2] = vector2.getS();
            texCoors[sceneType][PLAY_UV_3D_B][i * 2 + 1] = vector2.getT() / 2 + 0.5F;
        }
    }

    /**
     * 获取场景类型
     * @return SCENE_TYPE_SKYBOX SCENE_TYPE_SPHERE  SCENE_TYPE_HALF_SPHERE
     */
    public int getSceneType() {
        return mSceneType;
    }

    /**
     * 设置场景类型
     * @param sceneType SCENE_TYPE_SKYBOX SCENE_TYPE_SPHERE  SCENE_TYPE_HALF_SPHERE
     */
    public void setSceneType(int sceneType) {
        mSceneType = sceneType;
    }

    /**
     *  获取播放类型
     * @return PLAY_TYPE_2D PLAY_TYPE_3D_LR  PLAY_TYPE_3D_TB
     */
    public int getPlayType() {
        return mPlayType;
    }

    /**
     * 设置播放类型
     * @param playType PLAY_TYPE_2D PLAY_TYPE_3D_LR  PLAY_TYPE_3D_TB
     */
    public void setPlayType(int playType) {
        mPlayType = playType;
    }

    /**
     * 获取渲染类型
     * @return RENDER_TYPE_IMAGE RENDER_TYPE_VIDEO
     */
    public int getRenderType() {
        return mRenderType;
    }

    /**
     * 设置然后类型
     * @param renderType RENDER_TYPE_IMAGE RENDER_TYPE_VIDEO
     */
    public void setRenderType(int renderType) {
        mRenderType = renderType;

        if (renderType == RENDER_TYPE_VIDEO){
            mTextureId = GLTextureUtils.createVideoTextureID();
            if (mTextureId > -1){
                mSurfaceTexture = new SurfaceTexture(mTextureId);
            }
        } else {
            mSurfaceTexture = null;
            mVideoPrepared = false;
        }
    }

    public void reset(){
        mRenderType = RENDER_TYPE_IMAGE;
        mSceneType = SCENE_TYPE_SKYBOX;
        mPlayType = PLAY_TYPE_2D;
        mRotateHeadview = new float[3];
        GLTextureUtils.releaseTexture(mTextureId);
        GLTextureUtils.releaseTexture(mLeftTextureId);
        mTextureId = -1;
        mSurfaceTexture = null;
        mResId = -1;
        mBitmap = null;
        mLeftTextureId = -1;
        mLeftResId = -1;
        mLeftBitmap = null;
        mVideoPrepared = false;
    }

    /**
     * 设置图片
     * @author lixianke  @Date 2015-3-11 下午5:06:59
     * @param resId 资源ID
     * @return
     */
    public void setImage(int resId){
        if (mResId == resId){
            return;
        }

        mResId = resId;
        mBitmap = null;

        if (isSurfaceCreated){
            getRootView().mCreateTextureQueue.offer(this);
        }
    }

    public void setLeftImage(int resId){
        if (mLeftResId == resId){
            return;
        }

        mLeftResId = resId;
        mLeftBitmap = null;

        if (isSurfaceCreated){
            getRootView().mCreateTextureQueue.offer(this);
        }
    }

    /**
     * 设置图片
     * @author lixianke  @Date 2015-3-11 下午5:07:28
     * @param bitmap Bitmap对象
     * @return
     */
    public void setImage(Bitmap bitmap){
        if (mBitmap == bitmap){
            return;
        }

        mBitmap = bitmap;
        mResId = -1;

        if (isSurfaceCreated){
            getRootView().mCreateTextureQueue.offer(this);
        }
    }

    public void setLeftImage(Bitmap bitmap){
        if (mLeftBitmap == bitmap){
            return;
        }

        mLeftBitmap = bitmap;
        mLeftResId = -1;

        if (isSurfaceCreated){
            getRootView().mCreateTextureQueue.offer(this);
        }
    }

    /**
     * h
     * @return
     */
    public SurfaceTexture getSurfaceTexture(){
        mVideoPrepared = true;
        return mSurfaceTexture;
    }

    /**
     * 旋转场景
     * @author lixianke  @Date 2015-6-19 下午5:39:51
     * @param angle 旋转角度
     * @param rx 沿x的向量分量
     * @param ry 沿y的向量分量
     * @param rz 沿z的向量分量
     * @return
     */
    public void rotate(float angle, float rx, float ry, float rz){
        Matrix.rotateM(getMatrixState().getCurrentMatrix(), 0, angle, rx, ry, rz);
    }

    public void setRotateHeadview(float[] view){
        mRotateHeadview = view;
    }

    @Override
    public void initDraw() {
        if (!isSurfaceCreated){
            isSurfaceCreated = true;
            isNeedInitVertex = true;
            createProgram();
            createTexture();
        }
    }

    @Override
    public void onBeforeDraw(boolean isLeft) {
        if (!isVisible() || !isSurfaceCreated || vertices[mSceneType] == null || mTextureId < 0){
            return;
        }

        if (mRenderType == RENDER_TYPE_VIDEO && !mVideoPrepared){
            return;
        }

        if (isNeedInitVertex){
            initVertex();
            initTextureBuffer();
        }

        if (mRenderType == RENDER_TYPE_VIDEO && mSurfaceTexture != null && isLeft){
            try {
                mSurfaceTexture.updateTexImage();
                float[] mtx = new float[16];
                mSurfaceTexture.getTransformMatrix(mtx);
            } catch (RuntimeException e) {
                return;
            }
        }

        GLMatrixState state = getMatrixState();

        if (isFixed()){
            float[] mtx = new float[16];
            Matrix.setIdentityM(mtx, 0);
            float[] out = new float[3];
            MojingSDK.getLastHeadEulerAngles(out);
            double zAngle = Math.toDegrees(out[2]);

            if (Math.abs(zAngle) > 90.0){
                Matrix.rotateM(mtx, 0, 180, 0, 0, 1);
            }
            state.setVMatrix(mtx);
        }

        float[] vMtx = state.getVMatrix();
        Matrix.rotateM(vMtx, 0, mRotateHeadview[0], 1, 0, 0);
        Matrix.rotateM(vMtx, 0, mRotateHeadview[1], 0, 1, 0);
        Matrix.rotateM(vMtx, 0, mRotateHeadview[2], 0, 0, 1);

        getEyeMatrix(vMtx, isLeft);

        state.pushMatrix();

        if (mRenderType == RENDER_TYPE_VIDEO){
            GLES30.glDisable(GLES30.GL_DEPTH_TEST);
        } else {
            GLES30.glBlendFunc(GLES30.GL_SRC_ALPHA, GLES30.GL_ONE_MINUS_SRC_ALPHA);
            GLES30.glEnable(GLES30.GL_BLEND);
        }

        GLES30.glUseProgram(mPrograms[mRenderType]);

        GLES30.glActiveTexture(GLES30.GL_TEXTURE0);

        if( isLeft && mLeftTextureId > -1 ){
            GLES30.glBindTexture(mRenderType == RENDER_TYPE_VIDEO ? GLES11Ext.GL_TEXTURE_EXTERNAL_OES :
                    GLES30.GL_TEXTURE_2D, mLeftTextureId);
        }else {
            GLES30.glBindTexture(mRenderType == RENDER_TYPE_VIDEO ? GLES11Ext.GL_TEXTURE_EXTERNAL_OES :
                    GLES30.GL_TEXTURE_2D, mTextureId);
        }

        GLES30.glUniformMatrix4fv(muMVPMatrixHandles[mRenderType], 1, false, getMatrixState().getFinalMatrix(), 0);
        GLES30.glUniform1f(muAlphaHandles[mRenderType], getAlpha());
        GLES30.glUniform1f(muMaskHandles[mRenderType], getMask());

        vertexVBO();

        int uvType = PLAY_UV_2D;
        if (mPlayType == PLAY_TYPE_3D_LR){
            if (isLeft){
                uvType = PLAY_UV_3D_L;
            } else {
                uvType = PLAY_UV_3D_R;
            }
        } else if (mPlayType == PLAY_TYPE_3D_TB) {
            if (isLeft){
                uvType = PLAY_UV_3D_T;
            } else {
                uvType = PLAY_UV_3D_B;
            }
        }
        textureVBO(uvType);
        GLES30.glDrawArrays(GLES30.GL_TRIANGLES, 0, vertices[mSceneType].length / 3);

        GLES30.glDisableVertexAttribArray(0);
        GLES30.glDisableVertexAttribArray(1);

        if (mRenderType == RENDER_TYPE_VIDEO){
            GLES30.glEnable(GLES30.GL_DEPTH_TEST);
        } else {
            GLES30.glDisable(GLES30.GL_BLEND);
        }
        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, 0);

        state.popMatrix();

    }

    @Override
    public void draw() {
    }

    private void createProgram(){
        String[] fs = new String[]{GLShaderManager.FRAGMENT_SENCE_IMAGE, GLShaderManager.FRAGMENT_SENCE_VIDEO};
        for (int i = 0; i < RENDER_TYPE_COUNT; i++){
            int vertexShader    = GLShaderManager.loadShader(GLES30.GL_VERTEX_SHADER, GLShaderManager.VERTEX_SENCE);
            int fragmentShader  = GLShaderManager.loadShader(GLES30.GL_FRAGMENT_SHADER, fs[i]);

            mPrograms[i] = GLES30.glCreateProgram();       // create empty OpenGL ES Program
            GLES30.glAttachShader(mPrograms[i], vertexShader);   // add the vertex shader to program
            GLES30.glAttachShader(mPrograms[i], fragmentShader); // add the fragment shader to program
            GLES30.glLinkProgram(mPrograms[i]);                  // creates OpenGL ES program executables
            muMVPMatrixHandles[i] = GLES30.glGetUniformLocation(mPrograms[i], "uMVPMatrix");
            muAlphaHandles[i] = GLES30.glGetUniformLocation(mPrograms[i], "uAlpha");
            muMaskHandles[i] = GLES30.glGetUniformLocation(mPrograms[i], "uMask");
            mTextureCoordHandles[i] = GLES30.glGetAttribLocation(mPrograms[i], "inputTextureCoordinate");
            mPositionHandles[i] = GLES30.glGetAttribLocation(mPrograms[i], "aPosition");
        }
    }

    private void vertexVBO() {

        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, vboVertexBufferIds[mSceneType]);
        // 传送顶点位置数据
        GLES30.glVertexAttribPointer(mPositionHandles[mRenderType], 3, GLES30.GL_FLOAT,
                false, 0, 0);
        GLES30.glEnableVertexAttribArray(0);
    }

    private void textureVBO(int uvType) {

        GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, vboTexCoorBufferIds[mSceneType][uvType]);
        // 传送顶点位置数据
        GLES30.glVertexAttribPointer(mTextureCoordHandles[mRenderType], 2, GLES30.GL_FLOAT,
                false, 0, 0);
        GLES30.glEnableVertexAttribArray(1);

    }

    protected void initVertex(){
        isNeedInitVertex = false;
        for (int i = 0; i < vertices.length; i++){
            if (vertices[i] != null) {
                int verLen = vertices[i].length * 4;
                ByteBuffer bb = ByteBuffer.allocateDirect(verLen);
                bb.order(ByteOrder.nativeOrder());
                FloatBuffer vertexBuffer = bb.asFloatBuffer();
                vertexBuffer.put(vertices[i]);
                vertexBuffer.position(0);

                if (vboVertexBufferIds[i] < 1){
                    IntBuffer buffers = IntBuffer.allocate(1);
                    GLES30.glGenBuffers(1,buffers);
                    vboVertexBufferIds[i] = buffers.get(0);
                }
                GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, vboVertexBufferIds[i]);
                GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER, verLen, vertexBuffer,
                        GLES30.GL_STATIC_DRAW);
            }
        }

    }

    private void initTextureBuffer(){
        for (int i = 0; i < texCoors.length; i++){
            for (int j = 0; j < texCoors[i].length; j++){
                if (texCoors[i][j] != null){
                    int textureLen = texCoors[i][j].length*4;
                    ByteBuffer llbb = ByteBuffer.allocateDirect(textureLen);
                    llbb.order(ByteOrder.nativeOrder());
                    FloatBuffer textureVerticesBuffer=llbb.asFloatBuffer();
                    textureVerticesBuffer.put(texCoors[i][j]);
                    textureVerticesBuffer.position(0);

                    if (vboTexCoorBufferIds[i][j] < 1){
                        IntBuffer buffers = IntBuffer.allocate(1);
                        GLES30.glGenBuffers(1,buffers);
                        vboTexCoorBufferIds[i][j] = buffers.get(0);
                    }
                    GLES30.glBindBuffer(GLES30.GL_ARRAY_BUFFER, vboTexCoorBufferIds[i][j]);
                    GLES30.glBufferData(GLES30.GL_ARRAY_BUFFER, textureLen, textureVerticesBuffer,
                            GLES30.GL_STATIC_DRAW);
                }
            }
        }
    }

    @Override
    public void createTexture(){
        if (!isSurfaceCreated){
            return;
        }

        boolean isRecycle = true;
        Bitmap bitmap = null;

        /// right
        if (mResId > -1){
            InputStream is = getContext().getResources().openRawResource(mResId);
            try {
                bitmap = BitmapFactory.decodeStream(is);
            } finally {
                try {
                    is.close();
                } catch(IOException e) {
                    e.printStackTrace();
                }
            }
        } else if (mBitmap != null){
            bitmap = mBitmap;
            isRecycle = false;
        }

        if (bitmap != null){
            releaseTexture(mTextureId);
            mTextureId = GLTextureUtils.initImageTexture(bitmap, isRecycle, false);
        }

        /// left
        isRecycle = true;
        bitmap = null;
        if (mLeftResId > -1){
            InputStream is = getContext().getResources().openRawResource(mLeftResId);
            try {
                bitmap = BitmapFactory.decodeStream(is);
            } finally {
                try {
                    is.close();
                } catch(IOException e) {
                    e.printStackTrace();
                }
            }
        } else if (mLeftBitmap != null){
            bitmap = mLeftBitmap;
            isRecycle = false;
        }

        if (bitmap != null){
            releaseTexture(mLeftTextureId);
            mLeftTextureId = GLTextureUtils.initImageTexture(bitmap, isRecycle, false);
        }
    }

    @Override
    public void onAfterDraw(boolean isLeft) {}

    @Override
    public void onSurfaceCreated() {}

    @Override
    public void onSurfaceChanged(int width, int height) {}

    @Override
    public void release() {
        isSurfaceCreated = false;
        if (mTextureId > -1 || mLeftTextureId > -1){
            BaseViewActivity activity = (BaseViewActivity)getContext();
            if (activity != null && activity.getRootView() != null) {
                activity.getRootView().queueEvent(new Runnable() {

                    @Override
                    public void run() {
                        if( mTextureId > -1) {
                            GLTextureUtils.releaseTexture(mTextureId);
                            mTextureId = -1;
                        }
                        if( mLeftTextureId > -1){
                            GLTextureUtils.releaseTexture(mLeftTextureId);
                            mLeftTextureId = -1;
                        }

                    }
                });
            }
        }
    }
}
