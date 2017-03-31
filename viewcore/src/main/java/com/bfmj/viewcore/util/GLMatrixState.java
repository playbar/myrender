package com.bfmj.viewcore.util;

import android.opengl.Matrix;

/**
 * 
 * ClassName: GLMatrixState <br/>
 * @author lixianke    
 * @date: 2015-3-9 上午9:49:47 <br/>  
 * description:
 */
public class GLMatrixState {
	private float[] mProjMatrix = new float[16];
	private float[] mVMatrix = new float[16];
	private float[] mTranslationMatrix = new float[16];
	private float[] mScaleMatrix = new float[16];
	private float[] mRotateMatrix = new float[16];
    private float[] currMatrix;
    
    //保护变换矩阵的栈  
    private float[][] mStack=new float[10][16];
    private int stackTop=-1;
    
    /**
     * 初始化矩阵
     * @author lixianke  @Date 2015-3-20 上午11:59:13
     * @param oMatrix 矩阵 
     * @return
     */
    public static float[] getInitMatrix(){
    	float[] matrix = new float[16];
    	Matrix.setRotateM(matrix, 0, 0, 1, 0, 0);
    	return matrix;
    }
    
    
    public GLMatrixState(){
    	initStack();
    }
    
    private void initStack(){
    	currMatrix = getInitMatrix();
    	mTranslationMatrix = getInitMatrix();
    	mScaleMatrix = getInitMatrix();
    	mRotateMatrix = getInitMatrix();
    }
  
    //保护变换矩阵
    public void pushMatrix(){
    	stackTop++;
//    	for(int i=0;i<16;i++)
//    	{
//    		mStack[stackTop][i]=currMatrix[i];
//    	}
        System.arraycopy(currMatrix, 0, mStack[stackTop], 0, 16);
    }
    
    //恢复变换矩阵
    public void popMatrix(){
        if (stackTop < 0){
            stackTop = 0;
        }
//    	for(int i=0;i<16;i++)
//    	{
//    		currMatrix[i]=mStack[stackTop][i];
//    	}
        System.arraycopy(mStack[stackTop], 0, currMatrix, 0, 16);
    	stackTop--;
    }
    
    public float[] getFinalMatrix(){
//    	float[] mMVPMatrix=new float[16];
//    	Matrix.multiplyMM(mMVPMatrix, 0, mVMatrix, 0, currMatrix, 0);
//        Matrix.multiplyMM(mMVPMatrix, 0, mProjMatrix, 0, mMVPMatrix, 0);
        return multiplay(multiplay(currMatrix, mVMatrix ), mProjMatrix);
    }

    public float[] multiplay(float[] a, float[] b){
        float[] d = new float[16];
        int i = 0;
        do {
            d[i * 4] = a[i * 4] * b[0] + a[i * 4 + 1] * b[4] + a[i * 4 + 2] * b[8] + a[i * 4 + 3] * b[12];
            d[i * 4 + 1] = a[i * 4] * b[1] + a[i * 4 + 1] * b[5] + a[i * 4 + 2] * b[9] + a[i * 4 + 3] * b[13];
            d[i * 4 + 2] = a[i * 4] * b[2] + a[i * 4 + 1] * b[6] + a[i * 4 + 2] * b[10] + a[i * 4 + 3] * b[14];
            d[i * 4 + 3] = a[i * 4] * b[3] + a[i * 4 + 1] * b[7] + a[i * 4 + 2] * b[11] + a[i * 4 + 3] * b[15];
        } while ((++i) < 4);

        return d;
    }
    
    public void setVMatrix(float[] matrix){
    	System.arraycopy(matrix, 0, mVMatrix, 0, 16);
    }
    
    public float[] getVMatrix(){
    	return mVMatrix;
    }
    
    public float[] getProjMatrix(){
    	return mProjMatrix;
    }
    
    public float[] getCurrentMatrix(){
    	return currMatrix;
    }
    
    public float[] getTranslationMatrix(){
    	return mTranslationMatrix;
    }
    
    public float[] getScaleMatrix(){
    	return mScaleMatrix;
    }
    
    public float[] getRotateMatrix(){
    	return mRotateMatrix;
    }
}