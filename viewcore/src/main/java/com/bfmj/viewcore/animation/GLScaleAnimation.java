package com.bfmj.viewcore.animation;

/**
 * 缩放动画类
 * @author zhangxin    
 * @Date: 2015-3-10 下午2:04:11 
 */
public class GLScaleAnimation extends GLAnimation{

	private float mToDx 	= 0.0f;
	private float mToDy 	= 0.0f;
	
	private float mFromDx 	= 1;
	private float mFromDy 	= 1;
	
    private float sumDx=0;
    private float sumDy=0;
    
    /**
	 * 创建位移动画对象
	 * @param dx 动画开始x坐标
	 * @param dy 动画开始y坐标
	 */
    public GLScaleAnimation(float dx, float dy) {   	
    	mToDx = dx;
    	mToDy = dy;
    }
    
    
    public GLScaleAnimation(float fromDx, float fromDy, float toDx, float toDy) {   	
    	mToDx = toDx;
    	mToDy = toDy;
    	
    	mFromDx = fromDx;
    	mFromDy = fromDy;
    }
    
	@Override
    protected void applyTransformation(float normalizedTime, GLTransformation t) {		
        float dx = 0f;               
        float dy = 0f;
        
        dx = (mToDx - mFromDx) * normalizedTime;
        dy = (mToDy - mFromDy) * normalizedTime;
        
        sumDx += dx;
        sumDy += dy;
        
        t.setXYZ(mFromDx+sumDx, mFromDy+sumDy, 1);
    }    
    
//	@Override
//    protected void applyTransformation(float normalizedTime, GLTransformation t) {		
//        float dx = 0f;               
//        float dy = 0f;
//        
//        if (mFromDx == -1) {
//        	mFromDx = getAnimView().getScaleX();
//        }
//        if (mFromDy == -1) {
//        	mFromDy = getAnimView().getScaleY();
//        }
//        
//        dx = (mToDx - mFromDx) * normalizedTime;
//        dy = (mToDy - mFromDy) * normalizedTime;
//        
//        sumDx += dx;
//        sumDy += dy;
//        
//        t.setXYZ(mFromDy+sumDx, mFromDy+sumDy, 1);
//    }
    
}
