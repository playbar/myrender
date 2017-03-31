package com.bfmj.viewcore.animation;

import com.bfmj.viewcore.render.GLScreenParams;


/**
 * 动画view该类位移动画：
 * 1.调用初始化方法initialize
 * 2.调用start()方法设置开始动画
 * 3.调用getTransformation方法取得缩放矩阵
 * @author zhangxin    
 * @Date: 2015-3-9 下午2:36:07 
 */
public class GLTranslateAnimation extends GLAnimation{

	
    private int mXType = ABSOLUTE;
    private int mYType = ABSOLUTE;
    private int mZType = ABSOLUTE;
    
    
	private float mDx 	= 0.0f;
	private float mDy 	= 0.0f;
	private float mDz 	= 0.0f;
	
    private float mXDelta;
    private float mYDelta;
    private float mZDelta;
    
    private float sumX=0;
    private float sumY=0;
    private float sumZ=0;
    
    private boolean isUseXYZ=false;
    
    
	/**
	 * 创建位移动画对象
	 * @param dx x轴位移，正数右移，负数左移
	 * @param dy y轴位移，正数上移，负数下移
	 * @param dz z轴位移，正数后移，负数前移
	 */
	public GLTranslateAnimation(float dx, float dy, float dz) 
	{
		mXDelta = mDx = dx;
		mYDelta = mDy = dy;
		mZDelta = mDz = dz;

		mXType = ABSOLUTE;
        mYType = ABSOLUTE;
        mZType = ABSOLUTE;
	}
	
	/**
	 * 暂不支持应用层使用。
	 * @param fromXType ... ：      有三种：ABSOLUTE、RELATIVE_TO_SELF、RELATIVE_TO_PARENT
	 */
    public GLTranslateAnimation(float dx, float dy, float dz, int xType,
            int yType, int zType) {
    	
		mXDelta = mDx = dx;
		mYDelta = mDy = dy;
		mZDelta = mDz = dz;
		
        mXType = xType;
        mYType = yType;
        mZType = zType;     
    }

    
    /**
     * @author zhangxin  @Date 2015-3-10 上午9:55:32
     * description:该方法在调用getTransformation之前调用，根据用户的选择的绝对距离，按自己比例，按父类比例来计算动画变换距离
     * @param width:动画view的宽度；
     * 			height:动画view的高度
     * 			parentWidth：动画view父类的宽度
     * 			parentHeight：动画view父类的高度
     * @return
     */
    public void initialize(int width, int height, int parentWidth, int parentHeight) {
        mXDelta = resolveSize(mXType, mDx, width, parentWidth);
        mYDelta = resolveSize(mYType, mDy, height, parentHeight);
        
        //view没有z轴，所以z轴移动比例按照高度做比例        
        mZDelta = resolveSize(mZType, mDz, height, parentHeight);
    }
    
    /**
     * 父类调用，计算变换矩阵，返回给动画view使用，应用层不直接使用。
     * @param interpolatedTime：经过插值计算的插值时间；
     * @param t:动画view传入的变换类，变换矩阵在此类中返回给动画view
     */
    float sum=0;
    @Override
    public void applyTransformation(float normalizedTime, GLTransformation t) {
        
    	float dx=0,dy=0,dz=0;
    	
    	if (Math.abs(sumX) < Math.abs(mXDelta)) {    		
    		dx = mXDelta * normalizedTime;
    	}
    	
    	if (Math.abs(sumY) < Math.abs(mYDelta)) {
    		dy = mYDelta * normalizedTime;
    	}
    	
    	if (Math.abs(sumZ) < Math.abs(mZDelta)) {
    		dz = mZDelta * normalizedTime;
    	}
        
		sumX += dx;
		sumY += dy;
		sumZ += dz;
		
		//最后一次补齐
		if (Math.abs(sumX) > Math.abs(mXDelta))
			dx = dx - (sumX - mXDelta);
		if (Math.abs(sumY) > Math.abs(mYDelta))
			dy = dy - (sumY - mYDelta);
		if (Math.abs(sumZ) > Math.abs(mZDelta))
			dz = dz - (sumZ - mZDelta);		

    	t.setXYZ(dx, dy, dz);
    }    

	public float getXDelta() {
		float xDpi = GLScreenParams.getXDpi();
		return mXDelta * xDpi;
	}

	public float getYDelta() {
		float yDpi = GLScreenParams.getYDpi();
		return mYDelta * yDpi;
	}

	public float getZDelta() {
		float yDpi = GLScreenParams.getYDpi();
		return mZDelta * yDpi;
	}

	public boolean isUseXYZ() {
		return isUseXYZ;
	}

	public void setUseXYZ(boolean isUseXYZ) {
		this.isUseXYZ = isUseXYZ;
	}
    
}
