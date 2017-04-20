package com.bfmj.viewcore.animation;

/**
 * 旋转动画类
 * @author zhangxin    
 * @Date 2015-6-19 下午5:11:54 
 */
public class GLRotateAnimation extends GLAnimation{
    private float mDegrees;

    //旋转轴
    private float mPivotX=1f;
    private float mPivotY=0.0f;
    private float mPivotZ=0.0f;
    
    /**
     * 旋转动画类的构造器
     * @param degrees: 旋转度数
     * @param pivotX: 设置为1，绕x轴旋转
     * @param pivotY: 设置为1，绕y轴旋转
     * @param pivotZ: 设置为1，绕z轴旋转
     */
    public GLRotateAnimation(float degrees, float pivotX, float pivotY, float pivotZ) {
    	mDegrees = degrees;

        mPivotX = pivotX;
        mPivotY = pivotY;
        mPivotZ = pivotZ;
    }
    
    float sum=0;
    @Override
    protected void applyTransformation(float normalizedTime, GLTransformation t) {
        float degrees = mDegrees * normalizedTime;
        
        sum += degrees;
        t.setDegree(degrees);
        t.setXYZ(mPivotX, mPivotY, mPivotZ);
    }    
       
}
