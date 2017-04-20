package com.bfmj.viewcore.animation;

/**
 * 透明度动画类
 * @author zhangxin    
 */
public class GLAlphaAnimation extends GLAnimation{

	private float mFromAlpha;
    private float mToAlpha;
    private float mAlpha=1;
    
    /**
     * 透明度动画构造器
     * @param fromAlpha 动画开始透明度。值范围0-1. 
     * @param toAlpha 动画结束透明度。值范围0-1.
     */
    public GLAlphaAnimation(float fromAlpha, float toAlpha) {
        mFromAlpha = fromAlpha;
        mToAlpha = toAlpha;
        mAlpha = fromAlpha;
    }
    
    
    @Override
    protected void applyTransformation(float interpolatedTime, GLTransformation t) 
    {        
    	mAlpha += (mToAlpha - mFromAlpha) * interpolatedTime;
        t.setAlpha(mAlpha);
        
//Log.d("test", "fff:"+t.getAlpha()+"");

    }
}
