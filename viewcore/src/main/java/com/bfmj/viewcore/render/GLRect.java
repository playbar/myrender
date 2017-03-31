package com.bfmj.viewcore.render;

public abstract class GLRect {
//	public static int bufferIndex = 1;
	
	private float mAlpha = 1.0f;
	private float mMask = 1.0f;
	
	private float vertices[] = {
       -0.5f,  0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f,
		0.5f,  0.5f, 0.0f,
		0.5f, -0.5f, 0.0f

    };
	
	public abstract void draw(float[] mtx);
	
	/**
     * 设置透明度
     * @author lixianke  @Date 2015-3-17 下午2:50:10
     * @param alpha 透明值 0-1 
     * @return
     */
    public void setAlpha(float alpha){
    	mAlpha = alpha;
    }
    
    /**
	 * 获取透明度
	 * @author lixianke  @Date 2015-3-12 下午4:15:05
	 * @param
	 * @return 透明度值，0~1之间
	 */
	public float getAlpha(){
    	return mAlpha;
    } 
    
    /**
     * 设置亮度
     * @author lixianke  @Date 2015-7-9 下午2:49:02
     * @param mask 亮度0-1
     * @return
     */
    public void setMask(float mask){
        mMask = mask;
    }
    
    /**
     * 获取亮度
     * @author lixianke  @Date 2015-7-9 下午2:56:19
     * @param
     * @return 亮度值，0~1之间
     */
    public float getMask(){
    	return mMask;
    }

	protected float[] getVertices() {
		return vertices;
	}
}